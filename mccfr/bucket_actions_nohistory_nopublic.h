#ifndef CAISHEN_MCCFR_BUCKET_ACTIONS_NOHISTORY_NOPUBLIC_H_
#define CAISHEN_MCCFR_BUCKET_ACTIONS_NOHISTORY_NOPUBLIC_H_

/**
 * @file bucket_actions_nohistory_nopublic.h
 * @author your name (you@domain.com)
 * @brief 文件定义了斗地主AbstractAction和AbstractInformationSet的算法。
 *        抽象可以做得选择：
 *          1. 手牌（原始action）组合 + history
 *          2. 手牌 + 2个对家的手牌数量 + histoory(2) + public
 *          3. AbstractAction组合 + 2个对家的手牌数量 + 当前回合中最大的action + public
 *          4. abstractAction组合 + 2个对家的手牌数量 + 当前回合中最大的action + public牌数 + public中的大牌（JQKA2等及4张全出完的rank）
 *          5. abstractAction组合 + 两个对家少于5张手牌时的具体手牌数量 + 当前回合中最大的action + public牌数 + public中的大牌（JQKA2等及4张全出完的rank）
 * 
 */

#include "doudizhu/doudizhu_game.h"
#include "cfr_doudizhu_player.h"

#include <Eigen/Dense>

