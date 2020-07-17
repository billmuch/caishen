#ifndef CAISHEN_DOUDIZHU_DOUDIZHU_ACTION_H_
#define CAISHEN_DOUDIZHU_DOUDIZHU_ACTION_H_

#include "doudizhu_types.h"

namespace caishen
{
namespace doudizhu
{

#define ACTION_TYPE_SIZE 38

/**
 * @brief Types of doudizhu action
 * 
 */
enum Action_Type
{
    ACTION_TYPE_SOLO = 0,
    ACTION_TYPE_SOLO_CHAIN_5,
    ACTION_TYPE_SOLO_CHAIN_6,
    ACTION_TYPE_SOLO_CHAIN_7,
    ACTION_TYPE_SOLO_CHAIN_8,
    ACTION_TYPE_SOLO_CHAIN_9,
    ACTION_TYPE_SOLO_CHAIN_10,
    ACTION_TYPE_SOLO_CHAIN_11,
    ACTION_TYPE_SOLO_CHAIN_12,
    ACTION_TYPE_PAIR,
    ACTION_TYPE_PAIR_CHAIN_3,
    ACTION_TYPE_PAIR_CHAIN_4,
    ACTION_TYPE_PAIR_CHAIN_5,
    ACTION_TYPE_PAIR_CHAIN_6,
    ACTION_TYPE_PAIR_CHAIN_7,
    ACTION_TYPE_PAIR_CHAIN_8,
    ACTION_TYPE_PAIR_CHAIN_9,
    ACTION_TYPE_PAIR_CHAIN_10,
    ACTION_TYPE_TRIO,
    ACTION_TYPE_TRIO_CHAIN_2,
    ACTION_TYPE_TRIO_CHAIN_3,
    ACTION_TYPE_TRIO_CHAIN_4,
    ACTION_TYPE_TRIO_CHAIN_5,
    ACTION_TYPE_TRIO_CHAIN_6,
    ACTION_TYPE_TRIO_SOLO,
    ACTION_TYPE_TRIO_SOLO_CHAIN_2,
    ACTION_TYPE_TRIO_SOLO_CHAIN_3,
    ACTION_TYPE_TRIO_SOLO_CHAIN_4,
    ACTION_TYPE_TRIO_SOLO_CHAIN_5,
    ACTION_TYPE_TRIO_PAIR,
    ACTION_TYPE_TRIO_PAIR_CHAIN_2,
    ACTION_TYPE_TRIO_PAIR_CHAIN_3,
    ACTION_TYPE_TRIO_PAIR_CHAIN_4,
    ACTION_TYPE_FOUR_TWO_SOLO,
    ACTION_TYPE_FOUR_TWO_PAIR,
    ACTION_TYPE_BOMB,
    ACTION_TYPE_ROCKET,
    ACTION_TYPE_PASS
};

/**
 * @brief Rank of doudizhu action
 * 
 * Action's Rank value equales to the lowest card rank of all the main cards (exclude attached cards) in the action except ACTION_TYPE_ROCKET.
 * For example, 
 *      "34567" of type ACTION_TYPE_SOLO_CHAIN_5's rank is 0, equals to Card 3's rank
 *      "2222" of type ACTION_TYPE_BOMB's rank is 12, equals to Card 2's rank
 *      "332222" of type ACTION_TYPE_FOUR_TWO_PAIR's rank is 12, equals to Card 2's rank. ('33' are attached cards and '2222' are main cards in this case)
 *      "BJRJ" of type ACTION_TYPE_ROCKET's rank is 0.
 * 
 */
typedef int Action_Rank;

typedef std::pair<Action_Type, Action_Rank> ActionDetails;

/**
 * @brief Represents cards a player played in one action. First 54 elements represents zero or one or severial cards from
 * a full 54 deck in 1d matrix just as @ref Cards_54_1d_Type. The last one is "Pass" in which the value of 1 means player 
 * played "Pass".
 * 
 * Also see @ref Cards_54_1d_Type. 
 * 
 */
typedef Eigen::Matrix<CARD_ARRAY_DATA_TYPE, 1, 55> ActionData;

/**
 * @brief Action class hold the action data and it's type and rank
 * 
 */
class Action
{
    friend bool operator==(const Action &, const Action &);
    friend bool operator!=(const Action &, const Action &);

public:
    Action(const std::shared_ptr<const ActionData> pActionData,
           Action_Type actionType,
           Action_Rank actionRank)
        : _pActionData(pActionData),
          _details(actionType, actionRank) {}

    ~Action() {}

    std::string toString();

    bool isValidTypeAndRank() const;

    inline const ActionDetails getDetails() const { return _details; }
    inline const std::shared_ptr<const ActionData> getActionData() const {return _pActionData; }
    inline const CARD_ARRAY_DATA_TYPE *data() const { return _pActionData->data(); }

private:
    const std::shared_ptr<const ActionData> _pActionData;
    ActionDetails _details;
};

inline bool operator==(const Action &la, const Action &ra)
{
    return ((la._details == ra._details) && (*la._pActionData) == (*ra._pActionData));
}

inline bool operator!=(const Action &la, const Action &ra)
{
    return !operator==(la, ra);
}

/**
 * @brief Represents the history of actions of the game
 * 
 */
typedef std::vector<Action> HistoryActions;

} // namespace doudizhu
} // namespace caishen

#endif
