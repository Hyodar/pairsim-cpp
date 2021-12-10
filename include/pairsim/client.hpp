
#ifndef PAIRSIM_CLIENT_HPP_
#define PAIRSIM_CLIENT_HPP_

#include <chrono>
#include <memory>

#include "node.hpp"
#include "client_model.hpp"

// Debugging log
#ifdef PAIRSIM_DEBUG_ENABLED
#include <iostream>
#define PAIRSIM_DEBUG(str) std::cout << "[PS] " << str << std::endl
#else
#define PAIRSIM_DEBUG(str)
#endif

namespace ps {

template <typename DurationType=std::chrono::milliseconds, typename DevicePtrType=std::shared_ptr<Device>>
class Client : public Node<ClientModel<DurationType, DevicePtrType>, DurationType, DevicePtrType> {
public:
    enum State {
        SHOULD_SETUP = 0,
        SETTING_UP = 1,
        SHOULD_GET_DATA = 2,
        GETTING_DATA = 3,
        SHOULD_SEND_DATA = 4,
        SENDING_DATA = 5,
        SHOULD_WAIT_TICK = 6,
        WAITING_TICK = 7,
    };

private:
    /**
     * How much should be waited between each request to know
     * if the server is ready to begin the communication.
     */
    std::chrono::milliseconds retryDelay;

    /**
     * Current client state.
     */
    State state;

public:
    /**
     * Creates a client instance, only initializes members.
     */
    Client() : Node<ClientModel<DurationType, DevicePtrType>, DurationType, DevicePtrType>{}, retryDelay{1000}, state{State::SHOULD_SETUP} {}

    /**
     * Destroys a client instance.
     */
    ~Client() {}

    /**
     * Gets the server state.
     */
    State getState() {
        return this->state;
    }

    /**
     * Sets the retry delay, which is the delay between ready checks with
     * the server.
     * \param _retryDelay Retry delay.
     */
    void setRetryDelay(std::chrono::milliseconds _retryDelay) {
        this->retryDelay = _retryDelay;
    }

    /**
     * Adds a device to the monitoring list.
     * After this, any data directed to it will be directly sent to
     * the instance's Device::deserialize.
     * \param d Device to be added.
     */
    void addDevice(DevicePtrType d) {
        PAIRSIM_DEBUG("Adding device " << d->getId());
        if (d == nullptr) {
            throw std::runtime_error("Caca 3");
        }

        this->queue.push(packet::deviceAdd<DevicePtrType>(d));

        this->devices.push_back(d);
        this->devicesByType[d->getDeviceType()][d->getId()] = d;
    }

    /**
     * Starts the setup phase.
     * Blocks until the client's data is sent and the server's data is received.
     */
    void setup() {
        state = State::SETTING_UP;

        this->checkParams();

        PAIRSIM_DEBUG("Dialing");
        this->sock.dial(this->address.c_str());
        PAIRSIM_DEBUG("Done!");
        this->running = true;

        // sends a READY to the server and waits for a READY
        PAIRSIM_DEBUG("Are you ready?");
        this->queue.push(packet::ready());
        this->flush();
        this->waitFor(PacketType::READY);
        PAIRSIM_DEBUG("OK, its ready?");

        // sends setup data
        PAIRSIM_DEBUG("Setting up then.");
        this->model->setup(this);
        this->queue.push(packet::setup());
        this->flush();
        PAIRSIM_DEBUG("Sent info. Now waiting for new data!");

        // processes received data until a SETUP is received
        this->waitFor(PacketType::SETUP);

        state = State::SHOULD_GET_DATA;
    }

    /**
     * Runs ClientModel::step and sends device data.
     * Blocks until the server's tick finishes.
     */
    void tick() {
        getData();
        sendData();
        waitTick();
    }

    /**
     * Wait for a TICK packet.
     */
    void waitTick() {
        state = State::WAITING_TICK;
        PAIRSIM_DEBUG("Waiting for tick.");
        this->waitFor(PacketType::TICK);
        state = State::SHOULD_GET_DATA;
    }

    /**
     * Get device data and step the model.
     */
    void getData() {
        state = State::GETTING_DATA;
        PAIRSIM_DEBUG("Sending data.");
        this->model->step(this);

        for (size_t i = 0; i < this->devices.size(); i++) {
            this->queue.push(packet::device<DevicePtrType>(this->devices[i]));
        }

        this->queue.push(packet::tick());
        state = State::SHOULD_SEND_DATA;
    }

    /**
     * Send queued data.
     */
    void sendData() {
        state = State::SENDING_DATA;
        this->flush();
        state = State::SHOULD_WAIT_TICK;
    }

private:
    /**
     * Handles a DEVICE_ADD packet.
     * \param msg JSON message received.
     */
    void handleDeviceAdd(json msg) {
        throw std::runtime_error("Caca 4");
    }

    /**
     * Handles an READY packet.
     * \param JSON message received.
     */
    void handleReady(json msg) {
        // no-op
    }

    /**
     * Handles a NOT_READY packet.
     * \param JSON message received.
     */
    void handleNotReady(json msg) {
        std::this_thread::sleep_for(retryDelay);
        this->queue.push(packet::ready());
        this->flush();
    }
};

}

#endif // PAIRSIM_CLIENT_HPP_
