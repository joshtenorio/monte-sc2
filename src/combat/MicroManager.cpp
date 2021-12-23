#include "MicroManager.h"

MicroManager::MicroManager(){
    logger = Logger("MicroManager");
}

void MicroManager::execute(SquadOrder& order, Monte::InfluenceMap& gmap, Monte::InfluenceMap& amap){

    if(gInterface->observation->GetGameLoop() % 4 == 0){

        medivacOnStep(order);
        siegeTankOnStep(order);
        reaperOnStep(gmap);
    }

    marineOnStep(order);
    liberatorOnStep(order);
}

void MicroManager::regroup(sc2::Point2D target){
    // brrrr
}

void MicroManager::initialize(){
    bioTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_MARINE);
    bioTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_MARAUDER);
    tankTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_SIEGETANK);
    tankTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED);

    timeToCleanup = false;

}

void MicroManager::addUnit(sc2::Tag tag){
    const sc2::Unit* unit = GameObject::getUnit(tag);
    if(!unit) return;
    switch(unit->unit_type.ToType()){
        case sc2::UNIT_TYPEID::TERRAN_SIEGETANK:
        case sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
            tanks.emplace_back(Tank(tag));
        break;
        case sc2::UNIT_TYPEID::TERRAN_REAPER:
            reapers.emplace_back(Reaper(tag));
        break;
        case sc2::UNIT_TYPEID::TERRAN_LIBERATOR:
        case sc2::UNIT_TYPEID::TERRAN_LIBERATORAG:
            liberators.emplace_back(Liberator(tag));
        break;
        default:
            bio.emplace_back(GameObject(tag));
        break;
    }
}

bool MicroManager::removeUnit(sc2::Tag tag){
    for(auto itr = bio.begin(); itr != bio.end(); ){
        if((*itr).getTag() == tag){
            itr = bio.erase(itr);
            return true;
        }
        else ++itr;
    }
    for(auto itr = tanks.begin(); itr != tanks.end(); ){
        if((*itr).getTag() == tag){
            itr = tanks.erase(itr);
            return true;
        }
        else ++itr;
    }
    for(auto itr = liberators.begin(); itr != liberators.end(); ){
        if((*itr).getTag() == tag){
            itr = liberators.erase(itr);
            return true;
        }
        else ++itr;
    }
    for(auto itr = reapers.begin(); itr != reapers.end(); ){
        if((*itr).getTag() == tag){
            itr = reapers.erase(itr);
            return true;
        }
        else ++itr;
    }

    return false;
}

void MicroManager::marineOnStep(SquadOrder& order){

    // TODO: use bio instead of this
    sc2::Units marines = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnits(bioTypes));
    for(const auto& m : marines)
        if(49 > sc2::DistanceSquared2D(gInterface->observation->GetGameInfo().enemy_start_locations.front(), m->pos) && !timeToCleanup){
                timeToCleanup = true;
                gInterface->actions->SendChat("Tag: reachedEnemyMain");
                gInterface->actions->SendChat("(happy) when it all seems like it's wrong (happy) just sing along to Elton John (happy)");
        }

    if(order.type == SquadOrderType::Attack && !timeToCleanup){
        gInterface->actions->UnitCommand(marines, sc2::ABILITY_ID::ATTACK_ATTACK, order.target);
        
    } // end if order.type == attack
    else if(order.type == SquadOrderType::Cleanup || timeToCleanup){
        sc2::Units enemy = gInterface->observation->GetUnits(sc2::Unit::Alliance::Enemy);
        for(const auto& m : marines){
            if(!enemy.empty() && m->orders.empty()){
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
    }
    else if(order.type == SquadOrderType::Defend) {
        for(auto& m : marines)
            if(sc2::DistanceSquared2D(m->pos, order.target) > 36)
                gInterface->actions->UnitCommand(m, sc2::ABILITY_ID::ATTACK_ATTACK, order.target);
    }
}

void MicroManager::medivacOnStep(SquadOrder& order){
    sc2::Units medivacs = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_MEDIVAC));
    for(auto& med : medivacs){
        // move each medivac to the marine that is closest to the enemy main
        sc2::Units marines = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnits(bioTypes));
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

