#pragma once

#include "api.h"
#include "Manager.h"

class CombatCommander : public Manager {
    public:

    void OnStep();
    protected:
    int foo;
};