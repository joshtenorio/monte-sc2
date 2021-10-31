#pragma once

#include "api.h"
#include "combat/micro/MicroManager.h"
#include "combat/Squad.h"

namespace Monte {
    enum BioState {
        Null = -1,
        Idle,
        Kiting,
        Attacking
    };

    enum StimState {
        Null = -1,
        Unstimmed, // change state to eligibleStim if we have health and enemies are nearby
        EligibleStim, // give marine order to stim, then change state to Stimmed
        Stimming,
        Stimmed, // change state to unstimmed once unit is not buffed anymore
    };

    struct Bio : public GameObject {
        public:
        Bio(sc2::Tag tag_) : GameObject(tag_) { state = BioState::Null; stim = StimState::Null; };
        BioState state;
        StimState stim;
    };
} // end namespace Monte

class BioManager : public MicroManager {
    public:
    BioManager() {};

    // give the GameObject an action based on its and its squad's states
    void doStateAction(Monte::GameObject* unit, Squad::State squadState, sc2::Point2D target);

    // update GameObject state if required
    void validateState(Monte::GameObject* unit, Squad::State squadState, sc2::Point2D target);

    protected:
    void manageStim(Monte::GameObject* unit, Squad::State squadState);
};