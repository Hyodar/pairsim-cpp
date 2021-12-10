
#ifndef PAIRSIM_SERVER_HPP_
#define PAIRSIM_SERVER_HPP_

#include <chrono>
#include <memory>

#include "node.hpp"
#include "server_model.hpp"

// Debugging log
#ifdef PAIRSIM_DEBUG_ENABLED
#include <iostream>
#define PAIRSIM_DEBUG(str) std::cout << "[PS] " << str << std::endl
#else
#define PAIRSIM_DEBUG(str)
#endif

namespace ps {

template <typename DurationType=std::chrono::milliseconds, typename DevicePtrType=std::shared_ptr<Device>>
class Server : public Node<ServerModel<DurationType, DevicePtrType>, DurationType, DevicePtrType> {
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
     * Current server state.
     */
    State state;

public:
    /**
     * Creates a server instance, only initializes members.
     */
    Server() : Node<ServerModel<DurationType, DevicePtrType>, DurationType, DevicePtrType>{}, state{State::SHOULD_SETUP} {}

    /**
     * Destroys a client instance.
     */
    ~Server() {}

    /**
     * Gets the server state.
     */
    State getState() {
        return this->state;
    }

    /**
     * Adds a device to the monitoring list.
     * After this, any data directed to it will be directly sent to
     * the instance's Device::deserialize.
     * \param d Device to be added.
     */
    void monitorDevice(DevicePtrType d) {
        PAIRSIM_DEBUG("Monitoring device " << d->getId());
        if (d == nullptr) {
            throw std::runtime_error("Caca 3");
        }

        this->devices.push_back(d);
        this->devicesByType[d->getDeviceType()][d->getId()] = d;
    }

    /**
     * Starts the setup phase.
     * Blocks until the client's data is received and the server's data is sent.
     */
    void setup() {
        this->state = State::SETTING_UP;
        this->checkParams();

        PAIRSIM_DEBUG("Listening...");
        this->sock.listen(this->address.c_str());
        this->running = true;

        PAIRSIM_DEBUG("Waiting for SETUP");
        this->waitFor(PacketType::SETUP);

        PAIRSIM_DEBUG("OK, now setting up this side");
        this->model->setup(this);
        this->queue.push(packet::setup());
        this->flush();
        this->state = State::SHOULD_WAIT_TICK;
    }

    void waitTick() {
        this->state = State::WAITING_TICK;
        PAIRSIM_DEBUG("Waiting TICK");
        this->waitFor(PacketType::TICK);
        this->state = State::SHOULD_GET_DATA;
    }

    void getData() {
        this->state = State::GETTING_DATA;
        PAIRSIM_DEBUG("Now getting this side's data");
        this->model->step(this);

        for (size_t i = 0; i < this->devices.size(); i++) {
            this->queue.push(packet::device<DevicePtrType>(this->devices[i]));
        }

        this->queue.push(packet::tick());
        this->state = State::SHOULD_SEND_DATA;
    }

    void sendData() {
        this->state = State::SENDING_DATA;
        PAIRSIM_DEBUG("Now sending this side's data");
        this->flush();
        this->state = State::SHOULD_WAIT_TICK;
    }

private:
    /**
     * Handles a DEVICE_ADD packet.
     * \param msg JSON message received.
     */
    void handleDeviceAdd(json msg) {
        auto device = this->model->onDeviceAdd(msg["_d"], msg["_id"]);
        if (device != nullptr) {
            device->setId(msg["_id"]);
            monitorDevice(device);
        }
    }

    /**
     * Handles an READY packet.
     * \param JSON message received.
     */
    void handleReady(json msg) {
        if (this->model->ready()) {
            this->queue.push(packet::ready());
            this->flush();
        }
        else {
            this->queue.push(packet::not_ready());
            this->flush();
        }
    }

    /**
     * Handles a NOT_READY packet.
     * \param JSON message received.
     */
    void handleNotReady(json msg) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        this->queue.push(packet::ready());
        this->flush();
    }
};

}

#endif // PAIRSIM_SERVER_HPP_
