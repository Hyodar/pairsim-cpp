
#ifndef PAIRSIM_PACKET_TYPE_HPP_
#define PAIRSIM_PACKET_TYPE_HPP_

namespace ps {

/**
 * Enum defining a packet type.
 * It's important to notice that the chars are intentional,
 * representing a byte and not a string.
 */
enum PacketType: std::uint8_t {
    DEVICE = 'D',
    DEVICE_ADD = 'd',
    ACTION = 'A',
    END = 'E',
    TICK = 'T',
    READY = 'R',
    NOT_READY = 'r',
    SETUP = 'S',
};

}

#endif // PAIRSIM_PACKET_TYPE_HPP_