namespace caishen
{
namespace cfr
{

enum AbstractAction
{
    AA_SOLO_RANK_0 = 0,
    AA_SOLO_RANK_1,
    AA_SOLO_RANK_2,
    AA_SOLO_RANK_3,
    AA_SOLO_RANK_4,
    AA_SOLO_RANK_5,
    AA_SOLO_RANK_6,
    AA_SOLO_RANK_7,
    AA_SOLO_RANK_8,
    AA_SOLO_RANK_9,
    AA_SOLO_RANK_10,
    AA_SOLO_RANK_11,
    AA_SOLO_RANK_12,
    AA_SOLO_RANK_13,
    AA_SOLO_RANK_14,
    AA_SOLO_CHAIN_5_RANK_0,
    AA_SOLO_CHAIN_5_RANK_1,
    AA_SOLO_CHAIN_5_RANK_2,
    AA_SOLO_CHAIN_5_RANK_3,
    AA_SOLO_CHAIN_5_RANK_4,
    AA_SOLO_CHAIN_5_RANK_5,
    AA_SOLO_CHAIN_5_RANK_6,
    AA_SOLO_CHAIN_5_RANK_7,
    AA_SOLO_CHAIN_6_RANK_0,
    AA_SOLO_CHAIN_6_RANK_1,
    AA_SOLO_CHAIN_6_RANK_2,
    AA_SOLO_CHAIN_6_RANK_3,
    AA_SOLO_CHAIN_6_RANK_4,
    AA_SOLO_CHAIN_6_RANK_5,
    AA_SOLO_CHAIN_6_RANK_6,
    AA_SOLO_CHAIN_7_RANK_0,
    AA_SOLO_CHAIN_7_RANK_1,
    AA_SOLO_CHAIN_7_RANK_2,
    AA_SOLO_CHAIN_7_RANK_3,
    AA_SOLO_CHAIN_7_RANK_4,
    AA_SOLO_CHAIN_7_RANK_5,
    AA_SOLO_CHAIN_8_RANK_0,
    AA_SOLO_CHAIN_8_RANK_1,
    AA_SOLO_CHAIN_8_RANK_2,
    AA_SOLO_CHAIN_8_RANK_3,
    AA_SOLO_CHAIN_8_RANK_4,
    AA_SOLO_CHAIN_9_RANK_0,
    AA_SOLO_CHAIN_9_RANK_1,
    AA_SOLO_CHAIN_9_RANK_2,
    AA_SOLO_CHAIN_9_RANK_3,
    AA_SOLO_CHAIN_10_RANK_0,
    AA_SOLO_CHAIN_10_RANK_1,
    AA_SOLO_CHAIN_10_RANK_2,
    AA_SOLO_CHAIN_11_RANK_0,
    AA_SOLO_CHAIN_11_RANK_1,
    AA_SOLO_CHAIN_12_RANK_0,
    AA_PAIR_RANK_0,
    AA_PAIR_RANK_1,
    AA_PAIR_RANK_2,
    AA_PAIR_RANK_3,
    AA_PAIR_RANK_4,
    AA_PAIR_RANK_5,
    AA_PAIR_RANK_6,
    AA_PAIR_RANK_7,
    AA_PAIR_RANK_8,
    AA_PAIR_RANK_9,
    AA_PAIR_RANK_10,
    AA_PAIR_RANK_11,
    AA_PAIR_RANK_12,
    AA_PAIR_CHAIN_3_RANK_0,
    AA_PAIR_CHAIN_3_RANK_1,
    AA_PAIR_CHAIN_3_RANK_2,
    AA_PAIR_CHAIN_3_RANK_3,
    AA_PAIR_CHAIN_3_RANK_4,
    AA_PAIR_CHAIN_3_RANK_5,
    AA_PAIR_CHAIN_3_RANK_6,
    AA_PAIR_CHAIN_3_RANK_7,
    AA_PAIR_CHAIN_3_RANK_8,
    AA_PAIR_CHAIN_3_RANK_9,
    AA_PAIR_CHAIN_4_RANK_0,
    AA_PAIR_CHAIN_4_RANK_1,
    AA_PAIR_CHAIN_4_RANK_2,
    AA_PAIR_CHAIN_4_RANK_3,
    AA_PAIR_CHAIN_4_RANK_4,
    AA_PAIR_CHAIN_4_RANK_5,
    AA_PAIR_CHAIN_4_RANK_6,
    AA_PAIR_CHAIN_4_RANK_7,
    AA_PAIR_CHAIN_4_RANK_8,
    AA_PAIR_CHAIN_5_RANK_0,
    AA_PAIR_CHAIN_5_RANK_1,
    AA_PAIR_CHAIN_5_RANK_2,
    AA_PAIR_CHAIN_5_RANK_3,
    AA_PAIR_CHAIN_5_RANK_4,
    AA_PAIR_CHAIN_5_RANK_5,
    AA_PAIR_CHAIN_5_RANK_6,
    AA_PAIR_CHAIN_5_RANK_7,
    AA_PAIR_CHAIN_6_RANK_0,
    AA_PAIR_CHAIN_6_RANK_1,
    AA_PAIR_CHAIN_6_RANK_2,
    AA_PAIR_CHAIN_6_RANK_3,
    AA_PAIR_CHAIN_6_RANK_4,
    AA_PAIR_CHAIN_6_RANK_5,
    AA_PAIR_CHAIN_6_RANK_6,
    AA_PAIR_CHAIN_7_RANK_0,
    AA_PAIR_CHAIN_7_RANK_1,
    AA_PAIR_CHAIN_7_RANK_2,
    AA_PAIR_CHAIN_7_RANK_3,
    AA_PAIR_CHAIN_7_RANK_4,
    AA_PAIR_CHAIN_7_RANK_5,
    AA_PAIR_CHAIN_8_RANK_0,
    AA_PAIR_CHAIN_8_RANK_1,
    AA_PAIR_CHAIN_8_RANK_2,
    AA_PAIR_CHAIN_8_RANK_3,
    AA_PAIR_CHAIN_8_RANK_4,
    AA_PAIR_CHAIN_9_RANK_0,
    AA_PAIR_CHAIN_9_RANK_1,
    AA_PAIR_CHAIN_9_RANK_2,
    AA_PAIR_CHAIN_9_RANK_3,
    AA_PAIR_CHAIN_10_RANK_0,
    AA_PAIR_CHAIN_10_RANK_1,
    AA_PAIR_CHAIN_10_RANK_2,
    AA_TRIO_RANK_0,
    AA_TRIO_RANK_1,
    AA_TRIO_RANK_2,
    AA_TRIO_RANK_3,
    AA_TRIO_RANK_4,
    AA_TRIO_RANK_5,
    AA_TRIO_RANK_6,
    AA_TRIO_RANK_7,
    AA_TRIO_RANK_8,
    AA_TRIO_RANK_9,
    AA_TRIO_RANK_10,
    AA_TRIO_RANK_11,
    AA_TRIO_RANK_12,
    AA_TRIO_CHAIN_2_RANK_0,
    AA_TRIO_CHAIN_2_RANK_1,
    AA_TRIO_CHAIN_2_RANK_2,
    AA_TRIO_CHAIN_2_RANK_3,
    AA_TRIO_CHAIN_2_RANK_4,
    AA_TRIO_CHAIN_2_RANK_5,
    AA_TRIO_CHAIN_2_RANK_6,
    AA_TRIO_CHAIN_2_RANK_7,
    AA_TRIO_CHAIN_2_RANK_8,
    AA_TRIO_CHAIN_2_RANK_9,
    AA_TRIO_CHAIN_2_RANK_10,
    AA_TRIO_CHAIN_3_RANK_0,
    AA_TRIO_CHAIN_3_RANK_1,
    AA_TRIO_CHAIN_3_RANK_2,
    AA_TRIO_CHAIN_3_RANK_3,
    AA_TRIO_CHAIN_3_RANK_4,
    AA_TRIO_CHAIN_3_RANK_5,
    AA_TRIO_CHAIN_3_RANK_6,
    AA_TRIO_CHAIN_3_RANK_7,
    AA_TRIO_CHAIN_3_RANK_8,
    AA_TRIO_CHAIN_3_RANK_9,
    AA_TRIO_CHAIN_4_RANK_0,
    AA_TRIO_CHAIN_4_RANK_1,
    AA_TRIO_CHAIN_4_RANK_2,
    AA_TRIO_CHAIN_4_RANK_3,
    AA_TRIO_CHAIN_4_RANK_4,
    AA_TRIO_CHAIN_4_RANK_5,
    AA_TRIO_CHAIN_4_RANK_6,
    AA_TRIO_CHAIN_4_RANK_7,
    AA_TRIO_CHAIN_4_RANK_8,
    AA_TRIO_CHAIN_5_RANK_0,
    AA_TRIO_CHAIN_5_RANK_1,
    AA_TRIO_CHAIN_5_RANK_2,
    AA_TRIO_CHAIN_5_RANK_3,
    AA_TRIO_CHAIN_5_RANK_4,
    AA_TRIO_CHAIN_5_RANK_5,
    AA_TRIO_CHAIN_5_RANK_6,
    AA_TRIO_CHAIN_5_RANK_7,
    AA_TRIO_CHAIN_6_RANK_0,
    AA_TRIO_CHAIN_6_RANK_1,
    AA_TRIO_CHAIN_6_RANK_2,
    AA_TRIO_CHAIN_6_RANK_3,
    AA_TRIO_CHAIN_6_RANK_4,
    AA_TRIO_CHAIN_6_RANK_5,
    AA_TRIO_CHAIN_6_RANK_6,
    AA_TRIO_SOLO_RANK_0,
    AA_TRIO_SOLO_RANK_1,
    AA_TRIO_SOLO_RANK_2,
    AA_TRIO_SOLO_RANK_3,
    AA_TRIO_SOLO_RANK_4,
    AA_TRIO_SOLO_RANK_5,
    AA_TRIO_SOLO_RANK_6,
    AA_TRIO_SOLO_RANK_7,
    AA_TRIO_SOLO_RANK_8,
    AA_TRIO_SOLO_RANK_9,
    AA_TRIO_SOLO_RANK_10,
    AA_TRIO_SOLO_RANK_11,
    AA_TRIO_SOLO_RANK_12,
    AA_TRIO_SOLO_CHAIN_2_RANK_0,
    AA_TRIO_SOLO_CHAIN_2_RANK_1,
    AA_TRIO_SOLO_CHAIN_2_RANK_2,
    AA_TRIO_SOLO_CHAIN_2_RANK_3,
    AA_TRIO_SOLO_CHAIN_2_RANK_4,
    AA_TRIO_SOLO_CHAIN_2_RANK_5,
    AA_TRIO_SOLO_CHAIN_2_RANK_6,
    AA_TRIO_SOLO_CHAIN_2_RANK_7,
    AA_TRIO_SOLO_CHAIN_2_RANK_8,
    AA_TRIO_SOLO_CHAIN_2_RANK_9,
    AA_TRIO_SOLO_CHAIN_2_RANK_10,
    AA_TRIO_SOLO_CHAIN_3_RANK_0,
    AA_TRIO_SOLO_CHAIN_3_RANK_1,
    AA_TRIO_SOLO_CHAIN_3_RANK_2,
    AA_TRIO_SOLO_CHAIN_3_RANK_3,
    AA_TRIO_SOLO_CHAIN_3_RANK_4,
    AA_TRIO_SOLO_CHAIN_3_RANK_5,
    AA_TRIO_SOLO_CHAIN_3_RANK_6,
    AA_TRIO_SOLO_CHAIN_3_RANK_7,
    AA_TRIO_SOLO_CHAIN_3_RANK_8,
    AA_TRIO_SOLO_CHAIN_3_RANK_9,
    AA_TRIO_SOLO_CHAIN_4_RANK_0,
    AA_TRIO_SOLO_CHAIN_4_RANK_1,
    AA_TRIO_SOLO_CHAIN_4_RANK_2,
    AA_TRIO_SOLO_CHAIN_4_RANK_3,
    AA_TRIO_SOLO_CHAIN_4_RANK_4,
    AA_TRIO_SOLO_CHAIN_4_RANK_5,
    AA_TRIO_SOLO_CHAIN_4_RANK_6,
    AA_TRIO_SOLO_CHAIN_4_RANK_7,
    AA_TRIO_SOLO_CHAIN_4_RANK_8,
    AA_TRIO_SOLO_CHAIN_5_RANK_0,
    AA_TRIO_SOLO_CHAIN_5_RANK_1,
    AA_TRIO_SOLO_CHAIN_5_RANK_2,
    AA_TRIO_SOLO_CHAIN_5_RANK_3,
    AA_TRIO_SOLO_CHAIN_5_RANK_4,
    AA_TRIO_SOLO_CHAIN_5_RANK_5,
    AA_TRIO_SOLO_CHAIN_5_RANK_6,
    AA_TRIO_SOLO_CHAIN_5_RANK_7,
    AA_TRIO_PAIR_RANK_0,
    AA_TRIO_PAIR_RANK_1,
    AA_TRIO_PAIR_RANK_2,
    AA_TRIO_PAIR_RANK_3,
    AA_TRIO_PAIR_RANK_4,
    AA_TRIO_PAIR_RANK_5,
    AA_TRIO_PAIR_RANK_6,
    AA_TRIO_PAIR_RANK_7,
    AA_TRIO_PAIR_RANK_8,
    AA_TRIO_PAIR_RANK_9,
    AA_TRIO_PAIR_RANK_10,
    AA_TRIO_PAIR_RANK_11,
    AA_TRIO_PAIR_RANK_12,
    AA_TRIO_PAIR_CHAIN_2_RANK_0,
    AA_TRIO_PAIR_CHAIN_2_RANK_1,
    AA_TRIO_PAIR_CHAIN_2_RANK_2,
    AA_TRIO_PAIR_CHAIN_2_RANK_3,
    AA_TRIO_PAIR_CHAIN_2_RANK_4,
    AA_TRIO_PAIR_CHAIN_2_RANK_5,
    AA_TRIO_PAIR_CHAIN_2_RANK_6,
    AA_TRIO_PAIR_CHAIN_2_RANK_7,
    AA_TRIO_PAIR_CHAIN_2_RANK_8,
    AA_TRIO_PAIR_CHAIN_2_RANK_9,
    AA_TRIO_PAIR_CHAIN_2_RANK_10,
    AA_TRIO_PAIR_CHAIN_3_RANK_0,
    AA_TRIO_PAIR_CHAIN_3_RANK_1,
    AA_TRIO_PAIR_CHAIN_3_RANK_2,
    AA_TRIO_PAIR_CHAIN_3_RANK_3,
    AA_TRIO_PAIR_CHAIN_3_RANK_4,
    AA_TRIO_PAIR_CHAIN_3_RANK_5,
    AA_TRIO_PAIR_CHAIN_3_RANK_6,
    AA_TRIO_PAIR_CHAIN_3_RANK_7,
    AA_TRIO_PAIR_CHAIN_3_RANK_8,
    AA_TRIO_PAIR_CHAIN_3_RANK_9,
    AA_TRIO_PAIR_CHAIN_4_RANK_0,
    AA_TRIO_PAIR_CHAIN_4_RANK_1,
    AA_TRIO_PAIR_CHAIN_4_RANK_2,
    AA_TRIO_PAIR_CHAIN_4_RANK_3,
    AA_TRIO_PAIR_CHAIN_4_RANK_4,
    AA_TRIO_PAIR_CHAIN_4_RANK_5,
    AA_TRIO_PAIR_CHAIN_4_RANK_6,
    AA_TRIO_PAIR_CHAIN_4_RANK_7,
    AA_TRIO_PAIR_CHAIN_4_RANK_8,
    AA_FOUR_TWO_SOLO_RANK_0,
    AA_FOUR_TWO_SOLO_RANK_1,
    AA_FOUR_TWO_SOLO_RANK_2,
    AA_FOUR_TWO_SOLO_RANK_3,
    AA_FOUR_TWO_SOLO_RANK_4,
    AA_FOUR_TWO_SOLO_RANK_5,
    AA_FOUR_TWO_SOLO_RANK_6,
    AA_FOUR_TWO_SOLO_RANK_7,
    AA_FOUR_TWO_SOLO_RANK_8,
    AA_FOUR_TWO_SOLO_RANK_9,
    AA_FOUR_TWO_SOLO_RANK_10,
    AA_FOUR_TWO_SOLO_RANK_11,
    AA_FOUR_TWO_SOLO_RANK_12,
    AA_FOUR_TWO_PAIR_RANK_0,
    AA_FOUR_TWO_PAIR_RANK_1,
    AA_FOUR_TWO_PAIR_RANK_2,
    AA_FOUR_TWO_PAIR_RANK_3,
    AA_FOUR_TWO_PAIR_RANK_4,
    AA_FOUR_TWO_PAIR_RANK_5,
    AA_FOUR_TWO_PAIR_RANK_6,
    AA_FOUR_TWO_PAIR_RANK_7,
    AA_FOUR_TWO_PAIR_RANK_8,
    AA_FOUR_TWO_PAIR_RANK_9,
    AA_FOUR_TWO_PAIR_RANK_10,
    AA_FOUR_TWO_PAIR_RANK_11,
    AA_FOUR_TWO_PAIR_RANK_12,
    AA_BOMB_RANK_0,
    AA_BOMB_RANK_1,
    AA_BOMB_RANK_2,
    AA_BOMB_RANK_3,
    AA_BOMB_RANK_4,
    AA_BOMB_RANK_5,
    AA_BOMB_RANK_6,
    AA_BOMB_RANK_7,
    AA_BOMB_RANK_8,
    AA_BOMB_RANK_9,
    AA_BOMB_RANK_10,
    AA_BOMB_RANK_11,
    AA_BOMB_RANK_12,
    AA_ROCKET_RANK_0,
    AA_PASS
};

class BANHNPInformationSetKey
{
public: 
    typedef std::array<unsigned char, 34> KeyDataType;

public:
    BANHNPInformationSetKey(const caishen::doudizhu::Game & game);
    BANHNPInformationSetKey(const KeyDataType & keyData);
    
