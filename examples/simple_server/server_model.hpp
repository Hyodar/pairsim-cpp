
#ifndef SERVER_MODEL_HPP_
#define SERVER_MODEL_HPP_

#include <vector>

#include "./plane.hpp"
#include <pairsim/server_model.hpp>
#include <pairsim/server.hpp>

class TestServerModel : public ps::ServerModel<> {
private:
    std::vector<std::shared_ptr<Plane>> planes;

public:
    std::shared_ptr<ps::Device> onDeviceAdd(std::string deviceType, std::uint32_t id) {
        std::shared_ptr<ps::Device> resp = nullptr;

        if (deviceType == "plane") {
            auto p = std::make_shared<Plane>();
            planes.push_back(p);

            resp = p;
        }

        return resp;
    }

    void setup(ps::Server<>* server) {
        // no-op
    }

    void step(ps::Server<>* server) {
        for (size_t i = 0; i < planes.size(); i++) {
            planes[i]->move(1, 1, 1);
        }

        server->sendAction("hello_world", "{\"s\":\"Hello World!\"}"_json);
    }

    void end() {
        // no-op
    }
};

#endif // SERVER_MODEL_HPP_
