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

    // handle medivacs and siege tanks every so often
    if(gInterface->observation->GetGameLoop() % 12 == 0){
        medivacOnStep();
        siegeTankOnStep();
    } // end if gameloop % 12 == 0

    

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
            // have army units rally at natural in the direction of the third expo
            sc2::Point2D third;
            sc2::Point2D natural;
            if(gInterface->map->getNthExpansion(1) != nullptr)
                natural = gInterface->map->getNthExpansion(1)->baseLocation;
            else return;
            if(gInterface->map->getNthExpansion(2) != nullptr)
                third = gInterface->map->getNthExpansion(2)->baseLocation;
            else return;
            float dx = third.x - natural.x, dy = third.y - natural.y;
            dx /= sqrt(dx*dx + dy*dy);
            dy /= sqrt(dx*dx + dy*dy);
            dx *= 2;
            dy *= 2;
            sc2::Point2D rally = sc2::Point2D(natural.x + dx, natural.y + dy);
            gInterface->actions->UnitCommand(unit_, sc2::ABILITY_ID::ATTACK_ATTACK, rally);
        break;
        }
        case sc2::UNIT_TYPEID::TERRAN_LIBERATOR:{
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
            sc2::Point2D maxPoint = gInterface->observation->GetGameInfo().playable_max;
            sc2::Point2D minPoint = gInterface->observation->GetGameInfo().playable_min;
            int minAdjust = 1;
            // TODO: is there a cleaner way to write this? keep the braces in case we need to print stuff here
            if(targetFlightPoint.x >= maxPoint.x){
                targetFlightPoint.x -= (targetFlightPoint.x - maxPoint.x - minAdjust);
            }
            else if(targetFlightPoint.x <= minPoint.x){
                targetFlightPoint.x += (minPoint.x - targetFlightPoint.x + minAdjust);
            }
            if(targetFlightPoint.y >= maxPoint.y){
                targetFlightPoint.y -= (targetFlightPoint.y - maxPoint.y - minAdjust);
            }
            else if(targetFlightPoint.y <= minPoint.y){
                targetFlightPoint.y += (minPoint.y - targetFlightPoint.y + minAdjust);
            }

            if(intermediateFlightPoint.x >= maxPoint.x){
                intermediateFlightPoint.x -= (intermediateFlightPoint.x - maxPoint.x - minAdjust);
            }
            else if(intermediateFlightPoint.x <= minPoint.x){
                intermediateFlightPoint.x += (minPoint.x - intermediateFlightPoint.x + minAdjust);
            }
            if(intermediateFlightPoint.y >= maxPoint.y){
                intermediateFlightPoint.y -= (intermediateFlightPoint.y - maxPoint.y - minAdjust);
            }
            else if(intermediateFlightPoint.y <= minPoint.y){
                intermediateFlightPoint.y += (minPoint.y - intermediateFlightPoint.y + minAdjust);
            }

            // give liberator commands
            gInterface->actions->UnitCommand(unit_, sc2::ABILITY_ID::MOVE_MOVE, intermediateFlightPoint);
            gInterface->actions->UnitCommand(unit_, sc2::ABILITY_ID::MOVE_MOVE, targetFlightPoint, true);
            gInterface->actions->UnitCommand(unit_, sc2::ABILITY_ID::MORPH_LIBERATORAGMODE, enemyMineralMidpoint, true);
        break;
        }
        case sc2::UNIT_TYPEID::TERRAN_SIEGETANK:{
            // have siege tanks rally at natural in the direction of the enemy main
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

            // tanks should siege only if we have less than 5 sieged up
            sc2::Units siegedTanks = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED));
            if(siegedTanks.size() < 5){
                gInterface->actions->UnitCommand(unit_, sc2::ABILITY_ID::MORPH_SIEGEMODE, true);
                defensiveTanks.emplace_back(unit_->tag);
            }
                
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

        // 2. get a list of nearby idle army
        //sc2::Units armyPool = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnits(bio));
        // TODO: for getClosestNUnits, add an overload where we can get multiple unit types
        sc2::Units armyPool = API::getClosestNUnits(unit_->pos, 50, 36, sc2::Unit::Alliance::Self);
        sc2::Units idleArmy;
        for(auto& a : armyPool)
            if(a->orders.empty() && !API::isStructure(a->unit_type.ToType())) idleArmy.emplace_back(a);

        // 3. get closest enemy units to unit_ and find their center
        sc2::Units enemies = API::getClosestNUnits(unit_->pos, 11, 12, sc2::Unit::Alliance::Enemy);
        float x = 0.0, y = 0.0;
        float numEnemies = (float) enemies.size();
        for(auto& e : enemies){
            x += e->pos.x;
            y += e->pos.y;
        }
        sc2::Point2D enemyCenter = sc2::Point2D(x/numEnemies, y/numEnemies);

        // 4. have idle army attack, and have some workers repair and some workers attack
        gInterface->actions->UnitCommand(idleArmy, sc2::ABILITY_ID::ATTACK_ATTACK, enemyCenter);
        if(API::isStructure(unit_->unit_type.ToType()))
            for(int n = 0; n < workers.size(); n++){
                if(n % 3 == 0){
                    gInterface->actions->UnitCommand(workers[n], sc2::ABILITY_ID::EFFECT_REPAIR, unit_);
                }
                else
                    gInterface->actions->UnitCommand(workers[n], sc2::ABILITY_ID::ATTACK_ATTACK, enemyCenter);
            }
        else
            gInterface->actions->UnitCommand(workers, sc2::ABILITY_ID::ATTACK_ATTACK, enemyCenter);
    }
}

