
#ifndef PLANE_HPP_
#define PLANE_HPP_

#include <pairsim/device.hpp>

class Plane : public ps::Device {
private:
    float x;
    float y;
    float z;

public:
    Plane() : ps::Device{"plane"} {}

    void move(float dx, float dy, float dz) {
        x += dx;
        y += dy;
        z += dz;
    }

    json serialize() {
        json pos;
        pos["x"] = x;
        pos["y"] = y;
        pos["z"] = z;

        json j;
        j["pos"] = pos;
        
        return j;
    }

    void deserialize(json j) {
        json pos = j["pos"].get<json>();

        x = pos["x"].get<float>();
        y = pos["y"].get<float>();
        z = pos["z"].get<float>();
    }
};

#endif // PLANE_HPP_