void MicroManager::siegeTankOnStep(SquadOrder& order){

    sc2::Units marines = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnits(bioTypes));
    sc2::Point2D enemyMain = gInterface->observation->GetGameInfo().enemy_start_locations.front();

    for(auto& st : tanks){
        const sc2::Unit* t = st.getUnit();
        if(t == nullptr) continue;

        sc2::Units closestEnemies = API::getClosestNUnits(t->pos, 100, 16, sc2::Unit::Alliance::Enemy,
                [](const sc2::Unit& u){
                return !u.is_flying;
                });
        sc2::Units nearbyTanks = API::getClosestNUnits(t->pos, 99, 10, sc2::Unit::Alliance::Self, sc2::IsUnits(tankTypes));
        sc2::Units localSupport = API::getClosestNUnits(t->pos, 99, 16, sc2::Unit::Alliance::Self, sc2::IsUnits(bioTypes));
        bool morph = false;

        switch(st.state){
            case TankState::Unsieged:
                morph = false;
                switch(order.type){
                    case SquadOrderType::Attack:
                    case SquadOrderType::Cleanup:
                        // first check if we are in range of an enemy structure r=13
                        for(auto& e : closestEnemies){
                            if(e->is_building && sc2::Distance2D(t->pos, e->pos) <= 13){
                                morph = true;
                                break;
                            }
                            else if(!e->is_building){
                                morph = true;
                                break;
                            }
                        }
                        if(morph){
                            // morph only if we have local support 
                            // probably needs tuning
                            if(localSupport.empty() && nearbyTanks.empty()) continue;
                            st.state = TankState::Sieging;
                        }
                        else{
                            // no enemies nearby, so follow marine closest to enemy main
                            const sc2::Unit* closestMarine = nullptr;
                            float d = std::numeric_limits<float>::max();
                            for(auto& ma : marines){
                                if(ma == nullptr) continue;
                                if(d > sc2::DistanceSquared2D(ma->pos, enemyMain)){
                                    closestMarine = ma;
                                    d = sc2::DistanceSquared2D(ma->pos, enemyMain);
                                }
                            } // end marine loop
                            if(closestMarine != nullptr){
                                float distToMarineSquared = sc2::DistanceSquared2D(closestMarine->pos, t->pos);
                                if(distToMarineSquared > 36)
                                    gInterface->actions->UnitCommand(t, sc2::ABILITY_ID::ATTACK_ATTACK, closestMarine->pos);
                            }
                        }
                    break;
                    case SquadOrderType::Defend:
                        if(sc2::Distance2D(st.getPos(), order.target) > order.radius/4){
                            st.attack(order.target);
                        }
                        else{
                            st.state = TankState::Sieging;
                        }
                    break;
                }

            break;
            case TankState::Sieged:
                morph = true;
            // stay sieged if:
            //  structure within r=13
            //  any unit within r=16
            //  squad is currently defending and we are within the order radius
            if(order.type == SquadOrderType::Defend){
                if(sc2::Distance2D(st.getPos(), order.target) > order.radius/4){
                    st.state = TankState::Unsieging;
                }
                else{
                    break;
                }
            }
            for(auto& e : closestEnemies){
                if(e->is_building && sc2::Distance2D(t->pos, e->pos) <= 13){
                    morph = false;
                    break;
                }
                else if(!e->is_building){
                    morph = false;
                    break;
                }
            }
            
            // unsiege if:
            //  no other nearby tank is unsieging (ie only one tank in a group can unsiege at a time)
                for(auto& mt : tanks){
                    const sc2::Unit* otherTank = mt.getUnit();
                    if(otherTank == nullptr) continue;
                    
                    // a nearby tank is unsieging, so don't unsiege
                    if(mt.state == TankState::Unsieging && DistanceSquared2D(t->pos, otherTank->pos) < 10){
                        morph = false;
                    }
                }
                if(morph)
                    st.state = TankState::Unsieging;
            break;
            case TankState::Sieging:
                gInterface->actions->UnitCommand(t, sc2::ABILITY_ID::MORPH_SIEGEMODE);
                if(t->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED)
                    st.state = TankState::Sieged;
            break;
            case TankState::Unsieging:
                gInterface->actions->UnitCommand(t, sc2::ABILITY_ID::MORPH_UNSIEGE);
                if(t->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SIEGETANK)
                    st.state = TankState::Unsieged;
            break;
            case TankState::Null:
                if(t->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SIEGETANK)
                    st.state = TankState::Unsieged;
                else if(t->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED)
                    st.state = TankState::Sieged;
            break;
            default:
            break;
        }

    } // end siege tank loop
}

