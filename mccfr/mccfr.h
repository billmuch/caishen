#ifndef CAISHEN_CFR_MCCFR_H_
#define CAISHEN_CFR_MCCFR_H_

#include <vector>
#include <memory>
#include <iostream>
#include <Eigen/Dense>

namespace caishen
{
namespace cfr
{

typedef Eigen::Array<char, 1, Eigen::Dynamic> AbstractActionSpace;

struct InformationSet
{
public:
    Eigen::Array<bool, Eigen::Dynamic, 1> legalActionMask;
    double uniformDistConstant;
    Eigen::Array<double, Eigen::Dynamic, 1> cumulativeRegrets;
    Eigen::Array<double, Eigen::Dynamic, 1> cumulativePolicy;
    Eigen::Array<double, Eigen::Dynamic, 1> currentPolicy;

public:
    InformationSet() {}
    InformationSet(int RESERVED_SIZE)
        : legalActionMask(RESERVED_SIZE),
          cumulativeRegrets(RESERVED_SIZE),
          cumulativePolicy(RESERVED_SIZE),
          currentPolicy(RESERVED_SIZE)
    {
    }

    void init(const AbstractActionSpace & legalActions);
    int getActionSize();
    void applyRegretMatching();
    void getAverageStrategy(Eigen::Ref<Eigen::Array<double, Eigen::Dynamic, 1>> averageStrategy);

    friend std::ostream &operator<<(std::ostream &, const InformationSet &);
};

int sampleActionIndex(const Eigen::Ref<const Eigen::Array<double, Eigen::Dynamic, 1>> &probs);

int getSubActionsSampleTimes(std::vector<double> &probs);
int sampleSubActions(std::vector<double> &probs);
template<typename Game, typename Action, typename ActionsProbabilities> void getSubActionsProbabilities(Game &game, std::vector<std::unique_ptr<Action>> &actions, std::vector<double> &probs)
{
    ActionsProbabilities::getProbabilities(game, actions, probs);
}

/**
 * @brief mccfr implement the algorithm of External Sampling MCCFR in Marc Lanctot's PHD thesis 
 * http://mlanctot.info/files/papers/PhD_Thesis_MarcLanctot.pdf.
 * 
 * @tparam InformationSetStore 
 * @tparam Game 
 * @tparam Player 
 * @tparam Action 
 * @param iss 
 * @param game x
 * @param pPlayers 
 * @param updatePlayerId 
 * @return double 
 */
template <typename InformationSetStore, typename Game, typename Player, typename Action, typename ActionsProbabilities>
double mccfr(InformationSetStore &iss, Game &game,
             std::vector<std::shared_ptr<Player>> pPlayers,
             int updatePlayerId)
{

    typename Game::Payoff payoff;

    int currPlayerId = game.getPlayerId();
    auto pCurrPlayer = pPlayers[currPlayerId];

    if (game.isOver())
    {
        game.getPayoff(payoff);
        return payoff[updatePlayerId];
    }

    InformationSet is(InformationSetStore::ABSTRACT_ACTION_SIZE);
    auto pKey = iss.getInformationSetKey(game);
    if (!iss.contains(*pKey))
    {
        AbstractActionSpace legalActions(is.getActionSize());
        pCurrPlayer->getLegalAbstractActions(game, legalActions);

        is.init(legalActions);
        
        iss.putInformationSet(*pKey, is);
    }
    iss.getInformationSet(*pKey, is);

    double util = 0.0;
    if (currPlayerId != updatePlayerId)
    {
        // sample opponent nodes
        int actionId = sampleActionIndex(is.currentPolicy);
        std::vector<std::unique_ptr<Action>> actions;
        pCurrPlayer->abstractAction2ActionCards(game, actionId, actions);
        assert(actions.size() >= 1);
        if (actions.size() == 1)
        {
            game.play(*(actions[0]));
            util = mccfr<InformationSetStore, Game, Player, Action, ActionsProbabilities>(iss, game, pPlayers, updatePlayerId);
            game.playBack();
        }
        else
        {
            std::vector<double> probs;
            getSubActionsProbabilities<Game, Action, ActionsProbabilities>(game, actions, probs);
            int index = sampleSubActions(probs);
            game.play(*(actions[index]));
            util = mccfr<InformationSetStore, Game, Player, Action, ActionsProbabilities>(iss, game, pPlayers, updatePlayerId);
            game.playBack();
        }
    }
    else
    {
        // travers over my nodes
        Eigen::Array<double, Eigen::Dynamic, 1> utils(is.legalActionMask.rows());
        for (int i = 0; i < is.legalActionMask.rows(); i++)
        {
            if (is.legalActionMask[i])
            {
                std::vector<std::unique_ptr<Action>> actions;
                pCurrPlayer->abstractAction2ActionCards(game, i, actions);
                assert(actions.size() >= 1);
                if (actions.size() == 1)
                {
                    game.play(*(actions[0]));
                    utils[i] = mccfr<InformationSetStore, Game, Player, Action, ActionsProbabilities>(iss, game, pPlayers, updatePlayerId);
                    game.playBack();
                }
                else
                {
                    std::vector<double> probs;
                    getSubActionsProbabilities<Game, Action, ActionsProbabilities>(game, actions, probs);
                    int n = getSubActionsSampleTimes(probs);
                    utils[i] = 0.0;
                    for (int _ = 0; _ < n; _++)
                    {
                        int index = sampleSubActions(probs);
                        game.play(*(actions[index]));
                        utils[i] = mccfr<InformationSetStore, Game, Player, Action, ActionsProbabilities>(iss, game, pPlayers, updatePlayerId);
                        game.playBack();
                    }
                    utils[i] /= n;
                }
                util += is.currentPolicy[i] * utils[i];
            }
        }

        //update regrets
        // q(z) = \pi_{-i} is equal to the sampling probabilty, it cancels with the counterfactual term
        is.cumulativeRegrets += (utils - util);
    }

    if (currPlayerId == (updatePlayerId + 1) % Game::NUM_PLAYER)
    {
        // Do simple average on player + 1's play.
        //
        // Below is the discussion of simple average and full average quoted from
        // Open Spiel's external_sampling_mccfr.h
        // https://github.com/billmuch/open_spiel/blob/master/open_spiel/algorithms/external_sampling_mccfr.h
        //     How to average the strategy. The 'simple' type does the averaging for
        //     player i + 1 mod num_players on player i's regret update pass; in two players
        //     this corresponds to the standard implementation (updating the average
        //     policy at opponent nodes). In n>2 players, this can be a problem for several
        //     reasons: first, it does not compute the estimate as described by the
        //     (unbiased) stochastically-weighted averaging in chapter 4 of Lanctot 2013
        //     commonly used in MCCFR because the denominator (important sampling
        //     correction) should include all the other sampled players as well so the
        //     sample reach no longer cancels with reach of the player updating their
        //     average policy. Second, if one player assigns zero probability to an action
        //     (leading to a subtree), the average policy of a different player in that
        //     subtree is no longer updated. Hence, the full averaging does not update the
        //     average policy in the regret passes but does a separate pass to update the
        //     average policy. Nevertheless, we set the simple type as the default because
        //     it is faster, seems to work better empirically, and it matches what was done
        //     in Pluribus (Brown and Sandholm. Superhuman AI for multiplayer poker.
        //     Science, 11, 2019).
        is.cumulativePolicy += is.currentPolicy;
    }

    // we're always updating, write is back
    iss.putInformationSet(*pKey, is);

    return util;
}

} // namespace cfr
} // namespace caishen

#endif