#include <iostream>
#include <fstream>
#include <experimental/filesystem>

#include "mccfr.h"
#include "doudizhu/doudizhu_card.h"
#include "bucket_actions_nohistory_nopublic.h"
#include "mem_informationset_store.h"


#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace caishen::doudizhu;
using namespace caishen::cfr;

using namespace std;

int main(int argc, char** argv) 
{
    string mccfr_data = "mccfr_data";
    int iter = 1;

    typedef std::shared_ptr<BANHNPPlayer> PlayerPtr;

    std::vector<PlayerPtr> pPlayers;
    pPlayers.push_back(PlayerPtr(new BANHNPPlayer(Game::LAND_LORD_PLAYER_ID)));
    pPlayers.push_back(PlayerPtr(new BANHNPPlayer(1)));
    pPlayers.push_back(PlayerPtr(new BANHNPPlayer(2)));

    std::array<int, 54> deck;
    Deck::setRandomSeed(100);
    
    spdlog::info("Doudizhu mccfr train started ...");
    spdlog::info("Doudizhu mccfr data path: {}, iterration: {}", mccfr_data, iter);

    if (!experimental::filesystem::exists(mccfr_data))
    {
        spdlog::info("mccfr data path {} not exists, making new data directory", mccfr_data);
        experimental::filesystem::create_directory(mccfr_data);
    }

    auto piss = MemoryInformationSetStore::getInstance();
    piss->start(mccfr_data);

    spdlog::info("Start trainning ...");
    for(int i = 0; i < iter; ++i)
    {
        Deck::deal(deck);
        std::unique_ptr<Game> pGame = Game::createNewGame(deck);

        mccfr<MemoryInformationSetStore, Game, BANHNPPlayer, Action, UniformDistributionActionsProbabilities>
            (*MemoryInformationSetStore::getInstance(), *pGame, pPlayers, Game::LAND_LORD_PLAYER_ID);
        // mccfr<MemoryInformationSetStore, Game, BANHNPPlayer, Action, UniformDistributionActionsProbabilities>
        //     (*MemoryInformationSetStore::getInstance(), *pGame, pPlayers, 1);
        // mccfr<MemoryInformationSetStore, Game, BANHNPPlayer, Action, UniformDistributionActionsProbabilities>
        //     (*MemoryInformationSetStore::getInstance(), *pGame, pPlayers, 2);

        if (iter % 1000 == 0)
        {
            spdlog::info("{} iterations done.", i);
            spdlog::info("Key size: {}, data file size: {}", piss->getKeySize(), piss->getDataFileSize());
        }
    }

    spdlog::info("Doudizhu mccfr train done, {} iterations run.", iter);
    spdlog::info("Key size: {}, data file size: {}", piss->getKeySize(), piss->getDataFileSize());

    piss->stop();
    
    return 0;
}