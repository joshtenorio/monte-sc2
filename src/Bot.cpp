#include "Bot.h"

using namespace sc2;

MarinePush* strategy; // this is file-global so i can delete it in OnGameEnd()
std::string version = "v1.0.0-alpha.16"; // update this everytime we upload
Bot::Bot(){
    wm = WorkerManager();
    map = Mapper();

    strategy = new MarinePush();
    pm = ProductionManager(dynamic_cast<Strategy*>(strategy));

    gInterface.reset(new Interface(Observation(), Actions(), Query(), Debug(), &wm, &map));
}

void Bot::OnGameStart(){
    pm.OnGameStart();
    std::cout << "map name: " << Observation()->GetGameInfo().map_name << "\n";
    std::cout << "bot version: " << version << std::endl;
    Actions()->SendChat("Tag: " + version);
    Actions()->SendChat("glhf :)");

}

void Bot::OnBuildingConstructionComplete(const Unit* building_){
    std::cout << UnitTypeToName(building_->unit_type) <<
        "(" << building_->tag << ") constructed" << std::endl;

    // if it is a supply depot, lower it
    if(building_->unit_type == sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT)
        Actions()->UnitCommand(building_, sc2::ABILITY_ID::MORPH_SUPPLYDEPOT_LOWER);
    
    switch(building_->unit_type.ToType()){
        case UNIT_TYPEID::TERRAN_ARMORY:
        case UNIT_TYPEID::TERRAN_BARRACKS:
        case UNIT_TYPEID::TERRAN_BUNKER:
        case UNIT_TYPEID::TERRAN_COMMANDCENTER:
        case UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
        case UNIT_TYPEID::TERRAN_FACTORY:
        case UNIT_TYPEID::TERRAN_FUSIONCORE:
        case UNIT_TYPEID::TERRAN_GHOSTACADEMY:
        case UNIT_TYPEID::TERRAN_MISSILETURRET:
        case UNIT_TYPEID::TERRAN_REFINERY:
        case UNIT_TYPEID::TERRAN_REFINERYRICH:
        case UNIT_TYPEID::TERRAN_SENSORTOWER:
        case UNIT_TYPEID::TERRAN_STARPORT:
        case UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
        case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
        case sc2::UNIT_TYPEID::TERRAN_FACTORYREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB:
        case sc2::UNIT_TYPEID::TERRAN_STARPORTREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB:
            pm.OnBuildingConstructionComplete(building_);
            break;
        default:
            break;
    }
}

// TODO: when making combatcommander class, move/delete this
// used for marine control
bool reachedEnemyMain = false;
void Bot::OnStep() {

    // initialize mapper (find expansions and ramps)
    if(Observation()->GetGameLoop() == 50)
        map.initialize();

    pm.OnStep();
    wm.OnStep();

    // raise supply depots if enemy is nearby
    sc2::Units depots = Observation()->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT));
    sc2::Units enemies = Observation()->GetUnits(sc2::Unit::Alliance::Enemy);
    for (auto& d : depots){
        bool enemyNearby = false;
        for (auto& e : enemies){
            if(sc2::DistanceSquared3D(d->pos, e->pos) < 15){ // TODO: tune this value
                enemyNearby = true;
                break;
            }
        } // end e : enemies
        if(enemyNearby) Actions()->UnitCommand(d, sc2::ABILITY_ID::MORPH_SUPPLYDEPOT_RAISE);
        else Actions()->UnitCommand(d, sc2::ABILITY_ID::MORPH_SUPPLYDEPOT_LOWER);
    } // end d : depots

    // handle marines
    if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARINE) > 10){
    //if(API::countIdleUnits(sc2::UNIT_TYPEID::TERRAN_MARINE) >= 15){
        sc2::Units marines = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_MARINE));
        sc2::Units enemy = Observation()->GetUnits(Unit::Alliance::Enemy);
        //std::cout << "sending a wave of marines\n";
        for(const auto& m : marines){
            if(25 > sc2::DistanceSquared2D(Observation()->GetGameInfo().enemy_start_locations.front(), m->pos) && !reachedEnemyMain){
                reachedEnemyMain = true;
                Actions()->SendChat("Tag: reachedEnemyMain");
            }
                
            
            if(!reachedEnemyMain)
                Actions()->UnitCommand(
                        m,
                        ABILITY_ID::ATTACK_ATTACK,
                        Observation()->GetGameInfo().enemy_start_locations.front());
            else if(!enemy.empty())
                Actions()->UnitCommand(m, sc2::ABILITY_ID::ATTACK_ATTACK, enemy.front()->pos);
                
        } // end for loop
    } // end if marine count > 10

    // handle medivacs every so often
    if(Observation()->GetGameLoop() % 12 == 0){
        sc2::Units medivacs = Observation()->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_MEDIVAC));
        for(auto& med : medivacs){
            // move each medivac to the closest marine
            sc2::Units marines = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_MARINE));
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
                Actions()->UnitCommand(med, sc2::ABILITY_ID::GENERAL_MOVE, closestMarine->pos);
            }
        } // end medivac loop
    } // end if gameloop % 100 == 0
}

