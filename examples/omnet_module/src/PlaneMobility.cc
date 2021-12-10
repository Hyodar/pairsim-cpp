
#include "PlaneMobility.h"

#include "./Plane.h"

Define_Module(PlaneMobility);

void PlaneMobility::initialize(int stage) {
    MovingMobilityBase::initialize(stage);
}

void PlaneMobility::handleMessage(cMessage* msg) {
    MovingMobilityBase::handleMessage(msg);
}

void PlaneMobility::move() {
    Plane* plane = check_and_cast<Plane*>(getParentModule());
    lastPosition = plane->position;
}