void CombatCommander::OnUnitEnterVision(const sc2::Unit* unit_){
    sm.OnUnitEnterVision(unit_);
}

void CombatCommander::marineOnStep(){
    int numPerWave = 8 + API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) * 4;

    if(API::countIdleUnits(sc2::UNIT_TYPEID::TERRAN_MARINE) + API::countIdleUnits(sc2::UNIT_TYPEID::TERRAN_MARAUDER) >= numPerWave || gInterface->observation->GetFoodUsed() >= 200){
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
                float distance = std::numeric_limits<float>::max();
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
    } // end if idle bio > wave amount
}

void CombatCommander::medivacOnStep(){
    sc2::Units medivacs = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_MEDIVAC));
    for(auto& med : medivacs){
        // move each medivac to the marine that is closest to the enemy main
        sc2::Units marines = gInterface->observation->GetUnits(Unit::Alliance::Self, IsUnits(bio));
        sc2::Point2D enemyMain = gInterface->observation->GetGameInfo().enemy_start_locations.front();
        const sc2::Unit* closestMarine = nullptr;
        float d = std::numeric_limits<float>::max();
        for(auto& ma : marines){
            if(ma != nullptr)
                if(d > sc2::DistanceSquared2D(ma->pos, enemyMain) && !ma->orders.empty()){
                    closestMarine = ma;
                    d = sc2::DistanceSquared2D(ma->pos, enemyMain);
                }
        } // end marine loop
        if(closestMarine == nullptr) // if no marines are productive, then pick the closest marine in general
            for(auto& ma : marines){
                if(ma != nullptr)
                    if(d > sc2::DistanceSquared2D(ma->pos, enemyMain)){
                        closestMarine = ma;
                        d = sc2::DistanceSquared2D(ma->pos, enemyMain);
            }
        } // end marine loop
        gInterface->debug->debugSphereOut(med->pos, sqrtf(13));
        gInterface->debug->sendDebug();
        if(d > 52 && closestMarine != nullptr){
            // if distance is > sqrt(52) then boost medivac
            gInterface->actions->UnitCommand(med, sc2::ABILITY_ID::EFFECT_MEDIVACIGNITEAFTERBURNERS);
            gInterface->actions->UnitCommand(med, sc2::ABILITY_ID::GENERAL_MOVE, closestMarine->pos, true);
        }
        else if(d > 13 && closestMarine != nullptr){
            // if distance to closest marine is > sqrt(13), move medivac to marine's position
            gInterface->actions->UnitCommand(med, sc2::ABILITY_ID::GENERAL_MOVE, closestMarine->pos);
        }
    } // end medivac loop
}

void CombatCommander::siegeTankOnStep(){
    sc2::Units siegeTanks = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_SIEGETANK));

    // unsieged tanks should follow the marine that is closest to the enemy main
    sc2::Point2D enemyMain = gInterface->observation->GetGameInfo().enemy_start_locations.front();
    for(auto& st : siegeTanks){
        // if tank is assigned to defend natural, ignore it
        bool defensive = false;
        for(auto& tag : defensiveTanks){
            if(tag == st->tag){
                defensive = true;
                break;
            }
        }
        if(defensive) continue;

        sc2::Units marines = gInterface->observation->GetUnits(Unit::Alliance::Self, IsUnits(bio));
        const sc2::Unit* closestMarine = nullptr;
        float d = std::numeric_limits<float>::max();
        for(auto& ma : marines){
            if(ma != nullptr)
                if(d > sc2::DistanceSquared2D(ma->pos, enemyMain) && !ma->orders.empty()){
                    closestMarine = ma;
                    d = sc2::DistanceSquared2D(ma->pos, enemyMain);
                }
        } // end marine loop

        if(d > 13 && closestMarine != nullptr){
            // if distance to closest marine is > sqrt(13), move tank to marine's position
            gInterface->actions->UnitCommand(st, sc2::ABILITY_ID::ATTACK_ATTACK, closestMarine->pos);
        }
    }
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