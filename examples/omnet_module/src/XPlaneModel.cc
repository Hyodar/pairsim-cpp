
#include "XPlaneModel.h"

XPlaneModel::XPlaneModel(cModule* _parentModule) {
    parentModule = _parentModule;
}

XPlaneModel::~XPlaneModel() {
    for (size_t i = 0; i < planes.size(); i++) {
        planes[i]->deleteModule();
    }
}

