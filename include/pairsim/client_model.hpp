
#ifndef PAIRSIM_CLIENT_MODEL_HPP_
#define PAIRSIM_CLIENT_MODEL_HPP_

#include <chrono>
#include <memory>

#include "./device.hpp"

namespace ps {

template <typename DurationType, typename DevicePtrType>
class Client;

template <typename DurationType=std::chrono::milliseconds, typename DevicePtrType=std::shared_ptr<Device>>
class ClientModel {

public:
    /**
     * Method that is run on the beginning of the model's lifetime,
     * right after the connection is successfully established and
     * th<DevicePtrType>e server is ready to receive setup info.
     * \param client ps::Client instance that the model is attached to.
     */
    virtual void setup(Client<DurationType, DevicePtrType>* client) = 0;

    /**
     * Method that is run before each data transmission tick.
     * Could be used to send actions and other info based on
     * the current state.
     * \param client ps::Client instance that the model is attached to.
     */
    virtual void step(Client<DurationType, DevicePtrType>* client) = 0;

    /**
     * Method that is run before the client's termination. Use it
     * to free resources and end other related procedures.
     * \param client ps::Client instance that the model is attached to.
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

#endif // PAIRSIM_CLIENT_MODEL_HPP_
