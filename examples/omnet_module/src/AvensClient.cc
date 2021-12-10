
#include "./XPlaneModel.h"
#include "PairsimClient.h"

Define_Module(PairsimClient);

PairsimClient::PairsimClient() {
    tickTimeout = nullptr;
}

PairsimClient::~PairsimClient() {
    cancelAndDelete(tickTimeout);
}

void PairsimClient::initialize() {
    tickTimeout = new cMessage("tick");

    client.setServerAddr("tcp://localhost:4001");
    client.setTickDuration(std::chrono::seconds(1));
    client.setModel(std::make_shared<XPlaneModel>(this));

    EV << "Setup completed." << std::endl;

    client.setup();
    scheduleAt(simTime() + (static_cast<double>(client.getTickDuration().count())) / 1000, tickTimeout);
}

void PairsimClient::handleMessage(cMessage *msg) {
    client.tick();
    scheduleAt(simTime() + ((double) client.getTickDuration().count()) / 1000, msg);
}
