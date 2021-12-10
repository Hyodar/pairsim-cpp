
#ifndef PAIRSIM_SERVER_MODEL_HPP_
#define PAIRSIM_SERVER_MODEL_HPP_

#include <chrono>
#include <memory>

#include "./device.hpp"

namespace ps {

template <typename DurationType, typename DevicePtrType>
class Server;

template <typename DurationType=std::chrono::milliseconds, typename DevicePtrType=std::shared_ptr<Device>>
class ServerModel {

public:
    /**
     * Callback that is called each time a device is created in the client.
     * The device type should be parsed to create the specific desired device.
     * \param deviceType String indicating the device type.
     * \param id Device ID.
     * \returns Shared pointer containing the newly created device.
     */
    virtual DevicePtrType onDeviceAdd(std::string deviceType, std::uint32_t id) = 0;

    /**
     * Method that is run on the beginning of the model's lifetime,
     * right after the connection is successfully established and
     * the server already received setup info from the client.
     * \param server ps::Server instance that the model is attached to.
     */
    virtual void setup(Server<DurationType, DevicePtrType>* server) = 0;

    /**
     * Method that is run before each data transmission tick.
     * Could be used to send actions and other info based on
     * the current state.
     * \param server ps::Server instance that the model is attached to.
     */
    virtual void step(Server<DurationType, DevicePtrType>* server) = 0;

    /**
     * Method that is run before the server's termination. Use it
     * to free resources and end other related procedures.
     * \param server ps::Server instance that the model is attached to.
     */
    virtual void end() = 0;

    /**
     * Indicates whether the model is ready to start the communication.
     * \returns `true` if the model is ready or `false` otherwise.
     */
    bool ready() {
        return true;
    }
};

}

#endif // PAIRSIM_SERVER_MODEL_HPP_
