#include "doudizhu_utils.h"
#include "doudizhu_action.h"

#include <unordered_map>

namespace caishen
{
namespace doudizhu
{

std::unordered_map<char, Suit> s_suitMap({{'S', SPADES}, {'H', HEARTS}, {'C', CLUBS}, {'D', DIAMONDS}, {'J', JOKERS}});

std::unordered_map<char, Rank> s_rankMap({{'3', THREE}, {'4', FOUR}, {'5', FIVE}, {'6', SIX}, {'7', SEVEN}, {'8', EIGHT}, {'9', NINE}, {'T', TEN}, {'J', JACK}, {'Q', QUEEN}, {'K', KING}, {'A', ACE}, {'2', TWO}, {'B', BLACK_JOKER}, {'R', RED_JOKER}});

bool cardsFromString(Eigen::Ref<Cards_54_1d_Type> cards, const std::string cardsString)
{
    cards.setZero();

    if (cards.size() % 2 != 0)
        return false;

    for (auto iter = cardsString.begin(); iter < cardsString.end(); iter += 2)
    {
        auto sp = s_suitMap.find(*iter);
        if (sp == s_suitMap.end())
            return false;
        Suit s = sp->second;
        auto rp = s_rankMap.find(*(iter + 1));
        if (rp == s_rankMap.end())
            return false;
        Rank r = rp->second;

        int index = Deck::indexInStandardDeck(s, r);
        if (cards[index] == 1)
            return false;
        cards[index] = 1;
    }

    return true;
}

const int s_actionTypeSumCards[] = {
    1, 5, 6, 7, 8, 9, 10, 11,
    12, 2, 6, 8, 10, 12, 14, 16,
    18, 20, 3, 6, 9, 12, 15, 18,
    4, 8, 12, 16, 20, 5, 10, 15,
    20, 6, 8, 4, 2, 1};

const Action_Rank s_actionTypeMaxRank[] = {
    14, 7, 6, 5, 4, 3, 2, 1,
    0, 12, 9, 8, 7, 6, 5, 4,
    3, 2, 12, 10, 9, 8, 7, 6,
    12, 10, 9, 8, 7, 12, 10, 9,
    8, 12, 12, 12, 0, 0};

//Another way to implement this function is to use have a pattern column sum array for each Action_Type and Action_Rank
// and compare it with the colSum.
//For example: the pattern for SOLO_CHAIN_5, rank[0] is Array<CARD_ARRAY_DATA_TYPE, 1, 13>({1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}),
//  the colSum calucated from teh ActionData should be the same.
#define BOTH_BJ_AND_RJ ((*pActionData)[52] == 1 && (*pActionData)[53] == 1)
#define LEFT_ADJACENT_TRIO (colSum[rank - 1] == 3)
#define RIGHT_ADJACENT_TRIO(N) (colSum[rank + N] == 3)
#define CONTAIN_BOMB ((colSum == 4).any())
bool isValidActionTypeAndRank(const std::shared_ptr<const ActionData> &pActionData, Action_Type type, Action_Rank rank)
{
    if (type == Action_Type::ACTION_TYPE_PASS)
    {
        return ((*pActionData)[54] == 1);
    }

    if ((rank > s_actionTypeMaxRank[type]) || (pActionData->sum() != s_actionTypeSumCards[type]))
    {
        return false;
    }

    if (type == Action_Type::ACTION_TYPE_ROCKET)
    {
        return BOTH_BJ_AND_RJ;
    }

    if (type == Action_Type::ACTION_TYPE_SOLO)
    {
        if (rank == 14)
        {
            return (*pActionData)[53] == 1;
        }
        else if (rank == 13)
        {
            return (*pActionData)[52] == 1;
        }
        else
        {
            int i = rank * 4;
            return ((*pActionData)[i] == 1) || ((*pActionData)[i + 1] == 1) || ((*pActionData)[i + 2] == 1) || ((*pActionData)[i + 3] == 1);
        }
    }

    // TODO: use Game::_cachedPlayerHandsByRank instead of colSum
    auto colSum = Eigen::Map<const Cards_52_2d_Type>(pActionData->data()).colwise().sum();

    switch (type)
    {
    case Action_Type::ACTION_TYPE_PASS:
    case Action_Type::ACTION_TYPE_SOLO:
    case Action_Type::ACTION_TYPE_ROCKET:
        assert(false);
        break;
    case Action_Type::ACTION_TYPE_SOLO_CHAIN_5:
        return colSum[rank] == 1 && colSum[rank + 1] == 1 && colSum[rank + 2] == 1 && colSum[rank + 3] == 1 && colSum[rank + 4] == 1;
    case Action_Type::ACTION_TYPE_SOLO_CHAIN_6:
        return colSum[rank] == 1 && colSum[rank + 1] == 1 && colSum[rank + 2] == 1 && colSum[rank + 3] == 1 && colSum[rank + 4] == 1 && colSum[rank + 5] == 1;
    case Action_Type::ACTION_TYPE_SOLO_CHAIN_7:
        return colSum[rank] == 1 && colSum[rank + 1] == 1 && colSum[rank + 2] == 1 && colSum[rank + 3] == 1 && colSum[rank + 4] == 1 && colSum[rank + 5] == 1 && colSum[rank + 6] == 1;
    case Action_Type::ACTION_TYPE_SOLO_CHAIN_8:
        return colSum[rank] == 1 && colSum[rank + 1] == 1 && colSum[rank + 2] == 1 && colSum[rank + 3] == 1 && colSum[rank + 4] == 1 && colSum[rank + 5] == 1 && colSum[rank + 6] == 1 && colSum[rank + 7] == 1;
    case Action_Type::ACTION_TYPE_SOLO_CHAIN_9:
        return colSum[rank] == 1 && colSum[rank + 1] == 1 && colSum[rank + 2] == 1 && colSum[rank + 3] == 1 && colSum[rank + 4] == 1 && colSum[rank + 5] == 1 && colSum[rank + 6] == 1 && colSum[rank + 7] == 1 && colSum[rank + 8] == 1;
    case Action_Type::ACTION_TYPE_SOLO_CHAIN_10:
        return colSum[rank] == 1 && colSum[rank + 1] == 1 && colSum[rank + 2] == 1 && colSum[rank + 3] == 1 && colSum[rank + 4] == 1 && colSum[rank + 5] == 1 && colSum[rank + 6] == 1 && colSum[rank + 7] == 1 && colSum[rank + 8] == 1 && colSum[rank + 9] == 1;
    case Action_Type::ACTION_TYPE_SOLO_CHAIN_11:
        return colSum[rank] == 1 && colSum[rank + 1] == 1 && colSum[rank + 2] == 1 && colSum[rank + 3] == 1 && colSum[rank + 4] == 1 && colSum[rank + 5] == 1 && colSum[rank + 6] == 1 && colSum[rank + 7] == 1 && colSum[rank + 8] == 1 && colSum[rank + 9] == 1 && colSum[rank + 10] == 1;
    case Action_Type::ACTION_TYPE_SOLO_CHAIN_12:
        return colSum[rank] == 1 && colSum[rank + 1] == 1 && colSum[rank + 2] == 1 && colSum[rank + 3] == 1 && colSum[rank + 4] == 1 && colSum[rank + 5] == 1 && colSum[rank + 6] == 1 && colSum[rank + 7] == 1 && colSum[rank + 8] == 1 && colSum[rank + 9] == 1 && colSum[rank + 10] == 1 && colSum[rank + 11] == 1;
    case Action_Type::ACTION_TYPE_PAIR:
        return colSum[rank] == 2;
    case Action_Type::ACTION_TYPE_PAIR_CHAIN_3:
        return colSum[rank] == 2 && colSum[rank + 1] == 2 && colSum[rank + 2] == 2;
    case Action_Type::ACTION_TYPE_PAIR_CHAIN_4:
        return colSum[rank] == 2 && colSum[rank + 1] == 2 && colSum[rank + 2] == 2 && colSum[rank + 3] == 2;
    case Action_Type::ACTION_TYPE_PAIR_CHAIN_5:
        return colSum[rank] == 2 && colSum[rank + 1] == 2 && colSum[rank + 2] == 2 && colSum[rank + 3] == 2 && colSum[rank + 4] == 2;
    case Action_Type::ACTION_TYPE_PAIR_CHAIN_6:
        return colSum[rank] == 2 && colSum[rank + 1] == 2 && colSum[rank + 2] == 2 && colSum[rank + 3] == 2 && colSum[rank + 4] == 2 && colSum[rank + 5] == 2;
    case Action_Type::ACTION_TYPE_PAIR_CHAIN_7:
        return colSum[rank] == 2 && colSum[rank + 1] == 2 && colSum[rank + 2] == 2 && colSum[rank + 3] == 2 && colSum[rank + 4] == 2 && colSum[rank + 5] == 2 && colSum[rank + 6] == 2;
    case Action_Type::ACTION_TYPE_PAIR_CHAIN_8:
        return colSum[rank] == 2 && colSum[rank + 1] == 2 && colSum[rank + 2] == 2 && colSum[rank + 3] == 2 && colSum[rank + 4] == 2 && colSum[rank + 5] == 2 && colSum[rank + 6] == 2 && colSum[rank + 7] == 2;
    case Action_Type::ACTION_TYPE_PAIR_CHAIN_9:
        return colSum[rank] == 2 && colSum[rank + 1] == 2 && colSum[rank + 2] == 2 && colSum[rank + 3] == 2 && colSum[rank + 4] == 2 && colSum[rank + 5] == 2 && colSum[rank + 6] == 2 && colSum[rank + 7] == 2 && colSum[rank + 8] == 2;
    case Action_Type::ACTION_TYPE_PAIR_CHAIN_10:
        return colSum[rank] == 2 && colSum[rank + 1] == 2 && colSum[rank + 2] == 2 && colSum[rank + 3] == 2 && colSum[rank + 4] == 2 && colSum[rank + 5] == 2 && colSum[rank + 6] == 2 && colSum[rank + 7] == 2 && colSum[rank + 8] == 2 && colSum[rank + 9] == 2;
    case Action_Type::ACTION_TYPE_TRIO:
        return colSum[rank] == 3;
    case Action_Type::ACTION_TYPE_TRIO_CHAIN_2:
        return colSum[rank] == 3 && colSum[rank + 1] == 3;
    case Action_Type::ACTION_TYPE_TRIO_CHAIN_3:
        return colSum[rank] == 3 && colSum[rank + 1] == 3 && colSum[rank + 2] == 3;
    case Action_Type::ACTION_TYPE_TRIO_CHAIN_4:
        return colSum[rank] == 3 && colSum[rank + 1] == 3 && colSum[rank + 2] == 3 && colSum[rank + 3] == 3;
    case Action_Type::ACTION_TYPE_TRIO_CHAIN_5:
        return colSum[rank] == 3 && colSum[rank + 1] == 3 && colSum[rank + 2] == 3 && colSum[rank + 3] == 3 && colSum[rank + 4] == 3;
    case Action_Type::ACTION_TYPE_TRIO_CHAIN_6:
        return colSum[rank] == 3 && colSum[rank + 1] == 3 && colSum[rank + 2] == 3 && colSum[rank + 3] == 3 && colSum[rank + 4] == 3 && colSum[rank + 5] == 3;
    case Action_Type::ACTION_TYPE_TRIO_SOLO:
        return colSum[rank] == 3;
    case Action_Type::ACTION_TYPE_TRIO_SOLO_CHAIN_2:
        if (BOTH_BJ_AND_RJ)
        {
            return false;
        }
        return colSum[rank] == 3 && colSum[rank + 1] == 3;
    case Action_Type::ACTION_TYPE_TRIO_SOLO_CHAIN_3:
        if (BOTH_BJ_AND_RJ || (rank > 0 && LEFT_ADJACENT_TRIO) || (rank < s_actionTypeMaxRank[ACTION_TYPE_TRIO_SOLO_CHAIN_3] && RIGHT_ADJACENT_TRIO(3)))
        {
            return false;
        }
        return colSum[rank] == 3 && colSum[rank + 1] == 3 && colSum[rank + 2] == 3;
    case Action_Type::ACTION_TYPE_TRIO_SOLO_CHAIN_4:
        if (BOTH_BJ_AND_RJ || (rank > 0 && LEFT_ADJACENT_TRIO) || (rank < s_actionTypeMaxRank[ACTION_TYPE_TRIO_SOLO_CHAIN_4] && RIGHT_ADJACENT_TRIO(4)) || CONTAIN_BOMB)
        {
            return false;
        }
        return colSum[rank] == 3 && colSum[rank + 1] == 3 && colSum[rank + 2] == 3 && colSum[rank + 3] == 3;
    case Action_Type::ACTION_TYPE_TRIO_SOLO_CHAIN_5:
        if (BOTH_BJ_AND_RJ || (rank > 0 && LEFT_ADJACENT_TRIO) || (rank < s_actionTypeMaxRank[ACTION_TYPE_TRIO_SOLO_CHAIN_5] && RIGHT_ADJACENT_TRIO(5)) || CONTAIN_BOMB)
        {
            return false;
        }
        return colSum[rank] == 3 && colSum[rank + 1] == 3 && colSum[rank + 2] == 3 && colSum[rank + 3] == 3 && colSum[rank + 4] == 3;
    case Action_Type::ACTION_TYPE_TRIO_PAIR:
        return (colSum == 2).any() && colSum[rank] == 3;
    case Action_Type::ACTION_TYPE_TRIO_PAIR_CHAIN_2:
        return ((colSum == 2).count() == 2) && colSum[rank] == 3 && colSum[rank + 1] == 3;
    case Action_Type::ACTION_TYPE_TRIO_PAIR_CHAIN_3:
        return ((colSum == 2).count() == 3) && colSum[rank] == 3 && colSum[rank + 1] == 3 && colSum[rank + 2] == 3;
    case Action_Type::ACTION_TYPE_TRIO_PAIR_CHAIN_4:
        return ((colSum == 2).count() == 4) && colSum[rank] == 3 && colSum[rank + 1] == 3 && colSum[rank + 2] == 3 && colSum[rank + 3] == 3;
    case Action_Type::ACTION_TYPE_FOUR_TWO_SOLO:
        return colSum[rank] == 4 && !BOTH_BJ_AND_RJ;
    case Action_Type::ACTION_TYPE_FOUR_TWO_PAIR:
        return colSum[rank] == 4 && ((colSum == 2).count() == 2);
    case Action_Type::ACTION_TYPE_BOMB:
        return colSum[rank] == 4;
    default:
        assert(false);
        break;
    }

    assert(false);
    return false;
}

void get_SOLO_ActionCards(const Cards_54_1d_Type &playerCards, Action_Rank rank, std::shared_ptr<ActionData> &pActionData)
{
    if (rank == 13 || rank == 14)
    {
        (*pActionData)[39 + rank] = 1;
        return;
    }
    else
    {
        for (int i = rank * 4; i < rank * 4 + 4; i++)
        {
            if (playerCards[i])
            {
                (*pActionData)[i] = 1;
                return;
            }
        }
    }
    assert(false);
}

void get_TYPEX_CHAINX_ActionCards(int TX, int CX, const Cards_54_1d_Type &playerCards, Action_Rank rank, std::shared_ptr<ActionData> &pActionData)
{
    for (int r = rank; r < rank + CX; r++)
    {
        int selected = 0;
        for (int i = 0; i < 4; i++)
        {
            if (playerCards[r * 4 + i])
            {
                (*pActionData)[r * 4 + i] = 1;
                if (++selected == TX)
                {
                    break;
                }
            }
        }
        assert(selected == TX);
    }
}

void getChainIndexes(const Eigen::Array<CARD_ARRAY_DATA_TYPE, 1, 12> &cards,
                     std::vector<std::pair<int, int>> &soloChainIndexes,
                     std::vector<std::pair<int, int>> &pairChainIndexes,
                     std::vector<std::pair<int, int>> &trioChainIndexes)
{
    soloChainIndexes.clear();
    pairChainIndexes.clear();
    trioChainIndexes.clear();

    int soloIndexCount = 0;
    int soloIndexStart;
    int pairIndexCount = 0;
    int pairIndexStart;
    int trioIndexCount = 0;
    int trioIndexStart;

    for (int i = 0; i < 12; i++) // no chains for "2BR"
    {
        if (cards[i] >= 1)
        {
            if (soloIndexCount == 0)
            {
                soloIndexStart = i;
            }
            soloIndexCount++;
        }
        else
        {
            if (soloIndexCount > 0)
            {
                if (soloIndexCount >= 5)
                {
                    soloChainIndexes.push_back(std::pair<int, int>(soloIndexStart, soloIndexCount));
                }
                soloIndexStart = 0;
                soloIndexCount = 0;
            }
        }

        if (cards[i] >= 2)
        {
            if (pairIndexCount == 0)
            {
                pairIndexStart = i;
            }
            pairIndexCount++;
        }
        else
        {
            if (pairIndexCount > 0)
            {
                if (pairIndexCount >= 3)
                {
                    pairChainIndexes.push_back(std::pair<int, int>(pairIndexStart, pairIndexCount));
                }
                pairIndexStart = 0;
                pairIndexCount = 0;
            }
        }

        if (cards[i] >= 3)
        {
            if (trioIndexCount == 0)
            {
                trioIndexStart = i;
            }
            trioIndexCount++;
        }
        else
        {
            if (trioIndexCount > 0)
            {
                if (trioIndexCount >= 2)
                {
                    trioChainIndexes.push_back(std::pair<int, int>(trioIndexStart, trioIndexCount));
                }
                trioIndexStart = 0;
                trioIndexCount = 0;
            }
        }
    }

    if (soloIndexCount >= 5)
    {
        soloChainIndexes.push_back(std::pair<int, int>(soloIndexStart, soloIndexCount));
    }
    if (pairIndexCount >= 3)
    {
        pairChainIndexes.push_back(std::pair<int, int>(pairIndexStart, pairIndexCount));
    }
    if (trioIndexCount >= 2)
    {
        trioChainIndexes.push_back(std::pair<int, int>(trioIndexStart, trioIndexCount));
    }
}

/**
 * @brief Chose r cards from n[s:-1] cards, cards with the same rank considered the same cards.
 *        Equal to the problem of chose combination of r items from 15 class of items, each class can have 0 to 4 same items.
 *        Interesting problem.
 * 
 * @param candidateCards 
 * @param pActionsWithAttachs 
 * @param candidateRanks orderd candidate card ranks, example [0, 0, 1, 2, 2, 2] means 2 cards of rank 0, 1 card of rank 1, 3 cards of rank 2
 * @param selectedRanks cards already selected
 * @param n the length of array candidateRanks
 * @param r cards to be selected
 * @param s start index in the array
 */
static void addAttachedActions(const Cards_54_1d_Type &candidateCards, const std::shared_ptr<ActionData> & pActionData, std::vector<std::shared_ptr<ActionData>> &pActionsWithAttachs, std::vector<int> &candidateRanks, std::vector<int> & selectedRanks, int n, int r, int s)
{
    if (r == 0)
    {
        std::shared_ptr<ActionData> pAData(new ActionData(*pActionData));
        for (int i : selectedRanks)
        {
            if (i < 13)
            {
                for (int j = i * 4; j < i * 4 + 4; j++)
                {
                    if ((candidateCards[j] == 1) && ((*pAData)[j] == 0))
                    {
                        (*pAData)[j] = 1;
                        break;
                    }
                }
            }
            else if(i == 13)
            {
                (*pAData)[52] = 1;
            }
            else
            {
                (*pAData)[53] = 1;
            }
        }

        if (!((*pAData)[52] && (*pAData)[53]))
        {
            pActionsWithAttachs.push_back(pAData);
        }
        return;
    }

    if (n - s >= r)
    {
        selectedRanks.push_back(candidateRanks[s]);
        addAttachedActions(candidateCards, pActionData, pActionsWithAttachs, candidateRanks, selectedRanks, n, r - 1, s + 1);
        selectedRanks.pop_back();

        int i;
        for (i = s + 1; i < n; i++)
        {
            if (candidateRanks[i] != candidateRanks[s])
                break;
        }
        if (i != n)
        {
            addAttachedActions(candidateCards, pActionData, pActionsWithAttachs, candidateRanks, selectedRanks, n, r, i);
        }
    }
}

void get_SOLO_AttachmentCards(const Cards_54_1d_Type &playerCards, const std::shared_ptr<ActionData> & pActionData, 
    int n, int excludeLeftTrio, int excludeRightTrio, std::vector<std::shared_ptr<ActionData>> &pActionsWithAttachs)
{
    assert(excludeRightTrio < 12);

    pActionsWithAttachs.clear();

    Cards_54_1d_Type cards_54_1d = playerCards - Eigen::Map<Cards_54_1d_Type>(pActionData->data());
    Eigen::Array<CARD_ARRAY_DATA_TYPE, 1, 13> cards_13_1d = Eigen::Map<Cards_52_2d_Type>(cards_54_1d.data()).colwise().sum();
    Eigen::Array<CARD_ARRAY_DATA_TYPE, 1, 13> action_cards_13_1d = Eigen::Map<Cards_52_2d_Type>(pActionData->data()).colwise().sum();

    for (int i = 0; i < 13; i++)
    {
        // no 4
        if (cards_13_1d[i] == 4)
        {
            cards_13_1d[i] -= 1;
            for (int j = i * 4; j < i * 4 + 1; j++)
            {
                if (cards_54_1d[j])
                {
                    cards_54_1d[j] = 0;
                    break;
                }
            }
        }

        // no adjacent 3
        if (i == excludeLeftTrio || i == excludeRightTrio)
        {
            if (cards_13_1d[i] == 3)
            {
                cards_13_1d[i] -= 1;
                for (int j = i * 4; j < i * 4 + 1; j++)
                {
                    if (cards_54_1d[j])
                    {
                        cards_54_1d[j] = 0;
                        break;
                    }
                }
            }
        }

        // no 4th card of trio/trio_chain cards
        if (action_cards_13_1d[i])
        {
            cards_13_1d[i] -= 1;
            for (int j = i * 4; j < i * 4 + 1; j++)
            {
                if (cards_54_1d[j])
                {
                    cards_54_1d[j] = 0;
                    break;
                }
            }
        }
    }

    assert(cards_54_1d.sum() >= n);

    std::vector<int> candidateRanks;
    candidateRanks.reserve(20);

    for (int i = 0; i < 13; i++)
    {
        for (int j = 0; j < cards_13_1d[i]; j++)
        {
            candidateRanks.push_back(i);
        }
    }
    if (cards_54_1d[52])
    {
        candidateRanks.push_back(13);
    }
    if (cards_54_1d[53])
    {
        candidateRanks.push_back(14);
    }

    std::vector<int> selectedRanks;
    selectedRanks.reserve(n);
    addAttachedActions(cards_54_1d, pActionData, pActionsWithAttachs, candidateRanks, selectedRanks, candidateRanks.size(), n, 0);
}

void get_PAIR_AttachmentCards(const Cards_54_1d_Type &playerCards, const std::shared_ptr<ActionData> & pActionData, 
        int n, std::vector<std::shared_ptr<ActionData>> &pActionsWithAttachs)
{
    pActionsWithAttachs.clear();

    Cards_54_1d_Type cards_54_1d = playerCards - Eigen::Map<Cards_54_1d_Type>(pActionData->data());
    Eigen::Array<CARD_ARRAY_DATA_TYPE, 1, 13> cards_13_1d = Eigen::Map<Cards_52_2d_Type>(cards_54_1d.data()).colwise().sum();

    std::vector<int> candidateRanks;
    candidateRanks.reserve(13);
    for (int i = 0; i < 13; i++)
    {
        if (cards_13_1d[i] >= 2)
        {
            candidateRanks.push_back(i);
        }
    }

    assert(candidateRanks.size() >= (unsigned int)n);
    std::vector<bool> v(candidateRanks.size());
    std::fill(v.begin(), v.begin() + n, true);

    do {
        std::shared_ptr<ActionData> pAData(new ActionData(*pActionData));
        for (std::size_t i = 0; i < v.size(); i++) {
            if (v[i]) {
                for (int _ = 0; _ < 2; _++)
                {
                    for (int j = candidateRanks[i] * 4; j < candidateRanks[i] * 4 + 4; j++)
                    {
                        if ((cards_54_1d[j] == 1) && ((*pAData)[j] == 0))
                        {
                            (*pAData)[j] = 1;
                            break;
                        }
                    }
                }
            }
        }
        pActionsWithAttachs.push_back(pAData);
    } while (std::prev_permutation(v.begin(), v.end())); 
}

} // namespace doudizhu
} // namespace caishen
