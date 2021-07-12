#include "Bot.h"
using namespace sc2;

MarinePush* strategy; // this is file-global so i can delete it in OnGameEnd()
std::string version = "v0_6_4"; // update this everytime we upload

std::vector<sc2::UNIT_TYPEID> depotTypes; // used for filtering for depots

Bot::Bot(){
    wm = WorkerManager();
    map = Mapper();
    cc = CombatCommander();

    strategy = new MarinePush();
    pm = ProductionManager(dynamic_cast<Strategy*>(strategy));
    logger = Logger("Bot");
    debug = Monte::Debug(Debug());

    gInterface.reset(new Interface(Observation(), Actions(), Query(), &debug, &wm, &map, 1));
}

void Bot::OnGameStart(){
    pm.OnGameStart();
    cc.OnGameStart();
    gInterface->matchID = logger.createOutputPrefix();
    logger.infoInit().withStr("map name: " + Observation()->GetGameInfo().map_name).write();
    logger.infoInit().withStr("bot version: " + version).write();
    logger.infoInit().withStr("match ID prefix:").withInt(gInterface->matchID).write();
    Actions()->SendChat("Tag: " + version);
    Actions()->SendChat("Tag: matchid_" + std::to_string(gInterface->matchID));
    Actions()->SendChat("glhf :)");

    depotTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT);
    depotTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED);



}

void Bot::OnBuildingConstructionComplete(const Unit* building_){
    logger.infoInit().withUnit(building_).withStr("constructed").write();
    logger.infoInit().withUnit(building_).withStr("constructed").write("constructed.txt");

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


void Bot::OnStep() {

    // initialize mapper (find expansions and ramps)
    if(Observation()->GetGameLoop() == 50)
        map.initialize();

    pm.OnStep();
    wm.OnStep();
    cc.OnStep();

    // raise supply depots if enemy is nearby
    sc2::Units depots = Observation()->GetUnits(sc2::Unit::Alliance::Self, IsUnits(depotTypes));
    sc2::Units enemies = Observation()->GetUnits(sc2::Unit::Alliance::Enemy);
    for (auto& d : depots){
        bool enemyNearby = false;
        for (auto& e : enemies){
            if(sc2::DistanceSquared3D(d->pos, e->pos) < 49){ // TODO: tune this value
                enemyNearby = true;
                break;
            }
        } // end e : enemies
        if(enemyNearby) Actions()->UnitCommand(d, sc2::ABILITY_ID::MORPH_SUPPLYDEPOT_RAISE);
        else Actions()->UnitCommand(d, sc2::ABILITY_ID::MORPH_SUPPLYDEPOT_LOWER);
    } // end d : depots

}

void Bot::OnUpgradeCompleted(sc2::UpgradeID upgrade_){
    logger.infoInit().withStr(sc2::UpgradeIDToName(upgrade_)).withStr("completed").write();
    pm.OnUpgradeCompleted(upgrade_);
}

void Bot::OnUnitCreated(const Unit* unit_){
    logger.infoInit().withUnit(unit_).withStr("was created").write();
    
    cc.OnUnitCreated(unit_); // FIXME: move this to the switch statement
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
    logger.infoInit().withUnit(unit_).withStr("was destroyed").write();
    
    // cc call needs to be here in case we need to remove a worker scout, we need to do so before worker pointer gets removed
    // additionally we also track dead town halls in scout manager
    cc.OnUnitDestroyed(unit_);
    // we only care if one of our units dies (for now, perhaps)
    if(unit_->alliance == Unit::Alliance::Self){

        
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
}

void Bot::OnUnitDamaged(const Unit* unit_, float health_, float shields_){
    cc.OnUnitDamaged(unit_, health_, shields_);
}

void Bot::OnUnitEnterVision(const sc2::Unit* unit_){
    cc.OnUnitEnterVision(unit_);
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
    Control()->SaveReplay("lastReplay.SC2Replay");
    logger.infoInit().withStr("game finished!").write();

    delete strategy;
}
