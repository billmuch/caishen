#include "gtest/gtest.h"
#include "doudizhu/doudizhu_game.h"
#include "doudizhu/doudizhu_utils.h"
#include "mem_informationset_store.h"
#include "bucket_actions_nohistory_nopublic.h"
#include <sstream>
#include <fstream>
#include <experimental/filesystem>
#include <bits/stdc++.h>

namespace caishen {
namespace cfr {
namespace {

using namespace caishen::cfr;
using namespace caishen::doudizhu;
using namespace std;

std::unique_ptr<Game> createGame(std::string landlordCards, std::string player1Cards, std::string player2Cards, std::string faceupCards = "")
{
    Cards_54_1d_Type cards;

    GameCards gameCards(GameCards::Zero());

    gameCards.row(PUBLIC_CARDS_ROW) = Cards_54_1d_Type::Ones();

    cardsFromString(cards, landlordCards);
    gameCards.row(LANDLORD_CARDS_ROW) += cards;
    gameCards.row(PUBLIC_CARDS_ROW) -= cards;

    cardsFromString(cards, player1Cards);
    gameCards.row(PLAYER_1_CARDS_ROW) += cards;
    gameCards.row(PUBLIC_CARDS_ROW) -= cards;

    cardsFromString(cards, player2Cards);
    gameCards.row(PLAYER_2_CARDS_ROW) += cards;
    gameCards.row(PUBLIC_CARDS_ROW) -= cards;

    if (faceupCards != "")
    {
        cardsFromString(cards, faceupCards);
        gameCards.row(FACEUP_CARDS_ROW) += cards;
    }

    return std::unique_ptr<Game>(new Game(0, gameCards, HistoryActions()));
}

TEST(BANHNPPlayer, getInformationSetKey)
{
    std::unique_ptr<Game> game = createGame("S4JR", "S3DA", "D2", "S5H5D5");

    //landlord plays Spades 4
    std::shared_ptr<ActionData> ad(new ActionData(ActionData::Zero()));
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S4");
    Action action = Action(ad, ACTION_TYPE_SOLO, 1);

    game->play(action);

    ASSERT_EQ(MemoryInformationSetStore::getInformationSetKey(*game)->str(), 
        "P1<S3DA>P2<1>P0<1>FACEUP<S5H5D5>PUBLIC<H3C3D3S4H4C4D4S5H5C5D5S6H6C6D6S7H7C7D7S8H8C8D8S9H9C9D9STHTCTDTSJHJCJDJSQHQCQDQSKHKCKDKSAHACAS2H2C2JB>H(-1)<S4>H(-2)<>");    

    //player1 plays Spades 3
    std::shared_ptr<ActionData> ad1(new ActionData(ActionData::Zero()));
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad1->data()), "DA");
    Action action1 = Action(ad1, ACTION_TYPE_SOLO, 11);
    game->play(action1);

    ASSERT_EQ(MemoryInformationSetStore::getInformationSetKey(*game)->str(),
        "P2<D2>P0<1>P1<1>FACEUP<S5H5D5>PUBLIC<H3C3D3S4H4C4D4S5H5C5D5S6H6C6D6S7H7C7D7S8H8C8D8S9H9C9D9STHTCTDTSJHJCJDJSQHQCQDQSKHKCKDKSAHACADAS2H2C2JB>H(-1)<DA>H(-2)<S4>");
}

class MemoryInformationSetTest : public ::testing::Test {

protected:
    std::string _workingDir;

    std::vector<std::shared_ptr<BANHNPInformationSetKey>> _keys;
    std::vector<std::shared_ptr<InformationSet>> _informationSets;

    std::string & getWorkingDir()
    {
        return _workingDir;
    }

    MemoryInformationSetTest() {
        _workingDir = testing::TempDir() + "/mem_informationset_store_db/";
        std::experimental::filesystem::remove_all(_workingDir);
        std::experimental::filesystem::create_directory(_workingDir);

        BANHNPInformationSetKey::KeyDataType kd({0});
        InformationSet is(MemoryInformationSetStore::ABSTRACT_ACTION_SIZE);
        for (int i = 0; i < 10; ++i)
        {
            kd[0] = i;
            is.cumulativePolicy[0] = i;
            is.cumulativeRegrets[0] = i;
            _keys.push_back(std::shared_ptr<BANHNPInformationSetKey>(new BANHNPInformationSetKey(kd)));
            _informationSets.push_back(std::shared_ptr<InformationSet>(new InformationSet(is)));
        }
    }