    virtual ~BANHNPInformationSetKey() {};

    bool operator<(const BANHNPInformationSetKey& key) const;

    virtual std::string str();

    const KeyDataType & data() const;

private:
    // 当前用户id, 1byte
    // 当前用户手牌，7bytes
    // 其余两个玩家手牌数量，2bytes
    // faceup 3张牌，3bytes，
    // public，7bytes
    // 最近2次history, 2*7bytes
    std::array<unsigned char, 34> _data;

private:
    static void cards2ByteArray(const Eigen::Array<caishen::doudizhu::CARD_ARRAY_DATA_TYPE, 1, 54> & cards, unsigned char * bytes);
    static void outputCards(std::ostream &strm, unsigned char * cardsInBytes);
};

struct BANHNPInformationSetKeyWrapper
{
    BANHNPInformationSetKeyWrapper(const BANHNPInformationSetKey & key) : _pKey(new BANHNPInformationSetKey(key)) {}
    BANHNPInformationSetKeyWrapper(const BANHNPInformationSetKey::KeyDataType & keyData) : _pKey(new BANHNPInformationSetKey(keyData)) {}
    BANHNPInformationSetKeyWrapper(const BANHNPInformationSetKeyWrapper & keyWrapper) : _pKey(keyWrapper._pKey) {}
    BANHNPInformationSetKeyWrapper(const std::shared_ptr<BANHNPInformationSetKey> & pKeyWrapper) : _pKey(pKeyWrapper) {}

