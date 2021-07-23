#pragma once

#include "api.h"
#include "micro/MicroManager.h"

class BioManager : public MicroManager {
    public:
    BioManager() {};

    void doStateAction(Monte::GameObject& unit, char squadState);
    char validateState(Monte::GameObject& unit, char squadState);

    protected:
    void manageStim();
};