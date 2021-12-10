
#ifndef __AVENS_PAIRSIM_PLANEMOBILITY_H_
#define __AVENS_PAIRSIM_PLANEMOBILITY_H_

#include <omnetpp.h>

#include "inet/mobility/base/MovingMobilityBase.h"

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class PlaneMobility : public inet::MovingMobilityBase {
  protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    void move();
};

#endif