    std::shared_ptr<BANHNPInformationSetKey> _pKey;

    bool operator<(const BANHNPInformationSetKeyWrapper& other) const
    {
        return *(_pKey) < *(other._pKey);
    }
};

struct BANHNPInformationSet : public BANHNPInformationSetKeyWrapper
{
    BANHNPInformationSet(const BANHNPInformationSetKeyWrapper & keyWrapper, const std::shared_ptr<InformationSet> &pis)
        :BANHNPInformationSetKeyWrapper(keyWrapper), _pIs(pis)
    {}

    BANHNPInformationSet(const BANHNPInformationSet & banhnpInformationSet)
        :BANHNPInformationSetKeyWrapper(banhnpInformationSet._pKey), _pIs(banhnpInformationSet._pIs)
    {}

    std::shared_ptr<InformationSet> _pIs;
};

class UniformDistributionActionsProbabilities
{
public:
    static void getProbabilities(const caishen::doudizhu::Game &game, const std::vector<std::unique_ptr<caishen::doudizhu::Action>> &actions, std::vector<double> &probs);
private:
    UniformDistributionActionsProbabilities(const caishen::doudizhu::Game &game, const std::vector<std::unique_ptr<caishen::doudizhu::Action>> &actions);
};

class BANHNPPlayer : public CFRDoudizhuPlayer
{
public:
    BANHNPPlayer(int playerId);
    virtual ~BANHNPPlayer() override;

    virtual void getLegalAbstractActions(caishen::doudizhu::Game& game, Eigen::Ref<AbstractActionSpace> legalActions) override;
    virtual void abstractAction2ActionCards(caishen::doudizhu::Game &game, int abstractActionId, std::vector<std::unique_ptr<caishen::doudizhu::Action>> & actions) override;

    virtual int pickAbstractAction(caishen::doudizhu::Game &game) override;

private:
    inline static void action2AbstractAction(caishen::doudizhu::Action_Type aType, std::vector<caishen::doudizhu::Action_Rank> &availableRanks, Eigen::Ref<AbstractActionSpace> legalActions);
};

} // namespace cfr
} // namespace caishen

#endif