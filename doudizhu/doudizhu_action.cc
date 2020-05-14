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

} // namespace doudizhu
} // namespace caishen
