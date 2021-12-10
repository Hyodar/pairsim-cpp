
#ifndef PAIRSIM_PACKET_HPP_
#define PAIRSIM_PACKET_HPP_

// JSON
#include <json.hpp>
using json = nlohmann::json;

// Internal classes
#include "./buffer.hpp"
#include "./packet_type.hpp"

namespace ps { namespace packet {

/**
 * Encodes a JSON object into a buffer.
 * Currently using CBOR encoding.
 */
static Buffer encode(json j) {
    return json::to_cbor(j);
}

/**
 * Decodes a byte array into a JSON object.
 * Currently using CBOR encoding.
 */
static json decode(std::uint8_t* buf, std::uint32_t size) {
    Buffer output(buf, buf + size);
    return json::from_cbor(output);
}

/**
 * Creates a DEVICE packet.
 * \param device Device whose data is to be sent.
 */
template <typename DevicePtrType>
static Buffer device(DevicePtrType device) {
    json j;

    j["_t"] = PacketType::DEVICE;
    j["_d"] = device->getDeviceType();
    j["_id"] = device->getId();
    j["d"] = device->serialize();

    return encode(j);
}

/**
 * Creates a DEVICE_ADD packet.
 * This packet should only be sent by the client.
 * \param device Device thats being added.
 */
template <typename DevicePtrType>
static Buffer deviceAdd(DevicePtrType device) {
    json j;

    j["_t"] = PacketType::DEVICE_ADD;
    j["_d"] = device->getDeviceType();
    j["_id"] = device->getId();

    return encode(j);
}

/**
 * Creates an ACTION packet.
 * \param actionName Action name.
 * \param params Action parameters as a JSON object.
 */
static Buffer action(std::string actionName, json params) {
    json j;

    j["_t"] = PacketType::ACTION;
    j["_a"] = actionName;
    j["d"] = params;

    return encode(j);
}

/**
 * Creates an END packet.
 */
static Buffer end() {
    json j;

    j["_t"] = PacketType::END;

    return encode(j);
}

/**
 * Creates a TICK packet.
 */
static Buffer tick() {
    json j;

    j["_t"] = PacketType::TICK;

    return encode(j);
}

/**
 * Creates a READY packet.
 */
static Buffer ready() {
    json j;

    j["_t"] = PacketType::READY;

    return encode(j);
}

#ifdef PAIRSIM_SERVER_HPP_
/**
 * Creates a NOT_READY packet.
 */
static Buffer not_ready() {
    json j;

    j["_t"] = PacketType::NOT_READY;

    return encode(j);
}
#endif

/**
 * Creates a SETUP packet.
 */
static Buffer setup() {
    json j;

    j["_t"] = PacketType::SETUP;
    
    return encode(j);
}

} }

#endif // PAIRSIM_PACKET_HPP_
