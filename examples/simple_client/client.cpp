
#include <iostream>
#include <csignal>

#define PAIRSIM_DEBUG_ENABLED
#include <pairsim/client.hpp>

#include "./client_model.hpp"

ps::Client<> client;

void sigintHandler(int signum) {
    client.end();
    exit(0);
}

int main() {
    std::signal(SIGINT, sigintHandler);

    auto tickDuration = std::chrono::seconds(1);
    client.setServerAddr("tcp://127.0.0.1:4001");
    client.setModel(std::make_shared<TestClientModel>());

    client.setup();

    while (!client.shouldEnd()) {
        std::this_thread::sleep_for(tickDuration);
        client.tick();
    }

    std::cout << "Ended!" << std::endl;
    client.end();

    return 0;
}
