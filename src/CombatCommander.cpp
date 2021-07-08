#include "CombatCommander.h"

void CombatCommander::OnGameStart(){
    bio.emplace_back(sc2::UNIT_TYPEID::TERRAN_MARINE);
    bio.emplace_back(sc2::UNIT_TYPEID::TERRAN_MARAUDER);

    reachedEnemyMain = false;
    sm.OnGameStart();
}

void CombatCommander::OnStep(){
        
    // scout manager
    sm.OnStep();

    sc2::Units marines = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnits(bio));
    for(auto& m : marines){
        manageStim(m);
    }
    
    // handle marines
    marineOnStep();

    // handle medivacs every so often
    if(gInterface->observation->GetGameLoop() % 12 == 0){
       medivacOnStep();
    } // end if gameloop % 12 == 0

    // handle liberators
    liberatorOnStep();

    // if we have a bunker, put marines in it
    sc2::Units bunkers = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_BUNKER));
    if(!bunkers.empty())
        for(auto& b : bunkers){
            // we have space in bunker so pick a marine to go in it
            if(b->cargo_space_taken < b->cargo_space_max && b->build_progress >= 1.0)
                for(auto& m : marines){
                    if(m->orders.empty()){
                        gInterface->actions->UnitCommand(m, sc2::ABILITY_ID::SMART, b);
                        logger.infoInit().withStr("loading").withUnit(m).withStr("in bunker").write();
                        break;
                    }
                }
        } // end for bunkers
} // end OnStep

void CombatCommander::OnUnitCreated(const Unit* unit_){
    if(unit_ == nullptr) return; // if for whatever reason its nullptr, dont do anything

    switch(unit_->unit_type.ToType()){
        case sc2::UNIT_TYPEID::TERRAN_MARAUDER:
        case sc2::UNIT_TYPEID::TERRAN_MARINE:
        case sc2::UNIT_TYPEID::TERRAN_MEDIVAC:{
            // have army units rally at natural in the direction of the enemy main
            sc2::Point2D enemyMain = gInterface->observation->GetGameInfo().enemy_start_locations.front();
            sc2::Point2D natural;
            if(gInterface->map->getNthExpansion(1) != nullptr)
                natural = gInterface->map->getNthExpansion(1)->baseLocation;
            else return;
            float dx = enemyMain.x - natural.x, dy = enemyMain.y - natural.y;
            dx /= sqrt(dx*dx + dy*dy);
            dy /= sqrt(dx*dx + dy*dy);
            dx *= 3;
            dy *= 3;
            sc2::Point2D rally = sc2::Point2D(natural.x + dx, natural.y + dy);
            gInterface->actions->UnitCommand(unit_, sc2::ABILITY_ID::ATTACK_ATTACK, rally);
        break;}
        case sc2::UNIT_TYPEID::TERRAN_LIBERATOR:{ // FIXME: make sure flight points are valid
            // first, generate target flight point
            sc2::Point2D enemyMain = gInterface->observation->GetGameInfo().enemy_start_locations.front();
            sc2::Point3D enemyMineralMidpoint;
            Expansion* e = gInterface->map->getClosestExpansion(sc2::Point3D(enemyMain.x, enemyMain.y, gInterface->observation->GetGameInfo().height));
            if(e == nullptr) return;
            enemyMineralMidpoint = e->mineralMidpoint;
            float dx = enemyMineralMidpoint.x - enemyMain.x, dy = enemyMineralMidpoint.y - enemyMain.y;
            dx /= sqrt(dx*dx + dy*dy);
            dy /= sqrt(dx*dx + dy*dy);
            dx *= 7;
            dy *= 7;
            sc2::Point2D targetFlightPoint = sc2::Point2D(enemyMineralMidpoint.x + dx, enemyMineralMidpoint.y + dy);
            
            // then get the intermediate flight point
            sc2::Point2D intermediateFlightPoint = sc2::Point2D(enemyMineralMidpoint.x + dx, unit_->pos.y);

            // validate the flight points (ie make sure they are within map bounds)
            // if they are not valid, adjust them so they fit within map bounds

            // give liberator commands
            gInterface->actions->UnitCommand(unit_, sc2::ABILITY_ID::MOVE_MOVE, intermediateFlightPoint);
            gInterface->actions->UnitCommand(unit_, sc2::ABILITY_ID::MOVE_MOVE, targetFlightPoint, true);
            gInterface->actions->UnitCommand(unit_, sc2::ABILITY_ID::MORPH_LIBERATORAGMODE, enemyMineralMidpoint, true);
        break;
        }
    }
    
}

void CombatCommander::OnUnitDestroyed(const sc2::Unit* unit_){
    sm.OnUnitDestroyed(unit_);
}

void CombatCommander::OnUnitDamaged(const sc2::Unit* unit_, float health_, float shields_){
    if(unit_->alliance != sc2::Unit::Alliance::Self) return;

    if(API::isStructure(unit_->unit_type.ToType()) || unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SCV){
        // 1. get a list of n closest workers to pull
        sc2::Units workers = API::getClosestNUnits(unit_->pos, 11, 12, sc2::Unit::Alliance::Self, sc2::UNIT_TYPEID::TERRAN_SCV);

        // 2. get a list of idle army
        sc2::Units armyPool = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnits(bio));
        sc2::Units idleArmy;
        for(auto& a : armyPool)
            if(a->orders.empty()) idleArmy.emplace_back(a);

        // 3. get closest enemy units to unit_ and find their center
        sc2::Units enemies = API::getClosestNUnits(unit_->pos, 11, 12, sc2::Unit::Alliance::Enemy);
        float x = 0.0, y = 0.0;
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
    sm.OnUnitEnterVision(unit_);
}

