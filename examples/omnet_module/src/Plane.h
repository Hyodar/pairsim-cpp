
#ifndef __AVENS_PAIRSIM_PLANE_H_
#define __AVENS_PAIRSIM_PLANE_H_

#include <omnetpp.h>

#include "./include/pairsim/device.hpp"
#include "inet/common/geometry/common/Coord.h"

using namespace omnetpp;

class Plane : public omnetpp::cModule, public ps::Device {
public:
    inet::Coord position;
protected:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    // virtual void move();
public:
    Plane();

    json serialize();
    void deserialize(json j);
};

#endif
