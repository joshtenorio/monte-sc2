#include "Bot.h"


MarinePush* strategy; // this is file-global so i can delete it in OnGameEnd()
std::string version = "v0_13_0"; // update this everytime we upload

std::vector<sc2::UNIT_TYPEID> depotTypes;
Bot::Bot(){

    wm = WorkerManager();

    map = Mapper();

    im = InformationManager();

    strategy = new MarinePush();
    pm = ProductionManager(dynamic_cast<Strategy*>(strategy));
    logger = Logger("Bot");
    debug = Monte::Debug(Debug());
    gInterface.reset(new Interface(Observation(), Actions(), Query(), &debug, &wm, &map, 1));
    cc = CombatCommander(dynamic_cast<Strategy*>(strategy));

}

void Bot::OnGameStart(){

    gInterface->matchID = logger.createOutputPrefix();
    Logger::setVersionNumber(version);
    logger.infoInit().withStr("map name: " + Observation()->GetGameInfo().map_name).write();
    logger.infoInit().withStr("bot version: " + version).write();
    logger.infoInit().withStr("match ID prefix:").withInt(gInterface->matchID).write();

    depotTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT);
    depotTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED);

    strategy->initialize();

    API::OnGameStart();
    im.OnGameStart();
    pm.OnGameStart();
    cc.OnGameStart();

    gInterface->debug->createTimer("botStepCounter");
    logger.initializePlot({"loop", "step size (ms)"}, "step size");

}

void Bot::OnBuildingConstructionComplete(const sc2::Unit* building_){
    logger.infoInit().withUnit(building_).withStr("constructed").write();
    //logger.infoInit().withUnit(building_).withStr("constructed").write("constructed.txt");

    // if it is a supply depot, lower it
    if(building_->unit_type == sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT)
        Actions()->UnitCommand(building_, sc2::ABILITY_ID::MORPH_SUPPLYDEPOT_LOWER);
    
    switch(building_->unit_type.ToType()){
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:
            wm.OnBuildingConstructionComplete(building_);
        case sc2::UNIT_TYPEID::TERRAN_ARMORY:
        case sc2::UNIT_TYPEID::TERRAN_BARRACKS:
        case sc2::UNIT_TYPEID::TERRAN_BUNKER:
        case sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
        case sc2::UNIT_TYPEID::TERRAN_FACTORY:
        case sc2::UNIT_TYPEID::TERRAN_FUSIONCORE:
        case sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY:
        case sc2::UNIT_TYPEID::TERRAN_MISSILETURRET:
        case sc2::UNIT_TYPEID::TERRAN_REFINERY:
        case sc2::UNIT_TYPEID::TERRAN_REFINERYRICH:
        case sc2::UNIT_TYPEID::TERRAN_SENSORTOWER:
        case sc2::UNIT_TYPEID::TERRAN_STARPORT:
        case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
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
    gInterface->debug->resetTimer("botStepCounter");

    if(Observation()->GetGameLoop() == 0){
        logger.tag(version);
        logger.tag("matchid_" + std::to_string(gInterface->matchID));
        std::string glhf = "glhf :)"; // TODO: need to add a chat overload that takes const char[] so we dont need to make a var for this
        logger.chat(glhf);
    }

    // initialize mapper (find expansions and ramps)
    if(Observation()->GetGameLoop() == 50)
        map.initialize();

    // get targets from information manager
    if(Observation()->GetGameLoop() > 60){
        cc.setLocationTarget(im.findLocationTarget());
        cc.setLocationDefense(im.findLocationDefense());
        cc.setHarassTarget(im.findHarassTarget(4));
    }


    im.OnStep();
    pm.OnStep();
    wm.OnStep();
    cc.OnStep();

    im.updateProductionConfig(pm.getProductionConfig());

    // raise supply depots if enemy is nearby
    sc2::Units depots = Observation()->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnits(depotTypes));
    sc2::Units enemies = Observation()->GetUnits(sc2::Unit::Alliance::Enemy);
    if(Observation()->GetGameLoop() % 15 == 0)
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

    long long stepSize = gInterface->debug->getTimer("botStepCounter");
    gInterface->debug->debugTextOut("\nstepSize: " + std::to_string(stepSize));
    gInterface->debug->sendDebug();
    //logger.addPlotData("step size", "loop", (float) gInterface->observation->GetGameLoop());
    //logger.addPlotData("step size", "step size (ms)", (float) brr);
    //logger.writePlotRow("step size");
}

void Bot::OnUpgradeCompleted(sc2::UpgradeID upgrade_){
    logger.infoInit().withStr(sc2::UpgradeIDToName(upgrade_)).withStr("completed").write();
    pm.OnUpgradeCompleted(upgrade_);
}

