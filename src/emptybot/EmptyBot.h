#pragma once

#include <sc2api/sc2_agent.h>

class EmptyBot : public sc2::Agent {
    public:
    EmptyBot() {};

    void OnGameStart() {};   
    void OnStep() {};
    void OnGameEnd() {};
};