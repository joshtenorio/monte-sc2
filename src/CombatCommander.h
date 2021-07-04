#pragma once

#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_agent.h>
#include "api.h"
#include "Manager.h"

class CombatCommander : public Manager {
    public:
    CombatCommander() {}; // empty constructor

    void OnGameStart();
    void OnStep();

    protected:
    int foo;
};