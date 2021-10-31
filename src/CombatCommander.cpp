#include "CombatCommander.h"

void CombatCommander::OnGameStart(){
    sm.OnGameStart();

}

void CombatCommander::OnStep(){
        
    // scout manager
    sm.OnStep();

    // update configs
    mainSquad.setConfig(config);

    // handle killing changelings every so often
    if(gInterface->observation->GetGameLoop() % 24 == 0){
        handleChangelings();
    } // end if gameloop % 24 == 0

} // end OnStep

void CombatCommander::OnUnitCreated(const Unit* unit_){
    if(unit_ == nullptr) return; // if for whatever reason its nullptr, dont do anything

    // TODO: don't add first 4 marines to attacksquad, they go into bunker

    switch(unit_->unit_type.ToType()){
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
        // pulling workers for attacking/repairing is disabled after worker rush window
        gInterface->actions->UnitCommand(idleArmy, sc2::ABILITY_ID::ATTACK_ATTACK, enemyCenter);

        if(gInterface->observation->GetGameLoop() > 3000) return;
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
