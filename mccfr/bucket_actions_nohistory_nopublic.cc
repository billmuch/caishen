#include "bucket_actions_nohistory_nopublic.h"
#include "doudizhu/doudizhu_utils.h"
#include "mem_informationset_store.h"

#include <iostream>
#include <string>
#include <algorithm>

namespace caishen
{
namespace cfr
{

using namespace caishen::doudizhu;

void BANHNPInformationSetKey::cards2ByteArray(const Eigen::Array<CARD_ARRAY_DATA_TYPE, 1, 54> & cards, unsigned char * bytes)
{
    for (int i = 0; i < 7; ++i)
    {
        bytes[i] = 0;
    }

    for (int i = 0; i < 54; ++i)
    {
        if (cards[i])
        {
            bytes[i/8] |= (1 << (i % 8));
        }
    }
}

BANHNPInformationSetKey::BANHNPInformationSetKey(const KeyDataType & keyData)
    : _data(keyData)
{

}

BANHNPInformationSetKey::BANHNPInformationSetKey(const caishen::doudizhu::Game & game)
{
    int currPlayerId = game.getPlayerId();

    _data[0] = static_cast<unsigned char>(currPlayerId);

    // 当前用户手牌 _data[1-7]
    cards2ByteArray(game.getCards().row(currPlayerId), _data.data() + 1);

    // 下个用户手牌数量_data[8]
    int nextPlayerId = (currPlayerId + 1)%3;
    _data[8] = static_cast<unsigned char>(game.getCards().row(nextPlayerId).sum());

    // 下下个用户手牌数量_data[9]
    nextPlayerId = (nextPlayerId + 1)%3;
    _data[9] = static_cast<unsigned char>(game.getCards().row(nextPlayerId).sum());

    // faceup _data[10-12]
    auto faceup = game.getCards().row(FACEUP_CARDS_ROW);
    int index = 10;
    for (int i = 0; i < 54; ++i)
    {
        if (faceup[i])
        {
            _data[index++] = i;
        }
    }
    assert(index == 13);

    // public _data[13-19]
    cards2ByteArray(game.getCards().row(PUBLIC_CARDS_ROW), _data.data() + 13);

    // history[-1] _data[20-26]
    auto & history = game.getHistory();
    if (!history.empty())
    {
        auto r = history.back();
        if (r.getDetails().first == Action_Type::ACTION_TYPE_PASS)
        {
            _data[26] |= (1 << 6);
        }
        else
        {
            cards2ByteArray(Eigen::Map<const Eigen::Array<CARD_ARRAY_DATA_TYPE, 1, 54>>(r.data()), _data.data() + 20);
        }
    }

    // history[-2] _data[27-33]
    if (history.size() > 1)
    {
        auto r = history[history.size() - 2];
        if (r.getDetails().first == Action_Type::ACTION_TYPE_PASS)
        {
            _data[33] |= (1 << 6);
        }
        else
        {
            cards2ByteArray(Eigen::Map<const Eigen::Array<CARD_ARRAY_DATA_TYPE, 1, 54>>(r.data()), _data.data() + 27);
        }
    }
}

bool BANHNPInformationSetKey::operator<(const BANHNPInformationSetKey& key) const
{
    return memcmp(_data.data(), key.data().data(), 34) < 0;
}

void BANHNPInformationSetKey::outputCards(std::ostream &strm, unsigned char * cardsInBytes)
{
    for (int i = 0; i < 54; i++)
    {
        if ( (cardsInBytes[i/8] & (1 << i % 8)) ) {
            strm << Deck::STANDARD_DECK[i];
        }
    }
}

std::string BANHNPInformationSetKey::str()
{
    std::ostringstream ostr;

    int currPlayerId = _data[0];

    // 当前用户手牌
    ostr << "P" << currPlayerId << "<";
    outputCards(ostr, _data.data() + 1);
    ostr << ">";

    // + 其余两个玩家的手牌数量
    int nextPlayerId = (currPlayerId + 1)%3;
    ostr << "P" << nextPlayerId << "<" << static_cast<int>(_data[8]) << ">";
    
    nextPlayerId = (nextPlayerId + 1)%3;
    ostr << "P" << nextPlayerId << "<" << static_cast<int>(_data[9]) << ">";

    // + FACEUP
    ostr << "FACEUP<" << Deck::STANDARD_DECK[_data[10]] << Deck::STANDARD_DECK[_data[11]] << Deck::STANDARD_DECK[_data[12]] << ">";

    // + PUBLIC
    ostr << "PUBLIC<";
    outputCards(ostr, _data.data() + 13);
    ostr << ">";

    // + 最近两次的history
    ostr << "H(-1)<";
    if ( (_data[26] & (1 << 6)) )
    {
        ostr << "PASS>";
    }
    else
    {
        outputCards(ostr, _data.data() + 20);
        ostr << ">";
    }

    ostr << "H(-2)<";
    if ( (_data[33] & (1 << 6)) )
    {
        ostr << "PASS>";
    }
    else
    {
        outputCards(ostr, _data.data() + 27);
        ostr << ">";
    }

    return ostr.str();
}

const BANHNPInformationSetKey::KeyDataType & BANHNPInformationSetKey::data() const
{
    return _data;
}

void UniformDistributionActionsProbabilities::getProbabilities(const caishen::doudizhu::Game &game, const std::vector<std::unique_ptr<caishen::doudizhu::Action>> &actions, std::vector<double> &probs)
{
    probs.clear();
    int size = actions.size();
    assert(size != 0);
    probs.assign(size, 1.0/size);
}

BANHNPPlayer::BANHNPPlayer(int playerId)
    : CFRDoudizhuPlayer(playerId)
{
}

BANHNPPlayer::~BANHNPPlayer()
{
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
    case ACTION_TYPE_PASS:
        for (Action_Rank r : availableRanks)
        {
            assert(r == 0);
            legalActions[r + AA_PASS] = 1;
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
        break;
    case AA_SOLO_CHAIN_7_RANK_0:
    case AA_SOLO_CHAIN_7_RANK_1:
    case AA_SOLO_CHAIN_7_RANK_2:
    case AA_SOLO_CHAIN_7_RANK_3:
    case AA_SOLO_CHAIN_7_RANK_4:
    case AA_SOLO_CHAIN_7_RANK_5:
        type = ACTION_TYPE_SOLO_CHAIN_7;
        rank = abstractActionId - AA_SOLO_CHAIN_7_RANK_0;
        break;
    case AA_SOLO_CHAIN_8_RANK_0:
    case AA_SOLO_CHAIN_8_RANK_1:
    case AA_SOLO_CHAIN_8_RANK_2:
    case AA_SOLO_CHAIN_8_RANK_3:
    case AA_SOLO_CHAIN_8_RANK_4:
        type = ACTION_TYPE_SOLO_CHAIN_8;
        rank = abstractActionId - AA_SOLO_CHAIN_8_RANK_0;
        break;
    case AA_SOLO_CHAIN_9_RANK_0:
    case AA_SOLO_CHAIN_9_RANK_1:
    case AA_SOLO_CHAIN_9_RANK_2:
    case AA_SOLO_CHAIN_9_RANK_3:
        type = ACTION_TYPE_SOLO_CHAIN_9;
        rank = abstractActionId - AA_SOLO_CHAIN_9_RANK_0;
        break;
    case AA_SOLO_CHAIN_10_RANK_0:
    case AA_SOLO_CHAIN_10_RANK_1:
    case AA_SOLO_CHAIN_10_RANK_2:
        type = ACTION_TYPE_SOLO_CHAIN_10;
        rank = abstractActionId - AA_SOLO_CHAIN_10_RANK_0;
        break;
    case AA_SOLO_CHAIN_11_RANK_0:
    case AA_SOLO_CHAIN_11_RANK_1:
        type = ACTION_TYPE_SOLO_CHAIN_11;
        rank = abstractActionId - AA_SOLO_CHAIN_11_RANK_0;
        break;
    case AA_SOLO_CHAIN_12_RANK_0:
        type = ACTION_TYPE_SOLO_CHAIN_12;
        rank = 0;
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
        break;
    case AA_PAIR_CHAIN_7_RANK_0:
    case AA_PAIR_CHAIN_7_RANK_1:
    case AA_PAIR_CHAIN_7_RANK_2:
    case AA_PAIR_CHAIN_7_RANK_3:
    case AA_PAIR_CHAIN_7_RANK_4:
    case AA_PAIR_CHAIN_7_RANK_5:
        type = ACTION_TYPE_PAIR_CHAIN_7;
        rank = abstractActionId - AA_PAIR_CHAIN_7_RANK_0;
        break;
    case AA_PAIR_CHAIN_8_RANK_0:
    case AA_PAIR_CHAIN_8_RANK_1:
    case AA_PAIR_CHAIN_8_RANK_2:
    case AA_PAIR_CHAIN_8_RANK_3:
    case AA_PAIR_CHAIN_8_RANK_4:
        type = ACTION_TYPE_PAIR_CHAIN_8;
        rank = abstractActionId - AA_PAIR_CHAIN_8_RANK_0;
        break;
    case AA_PAIR_CHAIN_9_RANK_0:
    case AA_PAIR_CHAIN_9_RANK_1:
    case AA_PAIR_CHAIN_9_RANK_2:
    case AA_PAIR_CHAIN_9_RANK_3:
        type = ACTION_TYPE_PAIR_CHAIN_9;
        rank = abstractActionId - AA_PAIR_CHAIN_9_RANK_0;
        break;
    case AA_PAIR_CHAIN_10_RANK_0:
    case AA_PAIR_CHAIN_10_RANK_1:
    case AA_PAIR_CHAIN_10_RANK_2:
        type = ACTION_TYPE_PAIR_CHAIN_10;
        rank = abstractActionId - AA_PAIR_CHAIN_10_RANK_0;
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
        break;
    case AA_ROCKET_RANK_0:
        type = ACTION_TYPE_ROCKET;
        rank = 0;
        break;
    case AA_PASS:
        type = ACTION_TYPE_PASS;
        rank = 0;
        break;
    default:
        assert(false);
    }

    game.getActionCards(type, rank, actions);
}

int BANHNPPlayer::pickAbstractAction(Game &game)
{
    auto piss = MemoryInformationSetStore::getInstance();
    InformationSet is(MemoryInformationSetStore::ABSTRACT_ACTION_SIZE);
    piss->getInformationSet(*(piss->getInformationSetKey(game)), is);

    auto i = std::max_element(is.currentPolicy.data(), is.currentPolicy.data() + is.cumulativePolicy.size());

    return i - is.currentPolicy.data();
}

} // namespace cfr
} // namespace caishen
