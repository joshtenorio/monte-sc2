#pragma once

#include "Squad.h"

class AttackSquad : public Squad {
    public:
    AttackSquad() {}; // is the {} required?

    void OnStep();

    protected:
    void validateState();
};