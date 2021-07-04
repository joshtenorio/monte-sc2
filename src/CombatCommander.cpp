#include "CombatCommander.h"


// used for marine control
bool reachedEnemyMain = false;
std::vector<sc2::UNIT_TYPEID> bio;


void CombatCommander::OnGameStart(){
    bio.emplace_back(sc2::UNIT_TYPEID::TERRAN_MARINE);
    bio.emplace_back(sc2::UNIT_TYPEID::TERRAN_MARAUDER);
}


void CombatCommander::OnStep(){

    // scout manager
    sm.OnStep();
    
    // handle marines
    if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARINE) + API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARAUDER) > 10){
    //if(API::countIdleUnits(sc2::UNIT_TYPEID::TERRAN_MARINE) >= 15){

        sc2::Units marines = gInterface->observation->GetUnits(Unit::Alliance::Self, IsUnits(bio));
        sc2::Units enemy = gInterface->observation->GetUnits(Unit::Alliance::Enemy);
        //std::cout << "sending a wave of marines\n";
        for(const auto& m : marines){
            if(25 > sc2::DistanceSquared2D(gInterface->observation->GetGameInfo().enemy_start_locations.front(), m->pos) && !reachedEnemyMain){
                reachedEnemyMain = true;
                gInterface->actions->SendChat("Tag: reachedEnemyMain");
            }
                
            
            if(!reachedEnemyMain)
                gInterface->actions->UnitCommand(
                        m,
                        ABILITY_ID::ATTACK_ATTACK,
                        gInterface->observation->GetGameInfo().enemy_start_locations.front());
            else if(!enemy.empty() && m->orders.empty()){
                // TODO: instead of getting the first enemy in enemy, we should get the closest enemy to m
                gInterface->actions->UnitCommand(m, sc2::ABILITY_ID::ATTACK_ATTACK, enemy.front()->pos);
            }
                
                
        } // end for loop
    } // end if marine count > 10

    // handle medivacs every so often
    if(gInterface->observation->GetGameLoop() % 12 == 0){
        sc2::Units medivacs = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_MEDIVAC));
        for(auto& med : medivacs){
            // move each medivac to the closest marine
            sc2::Units marines = gInterface->observation->GetUnits(Unit::Alliance::Self, IsUnits(bio));
            const sc2::Unit* closestMarine = nullptr;
            float d = 10000;
            for(auto& ma : marines){
                if(ma != nullptr)
                    if(d > sc2::DistanceSquared2D(ma->pos, med->pos)){
                        closestMarine = ma;
                        d = sc2::DistanceSquared2D(ma->pos, med->pos);
                    }
            } // end marine loop
            if(d > 9 && closestMarine != nullptr){
                // if distance to closest marine is > 5, move medivac to marine's position
                gInterface->actions->UnitCommand(med, sc2::ABILITY_ID::GENERAL_MOVE, closestMarine->pos);
            }
        } // end medivac loop
    } // end if gameloop % 100 == 0
}

void CombatCommander::OnUnitDestroyed(const sc2::Unit* unit_){
    sm.OnUnitDestroyed(unit_);
}

void CombatCommander::OnUnitDamaged(const sc2::Unit* unit_, float health_, float shields_){
    if(API::isStructure(unit_->unit_type.ToType())){
        // 1. get a list of n closest workers to pull
        // 2. get a list of idle army
        // 3. get closest enemy units to unit_
        // 4. have workers and idle army attack them
    }
}

void CombatCommander::OnUnitEnterVision(const sc2::Unit* unit_){

}
