
#ifndef __AVENS_PAIRSIM_SERVER_H_
#define __AVENS_PAIRSIM_SERVER_H_

#include <omnetpp.h>

#include "./include/pairsim/client.hpp"

using namespace omnetpp;

class PairsimClient : public cSimpleModule {
private:
    cMessage* tickTimeout;
    ps::Client<> client;
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
public:
    PairsimClient();
    ~PairsimClient();
};

#endif
