
#include <sc2api/sc2_coordinator.h>
#include <sc2api/sc2_gametypes.h>
#include <iostream>
#include "Bot.h"

using namespace sc2;

int main(int argc, char* argv[]) {
	Coordinator coordinator;
	coordinator.LoadSettings(argc, argv);

	Bot bot;
	coordinator.SetParticipants({
		CreateParticipant(Race::Terran, &bot, "Monte"),
		CreateComputer(Race::Protoss)
	});

    coordinator.SetRealtime(false);
    
	coordinator.LaunchStarcraft();
    // TODO: use the commented-out main function when I start having reasons to test on multiple maps
    //       this is so it is possible to do stuff like bin/Monte -e ... -m map1
    //                                                  bin/Monte -e ... -m map2
    //                                                  etc etc
    coordinator.StartGame(argv[1]);

	while (coordinator.Update()) {
    }

    return 0;
}


/**
int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Provide either name of the map file or path to it!" << std::endl;
        return 1;
    }

    sc2::Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);


    // NOTE: Uncomment to increase speed of steps processing.
    // Disables ability to control your bot during game.
    // Recommended for competitions.
    coordinator.SetRawAffectsSelection(true);

    Bot bot;
    coordinator.SetParticipants(
        {
            CreateParticipant(sc2::Race::Terran, &bot, "Monte"),
            CreateComputer(
                sc2::Race::Random,
                sc2::Difficulty::CheatInsane,
                sc2::AIBuild::Rush,
                "CheatInsane"
                )
        });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(argv[1]);

    while (coordinator.Update())
    {}

    return 0;
} */
