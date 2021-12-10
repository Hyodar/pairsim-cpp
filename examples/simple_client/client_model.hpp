
#ifndef CLIENT_MODEL_HPP_
#define CLIENT_MODEL_HPP_

#include <vector>

#include "./plane.hpp"
#include <pairsim/client_model.hpp>
#include <pairsim/client.hpp>

class TestClientModel : public ps::ClientModel<> {
private:
    std::vector<std::shared_ptr<Plane>> planes;

public:
    void helloWorld(json params) {
        std::cout << "Hello World! " << params["s"].get<std::string>() << std::endl;
    }

    void setup(ps::Client<>* client) {
        for (size_t i = 0; i < 5; i++) {
            auto p = std::make_shared<Plane>();
            planes.push_back(p);
            
            client->addDevice(p);
        }

        client->addAction("hello_world", [this](json params) {
            helloWorld(params);
        });
    }

    void step(ps::Client<>* client) {
        // no-op
    }

    void end() {
        // no-op
    }
};

#endif // CLIENT_MODEL_HPP_
