#pragma once

#include "Squad.h"

class AttackSquad : public Squad {
    public:

    enum State {
        Init,
        Bide,
        Attack
    };

    AttackSquad() {};

    void OnStep();

    protected:
    void validateState();
};