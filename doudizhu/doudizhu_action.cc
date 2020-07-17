#include "doudizhu_action.h"
#include "doudizhu_utils.h"

#include <Eigen/Dense>

namespace caishen
{
namespace doudizhu
{

bool Action::isValidTypeAndRank() const
{
    Action_Type type;
    Action_Rank rank;

    std::tie(type, rank) = getDetails();

    return isValidActionTypeAndRank(_pActionData, type, rank);
}

const std::string Action_Type_Strings[38] = {
    "ACTION_TYPE_SOLO",
    "ACTION_TYPE_SOLO_CHAIN_5",
    "ACTION_TYPE_SOLO_CHAIN_6",
    "ACTION_TYPE_SOLO_CHAIN_7",
    "ACTION_TYPE_SOLO_CHAIN_8",
    "ACTION_TYPE_SOLO_CHAIN_9",
    "ACTION_TYPE_SOLO_CHAIN_10",
    "ACTION_TYPE_SOLO_CHAIN_11",
    "ACTION_TYPE_SOLO_CHAIN_12",
    "ACTION_TYPE_PAIR",
    "ACTION_TYPE_PAIR_CHAIN_3",
    "ACTION_TYPE_PAIR_CHAIN_4",
    "ACTION_TYPE_PAIR_CHAIN_5",
    "ACTION_TYPE_PAIR_CHAIN_6",
    "ACTION_TYPE_PAIR_CHAIN_7",
    "ACTION_TYPE_PAIR_CHAIN_8",
    "ACTION_TYPE_PAIR_CHAIN_9",
    "ACTION_TYPE_PAIR_CHAIN_10",
    "ACTION_TYPE_TRIO",
    "ACTION_TYPE_TRIO_CHAIN_2",
    "ACTION_TYPE_TRIO_CHAIN_3",
    "ACTION_TYPE_TRIO_CHAIN_4",
    "ACTION_TYPE_TRIO_CHAIN_5",
    "ACTION_TYPE_TRIO_CHAIN_6",
    "ACTION_TYPE_TRIO_SOLO",
    "ACTION_TYPE_TRIO_SOLO_CHAIN_2",
    "ACTION_TYPE_TRIO_SOLO_CHAIN_3",
    "ACTION_TYPE_TRIO_SOLO_CHAIN_4",
    "ACTION_TYPE_TRIO_SOLO_CHAIN_5",
    "ACTION_TYPE_TRIO_PAIR",
    "ACTION_TYPE_TRIO_PAIR_CHAIN_2",
    "ACTION_TYPE_TRIO_PAIR_CHAIN_3",
    "ACTION_TYPE_TRIO_PAIR_CHAIN_4",
    "ACTION_TYPE_FOUR_TWO_SOLO",
    "ACTION_TYPE_FOUR_TWO_PAIR",
    "ACTION_TYPE_BOMB",
    "ACTION_TYPE_ROCKET",
    "ACTION_TYPE_PASS"
};

std::string Action::toString()
{
    std::ostringstream oss;

    oss << "(" << Action_Type_Strings[_details.first] 
        << ", " << _details.second << "): \"";


    if (_details.first != ACTION_TYPE_PASS)
    {
        for (int i = 0; i < 54; i++)
        {
            if ((*_pActionData)[i])
            oss << Deck::STANDARD_DECK[i];
        }
    }

    oss << "\"";
    return oss.str();
}

} // namespace doudizhu
} // namespace caishen
