#ifndef CAISHEN_DOUDIZHU_DOUDIZHU_UTILS_H_
#define CAISHEN_DOUDIZHU_DOUDIZHU_UTILS_H_

#include "doudizhu_card.h"
#include "doudizhu_types.h"
#include "doudizhu_action.h"

namespace caishen
{
namespace doudizhu
{

bool cardsFromString(Eigen::Ref<Cards_54_1d_Type> cards, const std::string cardsString);

bool isValidActionTypeAndRank(const std::shared_ptr<const ActionData>& pActionData, Action_Type type, Action_Rank rank);

void getChainIndexes(const Eigen::Array<CARD_ARRAY_DATA_TYPE, 1, 12> & cards, 
                            std::vector<std::pair<int, int>> & soloChainIndexes,
                            std::vector<std::pair<int, int>> & pairChainIndexes,
                            std::vector<std::pair<int, int>> & trioChainIndexes);

void get_SOLO_ActionCards(const Cards_54_1d_Type & playerCards, Action_Rank rank, std::shared_ptr<ActionData>& pActionData);
inline void get_ROCKET_ActionCards(const Cards_54_1d_Type & playerCards, Action_Rank rank, std::shared_ptr<ActionData>& pActionData)
{
    assert(playerCards[52] == 1 && playerCards[53] == 1);
    (*pActionData)[52] = 1;
    (*pActionData)[53] = 1;
}
inline void get_PASS_ActionCards(const Cards_54_1d_Type & playerCards, Action_Rank rank, std::shared_ptr<ActionData>& pActionData)
{
    (*pActionData)[54] = 1;
}
void get_TYPEX_CHAINX_ActionCards(int TX, int CX, const Cards_54_1d_Type & playerCards, Action_Rank rank, std::shared_ptr<ActionData>& pActionData);
void get_SOLO_AttachmentCards(const Cards_54_1d_Type &playerCards, const std::shared_ptr<ActionData> & pActionData, 
        int n, int excludeLeftTrio, int excludeRightTrio, std::vector<std::shared_ptr<ActionData>> &pActionsWithAttachs);
void get_PAIR_AttachmentCards(const Cards_54_1d_Type &playerCards, const std::shared_ptr<ActionData> & pActionData, 
        int n, std::vector<std::shared_ptr<ActionData>> &pActionsWithAttachs);


} // namespace doudizhu
} // namespace caishen

#endif