void MicroManager::reaperOnStep(Monte::InfluenceMap& groundMap){
    sc2::Point2D enemyMain = gInterface->observation->GetGameInfo().enemy_start_locations.front();
    // generate influence map
    for(auto& reaper : reapers){
        const sc2::Unit* r = reaper.getUnit();
        if(r == nullptr) continue;

        // r = 11, which is slightly higher than reaper's vision (9) in case there are nearby friendlies
        // that give more vision of surrounding location
        sc2::Units allLocalEnemy = API::getClosestNUnits(r->pos, 99, 9, sc2::Unit::Alliance::Enemy);
        sc2::Units localEnemies;
        sc2::Units localEnemyWorkers;
        const sc2::Unit* target = nullptr;
        float targetHP = std::numeric_limits<float>::max();
        for(auto& e : allLocalEnemy){
            if(!e->is_building) localEnemies.emplace_back(e);
            if(API::isWorker(e->unit_type.ToType())) localEnemyWorkers.emplace_back(e);
        }

        switch(reaper.state){
            case ReaperState::Init:
                reaper.state = ReaperState::Move;
                reaper.targetLocation = enemyMain;
            break;
            case ReaperState::Move:
                if(reaper.targetLocation.x == -1){
                    logger.errorInit().withUnit(r).withStr("has invalid target location").write();
                    reaper.state = ReaperState::Null; // invalid target location
                    continue;
                }
                // do state action
                gInterface->actions->UnitCommand(r, sc2::ABILITY_ID::MOVE_MOVE, reaper.targetLocation);
                // validate state
                if(sc2::Distance2D(r->pos, reaper.targetLocation) <= 11){
                    reaper.state = ReaperState::Attack;
                }
                else{
                    for(auto& e : localEnemies){
                        if(e->is_building) continue;
                        reaper.state = ReaperState::Attack;
                        break;
                    }
                }

            break;
            case ReaperState::Attack:{
                // do state action

                int attackCase = 0; // for debug, mostly
                // this means there are no enemy combatants (besides workers) so
                // this is case 1
                if(localEnemies.size() == localEnemyWorkers.size() && !localEnemies.empty()){
                    // TODO: find lowest hp worker in weapon range, else just get closest worker
                    attackCase = 1;
                    target = nullptr; // TODO this is probably redundant lol
                    for(auto& e : localEnemyWorkers){
                        if(e->health < targetHP && sc2::DistanceSquared2D(e->pos, r->pos) <= 25){
                            target = e;
                            targetHP = e->health;
                        }
                    }
                    if(target == nullptr){ // all the workers have same hp so just get closest
                        target = localEnemyWorkers.front();
                        for(auto& e : localEnemyWorkers){
                            if(sc2::DistanceSquared2D(e->pos, r->pos) < sc2::DistanceSquared2D(target->pos, r->pos))
                                target = e;
                        }
                    }
                    gInterface->actions->UnitCommand(r, sc2::ABILITY_ID::ATTACK, target);
                } // end case 1
                else{
                    // either one of case 2 or 3
                    // if we can find an enemy combatant in weapon range, it is case 3
                    // else it is case 2
                    for(auto& e : localEnemies){
                        if(sc2::DistanceSquared2D(e->pos, r->pos) <= 25 && !API::isWorker(e->unit_type.ToType())){
                            // case 3
                            // prioritise lowest hp enemy in weapon range
                            for(auto& e : localEnemies){
                                if(!API::isWorker(e->unit_type.ToType()) && sc2::DistanceSquared2D(e->pos, r->pos) < 25 && e->health < targetHP){
                                    target = e;
                                    targetHP = e->health;
                                }
                            }
                            // else just closest enemy in weapon range
                            if(target == nullptr)
                                target = localEnemies.front();
                                for(auto& e : localEnemies){
                                    if(!API::isWorker(e->unit_type.ToType()) && sc2::DistanceSquared2D(e->pos, r->pos) < 25)
                                        target = e;
                                }
                            if(target != nullptr)
                                gInterface->actions->UnitCommand(r, sc2::ABILITY_ID::ATTACK, target);
                            attackCase = 3;
                            break;
                        }
                    }
                    if(attackCase != 3){
                        attackCase = 2;
                        // prioritise lowest health worker in weapon range
                        for(auto& e : localEnemyWorkers){
                            if(e->health < targetHP && sc2::DistanceSquared2D(e->pos, r->pos) <= 25){
                                target = e;
                                targetHP = e->health;
                            }
                        }
                        // else, closest worker in weapon range
                        if(target == nullptr && !localEnemyWorkers.empty()){
                            target = localEnemyWorkers.front();
                            for(auto& e : localEnemyWorkers){
                                if(sc2::DistanceSquared2D(e->pos, r->pos) <= sc2::DistanceSquared2D(target->pos, r->pos))
                                    target = e;
                            }
                        }
                        // else, just get closest enemy
                        else if(target == nullptr && !localEnemies.empty()){
                            target = localEnemies.front();
                            for(auto& e : localEnemies){
                                if(sc2::DistanceSquared2D(e->pos, r->pos) <= sc2::DistanceSquared2D(target->pos, r->pos))
                                    target = e;
                            }
                        }
                        if(target != nullptr)
                            gInterface->actions->UnitCommand(r, sc2::ABILITY_ID::ATTACK, target);
                    } // end case 2

                } // end case 2/3
                
                // validate state
                if(r->health <= 20){ // bide if we are below 1/3 hp
                    sc2::Point2D centerMap = sc2::Point2D(gInterface->observation->GetGameInfo().playable_max.x/2, gInterface->observation->GetGameInfo().playable_max.y/2);
                    reaper.targetLocation = centerMap; // bide at center of map
                    reaper.state = ReaperState::Bide;
                }
                else if(r->weapon_cooldown){
                    reaper.state = ReaperState::Kite;
                }
                else if(localEnemies.empty()){
                    reaper.targetLocation = enemyMain;
                    reaper.state = ReaperState::Move;
                }
            break;}
            case ReaperState::Kite:
                // do state action
                // skip doing action if no enemies are nearby
                if(!localEnemies.empty()){
                    gInterface->actions->UnitCommand(r, sc2::ABILITY_ID::MOVE_MOVE, groundMap.getSafeWaypoint(r->pos));
                }

                // validate state
                if(r->health <= 20){ // bide if we are below 1/3 hp
                    reaper.targetLocation = gInterface->map->getNthExpansion(3)->baseLocation;
                    reaper.state = ReaperState::Bide;
                }
                else if(!r->weapon_cooldown){
                    reaper.state = ReaperState::Attack;
                }
            break;
            case ReaperState::Bide:
                // do state action
                gInterface->actions->UnitCommand(r, sc2::ABILITY_ID::MOVE_MOVE, groundMap.getOptimalWaypoint(r->pos, reaper.targetLocation));
                // validate state
                if(r->health > 40){
                    reaper.targetLocation = enemyMain;
                    reaper.state = ReaperState::Move;
                }
            break;
            case ReaperState::Null:
            default:
                reaper.state = ReaperState::Init;
            break;
        }
    } // for r : reapers
}