void Bot::OnUpgradeCompleted(sc2::UpgradeID upgrade_){
    std::cout << sc2::UpgradeIDToName(upgrade_) << " completed" << std::endl;
    pm.OnUpgradeCompleted(upgrade_);
}

void Bot::OnUnitCreated(const Unit* unit_){
    std::cout << UnitTypeToName(unit_->unit_type) <<
        "(" << unit_->tag << ") was created" << std::endl;
    
    switch(unit_->unit_type.ToType()){
        case UNIT_TYPEID::TERRAN_SCV:
            wm.OnUnitCreated(unit_);
            break;
        case UNIT_TYPEID::TERRAN_ARMORY:
        case UNIT_TYPEID::TERRAN_BARRACKS:
        case UNIT_TYPEID::TERRAN_BUNKER:
        case UNIT_TYPEID::TERRAN_COMMANDCENTER:
        case UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
        case UNIT_TYPEID::TERRAN_FACTORY:
        case UNIT_TYPEID::TERRAN_FUSIONCORE:
        case UNIT_TYPEID::TERRAN_GHOSTACADEMY:
        case UNIT_TYPEID::TERRAN_MISSILETURRET:
        case UNIT_TYPEID::TERRAN_REFINERY:
        case UNIT_TYPEID::TERRAN_REFINERYRICH:
        case UNIT_TYPEID::TERRAN_SENSORTOWER:
        case UNIT_TYPEID::TERRAN_STARPORT:
        case UNIT_TYPEID::TERRAN_SUPPLYDEPOT: // TODO: hehehe
            pm.OnUnitCreated(unit_);
            break;
        default:
            pm.OnUnitCreated(unit_);
            break;
    }
}

void Bot::OnUnitIdle(const Unit* unit) {
    switch (unit->unit_type.ToType()){
        case UNIT_TYPEID::TERRAN_COMMANDCENTER:
            break;
        case UNIT_TYPEID::TERRAN_SCV:
            wm.OnUnitIdle(unit);
            break;
        case UNIT_TYPEID::TERRAN_BARRACKS:
            break;
        case UNIT_TYPEID::TERRAN_REFINERY:
        case UNIT_TYPEID::TERRAN_REFINERYRICH:
            break;
        default:
            break;
    }
}

void Bot::OnUnitDestroyed(const Unit* unit_){
    std::cout << UnitTypeToName(unit_->unit_type) <<
         "(" << unit_->tag << ") was destroyed" << std::endl;
    
    // we only care if one of our units dies (for now, perhaps)
    if(unit_->alliance == Unit::Alliance::Self)
        switch(unit_->unit_type.ToType()){
            case UNIT_TYPEID::TERRAN_SCV:
                // the pm gets called first, bc we need to remove worker pointer from 
                // a Construction in bm if it is building it
                pm.OnUnitDestroyed(unit_);
                wm.OnUnitDestroyed(unit_);
                break;
            case UNIT_TYPEID::TERRAN_ARMORY:
            case UNIT_TYPEID::TERRAN_BARRACKS:
            case UNIT_TYPEID::TERRAN_BUNKER:
            case UNIT_TYPEID::TERRAN_COMMANDCENTER:
            case UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
            case UNIT_TYPEID::TERRAN_FACTORY:
            case UNIT_TYPEID::TERRAN_FUSIONCORE:
            case UNIT_TYPEID::TERRAN_GHOSTACADEMY:
            case UNIT_TYPEID::TERRAN_MISSILETURRET:
            case UNIT_TYPEID::TERRAN_REFINERY:
            case UNIT_TYPEID::TERRAN_REFINERYRICH:
            case UNIT_TYPEID::TERRAN_SENSORTOWER:
            case UNIT_TYPEID::TERRAN_STARPORT:
            case UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
                pm.OnUnitDestroyed(unit_);
                break;
            default:
                break;
        }

}

void Bot::OnError(const std::vector<ClientError>& client_errors,
        const std::vector<std::string>& protocol_errors){
    for (const auto i : client_errors) {
        std::cerr << "Encountered client error: " <<
            static_cast<int>(i) << std::endl;
    }

    for (const auto& i : protocol_errors)
        std::cerr << "Encountered protocol error: " << i << std::endl;
}

void Bot::OnGameEnd(){
    Control()->SaveReplay("data\\lastReplay.SC2Replay");
    std::cout << "game finished!\n";
    delete strategy;
}