void Bot::OnUnitCreated(const sc2::Unit* unit_){
    logger.infoInit().withUnit(unit_).withStr("was created").write();

    
    switch(unit_->unit_type.ToType()){
        case sc2::UNIT_TYPEID::TERRAN_SCV:
            wm.OnUnitCreated(unit_);
            break;
        case sc2::UNIT_TYPEID::TERRAN_ARMORY:
        case sc2::UNIT_TYPEID::TERRAN_BARRACKS:
        case sc2::UNIT_TYPEID::TERRAN_BUNKER:
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:
        case sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
        case sc2::UNIT_TYPEID::TERRAN_FACTORY:
        case sc2::UNIT_TYPEID::TERRAN_FUSIONCORE:
        case sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY:
        case sc2::UNIT_TYPEID::TERRAN_MISSILETURRET:
        case sc2::UNIT_TYPEID::TERRAN_REFINERY:
        case sc2::UNIT_TYPEID::TERRAN_REFINERYRICH:
        case sc2::UNIT_TYPEID::TERRAN_SENSORTOWER:
        case sc2::UNIT_TYPEID::TERRAN_STARPORT:
        case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT: // TODO: hehehe
            pm.OnUnitCreated(unit_);
            break;
        case sc2::UNIT_TYPEID::TERRAN_MARINE:
        case sc2::UNIT_TYPEID::TERRAN_MARAUDER:
        case sc2::UNIT_TYPEID::TERRAN_REAPER:
        case sc2::UNIT_TYPEID::TERRAN_GHOST:
        case sc2::UNIT_TYPEID::TERRAN_WIDOWMINE:
        case sc2::UNIT_TYPEID::TERRAN_WIDOWMINEBURROWED:
        case sc2::UNIT_TYPEID::TERRAN_CYCLONE:
        case sc2::UNIT_TYPEID::TERRAN_HELLION:
        case sc2::UNIT_TYPEID::TERRAN_HELLIONTANK:
        case sc2::UNIT_TYPEID::TERRAN_SIEGETANK:
        case sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
        case sc2::UNIT_TYPEID::TERRAN_THOR:
        case sc2::UNIT_TYPEID::TERRAN_THORAP:
        case sc2::UNIT_TYPEID::TERRAN_MEDIVAC:
        case sc2::UNIT_TYPEID::TERRAN_VIKINGASSAULT:
        case sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
        case sc2::UNIT_TYPEID::TERRAN_LIBERATOR:
        case sc2::UNIT_TYPEID::TERRAN_LIBERATORAG:
        case sc2::UNIT_TYPEID::TERRAN_RAVEN:
        case sc2::UNIT_TYPEID::TERRAN_BANSHEE:
        case sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER:
            pm.OnUnitCreated(unit_);
            cc.OnUnitCreated(unit_);
            break;
        default:
            pm.OnUnitCreated(unit_);
            break;
    }
}

void Bot::OnUnitIdle(const sc2::Unit* unit) {
    switch (unit->unit_type.ToType()){
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:
            break;
        case sc2::UNIT_TYPEID::TERRAN_SCV:
            wm.OnUnitIdle(unit);
            break;
        case sc2::UNIT_TYPEID::TERRAN_BARRACKS:
            break;
        case sc2::UNIT_TYPEID::TERRAN_REFINERY:
        case sc2::UNIT_TYPEID::TERRAN_REFINERYRICH:
            break;
        default:
            break;
    }
}

void Bot::OnUnitDestroyed(const sc2::Unit* unit_){
    logger.infoInit().withUnit(unit_).withStr("was destroyed").write();
    
    // cc call needs to be here in case we need to remove a worker scout, we need to do so before worker pointer gets removed
    // additionally we also track dead town halls in scout manager
    cc.OnUnitDestroyed(unit_);
    // we only care if one of our units dies (for now, perhaps)
    if(unit_->alliance == sc2::Unit::Alliance::Self){

        
        switch(unit_->unit_type.ToType()){
            case sc2::UNIT_TYPEID::TERRAN_SCV:
            case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
            case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
            case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER: // hopefully moving this here doesnt break anything lmao
                // the pm gets called first, bc we need to remove worker pointer from 
                // a Construction in bm if it is building it
                pm.OnUnitDestroyed(unit_);
                wm.OnUnitDestroyed(unit_);
                break;
            case sc2::UNIT_TYPEID::TERRAN_ARMORY:
            case sc2::UNIT_TYPEID::TERRAN_BARRACKS:
            case sc2::UNIT_TYPEID::TERRAN_BUNKER:
            
            case sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
            case sc2::UNIT_TYPEID::TERRAN_FACTORY:
            case sc2::UNIT_TYPEID::TERRAN_FUSIONCORE:
            case sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY:
            case sc2::UNIT_TYPEID::TERRAN_MISSILETURRET:
            case sc2::UNIT_TYPEID::TERRAN_REFINERY:
            case sc2::UNIT_TYPEID::TERRAN_REFINERYRICH:
            case sc2::UNIT_TYPEID::TERRAN_SENSORTOWER:
            case sc2::UNIT_TYPEID::TERRAN_STARPORT:
            case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
                pm.OnUnitDestroyed(unit_);
                break;
            default:
                break;
        }
    }
}

void Bot::OnUnitDamaged(const sc2::Unit* unit_, float health_, float shields_){
    if(unit_->alliance == sc2::Unit::Alliance::Self)
        pm.OnUnitDamaged(unit_, health_, shields_);
    cc.OnUnitDamaged(unit_, health_, shields_);
}

void Bot::OnUnitEnterVision(const sc2::Unit* unit_){
    cc.OnUnitEnterVision(unit_);
}

void Bot::OnError(const std::vector<sc2::ClientError>& client_errors,
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
