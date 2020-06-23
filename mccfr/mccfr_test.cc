#include "gtest/gtest.h"
#include "mccfr.h"
#include "doudizhu/doudizhu_game.h"
#include "doudizhu/doudizhu_utils.h"
#include "bucket_actions_nohistory_nopublic.h"
#include <sstream>
#include <bits/stdc++.h>


namespace caishen {
namespace cfr {
namespace {

using namespace caishen::cfr;
using namespace caishen::doudizhu;

class KuhnPokerAction
{
public:
    KuhnPokerAction(char c) : action(c) {}
    ~KuhnPokerAction() {}
public:
    char action;
};

class KuhnPokerGame
{
public:
    static const int NUM_PLAYER = 2;
    static std::default_random_engine s_e;
    typedef Eigen::Array<double, 2, 1> Payoff;

public:
    KuhnPokerGame()
    {
        _cards[0] = 1;
        _cards[1] = 2;
        _cards[2] = 3;
        std::shuffle(std::begin(_cards), std::end(_cards), s_e);
        _history = "";
    }

    ~KuhnPokerGame() {};

    bool play(KuhnPokerAction &action)
    {
        assert(action.action == 'p' || action.action == 'b');
        _history += action.action;
        return true;
    }

    bool playBack()
    {
        _history = _history.substr(0, _history.length() - 1);
        return true;
    }

    inline const bool isOver()
    {
        Payoff payoff;
        return getPayoff(payoff);
    }

    const bool getPayoff(Eigen::Ref<Payoff> payoff)
    {
        payoff.setZero();
        if (_history == "pp")
        {
            int index = _cards[0] > _cards[1] ? 0 : 1;
            payoff[index] = 1;
            payoff[1-index] = -1;
            return true;
        }
        else if (_history == "pbp")
        {
            payoff[0] = -1;
            payoff[1] = 1;
            return true;
        }
        else if (_history == "pbb" || _history == "bb")
        {
            int index = _cards[0] > _cards[1] ? 0 : 1;
            payoff[index] = 2;
            payoff[1 - index] = -2;
            return true;
        }
        else if (_history == "bp")
        {
            payoff[0] = 1;
            payoff[1] = -1;
            return true;
        }

        return false;
    }

    inline const int getPlayerId() 
    { 
        return _history.length() % NUM_PLAYER;
    }

    std::string getHistory() {return _history;}
    std::string getPlayerCard() 
    {
        std::stringstream ss;
        ss <<  _cards[getPlayerId()];
        return ss.str(); 
    }

private:
    std::array<int, 3> _cards;
    std::string _history;
};

std::default_random_engine KuhnPokerGame::s_e(0);

class KuhnPokerPlayer
{
public:
    void getLegalAbstractActions(KuhnPokerGame &game, Eigen::Ref<AbstractActionSpace> legalActions)
    {
        legalActions.setOnes();
    }

    void abstractAction2ActionCards(KuhnPokerGame &game, int abstractActionId, std::vector<std::unique_ptr<KuhnPokerAction>> &actions)
    {
        actions.clear();
        char c = abstractActionId == 0 ? 'p' : 'b';
        actions.push_back(std::unique_ptr<KuhnPokerAction>(new KuhnPokerAction(c)));
    }
};

class KuhnPokerActionsProbabilities
{
public:
    KuhnPokerActionsProbabilities(KuhnPokerGame &game, std::vector<std::unique_ptr<KuhnPokerAction>> &actions){}
    static void getProbabilities(const KuhnPokerGame &game, const std::vector<std::unique_ptr<KuhnPokerAction>> &actions, std::vector<double> &probs)
    {
        //this function should not be called
        assert(false);
    }
};

class KuhnPokerInformationSetStore
{
public:
    static const int ABSTRACT_ACTION_SIZE = 2;

public:
    static std::unique_ptr<const std::string> getInformationSetKey(KuhnPokerGame& game)
    {
        return std::unique_ptr<const std::string>(new std::string(game.getPlayerCard() + game.getHistory()));
    }

public:
    void getInformationSet(const std::string & key, InformationSet & is)
    {
        auto iter = _iss.find(key);
        if (iter == _iss.end())
        {
            InformationSet newIs(2);
            newIs.cumulativeRegrets.setZero();
            newIs.cumulativePolicy.setZero();
            _iss.insert(std::pair<std::string, InformationSet>(key, newIs));

            is = newIs;
        }
        else
        {
            is = iter->second;
        }
        is.applyRegretMatching();
    }

    bool contains(const std::string & key) { return _iss.find(key) != _iss.end(); }
    
