
#ifndef XPLANEMODEL_H_
#define XPLANEMODEL_H_

#include <vector>

#include <omnetpp/cmodule.h>
#include <omnetpp/ccomponenttype.h>

#include <pairsim/client.hpp>
#include <pairsim/client_model.hpp>

#include "Plane.h"

class XPlaneModel : public ps::ClientModel<> {
private:
    cModule* parentModule;
    std::vector<std::shared_ptr<Plane>> planes;
public:
    XPlaneModel(cModule* _parentModule);
    ~XPlaneModel();

    void setup(ps::Client<>* client) {
        for (size_t i = 0; i < 1; i++) {
            cModuleType* planeType = cModuleType::get("avens_pairsim.Plane");

            cModule* module = planeType->createScheduleInit("plane", parentModule->getParentModule());

            auto plane = std::shared_ptr<Plane>(check_and_cast<Plane*>(module));

            planes.push_back(plane);
            client->addDevice(plane);

            client->addAction("hello_world", [](json param) {
               // no-op
            });
        }
    }

    void step(ps::Client<>* client) {
        // no-op
    }

    void end() {
        // no-op
    }
};

#endif /* XPLANEMODEL_H_ */
