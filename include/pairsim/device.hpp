
#ifndef PAIRSIM_DEVICE_HPP_
#define PAIRSIM_DEVICE_HPP_

#include <json.hpp>
using json = nlohmann::json;

static std::uint32_t deviceCount = 0;

namespace ps {

class Device {

private:
    /** Device ID. Should be unique for each device. */
    std::uint32_t id;
    
    /** Device Type. This should be set in each child class. */
    std::string deviceType;

public:
    /**
     * Builds a JSON object with the info to be sent.
     */
    virtual json serialize() = 0;

    /**
     * Reads a JSON object with info from the other node.
     */
    virtual void deserialize(json j) = 0;

    /**
     * Creates a Device instance.
     * \param _deviceType Device type, defined by a std::string.
     */
    Device(std::string _deviceType) : id{++deviceCount}, deviceType{_deviceType} {}

    /**
     * Gets the device's ID.
     * \returns The device's ID.
     */
    std::uint32_t getId() { return id; }

    /**
     * Sets the device's ID. This is needed when receiving an DEVICE_ADD event.
     * \param _id ID value to be used to this instance's ID.
     */
    void setId(std::uint32_t _id) { id = _id; }

    /**
     * Gets the device type as a string.
     * \returns Device type.
     */
    std::string getDeviceType() { return deviceType; }
};

}

#endif // PAIRSIM_DEVICE_HPP_
