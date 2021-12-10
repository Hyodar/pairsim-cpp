
#include <thread>
#include <chrono>
#include <csignal>

#define PAIRSIM_DEBUG_ENABLED
#include <pairsim/server.hpp>

#include "./server_model.hpp"

#include <iostream>

ps::Server<> server;

void sigintHandler(int signum) {
    server.end();
    exit(0);
}

int main() {
    std::signal(SIGINT, sigintHandler);

    auto tickDuration = std::chrono::seconds(1);
    server.setServerAddr("tcp://127.0.0.1:4001");
    server.setModel(std::make_shared<TestServerModel>());

    server.setup();

    while (!server.shouldEnd()) {
        server.waitTick();
        // other tasks would be here
        std::this_thread::sleep_for(tickDuration);
        server.sendData();
    }

    std::cout << "Ending execution." << std::endl;
    server.end();

    return 0;
}