void MicroManager::liberatorOnStep(SquadOrder& order){
    if(order.type == SquadOrderType::Harass)
    for(auto& l : liberators){
        const sc2::Unit* unit = l.getUnit();
        if(!unit) continue;

        switch(l.state){
            case LiberatorState::Init:{ // select a target and generate flight points
                Expansion* target = gInterface->map->getClosestExpansion(API::toPoint3D(order.target));
                if(!target) continue;

                // generate flight points
                sc2::Point2D maxPoint = gInterface->observation->GetGameInfo().playable_max;
                sc2::Point2D minPoint = gInterface->observation->GetGameInfo().playable_min;
                int minAdjust = 1;
                l.target = target->mineralMidpoint;
                l.targetFlightPoint = Monte::getPoint2D(l.target, Monte::Vector2D(target->baseLocation, l.target), 7);
                l.intermediateFlightPoint = sc2::Point2D(unit->pos.x, l.targetFlightPoint.y);
                
                if(l.targetFlightPoint.x >= maxPoint.x){
                    l.targetFlightPoint.x -= (l.targetFlightPoint.x - maxPoint.x - minAdjust);
                }
                else if(l.targetFlightPoint.x <= minPoint.x){
                    l.targetFlightPoint.x += (minPoint.x - l.targetFlightPoint.x + minAdjust);
                }
                if(l.targetFlightPoint.y >= maxPoint.y){
                    l.targetFlightPoint.y -= (l.targetFlightPoint.y - maxPoint.y - minAdjust);
                }
                else if(l.targetFlightPoint.y <= minPoint.y){
                    l.targetFlightPoint.y += (minPoint.y - l.targetFlightPoint.y + minAdjust);
                }

                if(l.intermediateFlightPoint.x >= maxPoint.x){
                    l.intermediateFlightPoint.x -= (l.intermediateFlightPoint.x - maxPoint.x - minAdjust);
                }
                else if(l.intermediateFlightPoint.x <= minPoint.x){
                    l.intermediateFlightPoint.x += (minPoint.x - l.intermediateFlightPoint.x + minAdjust);
                }
                if(l.intermediateFlightPoint.y >= maxPoint.y){
                    l.intermediateFlightPoint.y -= (l.intermediateFlightPoint.y - maxPoint.y - minAdjust);
                }
                else if(l.intermediateFlightPoint.y <= minPoint.y){
                    l.intermediateFlightPoint.y += (minPoint.y - l.intermediateFlightPoint.y + minAdjust);
                }

                // if flight points are valid, then transition to movingToIntermediate
                if(l.intermediateFlightPoint.x != -1){
                    l.state = LiberatorState::movingToIntermediate;
                }
                break;
            }
            case LiberatorState::movingToIntermediate: // TODO: influence maps
                gInterface->actions->UnitCommand(unit, sc2::ABILITY_ID::MOVE_MOVE, l.intermediateFlightPoint);
                if(sc2::DistanceSquared2D(unit->pos, l.intermediateFlightPoint) < 9){
                    l.state = LiberatorState::movingToTarget;
                }
                break;
            case LiberatorState::movingToTarget: // TODO: influence maps
                gInterface->actions->UnitCommand(unit, sc2::ABILITY_ID::MOVE_MOVE, l.targetFlightPoint);
                if(sc2::DistanceSquared2D(unit->pos, l.targetFlightPoint) < 9){
                    l.state = LiberatorState::Sieging;
                }
                break;
            case LiberatorState::Sieging: 
                gInterface->actions->UnitCommand(unit, sc2::ABILITY_ID::MORPH_LIBERATORAGMODE, l.target);
                if(unit->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_LIBERATORAG){
                    l.state = LiberatorState::Sieged;
                }
                break;
            case LiberatorState::Sieged: // basically do nothing ? or unsiege and go away if there is a lot of enemies
                break;
            case LiberatorState::Evade: // maybe a redundant state; perhaps could reselect a target? if so, rename state
                break;
            case LiberatorState::Null:
            default:
                l.state = LiberatorState::Init;
                break;
        } // end switch l.state
    } // end for l : liberators
}