    ~MemoryInformationSetTest() {
        std::experimental::filesystem::remove_all(_workingDir);
    }
};

TEST_F(MemoryInformationSetTest, LRU_MemoryInformationSetCache_appendInformationSetToFile_loadInformationSetFromFile)
{
    // fstream fs;
    // fs.open(_workingDir + "tmp.data", ios::in | ios::out | ios::trunc | ios::binary);

    // LRU_MemoryInformationSetCache lruCache(5);

    // std::vector<unsigned long long> positions;
    // for (int i = 0; i < 10; ++i)
    // {
    //     positions.push_back(fs.tellp());
    //     LRU_MemoryInformationSetCache::appendInformationSetToFile(fs, *_informationSets[i]);
    // }

    // for (int i = 0; i < 10; ++i)
    // {
    //     auto pis = LRU_MemoryInformationSetCache::loadInformationSetFromFile(fs, positions[i]);
    //     ASSERT_DOUBLE_EQ(pis->cumulativePolicy[0], _informationSets[i]->cumulativePolicy[0]);
    //     ASSERT_DOUBLE_EQ(pis->cumulativeRegrets[0], _informationSets[i]->cumulativeRegrets[0]);
    // }
}

TEST_F(MemoryInformationSetTest, LRU_MemoryInformationSetCache_TestStorage)
{
    LRU_MemoryInformationSetCache lruCache(5);
    lruCache.setupWorkingDir(getWorkingDir());

    ASSERT_TRUE(lruCache.loadIndexFile());
    ASSERT_TRUE(lruCache.loadDataFile());

    for (int i = 0; i < 10; ++i)
    {
        lruCache.putInformationSet(*_keys[i], *_informationSets[i]);
    }

    ASSERT_EQ(lruCache._indexes.size(), 10);
    ASSERT_EQ(lruCache._isCache.size(), 5);

    ASSERT_TRUE(lruCache.flushCacheDataToFile());
    ASSERT_TRUE(lruCache.flushIndexFile());

    lruCache.clear();

    lruCache.setupWorkingDir(getWorkingDir());
    ASSERT_TRUE(lruCache.loadIndexFile());
    ASSERT_TRUE(lruCache.loadDataFile());

    for (int i = 1; i < 10; ++i)
    {
        InformationSet is(MemoryInformationSetStore::ABSTRACT_ACTION_SIZE);
        lruCache.getInformationSet(*_keys[i], is);
        ASSERT_DOUBLE_EQ(is.cumulativePolicy[0], _informationSets[i]->cumulativePolicy[0]);
        ASSERT_DOUBLE_EQ(is.cumulativeRegrets[0], _informationSets[i]->cumulativeRegrets[0]);
    }

    ASSERT_TRUE(lruCache.flushCacheDataToFile());
    ASSERT_TRUE(lruCache.flushIndexFile());

    lruCache.clear();
}

TEST_F(MemoryInformationSetTest, LRU_MemoryInformationSetCache_TestLRU)
{
    LRU_MemoryInformationSetCache lruCache(3);
    lruCache.setupWorkingDir(getWorkingDir());

    ASSERT_TRUE(lruCache.loadIndexFile());
    ASSERT_TRUE(lruCache.loadDataFile());

    for (int i = 0; i < 3; ++i)
    {
        lruCache.putInformationSet(*_keys[i], *_informationSets[i]);
    }

    ASSERT_EQ(lruCache._isCache.size(), 3);
    InformationSet is(MemoryInformationSetStore::ABSTRACT_ACTION_SIZE);

    // cache item sequence: 2, 1, 0
    ASSERT_EQ(lruCache._isCache.front()._pIs->cumulativePolicy[0], _informationSets[2]->cumulativePolicy[0]);
    ASSERT_EQ(lruCache._isCache.back()._pIs->cumulativePolicy[0], _informationSets[0]->cumulativePolicy[0]);
    
    lruCache.getInformationSet(*_keys[0], is);
    // cache item sequence: 0, 2, 1
    ASSERT_EQ(lruCache._isCache.front()._pIs->cumulativePolicy[0], _informationSets[0]->cumulativePolicy[0]);
    ASSERT_EQ(lruCache._isCache.back()._pIs->cumulativePolicy[0], _informationSets[1]->cumulativePolicy[0]);

    lruCache.putInformationSet(*_keys[3], *_informationSets[3]);
    // cache item sequence: 3, 0, 2
    ASSERT_EQ(lruCache._isCache.front()._pIs->cumulativePolicy[0], _informationSets[3]->cumulativePolicy[0]);
    ASSERT_EQ((++lruCache._isCache.begin())->_pIs->cumulativePolicy[0], _informationSets[0]->cumulativePolicy[0]);
    ASSERT_EQ(lruCache._isCache.back()._pIs->cumulativePolicy[0], _informationSets[2]->cumulativePolicy[0]);

    // make sure only one copy of key in memory
    for (int i = 0; i < 3; ++i)
    {
        auto iter = lruCache._indexes.find(*_keys[i]);
        
        if (iter->first._pKey->data()[0] == 1)
        {
            ASSERT_EQ(iter->first._pKey.use_count(), 1);
            ASSERT_EQ(iter->second.second, lruCache._isCache.end());
        }
        else
        {
            ASSERT_EQ(iter->first._pKey.get(), iter->second.second->_pKey.get());
            ASSERT_EQ(iter->first._pKey.use_count(), 2);
            ASSERT_EQ(iter->second.second->_pIs.use_count(), 1);
        }
    }

    ASSERT_TRUE(lruCache.flushCacheDataToFile());
    ASSERT_TRUE(lruCache.flushIndexFile());

    lruCache.clear();
}

TEST_F(MemoryInformationSetTest, MemoryInformationSetStore_TestServiceStartStop)
{
    MemoryInformationSetStore * pStore = MemoryInformationSetStore::getInstance();
    ASSERT_FALSE(pStore->isRunning());
    ASSERT_TRUE(pStore->start(getWorkingDir()));
    ASSERT_TRUE(pStore->isRunning());

    ASSERT_FALSE(pStore->start(getWorkingDir()));
    ASSERT_FALSE(pStore->clean(getWorkingDir()));

    ASSERT_TRUE(pStore->stop());
    ASSERT_FALSE(pStore->isRunning());
}

TEST_F(MemoryInformationSetTest, MemoryInformationSetStore_TestClean)
{
    LRU_MemoryInformationSetCache lruCache(5);
    lruCache.setupWorkingDir(getWorkingDir());

    ASSERT_TRUE(lruCache.loadIndexFile());
    ASSERT_TRUE(lruCache.loadDataFile());

    //cache full, data file has 5 items
    for (int i = 0; i < 10; ++i)
    {
        lruCache.putInformationSet(*_keys[i], *_informationSets[i]);
    }

    //data file has 15 items
    for (int i = 0; i < 10; ++i)
    {
        lruCache.putInformationSet(*_keys[i], *_informationSets[i]);
    }

    //data file has 20 items
    ASSERT_TRUE(lruCache.flushCacheDataToFile());
    ASSERT_TRUE(lruCache.flushIndexFile());

    std::string dataFilePath = lruCache._dataFilePath;

    lruCache.clear();

    constexpr int ITEM_SIZE = (MemoryInformationSetStore::ABSTRACT_ACTION_SIZE / 8) + ((MemoryInformationSetStore::ABSTRACT_ACTION_SIZE % 8) ? 1 : 0)
                + (2 * MemoryInformationSetStore::ABSTRACT_ACTION_SIZE * sizeof(double));

    //file has 20 items
    ASSERT_EQ(std::experimental::filesystem::file_size(dataFilePath), 20 * ITEM_SIZE);

    ASSERT_TRUE(MemoryInformationSetStore::getInstance()->clean(getWorkingDir()));

    // file has 10 items after clean
    ASSERT_EQ(std::experimental::filesystem::file_size(dataFilePath), 10 * ITEM_SIZE);
}

} // namespace
} // namespace cfr
} // namespace caishen