void CombatCommander::marineOnStep(){
    int numPerWave = 8 + API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) * 4;

    if(API::countIdleUnits(sc2::UNIT_TYPEID::TERRAN_MARINE) + API::countIdleUnits(sc2::UNIT_TYPEID::TERRAN_MARAUDER) >= numPerWave){
        sc2::Units marines = gInterface->observation->GetUnits(Unit::Alliance::Self, IsUnits(bio));
        sc2::Units enemy = gInterface->observation->GetUnits(Unit::Alliance::Enemy);
        //std::cout << "sending a wave of marines\n";
        for(const auto& m : marines){
            if(25 > sc2::DistanceSquared2D(gInterface->observation->GetGameInfo().enemy_start_locations.front(), m->pos) && !reachedEnemyMain){
                reachedEnemyMain = true;
                gInterface->actions->SendChat("Tag: reachedEnemyMain");
            }
            
            // attack closest enemy expansion
            if(!reachedEnemyMain && m->orders.empty()){
                // TODO: in mapper make a "closest <owner> expo" function
                Expansion* closestEnemyExpo = nullptr;
                for(int n = 0; n < gInterface->map->numOfExpansions(); n++){
                    // check if it is an enemy expansion and we are not at that base
                    if(
                        gInterface->map->getNthExpansion(n)->ownership == OWNER_ENEMY &&
                        sc2::DistanceSquared2D(m->pos, gInterface->map->getNthExpansion(n)->baseLocation) > 25)
                        {
                        closestEnemyExpo = gInterface->map->getNthExpansion(n);
                    }
                } // end for expansions
                if(closestEnemyExpo != nullptr){
                    gInterface->actions->UnitCommand(
                            m,
                            ABILITY_ID::ATTACK_ATTACK,
                            closestEnemyExpo->baseLocation);
                }
                else{
                    gInterface->actions->UnitCommand(
                        m,
                        ABILITY_ID::ATTACK_ATTACK,
                        gInterface->observation->GetGameInfo().enemy_start_locations.front());
                }
            }
            else if(!enemy.empty() && m->orders.empty()){
                const sc2::Unit* closest = nullptr;
                float distance = 9000;
                for(auto& e : enemy)
                    if(sc2::DistanceSquared2D(e->pos, m->pos) < distance && !e->is_flying){
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
    } // end if idle bio > 12
}

void CombatCommander::medivacOnStep(){
    sc2::Units medivacs = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_MEDIVAC));
    for(auto& med : medivacs){
        // move each medivac to the closest productive marine
        sc2::Units marines = gInterface->observation->GetUnits(Unit::Alliance::Self, IsUnits(bio));
        const sc2::Unit* closestMarine = nullptr;
        float d = 10000;
        for(auto& ma : marines){
            if(ma != nullptr)
                if(d > sc2::DistanceSquared2D(ma->pos, med->pos) && !ma->orders.empty()){
                    closestMarine = ma;
                    d = sc2::DistanceSquared2D(ma->pos, med->pos);
                }
        } // end marine loop
        if(closestMarine == nullptr) // if no marines are productive, then pick the closest marine in general
            for(auto& ma : marines){
                if(ma != nullptr)
                    if(d > sc2::DistanceSquared2D(ma->pos, med->pos)){
                        closestMarine = ma;
                        d = sc2::DistanceSquared2D(ma->pos, med->pos);
            }
        } // end marine loop

        if(d > 13 && closestMarine != nullptr){
            // if distance to closest marine is > sqrt(13), move medivac to marine's position
            gInterface->actions->UnitCommand(med, sc2::ABILITY_ID::GENERAL_MOVE, closestMarine->pos);
        }
    } // end medivac loop
}

void CombatCommander::liberatorOnStep(){

}


void CombatCommander::manageStim(const sc2::Unit* unit){
    
    if(unit == nullptr) return;
    std::vector<sc2::BuffID> buffs = unit->buffs;
    for(auto& b : buffs)
        if(b.ToType() == sc2::BUFF_ID::STIMPACK || b.ToType() == sc2::BUFF_ID::STIMPACKMARAUDER){
            return;
        }
    
    // if we have a decent amount of health left and there are enemies nearby
    if(
        unit->health/unit->health_max >= 0.6 &&
        !API::getClosestNUnits(unit->pos, 5, 8, sc2::Unit::Alliance::Enemy).empty()){

        switch(unit->unit_type.ToType()){
            case sc2::UNIT_TYPEID::TERRAN_MARINE:
                gInterface->actions->UnitCommand(unit, sc2::ABILITY_ID::EFFECT_STIM_MARINE);
                break;
            case sc2::UNIT_TYPEID::TERRAN_MARAUDER:
                gInterface->actions->UnitCommand(unit, sc2::ABILITY_ID::EFFECT_STIM_MARAUDER);
                break;
            default:
                return;
        }
    }
}