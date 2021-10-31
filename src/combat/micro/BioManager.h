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

    protected:
    void doSquadMove(Monte::GameObject* unit, sc2::Point2D target);
    void doSquadAttack(Monte::GameObject* unit, sc2::Point2D target);
    void doSquadDefend(Monte::GameObject* unit, sc2::Point2D target);
    void doSquadNull(Monte::GameObject* unit, sc2::Point2D target);
    void doSquadInit(Monte::GameObject* unit, sc2::Point2D target);

    void validateSquadMove(Monte::GameObject* unit, sc2::Point2D target);
    void validateSquadAttack(Monte::GameObject* unit, sc2::Point2D target);
    void validateSquadDefend(Monte::GameObject* unit, sc2::Point2D target);
    void validateSquadNull(Monte::GameObject* unit, sc2::Point2D target);
    void validateSquadInit(Monte::GameObject* unit, sc2::Point2D target);

    void manageStim(Monte::GameObject* unit, Squad::State squadState);
};