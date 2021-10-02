#pragma once

#include "Squad.h"

class AttackSquad : public Squad {
    public:

    AttackSquad() {};

    void OnStep();

    protected:
    void validateState();

};