#include "bucket_actions_nohistory_nopublic.h"
#include "doudizhu/doudizhu_utils.h"

namespace caishen
{
namespace cfr
{

using namespace caishen::doudizhu;

BANHNPPlayer::BANHNPPlayer(int playerId)
    : CFRDoudizhuPlayer(playerId)
{
}

BANHNPPlayer::~BANHNPPlayer()
{
}

/**
 * @brief 根据game的state，和InformationSet的抽象算法，计算出AbstractInformationSet的key
 * 
 * @param game 
 * @return std::unique_ptr<const std::string> 
 */
std::unique_ptr<const std::string> BANHNPPlayer::getInformationSetKey(caishen::doudizhu::Game& game)
{
    return std::unique_ptr<std::string>(new std::string(""));
}

void BANHNPPlayer::action2AbstractAction(Action_Type aType, std::vector<Action_Rank> &availableRanks, Eigen::Ref<AbstractActionSpace> legalActions)
{
    switch(aType)
    {
    case ACTION_TYPE_SOLO:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 14);
            legalActions[r + AA_SOLO_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_SOLO_CHAIN_5:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 7);
            legalActions[r + AA_SOLO_CHAIN_5_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_SOLO_CHAIN_6:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 6);
            legalActions[r + AA_SOLO_CHAIN_6_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_SOLO_CHAIN_7:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 5);
            legalActions[r + AA_SOLO_CHAIN_7_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_SOLO_CHAIN_8:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 4);
            legalActions[r + AA_SOLO_CHAIN_8_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_SOLO_CHAIN_9:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 3);
            legalActions[r + AA_SOLO_CHAIN_9_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_SOLO_CHAIN_10:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 2);
            legalActions[r + AA_SOLO_CHAIN_10_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_SOLO_CHAIN_11:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 1);
            legalActions[r + AA_SOLO_CHAIN_11_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_SOLO_CHAIN_12:
        for (Action_Rank r : availableRanks)
        {
            assert(r == 0);
            legalActions[AA_SOLO_CHAIN_12_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_PAIR:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 12);
            legalActions[r + AA_PAIR_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_PAIR_CHAIN_3:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 9);
            legalActions[r + AA_PAIR_CHAIN_3_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_PAIR_CHAIN_4:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 8);
            legalActions[r + AA_PAIR_CHAIN_4_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_PAIR_CHAIN_5:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 7);
            legalActions[r + AA_PAIR_CHAIN_5_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_PAIR_CHAIN_6:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 6);
            legalActions[r + AA_PAIR_CHAIN_6_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_PAIR_CHAIN_7:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 5);
            legalActions[r + AA_PAIR_CHAIN_7_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_PAIR_CHAIN_8:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 4);
            legalActions[r + AA_PAIR_CHAIN_8_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_PAIR_CHAIN_9:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 3);
            legalActions[r + AA_PAIR_CHAIN_9_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_PAIR_CHAIN_10:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 2);
            legalActions[r + AA_PAIR_CHAIN_10_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_TRIO:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 12);
            legalActions[r + AA_TRIO_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_TRIO_CHAIN_2:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 10);
            legalActions[r + AA_TRIO_CHAIN_2_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_TRIO_CHAIN_3:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 9);
            legalActions[r + AA_TRIO_CHAIN_3_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_TRIO_CHAIN_4:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 8);
            legalActions[r + AA_TRIO_CHAIN_4_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_TRIO_CHAIN_5:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 7);
            legalActions[r + AA_TRIO_CHAIN_5_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_TRIO_CHAIN_6:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 6);
            legalActions[r + AA_TRIO_CHAIN_6_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_TRIO_SOLO:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 12);
            legalActions[r + AA_TRIO_SOLO_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_TRIO_SOLO_CHAIN_2:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 10);
            legalActions[AA_TRIO_SOLO_CHAIN_2_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_TRIO_SOLO_CHAIN_3:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 9);
            legalActions[r + AA_TRIO_SOLO_CHAIN_3_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_TRIO_SOLO_CHAIN_4:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 8);
            legalActions[r + AA_TRIO_SOLO_CHAIN_4_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_TRIO_SOLO_CHAIN_5:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 7);
            legalActions[r + AA_TRIO_SOLO_CHAIN_5_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_TRIO_PAIR:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 12);
            legalActions[r + AA_TRIO_PAIR_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_TRIO_PAIR_CHAIN_2:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 10);
            legalActions[r + AA_TRIO_PAIR_CHAIN_2_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_TRIO_PAIR_CHAIN_3:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 9);
            legalActions[r + AA_TRIO_PAIR_CHAIN_3_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_TRIO_PAIR_CHAIN_4:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 8);
            legalActions[r + AA_TRIO_PAIR_CHAIN_4_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_FOUR_TWO_SOLO:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 12);
            legalActions[r + AA_FOUR_TWO_SOLO_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_FOUR_TWO_PAIR:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 12);
            legalActions[r + AA_FOUR_TWO_PAIR_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_BOMB:
        for (Action_Rank r : availableRanks)
        {
            assert(r <= 12);
            legalActions[r + AA_BOMB_RANK_0] = 1;
        }
        return;
    case ACTION_TYPE_ROCKET:
        for (Action_Rank r : availableRanks)
        {
            assert(r == 0);
            legalActions[r + AA_ROCKET_RANK_0] = 1;
        }
        return;
    default:
        assert(false);
    }
}

void BANHNPPlayer::getLegalAbstractActions(Game &game, Eigen::Ref<AbstractActionSpace> legalActions)
{
    legalActions.setZero();

    std::vector<std::vector<Action_Rank>> legalGameActions;
    game.getLegalActions(legalGameActions);
    for (unsigned int i = 0; i < legalGameActions.size(); i++)
    {
        action2AbstractAction(Action_Type(i), legalGameActions[i], legalActions);
    }
}

void BANHNPPlayer::abstractAction2ActionCards(Game &game, int abstractActionId, std::vector<std::unique_ptr<Action>> & actions)
{
    std::shared_ptr<ActionData> ad(new ActionData());
    ad->setZero();
    std::vector<std::shared_ptr<ActionData>> ads;
    Action_Type type;
    Action_Rank rank;
    std::vector<Action_Rank> availableRanks;

    int currPlayerId = game.getPlayerId();

    auto playerCards = game.getCards().row(currPlayerId);

    assert(abstractActionId >= AA_SOLO_RANK_0 && abstractActionId <= AA_PASS);
    switch(abstractActionId)
    {
    case AA_SOLO_RANK_0:
    case AA_SOLO_RANK_1:
    case AA_SOLO_RANK_2:
    case AA_SOLO_RANK_3:
    case AA_SOLO_RANK_4:
    case AA_SOLO_RANK_5:
    case AA_SOLO_RANK_6:
    case AA_SOLO_RANK_7:
    case AA_SOLO_RANK_8:
    case AA_SOLO_RANK_9:
    case AA_SOLO_RANK_10:
    case AA_SOLO_RANK_11:
    case AA_SOLO_RANK_12:
    case AA_SOLO_RANK_13:
    case AA_SOLO_RANK_14:
        type = ACTION_TYPE_SOLO;
        rank = abstractActionId - AA_SOLO_RANK_0;
        get_SOLO_ActionCards(playerCards, rank, ad);
        break;
    case AA_SOLO_CHAIN_5_RANK_0:
    case AA_SOLO_CHAIN_5_RANK_1:
    case AA_SOLO_CHAIN_5_RANK_2:
    case AA_SOLO_CHAIN_5_RANK_3:
    case AA_SOLO_CHAIN_5_RANK_4:
    case AA_SOLO_CHAIN_5_RANK_5:
    case AA_SOLO_CHAIN_5_RANK_6:
    case AA_SOLO_CHAIN_5_RANK_7:
        type = ACTION_TYPE_SOLO_CHAIN_5;
        rank = abstractActionId - AA_SOLO_CHAIN_5_RANK_0;
        get_TYPEX_CHAINX_ActionCards(1, 5, playerCards, rank, ad);
        break;
    case AA_SOLO_CHAIN_6_RANK_0:
    case AA_SOLO_CHAIN_6_RANK_1:
    case AA_SOLO_CHAIN_6_RANK_2:
    case AA_SOLO_CHAIN_6_RANK_3:
    case AA_SOLO_CHAIN_6_RANK_4:
    case AA_SOLO_CHAIN_6_RANK_5:
    case AA_SOLO_CHAIN_6_RANK_6:
        type = ACTION_TYPE_SOLO_CHAIN_6;
        rank = abstractActionId - AA_SOLO_CHAIN_6_RANK_0;
        get_TYPEX_CHAINX_ActionCards(1, 6, playerCards, rank, ad);
        break;
    case AA_SOLO_CHAIN_7_RANK_0:
    case AA_SOLO_CHAIN_7_RANK_1:
    case AA_SOLO_CHAIN_7_RANK_2:
    case AA_SOLO_CHAIN_7_RANK_3:
    case AA_SOLO_CHAIN_7_RANK_4:
    case AA_SOLO_CHAIN_7_RANK_5:
        type = ACTION_TYPE_SOLO_CHAIN_7;
        rank = abstractActionId - AA_SOLO_CHAIN_7_RANK_0;
        get_TYPEX_CHAINX_ActionCards(1, 7, playerCards, rank, ad);
        break;
    case AA_SOLO_CHAIN_8_RANK_0:
    case AA_SOLO_CHAIN_8_RANK_1:
    case AA_SOLO_CHAIN_8_RANK_2:
    case AA_SOLO_CHAIN_8_RANK_3:
    case AA_SOLO_CHAIN_8_RANK_4:
        type = ACTION_TYPE_SOLO_CHAIN_8;
        rank = abstractActionId - AA_SOLO_CHAIN_8_RANK_0;
        get_TYPEX_CHAINX_ActionCards(1, 8, playerCards, rank, ad);
        break;
    case AA_SOLO_CHAIN_9_RANK_0:
    case AA_SOLO_CHAIN_9_RANK_1:
    case AA_SOLO_CHAIN_9_RANK_2:
    case AA_SOLO_CHAIN_9_RANK_3:
        type = ACTION_TYPE_SOLO_CHAIN_9;
        rank = abstractActionId - AA_SOLO_CHAIN_9_RANK_0;
        get_TYPEX_CHAINX_ActionCards(1, 9, playerCards, rank, ad);
        break;
    case AA_SOLO_CHAIN_10_RANK_0:
    case AA_SOLO_CHAIN_10_RANK_1:
    case AA_SOLO_CHAIN_10_RANK_2:
        type = ACTION_TYPE_SOLO_CHAIN_10;
        rank = abstractActionId - AA_SOLO_CHAIN_10_RANK_0;
        get_TYPEX_CHAINX_ActionCards(1, 10, playerCards, rank, ad);
        break;
    case AA_SOLO_CHAIN_11_RANK_0:
    case AA_SOLO_CHAIN_11_RANK_1:
        type = ACTION_TYPE_SOLO_CHAIN_11;
        rank = abstractActionId - AA_SOLO_CHAIN_11_RANK_0;
        get_TYPEX_CHAINX_ActionCards(1, 11, playerCards, rank, ad);
        break;
    case AA_SOLO_CHAIN_12_RANK_0:
        type = ACTION_TYPE_SOLO_CHAIN_12;
        rank = 0;
        get_TYPEX_CHAINX_ActionCards(1, 12, playerCards, rank, ad);
        break;
    case AA_PAIR_RANK_0:
    case AA_PAIR_RANK_1:
    case AA_PAIR_RANK_2:
    case AA_PAIR_RANK_3:
    case AA_PAIR_RANK_4:
    case AA_PAIR_RANK_5:
    case AA_PAIR_RANK_6:
    case AA_PAIR_RANK_7:
    case AA_PAIR_RANK_8:
    case AA_PAIR_RANK_9:
    case AA_PAIR_RANK_10:
    case AA_PAIR_RANK_11:
    case AA_PAIR_RANK_12:
        type = ACTION_TYPE_PAIR;
        rank = abstractActionId - AA_PAIR_RANK_0;
        get_TYPEX_CHAINX_ActionCards(2, 1, playerCards, rank, ad);
        break;
    case AA_PAIR_CHAIN_3_RANK_0:
    case AA_PAIR_CHAIN_3_RANK_1:
    case AA_PAIR_CHAIN_3_RANK_2:
    case AA_PAIR_CHAIN_3_RANK_3:
    case AA_PAIR_CHAIN_3_RANK_4:
    case AA_PAIR_CHAIN_3_RANK_5:
    case AA_PAIR_CHAIN_3_RANK_6:
    case AA_PAIR_CHAIN_3_RANK_7:
    case AA_PAIR_CHAIN_3_RANK_8:
    case AA_PAIR_CHAIN_3_RANK_9:
        type = ACTION_TYPE_PAIR_CHAIN_3;
        rank = abstractActionId - AA_PAIR_CHAIN_3_RANK_0;
        get_TYPEX_CHAINX_ActionCards(2, 3, playerCards, rank, ad);
        break;
    case AA_PAIR_CHAIN_4_RANK_0:
    case AA_PAIR_CHAIN_4_RANK_1:
    case AA_PAIR_CHAIN_4_RANK_2:
    case AA_PAIR_CHAIN_4_RANK_3:
    case AA_PAIR_CHAIN_4_RANK_4:
    case AA_PAIR_CHAIN_4_RANK_5:
    case AA_PAIR_CHAIN_4_RANK_6:
    case AA_PAIR_CHAIN_4_RANK_7:
    case AA_PAIR_CHAIN_4_RANK_8:
        type = ACTION_TYPE_PAIR_CHAIN_4;
        rank = abstractActionId - AA_PAIR_CHAIN_4_RANK_0;
        get_TYPEX_CHAINX_ActionCards(2, 4, playerCards, rank, ad);
        break;
    case AA_PAIR_CHAIN_5_RANK_0:
    case AA_PAIR_CHAIN_5_RANK_1:
    case AA_PAIR_CHAIN_5_RANK_2:
    case AA_PAIR_CHAIN_5_RANK_3:
    case AA_PAIR_CHAIN_5_RANK_4:
    case AA_PAIR_CHAIN_5_RANK_5:
    case AA_PAIR_CHAIN_5_RANK_6:
    case AA_PAIR_CHAIN_5_RANK_7:
        type = ACTION_TYPE_PAIR_CHAIN_5;
        rank = abstractActionId - AA_PAIR_CHAIN_5_RANK_0;
        get_TYPEX_CHAINX_ActionCards(2, 5, playerCards, rank, ad);
        break;
    case AA_PAIR_CHAIN_6_RANK_0:
    case AA_PAIR_CHAIN_6_RANK_1:
    case AA_PAIR_CHAIN_6_RANK_2:
    case AA_PAIR_CHAIN_6_RANK_3:
    case AA_PAIR_CHAIN_6_RANK_4:
    case AA_PAIR_CHAIN_6_RANK_5:
    case AA_PAIR_CHAIN_6_RANK_6:
        type = ACTION_TYPE_PAIR_CHAIN_6;
        rank = abstractActionId - AA_PAIR_CHAIN_6_RANK_0;
        get_TYPEX_CHAINX_ActionCards(2, 6, playerCards, rank, ad);
        break;
    case AA_PAIR_CHAIN_7_RANK_0:
    case AA_PAIR_CHAIN_7_RANK_1:
    case AA_PAIR_CHAIN_7_RANK_2:
    case AA_PAIR_CHAIN_7_RANK_3:
    case AA_PAIR_CHAIN_7_RANK_4:
    case AA_PAIR_CHAIN_7_RANK_5:
        type = ACTION_TYPE_PAIR_CHAIN_7;
        rank = abstractActionId - AA_PAIR_CHAIN_7_RANK_0;
        get_TYPEX_CHAINX_ActionCards(2, 7, playerCards, rank, ad);
        break;
    case AA_PAIR_CHAIN_8_RANK_0:
    case AA_PAIR_CHAIN_8_RANK_1:
    case AA_PAIR_CHAIN_8_RANK_2:
    case AA_PAIR_CHAIN_8_RANK_3:
    case AA_PAIR_CHAIN_8_RANK_4:
        type = ACTION_TYPE_PAIR_CHAIN_8;
        rank = abstractActionId - AA_PAIR_CHAIN_8_RANK_0;
        get_TYPEX_CHAINX_ActionCards(2, 8, playerCards, rank, ad);
        break;
    case AA_PAIR_CHAIN_9_RANK_0:
    case AA_PAIR_CHAIN_9_RANK_1:
    case AA_PAIR_CHAIN_9_RANK_2:
    case AA_PAIR_CHAIN_9_RANK_3:
        type = ACTION_TYPE_PAIR_CHAIN_9;
        rank = abstractActionId - AA_PAIR_CHAIN_9_RANK_0;
        get_TYPEX_CHAINX_ActionCards(2, 9, playerCards, rank, ad);
        break;
    case AA_PAIR_CHAIN_10_RANK_0:
    case AA_PAIR_CHAIN_10_RANK_1:
    case AA_PAIR_CHAIN_10_RANK_2:
        type = ACTION_TYPE_PAIR_CHAIN_10;
        rank = abstractActionId - AA_PAIR_CHAIN_10_RANK_0;
        get_TYPEX_CHAINX_ActionCards(2, 10, playerCards, rank, ad);
        break;
    case AA_TRIO_RANK_0:
    case AA_TRIO_RANK_1:
    case AA_TRIO_RANK_2:
    case AA_TRIO_RANK_3:
    case AA_TRIO_RANK_4:
    case AA_TRIO_RANK_5:
    case AA_TRIO_RANK_6:
    case AA_TRIO_RANK_7:
    case AA_TRIO_RANK_8:
    case AA_TRIO_RANK_9:
    case AA_TRIO_RANK_10:
    case AA_TRIO_RANK_11:
    case AA_TRIO_RANK_12:
        type = ACTION_TYPE_TRIO;
        rank = abstractActionId - AA_TRIO_RANK_0;
        get_TYPEX_CHAINX_ActionCards(3, 1, playerCards, rank, ad);
        break;
    case AA_TRIO_CHAIN_2_RANK_0:
    case AA_TRIO_CHAIN_2_RANK_1:
    case AA_TRIO_CHAIN_2_RANK_2:
    case AA_TRIO_CHAIN_2_RANK_3:
    case AA_TRIO_CHAIN_2_RANK_4:
    case AA_TRIO_CHAIN_2_RANK_5:
    case AA_TRIO_CHAIN_2_RANK_6:
    case AA_TRIO_CHAIN_2_RANK_7:
    case AA_TRIO_CHAIN_2_RANK_8:
    case AA_TRIO_CHAIN_2_RANK_9:
    case AA_TRIO_CHAIN_2_RANK_10:
        type = ACTION_TYPE_TRIO_CHAIN_2;
        rank = abstractActionId - AA_TRIO_CHAIN_2_RANK_0;
        get_TYPEX_CHAINX_ActionCards(3, 2, playerCards, rank, ad);
        break;
    case AA_TRIO_CHAIN_3_RANK_0:
    case AA_TRIO_CHAIN_3_RANK_1:
    case AA_TRIO_CHAIN_3_RANK_2:
    case AA_TRIO_CHAIN_3_RANK_3:
    case AA_TRIO_CHAIN_3_RANK_4:
    case AA_TRIO_CHAIN_3_RANK_5:
    case AA_TRIO_CHAIN_3_RANK_6:
    case AA_TRIO_CHAIN_3_RANK_7:
    case AA_TRIO_CHAIN_3_RANK_8:
    case AA_TRIO_CHAIN_3_RANK_9:
        type = ACTION_TYPE_TRIO_CHAIN_3;
        rank = abstractActionId - AA_TRIO_CHAIN_3_RANK_0;
        get_TYPEX_CHAINX_ActionCards(3, 3, playerCards, rank, ad);
        break;
    case AA_TRIO_CHAIN_4_RANK_0:
    case AA_TRIO_CHAIN_4_RANK_1:
    case AA_TRIO_CHAIN_4_RANK_2:
    case AA_TRIO_CHAIN_4_RANK_3:
    case AA_TRIO_CHAIN_4_RANK_4:
    case AA_TRIO_CHAIN_4_RANK_5:
    case AA_TRIO_CHAIN_4_RANK_6:
    case AA_TRIO_CHAIN_4_RANK_7:
    case AA_TRIO_CHAIN_4_RANK_8:
        type = ACTION_TYPE_TRIO_CHAIN_4;
        rank = abstractActionId - AA_TRIO_CHAIN_4_RANK_0;
        get_TYPEX_CHAINX_ActionCards(3, 4, playerCards, rank, ad);
        break;
    case AA_TRIO_CHAIN_5_RANK_0:
    case AA_TRIO_CHAIN_5_RANK_1:
    case AA_TRIO_CHAIN_5_RANK_2:
    case AA_TRIO_CHAIN_5_RANK_3:
    case AA_TRIO_CHAIN_5_RANK_4:
    case AA_TRIO_CHAIN_5_RANK_5:
    case AA_TRIO_CHAIN_5_RANK_6:
    case AA_TRIO_CHAIN_5_RANK_7:
        type = ACTION_TYPE_TRIO_CHAIN_5;
        rank = abstractActionId - AA_TRIO_CHAIN_5_RANK_0;
        get_TYPEX_CHAINX_ActionCards(3, 5, playerCards, rank, ad);
        break;
    case AA_TRIO_CHAIN_6_RANK_0:
    case AA_TRIO_CHAIN_6_RANK_1:
    case AA_TRIO_CHAIN_6_RANK_2:
    case AA_TRIO_CHAIN_6_RANK_3:
    case AA_TRIO_CHAIN_6_RANK_4:
    case AA_TRIO_CHAIN_6_RANK_5:
    case AA_TRIO_CHAIN_6_RANK_6:
        type = ACTION_TYPE_TRIO_CHAIN_6;
        rank = abstractActionId - AA_TRIO_CHAIN_6_RANK_0;
        get_TYPEX_CHAINX_ActionCards(3, 6, playerCards, rank, ad);
        break;
    case AA_TRIO_SOLO_RANK_0:
    case AA_TRIO_SOLO_RANK_1:
    case AA_TRIO_SOLO_RANK_2:
    case AA_TRIO_SOLO_RANK_3:
    case AA_TRIO_SOLO_RANK_4:
    case AA_TRIO_SOLO_RANK_5:
    case AA_TRIO_SOLO_RANK_6:
    case AA_TRIO_SOLO_RANK_7:
    case AA_TRIO_SOLO_RANK_8:
    case AA_TRIO_SOLO_RANK_9:
    case AA_TRIO_SOLO_RANK_10:
    case AA_TRIO_SOLO_RANK_11:
    case AA_TRIO_SOLO_RANK_12:
        type = ACTION_TYPE_TRIO_SOLO;
        rank = abstractActionId - AA_TRIO_SOLO_RANK_0;
        get_TYPEX_CHAINX_ActionCards(3, 1, playerCards, rank, ad);
        get_SOLO_AttachmentCards(playerCards, ad, 1, -1, -1, ads);
        break;
    case AA_TRIO_SOLO_CHAIN_2_RANK_0:
    case AA_TRIO_SOLO_CHAIN_2_RANK_1:
    case AA_TRIO_SOLO_CHAIN_2_RANK_2:
    case AA_TRIO_SOLO_CHAIN_2_RANK_3:
    case AA_TRIO_SOLO_CHAIN_2_RANK_4:
    case AA_TRIO_SOLO_CHAIN_2_RANK_5:
    case AA_TRIO_SOLO_CHAIN_2_RANK_6:
    case AA_TRIO_SOLO_CHAIN_2_RANK_7:
    case AA_TRIO_SOLO_CHAIN_2_RANK_8:
    case AA_TRIO_SOLO_CHAIN_2_RANK_9:
    case AA_TRIO_SOLO_CHAIN_2_RANK_10:
        type = ACTION_TYPE_TRIO_SOLO_CHAIN_2;
        rank = abstractActionId - AA_TRIO_SOLO_CHAIN_2_RANK_0;
        get_TYPEX_CHAINX_ActionCards(3, 2, playerCards, rank, ad);
        get_SOLO_AttachmentCards(playerCards, ad, 2, -1, -1, ads);
        break;
    case AA_TRIO_SOLO_CHAIN_3_RANK_0:
    case AA_TRIO_SOLO_CHAIN_3_RANK_1:
    case AA_TRIO_SOLO_CHAIN_3_RANK_2:
    case AA_TRIO_SOLO_CHAIN_3_RANK_3:
    case AA_TRIO_SOLO_CHAIN_3_RANK_4:
    case AA_TRIO_SOLO_CHAIN_3_RANK_5:
    case AA_TRIO_SOLO_CHAIN_3_RANK_6:
    case AA_TRIO_SOLO_CHAIN_3_RANK_7:
    case AA_TRIO_SOLO_CHAIN_3_RANK_8:
    case AA_TRIO_SOLO_CHAIN_3_RANK_9:
        type = ACTION_TYPE_TRIO_SOLO_CHAIN_3;
        rank = abstractActionId - AA_TRIO_SOLO_CHAIN_3_RANK_0;
        get_TYPEX_CHAINX_ActionCards(3, 3, playerCards, rank, ad);
        get_SOLO_AttachmentCards(playerCards, ad, 3, rank - 1, rank + 3 < 12 ? rank + 3 : -1, ads);
        break;
    case AA_TRIO_SOLO_CHAIN_4_RANK_0:
    case AA_TRIO_SOLO_CHAIN_4_RANK_1:
    case AA_TRIO_SOLO_CHAIN_4_RANK_2:
    case AA_TRIO_SOLO_CHAIN_4_RANK_3:
    case AA_TRIO_SOLO_CHAIN_4_RANK_4:
    case AA_TRIO_SOLO_CHAIN_4_RANK_5:
    case AA_TRIO_SOLO_CHAIN_4_RANK_6:
    case AA_TRIO_SOLO_CHAIN_4_RANK_7:
    case AA_TRIO_SOLO_CHAIN_4_RANK_8:
        type = ACTION_TYPE_TRIO_SOLO_CHAIN_4;
        rank = abstractActionId - AA_TRIO_SOLO_CHAIN_4_RANK_0;
        get_TYPEX_CHAINX_ActionCards(3, 4, playerCards, rank, ad);
        get_SOLO_AttachmentCards(playerCards, ad, 4, rank - 1, rank + 4 < 12 ? rank + 4 : -1, ads);
        break;
    case AA_TRIO_SOLO_CHAIN_5_RANK_0:
    case AA_TRIO_SOLO_CHAIN_5_RANK_1:
    case AA_TRIO_SOLO_CHAIN_5_RANK_2:
    case AA_TRIO_SOLO_CHAIN_5_RANK_3:
    case AA_TRIO_SOLO_CHAIN_5_RANK_4:
    case AA_TRIO_SOLO_CHAIN_5_RANK_5:
    case AA_TRIO_SOLO_CHAIN_5_RANK_6:
    case AA_TRIO_SOLO_CHAIN_5_RANK_7:
        type = ACTION_TYPE_TRIO_SOLO_CHAIN_5;
        rank = abstractActionId - AA_TRIO_SOLO_CHAIN_5_RANK_0;
        get_TYPEX_CHAINX_ActionCards(3, 5, playerCards, rank, ad);
        get_SOLO_AttachmentCards(playerCards, ad, 5, rank - 1, rank + 5 < 12 ? rank + 5 : -1, ads);
        break;
    case AA_TRIO_PAIR_RANK_0:
    case AA_TRIO_PAIR_RANK_1:
    case AA_TRIO_PAIR_RANK_2:
    case AA_TRIO_PAIR_RANK_3:
    case AA_TRIO_PAIR_RANK_4:
    case AA_TRIO_PAIR_RANK_5:
    case AA_TRIO_PAIR_RANK_6:
    case AA_TRIO_PAIR_RANK_7:
    case AA_TRIO_PAIR_RANK_8:
    case AA_TRIO_PAIR_RANK_9:
    case AA_TRIO_PAIR_RANK_10:
    case AA_TRIO_PAIR_RANK_11:
    case AA_TRIO_PAIR_RANK_12:
        type = ACTION_TYPE_TRIO_PAIR;
        rank = abstractActionId - AA_TRIO_PAIR_RANK_0;
        get_TYPEX_CHAINX_ActionCards(3, 1, playerCards, rank, ad);
        get_PAIR_AttachmentCards(playerCards, ad, 1, ads);
        break;
    case AA_TRIO_PAIR_CHAIN_2_RANK_0:
    case AA_TRIO_PAIR_CHAIN_2_RANK_1:
    case AA_TRIO_PAIR_CHAIN_2_RANK_2:
    case AA_TRIO_PAIR_CHAIN_2_RANK_3:
    case AA_TRIO_PAIR_CHAIN_2_RANK_4:
    case AA_TRIO_PAIR_CHAIN_2_RANK_5:
    case AA_TRIO_PAIR_CHAIN_2_RANK_6:
    case AA_TRIO_PAIR_CHAIN_2_RANK_7:
    case AA_TRIO_PAIR_CHAIN_2_RANK_8:
    case AA_TRIO_PAIR_CHAIN_2_RANK_9:
    case AA_TRIO_PAIR_CHAIN_2_RANK_10:
        type = ACTION_TYPE_TRIO_PAIR_CHAIN_2;
        rank = abstractActionId - AA_TRIO_PAIR_CHAIN_2_RANK_0;
        get_TYPEX_CHAINX_ActionCards(3, 2, playerCards, rank, ad);
        get_PAIR_AttachmentCards(playerCards, ad, 2, ads);
        break;
    case AA_TRIO_PAIR_CHAIN_3_RANK_0:
    case AA_TRIO_PAIR_CHAIN_3_RANK_1:
    case AA_TRIO_PAIR_CHAIN_3_RANK_2:
    case AA_TRIO_PAIR_CHAIN_3_RANK_3:
    case AA_TRIO_PAIR_CHAIN_3_RANK_4:
    case AA_TRIO_PAIR_CHAIN_3_RANK_5:
    case AA_TRIO_PAIR_CHAIN_3_RANK_6:
    case AA_TRIO_PAIR_CHAIN_3_RANK_7:
    case AA_TRIO_PAIR_CHAIN_3_RANK_8:
    case AA_TRIO_PAIR_CHAIN_3_RANK_9:
        type = ACTION_TYPE_TRIO_PAIR_CHAIN_3;
        rank = abstractActionId - AA_TRIO_PAIR_CHAIN_3_RANK_0;
        get_TYPEX_CHAINX_ActionCards(3, 3, playerCards, rank, ad);
        get_PAIR_AttachmentCards(playerCards, ad, 3, ads);
        break;
    case AA_TRIO_PAIR_CHAIN_4_RANK_0:
    case AA_TRIO_PAIR_CHAIN_4_RANK_1:
    case AA_TRIO_PAIR_CHAIN_4_RANK_2:
    case AA_TRIO_PAIR_CHAIN_4_RANK_3:
    case AA_TRIO_PAIR_CHAIN_4_RANK_4:
    case AA_TRIO_PAIR_CHAIN_4_RANK_5:
    case AA_TRIO_PAIR_CHAIN_4_RANK_6:
    case AA_TRIO_PAIR_CHAIN_4_RANK_7:
    case AA_TRIO_PAIR_CHAIN_4_RANK_8:
        type = ACTION_TYPE_TRIO_PAIR_CHAIN_4;
        rank = abstractActionId - AA_TRIO_PAIR_CHAIN_4_RANK_0;
        get_TYPEX_CHAINX_ActionCards(3, 4, playerCards, rank, ad);
        get_PAIR_AttachmentCards(playerCards, ad, 4, ads);
        break;
    case AA_FOUR_TWO_SOLO_RANK_0:
    case AA_FOUR_TWO_SOLO_RANK_1:
    case AA_FOUR_TWO_SOLO_RANK_2:
    case AA_FOUR_TWO_SOLO_RANK_3:
    case AA_FOUR_TWO_SOLO_RANK_4:
    case AA_FOUR_TWO_SOLO_RANK_5:
    case AA_FOUR_TWO_SOLO_RANK_6:
    case AA_FOUR_TWO_SOLO_RANK_7:
    case AA_FOUR_TWO_SOLO_RANK_8:
    case AA_FOUR_TWO_SOLO_RANK_9:
    case AA_FOUR_TWO_SOLO_RANK_10:
    case AA_FOUR_TWO_SOLO_RANK_11:
    case AA_FOUR_TWO_SOLO_RANK_12:
        type = ACTION_TYPE_FOUR_TWO_SOLO;
        rank = abstractActionId - AA_FOUR_TWO_SOLO_RANK_0;
        get_TYPEX_CHAINX_ActionCards(4, 1, playerCards, rank, ad);
        get_SOLO_AttachmentCards(playerCards, ad, 2, -1, -1, ads);
        break;
    case AA_FOUR_TWO_PAIR_RANK_0:
    case AA_FOUR_TWO_PAIR_RANK_1:
    case AA_FOUR_TWO_PAIR_RANK_2:
    case AA_FOUR_TWO_PAIR_RANK_3:
    case AA_FOUR_TWO_PAIR_RANK_4:
    case AA_FOUR_TWO_PAIR_RANK_5:
    case AA_FOUR_TWO_PAIR_RANK_6:
    case AA_FOUR_TWO_PAIR_RANK_7:
    case AA_FOUR_TWO_PAIR_RANK_8:
    case AA_FOUR_TWO_PAIR_RANK_9:
    case AA_FOUR_TWO_PAIR_RANK_10:
    case AA_FOUR_TWO_PAIR_RANK_11:
    case AA_FOUR_TWO_PAIR_RANK_12:
        type = ACTION_TYPE_FOUR_TWO_PAIR;
        rank = abstractActionId - AA_FOUR_TWO_PAIR_RANK_0;
        get_TYPEX_CHAINX_ActionCards(4, 1, playerCards, rank, ad);
        get_PAIR_AttachmentCards(playerCards, ad, 2, ads);
        break;
    case AA_BOMB_RANK_0:
    case AA_BOMB_RANK_1:
    case AA_BOMB_RANK_2:
    case AA_BOMB_RANK_3:
    case AA_BOMB_RANK_4:
    case AA_BOMB_RANK_5:
    case AA_BOMB_RANK_6:
    case AA_BOMB_RANK_7:
    case AA_BOMB_RANK_8:
    case AA_BOMB_RANK_9:
    case AA_BOMB_RANK_10:
    case AA_BOMB_RANK_11:
    case AA_BOMB_RANK_12:
        type = ACTION_TYPE_BOMB;
        rank = abstractActionId - AA_BOMB_RANK_0;
        get_TYPEX_CHAINX_ActionCards(4, 1, playerCards, rank, ad);
        break;
    case AA_ROCKET_RANK_0:
        type = ACTION_TYPE_ROCKET;
        rank = 0;
        get_ROCKET_ActionCards(playerCards, rank, ad);
        break;
    case AA_PASS:
        type = ACTION_TYPE_PASS;
        rank = 0;
        get_PASS_ActionCards(playerCards, rank, ad);
        break;
    default:
        assert(false);
    }

    if (ads.empty())
    {
        ads.push_back(ad);
    }

    if (currPlayerId == Game::LAND_LORD_PLAYER_ID)
    {
        auto faceupCards = game.getCards().row(FACEUP_CARDS_ROW);
        for (int i = 0; i < 54; i++)
        {
            if (faceupCards[i] == 1)
            {
                if (i >= 52)
                {
                    continue;
                }
                
                for (auto pActionData : ads)
                {
                    if ((*pActionData)[i])
                    {
                        for (int j = i / 4 * 4; j < i / 4 * 4 + 4; j++)
                        {
                            if (j == i)
                            {
                                continue;
                            }
                            if (playerCards[j] && !(*pActionData)[j])
                            {
                                (*pActionData)[j] = 1;
                                (*pActionData)[i] = 0;
                                break;
                            } 
                        }
                    }
                }
            }
        }
    }

    for (auto pActionData : ads)
    {
        actions.push_back(std::unique_ptr<Action>(new Action(pActionData, type, rank)));
    }
}

int BANHNPPlayer::pickAbstractAction(Game &game)
{
    return 0;
}

} // namespace cfr
} // namespace caishen
