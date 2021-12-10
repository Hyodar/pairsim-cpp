
#ifdef NOT_MOCK
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#else
#include "XPLMMock.h"
#endif

#include <cstring>
#include <cfloat>

#include <pairsim/server_model.hpp>

#define PAIRSIM_DEBUG_ENABLED
#include <pairsim/server.hpp>

#if IBM
	#include <windows.h>
#endif

#ifndef XPLM300
	#error This is made to be compiled against the XPLM300 SDK
#endif

using duration_t = double;
using AvensServer = ps::Server<duration_t>;

class UserPlane : public ps::Device {
private:
    double x;
    double y;
    double z;

    XPLMDataRef xRef;
    XPLMDataRef yRef;
    XPLMDataRef zRef;

public:
    UserPlane() : ps::Device{"user_plane"}, x{0}, y{0}, z{0}, xRef{NULL}, yRef{NULL}, zRef{NULL} {
        xRef = XPLMFindDataRef("sim/flightmodel/position/local_x");
        yRef = XPLMFindDataRef("sim/flightmodel/position/local_y");
        zRef = XPLMFindDataRef("sim/flightmodel/position/local_z");
    }

    void getData() {
        x = XPLMGetDatad(xRef);
        y = XPLMGetDatad(yRef);
        z = XPLMGetDatad(zRef);
    }

    json serialize() {
        getData();

        json pos;
        pos["x"] = x;
        pos["y"] = y;
        pos["z"] = z;

        json j;
        j["pos"] = pos;
        
        return j;
    }

    void deserialize(json j) {
        // no-op
    }
};

class XPlaneModel : public ps::ServerModel<duration_t> {
private:
    std::shared_ptr<UserPlane> userPlane;
    bool* isReady;

public:
    XPlaneModel(bool* _isReady) {
        isReady = _isReady;
    }

    std::shared_ptr<ps::Device> onDeviceAdd(std::string deviceType, std::uint32_t id) {
        std::shared_ptr<ps::Device> resp = nullptr;

        if (deviceType == "user_plane") {
            auto p = std::make_shared<UserPlane>();
            p->setId(id);
            
            userPlane = p;
            resp = p;
        }

        return resp;
    }

    void setup(AvensServer* server) {
        // no-op
    }

    void step(AvensServer* server) {
        // no-op
    }

    void end() {
        // no-op
    }

    bool ready() {
        return *isReady;
    }
};

static AvensServer* server = nullptr;
static bool flightReady = false;
static bool paused = false;
static bool shouldPause = true;
static XPLMDataRef timeSpeedRef;
static constexpr uint8_t EXECUTION_SPEED = 1;

void pause() {
    std::cout << "paused" << std::endl;
    XPLMSetDatai(timeSpeedRef, 0);
    paused = true;
}

void unpause() {
    std::cout << "unpaused" << std::endl;
    XPLMSetDatai(timeSpeedRef, EXECUTION_SPEED);
    paused = false;
}

float serverCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void* inRefcon) {
    std::cout << server->getState() << std::endl;
    if ((inElapsedSinceLastCall - server->getTickDuration()) > FLT_EPSILON) {
        exit(1);
    }

    flightReady = true; // maybe this could be in the message callback, whenever a plane is loaded

    if (server->getState() == AvensServer::State::SHOULD_GET_DATA) {
        if (paused) {
            unpause();
            shouldPause = false;

            std::thread thr([]() {
                std::this_thread::sleep_for(std::chrono::microseconds(static_cast<long long>(server->getTickDuration() * 1000000)));
                shouldPause = true;
            });
            thr.detach();
        }
        else if (shouldPause) {
            pause();
            server->getData();
            
            std::thread thr([]() {
                server->sendData();
                server->waitTick();
            });
            thr.detach();
        }
    }

    return server->getTickDuration();
}

PLUGIN_API int XPluginStart(char* outName, char* outSig, char* outDesc) {
    strcpy(outName, "AvensPlugin");
    strcpy(outSig, "avens.plugin");
    strcpy(outDesc, "Avens Plugin");

    timeSpeedRef = XPLMFindDataRef("sim/time/sim_speed");
    server = new AvensServer();

    server->setTickDuration(0.01);
    server->setServerAddr("tcp://localhost:4001");
    server->setModel(std::make_shared<XPlaneModel>(&flightReady));

    std::thread thr([]() {
        server->setup();
        server->waitTick();
    });
    thr.detach();

    XPLMCreateFlightLoop_t flightLoop;

    flightLoop.structSize = sizeof(flightLoop);
    flightLoop.phase = xplm_FlightLoop_Phase_AfterFlightModel;
    flightLoop.callbackFunc = &serverCallback;
    flightLoop.inRefcon = nullptr;

    XPLMFlightLoopID flightLoopId = XPLMCreateFlightLoop(&flightLoop);
    XPLMScheduleFlightLoop(flightLoopId, server->getTickDuration(), true);

    return 1;
}

PLUGIN_API void	XPluginStop() { server->end(); delete server; server = nullptr; }
PLUGIN_API void XPluginDisable() {}
PLUGIN_API int XPluginEnable() { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void* inParam) {}

#ifndef NOT_MOCK
int main() {
    XPluginEnable();
    char* str = new char[100];
    XPluginStart(str, str, str);
    delete str;
    for (;;) {}
}
#endif
