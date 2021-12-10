
#ifndef PAIRSIM_NODE_HPP_
#define PAIRSIM_NODE_HPP_

// Standard lib utilities
#include <string>
#include <queue>
#include <chrono>
#include <thread>

// NNG
#include <nngpp/protocol/pair0.h>
#include <nngpp/nngpp.h>

// JSON handling
#include <json.hpp>
using json = nlohmann::json;

// Internal classes
#include "device.hpp"
#include "buffer.hpp"
#include "packet.hpp"
#include "packet_type.hpp"

// Debugging log
#ifdef PAIRSIM_DEBUG_ENABLED
#include <iostream>
#define PAIRSIM_DEBUG(str) std::cout << "[PS] " << str << std::endl
#else
#define PAIRSIM_DEBUG(str)
#endif

namespace ps {

template <typename ModelClass, typename DurationType, typename DevicePtrType>
class Node {
protected:
    /** Whether the node is running or not. */
    bool running;

    /** Tick duration. */
    DurationType tickDuration;

    /** The address the client will dial / the server will listen. */
    std::string address;

    /** The model which customizes the execution behavior. */
    std::shared_ptr<ModelClass> model;

    /** The packet queue */
    std::queue<Buffer> queue;

    /** A simple container to store devices for monitoring. */
    std::vector<DevicePtrType> devices;

    /** A more complex container to simplify received data storage. */
    std::map<std::string, std::map<std::uint32_t, DevicePtrType>> devicesByType;

    /** Container that relates a callback to an action name */
    std::map<std::string, std::function<void(json)>> actionCallbacks;

    /** NNG socket. Currently a v0 Pair.*/
    nng::socket sock;

public:
    /**
     * Creates a node instance, only initializes members.
     */
    Node() : running{false}, tickDuration{0}, address{""}, model{nullptr},
             sock{nng::pair::v0::open()} {}

    /**
     * Destroys a node instance.
     */
    ~Node() {}

    /**
     * Sets the server's address.
     * \param _address Address, e.g. "tcp://localhost:4000".
     */
    void setServerAddr(std::string _address) {
        PAIRSIM_DEBUG("Setting server address");
        address = _address;
    }

    /**
     * Sets the current model.
     * \param _model Model instance. The instance will be safely
     * deleted when the last instance of the shared pointer is destroyed.
     */
    void setModel(std::shared_ptr<ModelClass> _model) {
        PAIRSIM_DEBUG("Setting model");
        model = _model;
    }

        /**
     * Sets the tick duration. This value is not used internally
     * and its storage is intended to ease its access when determining
     * when to run Server::tick() again.
     * \param _tickDuration Tick duration.
     */
    void setTickDuration(DurationType _tickDuration) {
        PAIRSIM_DEBUG("Setting tick duration");
        tickDuration = _tickDuration;
    }

    /**
     * Returns the tick duration. The result can be easily casted to different
     * units supported by std::chrono, effectively converting units.
     * \returns Previously set tick duration.
     */
    DurationType getTickDuration() {
        // PAIRSIM_DEBUG("Getting tick duration");
        return tickDuration;
    }

    /**
     * Whether the connection ended.
     * \returns `true` if the connection ended or `false` otherwise.
     */
    bool shouldEnd() {
        PAIRSIM_DEBUG("Should end?");
        return !running;
    }

    /**
     * Relates an action name to a callback.
     * \param actionName Action name.
     * \param cb Action callback.
     */
    void addAction(std::string actionName, std::function<void(json)> cb) {
        PAIRSIM_DEBUG("Adding action " << actionName);
        actionCallbacks[actionName] = cb;
    }

    /**
     * Sends an action by its name, together with arbitrary parameters.
     * \param actionName Action name.
     * \param params JSON defined parameters.
     */
    void sendAction(std::string actionName, json params) {
        PAIRSIM_DEBUG("Sending action " << actionName);
        queue.push(packet::action(actionName, params));
    }

    /**
     * Starts the setup phase.
     * Blocks until the client's data is received and the server's data is sent.
     */
    virtual void setup() = 0;

    /**
     * Ends execution. Will also try to end the client's
     * execution if possible (i.e. its socket is still open).
     * If called when the server is not running yet or Server::end
     * was already called, it's a no-op.
     * \param shouldEndPair Whether to try and send an END packet
     * to the client.
     */
    void end(bool shouldEndPair=true) {
        if (running) {
            PAIRSIM_DEBUG("Finishing execution.");
            model->end();

            if (shouldEndPair) {
                queue.push(packet::end());
                flush();
            }

            running = false;
        }
    }

