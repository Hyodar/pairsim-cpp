//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Plane.h"

Define_Module(Plane);

Plane::Plane() : ps::Device{"user_plane"} {}


json Plane::serialize() {
    json pos;
    pos["x"] = position.x;
    pos["y"] = position.y;
    pos["z"] = position.z;

    json j;
    j["pos"] = pos;

    return j;
}

void Plane::deserialize(json j) {
    json pos = j["pos"].get<json>();

    position.x = pos["x"];
    position.y = pos["y"];
    position.z = pos["z"];
}

void Plane::initialize(int stage) {
    // MovingMobilityBase::initialize(stage);
    cModule::initialize(stage);
}

void Plane::handleMessage(cMessage *msg) {
    // no-op
}

//void Plane::move() {
//    // no-op
//}

