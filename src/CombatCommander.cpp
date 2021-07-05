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
    //if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARINE) + API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARAUDER) > 10){
    if(API::countIdleUnits(sc2::UNIT_TYPEID::TERRAN_MARINE) + API::countIdleUnits(sc2::UNIT_TYPEID::TERRAN_MARAUDER) >= 12){

        sc2::Units marines = gInterface->observation->GetUnits(Unit::Alliance::Self, IsUnits(bio));
        sc2::Units enemy = gInterface->observation->GetUnits(Unit::Alliance::Enemy);
        //std::cout << "sending a wave of marines\n";
        for(const auto& m : marines){
            if(25 > sc2::DistanceSquared2D(gInterface->observation->GetGameInfo().enemy_start_locations.front(), m->pos) && !reachedEnemyMain){
                reachedEnemyMain = true;
                gInterface->actions->SendChat("Tag: reachedEnemyMain");
            }
                
            
            if(!reachedEnemyMain && m->orders.empty())
                gInterface->actions->UnitCommand(
                        m,
                        ABILITY_ID::ATTACK_ATTACK,
                        gInterface->observation->GetGameInfo().enemy_start_locations.front());
            else if(!enemy.empty() && m->orders.empty()){
                // TODO: instead of getting the first enemy in enemy, we should get the closest enemy to m
                sc2::Unit* closest = nullptr;
                float distance = 9000;
                for(auto& e : enemy)
                    if(sc2::DistanceSquared2D(e->pos, m->pos) < distance){
                        closest = e;
                        distance = sc2::DistanceSquared2D(e->pos, m->pos);
                    }
                    if(closest != nullptr)
                        gInterface->actions->UnitCommand(
                            m,
                            sc2::ABILITY_ID::ATTACK_ATTACK,
                            closest->pos);
            } // end else if
                
                
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
        sc2::Units workers = API::getClosestNUnits(unit_->pos, 11, 12, sc2::Unit::Alliance::Self, sc2::UNIT_TYPEID::TERRAN_SCV);
        printf("workers nearby: %d\t", workers.size());
        // 2. get a list of idle army
        sc2::Units armyPool = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnits(bio));
        sc2::Units idleArmy;
        for(auto& a : armyPool)
            if(a->orders.empty()) idleArmy.emplace_back(a);
        printf("idle army: %d\n", idleArmy.size());
        // 3. get closest enemy units to unit_ and find their center
        sc2::Units enemies = API::getClosestNUnits(unit_->pos, 11, 12, sc2::Unit::Alliance::Enemy);
        float x, y = 0.0;
        float numEnemies = (float) enemies.size();
        for(auto& e : enemies){
            x += e->pos.x;
            y += e->pos.y;
        }
        sc2::Point2D enemyCenter = sc2::Point2D(x/numEnemies, y/numEnemies);

        // 4. have workers and idle army attack them
        gInterface->actions->UnitCommand(workers, sc2::ABILITY_ID::ATTACK_ATTACK, enemyCenter);
        gInterface->actions->UnitCommand(idleArmy, sc2::ABILITY_ID::ATTACK_ATTACK, enemyCenter);
    }
}

void CombatCommander::OnUnitEnterVision(const sc2::Unit* unit_){

}