    void putInformationSet(const std::string & key, InformationSet & is)
    {
        _iss[key] = is;
    }

public:    
    std::map<std::string, InformationSet> _iss;
};

TEST(mccfr, mccfr)
{
    KuhnPokerInformationSetStore iss;
    std::vector<std::shared_ptr<KuhnPokerPlayer>> pPlayers;
    pPlayers.push_back(std::shared_ptr<KuhnPokerPlayer>(new KuhnPokerPlayer()));
    pPlayers.push_back(std::shared_ptr<KuhnPokerPlayer>(new KuhnPokerPlayer()));

    for(int i = 0; i < 1000; i++)
    {
        KuhnPokerGame game;
        mccfr<KuhnPokerInformationSetStore, KuhnPokerGame, KuhnPokerPlayer, KuhnPokerAction, KuhnPokerActionsProbabilities>
            (iss, game, pPlayers, 0);
        mccfr<KuhnPokerInformationSetStore, KuhnPokerGame, KuhnPokerPlayer, KuhnPokerAction, KuhnPokerActionsProbabilities>
            (iss, game, pPlayers, 1);
    }

    Eigen::Array<double, 2, 1> averageStrategy;
    iss._iss["3"].getAverageStrategy(averageStrategy);
    // ASSERT_TRUE(averageStrategy.isApprox(Eigen::Array<double, 2, 1>({0.0, 1.0}), 0.1));
    std::cout << "3, [" << averageStrategy[0] << ", " << averageStrategy[1] << "]" << std::endl;

    iss._iss["3b"].getAverageStrategy(averageStrategy);
    ASSERT_TRUE(averageStrategy.isApprox(Eigen::Array<double, 2, 1>({0.0, 1.0}), 0.1));
    iss._iss["3p"].getAverageStrategy(averageStrategy);
    ASSERT_TRUE(averageStrategy.isApprox(Eigen::Array<double, 2, 1>({0.0, 1.0}), 0.1));
    iss._iss["3pb"].getAverageStrategy(averageStrategy);
    ASSERT_TRUE(averageStrategy.isApprox(Eigen::Array<double, 2, 1>({0.0, 1.0}), 0.1));

    iss._iss["2"].getAverageStrategy(averageStrategy);
    ASSERT_TRUE(averageStrategy.isApprox(Eigen::Array<double, 2, 1>({1.0, 0.0}), 0.1));
    
    iss._iss["2b"].getAverageStrategy(averageStrategy);
    // ASSERT_TRUE(averageStrategy.isApprox(Eigen::Array<double, 2, 1>({0.33, 0.67}), 0.1));
    std::cout << "2b, [" << averageStrategy[0] << ", " << averageStrategy[1] << "]" << std::endl;

    iss._iss["2p"].getAverageStrategy(averageStrategy);
    // ASSERT_TRUE(averageStrategy.isApprox(Eigen::Array<double, 2, 1>({1.0, 0.0}), 0.1));
    std::cout << "2p, [" << averageStrategy[0] << ", " << averageStrategy[1] << "]" << std::endl;

    iss._iss["2pb"].getAverageStrategy(averageStrategy);
    // ASSERT_TRUE(averageStrategy.isApprox(Eigen::Array<double, 2, 1>({0.0, 1.0}), 0.1));
    std::cout << "2pb, [" << averageStrategy[0] << ", " << averageStrategy[1] << "]" << std::endl;

    iss._iss["1"].getAverageStrategy(averageStrategy);
    // ASSERT_TRUE(averageStrategy.isApprox(Eigen::Array<double, 2, 1>({0.0, 1.0}), 0.1));
    std::cout << "1, [" << averageStrategy[0] << ", " << averageStrategy[1] << "]" << std::endl;

    iss._iss["1b"].getAverageStrategy(averageStrategy);
    ASSERT_TRUE(averageStrategy.isApprox(Eigen::Array<double, 2, 1>({1.0, 0.0}), 0.1));
    
    iss._iss["1p"].getAverageStrategy(averageStrategy);
    // ASSERT_TRUE(averageStrategy.isApprox(Eigen::Array<double, 2, 1>({0.0, 1.0}), 0.1));
    std::cout << "1p, [" << averageStrategy[0] << ", " << averageStrategy[1] << "]" << std::endl;

    iss._iss["1pb"].getAverageStrategy(averageStrategy);
    ASSERT_TRUE(averageStrategy.isApprox(Eigen::Array<double, 2, 1>({1.0, 0.0}), 0.1));

    // for (auto iter = iss._iss.begin(); iter != iss._iss.end(); ++iter)
    // {
    //     Eigen::Array<double, 2, 1> averageStrategy;
    //     iter->second.getAverageStrategy(averageStrategy);
    //     std::cout << iter->first 
    //         << ", [" << averageStrategy[0] << ", " << averageStrategy[1] << "]" << std::endl;
    // }

}

} // namespace
} // namespace cfr
} // namespace caishen