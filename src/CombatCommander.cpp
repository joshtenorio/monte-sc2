#include "CombatCommander.h"

void CombatCommander::OnGameStart(){
    bio.emplace_back(sc2::UNIT_TYPEID::TERRAN_MARINE);
    bio.emplace_back(sc2::UNIT_TYPEID::TERRAN_MARAUDER);
    tankTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_SIEGETANK);
    tankTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED);

    reachedEnemyMain = false;
    sm.OnGameStart();
}

void CombatCommander::OnStep(){
        
    // scout manager
    sm.OnStep();

    // handle killing changelings every so often
    if(gInterface->observation->GetGameLoop() % 24 == 0){
        handleChangelings();
    } // end if gameloop % 24 == 0

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

            sc2::Point2D rally = Monte::getPoint2D(natural, Monte::Vector2D(natural, third), 2);
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
            sc2::Point2D targetFlightPoint = Monte::getPoint2D(enemyMineralMidpoint, Monte::Vector2D(enemyMain, enemyMineralMidpoint), 7);
            
            // then get the intermediate flight point
            sc2::Point2D intermediateFlightPoint = sc2::Point2D(unit_->pos.x, targetFlightPoint.y);

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
            tanks.emplace_back(Monte::Tank(unit_->tag));
            // have siege tanks rally at natural in the direction of the enemy main
            sc2::Point2D enemyMain = gInterface->observation->GetGameInfo().enemy_start_locations.front();
            if(
                    gInterface->observation->GetGameInfo().map_name == "Blackburn AIE" &&
                    gInterface->map->getNthExpansion(gInterface->map->numOfExpansions() - 1) != nullptr
                    )
                    enemyMain = gInterface->map->getNthExpansion(gInterface->map->numOfExpansions() - 1)->baseLocation;
            sc2::Point2D natural;
            if(gInterface->map->getNthExpansion(1) != nullptr)
                natural = gInterface->map->getNthExpansion(1)->baseLocation;
            else return;

            sc2::Point2D rally = Monte::getPoint2D(natural, Monte::Vector2D(natural, enemyMain), 3);
            gInterface->actions->UnitCommand(unit_, sc2::ABILITY_ID::ATTACK_ATTACK, rally);
                
        break;
        }
    }
    
}

void CombatCommander::OnUnitDestroyed(const sc2::Unit* unit_){
    sm.OnUnitDestroyed(unit_);
    if(unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SIEGETANK || unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED){
        for(auto itr = tanks.begin(); itr != tanks.end(); ){
            if(unit_->tag == (*itr).tag){
                itr = tanks.erase(itr);
                break;
            }
            else ++itr;
        }
    } // end if unit is tank
}

void CombatCommander::OnUnitDamaged(const sc2::Unit* unit_, float health_, float shields_){
    if(unit_->alliance != sc2::Unit::Alliance::Self) return;

    if(API::isStructure(unit_->unit_type.ToType()) || unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SCV){
        // 1. get a list of 7 closest workers to pull
        sc2::Units workers;
        workers = API::getClosestNUnits(unit_->pos, 7, 9, sc2::Unit::Alliance::Self, sc2::UNIT_TYPEID::TERRAN_SCV);

        // 2. get a list of nearby idle army
        //sc2::Units armyPool = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnits(bio));
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
                gInterface->actions->SendChat("(happy) when it all seems like it's wrong (happy) just sing along to Elton John (happy)");
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
                        break;
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
            } // end if !reachedEnemyMain && m->orders.empty()
            else if(!enemy.empty() && m->orders.empty()){
                const sc2::Unit* closest = nullptr;
                float distance = std::numeric_limits<float>::max();

                for(auto& e : enemy)
                    // prioritise an enemy that is not flying
                    if(sc2::DistanceSquared2D(e->pos, m->pos) < distance && (!e->is_flying)){
                        closest = e;
                        distance = sc2::DistanceSquared2D(e->pos, m->pos);
                    }

                if(closest == nullptr)
                    for(auto& e : enemy)
                        // if we cant find a ground target then just target any visible enemy
                        if(sc2::DistanceSquared2D(e->pos, m->pos) < distance &&
                            (e->cloak == sc2::Unit::CloakState::CloakedDetected || e->cloak == sc2::Unit::CloakState::NotCloaked)){
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

        if(closestMarine != nullptr){
            float distSquaredToMarine = sc2::DistanceSquared2D(closestMarine->pos, med->pos);
            if(distSquaredToMarine > 121){
                // if medivac is somewhat far then we should boost
                gInterface->actions->UnitCommand(med, sc2::ABILITY_ID::EFFECT_MEDIVACIGNITEAFTERBURNERS);
                gInterface->actions->UnitCommand(med, sc2::ABILITY_ID::GENERAL_MOVE, closestMarine->pos, true);
            }
            else if(distSquaredToMarine > 36){
                // if distance to closest marine is > sqrt(36), move medivac to marine's position
            gInterface->actions->UnitCommand(med, sc2::ABILITY_ID::GENERAL_MOVE, closestMarine->pos);
            }

        }
    } // end medivac loop
}

void CombatCommander::siegeTankOnStep(){

    sc2::Units marines = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnits(bio));
    for(auto& st : tanks){
        if(st.state == Monte::TankState::Unsieged){

        }
        else if(st.state == Monte::TankState::Sieged){

        }
        else if(st.state == Monte::TankState::Null){

        }
    } // end siege tank loop
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

void CombatCommander::handleChangelings(){
    // TODO: move this to somehwere where we only do this once
    std::vector<sc2::UNIT_TYPEID> changelings;
    changelings.emplace_back(sc2::UNIT_TYPEID::ZERG_CHANGELINGMARINE);
    changelings.emplace_back(sc2::UNIT_TYPEID::ZERG_CHANGELINGMARINESHIELD);

    sc2::Units friendlyMarines = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_MARINE));
    if(friendlyMarines.empty()) return;

    // TODO: move the "figure out what enemy race is" code into InformationManager initialize
    std::vector<sc2::PlayerInfo> info = gInterface->observation->GetGameInfo().player_info;
    for(auto p : info)
        if(p.race_requested == sc2::Race::Zerg || p.race_actual == sc2::Race::Zerg){ // FIXME: simplify this at some point please
            sc2::Units enemyChangelings = gInterface->observation->GetUnits(sc2::Unit::Alliance::Enemy, sc2::IsUnits(changelings));
            if(enemyChangelings.empty()) return;

            for(auto& c : enemyChangelings){
                const sc2::Unit* closestMarine = friendlyMarines.front();
                for(auto& m : friendlyMarines){
                    if(sc2::DistanceSquared2D(c->pos, m->pos) < sc2::DistanceSquared2D(c->pos, closestMarine->pos))
                        closestMarine = m;
                }
                
                // kill changeling with closest marine
                gInterface->actions->UnitCommand(closestMarine, sc2::ABILITY_ID::ATTACK_ATTACK, c);
            } // end for c : changelings
        } // end if race == zerg
}