    /**
     * Retrieves device data and steps model.
     */
    virtual void getData() = 0;
    
    /**
     * Sends queued data.
     * \param duration This tick's duration. It will be used in comparison
     * with the set tickDuration as an overflow ratio.
     */
    virtual void sendData() = 0;
    
    /**
     * Waits for a TICK packet.
     */
    virtual void waitTick() = 0;

protected:
    /**
     * Flushes the packet queue, sending all queued data.
     */
    void flush() {
        PAIRSIM_DEBUG("Flushing queue.");
        for (; queue.size(); queue.pop()) {
            const Buffer buf = queue.front();
            sock.send(nng::view(buf.data(), buf.size()));
        }
    }

    /**
     * Waits (blocking) for a specific packet type, while handling other types.
     * When the expected packet type is encountered, it'll first be handled
     * normally and only then the function will unblock the execution.
     * \param p Packet type to be waited.
     */
    void waitFor(PacketType p) {
        PAIRSIM_DEBUG("Waiting for: " << p);
        bool shouldBreak = false;

        while (!shouldBreak && running) {
            PAIRSIM_DEBUG("Waiting...");
            const nng::buffer buf = sock.recv();
            const json msg = packet::decode((std::uint8_t*) buf.data(), buf.size());
            const PacketType packetType = PacketType(msg["_t"].get<std::uint8_t>());

            switch (packetType) {
                case PacketType::ACTION:
                    PAIRSIM_DEBUG("Received ACTION:" << msg.dump());
                    handleAction(msg);
                    break;
                case PacketType::DEVICE:
                    PAIRSIM_DEBUG("Received DEVICE:" << msg.dump());
                    handleDevice(msg);
                    break;
                case PacketType::DEVICE_ADD:
                    PAIRSIM_DEBUG("Received DEVICE_ADD:" << msg.dump());
                    handleDeviceAdd(msg);
                    break;
                case PacketType::END:
                    PAIRSIM_DEBUG("Received END:" << msg.dump());
                    handleEnd(msg);
                    break;
                case PacketType::READY:
                    PAIRSIM_DEBUG("Received READY:" << msg.dump());
                    handleReady(msg);
                    break;
                case PacketType::NOT_READY:
                    PAIRSIM_DEBUG("Received NOT_READY:" << msg.dump());
                    handleNotReady(msg);
                case PacketType::TICK:
                    PAIRSIM_DEBUG("Received TICK:" << msg.dump());
                    handleTick(msg);
                    break;
                case PacketType::SETUP:
                    PAIRSIM_DEBUG("Received SETUP:" << msg.dump());
                    handleSetup(msg);
                    break;
            }

            if (packetType == p) {
                shouldBreak = true;
            }
        }
    }

    /**
     * Handles an ACTION packet.
     * \param msg JSON message received.
     */
    void handleAction(json msg) {
        const auto cb = actionCallbacks.find(msg["_a"].get<std::string>());

        if (cb == actionCallbacks.end()) {
            throw std::runtime_error("Caca");
        }

        cb->second(msg["d"]);
    }

    /**
     * Handles a DEVICE packet.
     * \param msg JSON message received.
     */
    void handleDevice(json msg) {
        const std::string deviceType = msg["_d"];
        const std::uint32_t id = msg["_id"];

        const auto device = devicesByType[deviceType][id];

        device->deserialize(msg["d"]);
    }

    /**
     * Handles a DEVICE_ADD packet.
     * \param msg JSON message received.
     */
    virtual void handleDeviceAdd(json msg) = 0;

    /**
     * Handles an END packet.
     * \param msg JSON message received.
     */
    void handleEnd(json msg) {
        end(false);
    }

    /**
     * Handles an READY packet.
     * \param JSON message received.
     */
    virtual void handleReady(json msg) = 0;

    /**
     * Handles a NOT_READY packet.
     * \param JSON message received.
     */
    virtual void handleNotReady(json msg) = 0;

    /**
     * Handles a TICK packet.
     * \param JSON message received.
     */
    void handleTick(json msg) {
        // no-op
    }

    /**
     * Handles a SETUP packet.
     * \param JSON message received.
     */
    void handleSetup(json msg) {
        // no-op
    }

    /**
     * Checks whether all needed parameters are set.
     */
    void checkParams() {
        if (address == "") {
            throw std::runtime_error("server address should be set.");
        }
        if (model == nullptr) {
            throw std::runtime_error("model should be set.");
        }
    }
};

}

#endif // PAIRSIM_NODE_HPP_
