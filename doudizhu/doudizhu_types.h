#ifndef CAISHEN_DOUDIZHU_DOUDIZHU_TYPES_H_
#define CAISHEN_DOUDIZHU_DOUDIZHU_TYPES_H_

#include <vector>
#include <Eigen/Dense>
#include <memory>

namespace caishen
{
namespace doudizhu
{

typedef char CARD_ARRAY_DATA_TYPE;

/**
 * @brief Represents zero or one or severial cards from a full 54 deck in 1d matrix (array).
 * 
 * Value of ONE of an element means the cards consists the corresponding card in the @ref Deck::STANDARD_CARDS.
 * 
 * For example, [0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 *  0,0,0,0,0,0,0,0,1,0] represent the cards consists 2 card. One is Hearts 3 and the other one is Black 
 * Joker.
 * 
 * Also see @ref Deck::STANDARD_CARDS
 * 
 */
typedef Eigen::Array<CARD_ARRAY_DATA_TYPE, 1, 54> Cards_54_1d_Type;

/**
 * @brief Represents zero or one or severial cards from a full 52 deck in 2d matrix.
 * 
 * Value of ONE of an element means the cards consists the corresponding card in the Deck::STANDARD_CARDS[0..52].
 * 
 * First row of the matrix represents Spades 3, 4 to king, ace and 2 in order.
 * Second row of the matrix represents Hearts 3, 4 to king, ace and 2 in order.
 * Third row of the matrix represents Clubs 3, 4 to king, ace and 2 in order.
 * Fourth row of the matrix represents Diamonds 3, 4 to king, ace and 2 in order.
 * 
 * For example, 
 *      0,1,0,0,0,0,0,0,0,0,0,0,0,
 *      0,0,0,0,0,0,0,0,0,0,0,0,0,
 *      0,0,0,0,0,0,0,0,0,0,0,0,0,
 *      0,0,0,0,0,0,0,0,0,0,1,0,0,
 * represent the cards consists 2 card. One is Spades 4 and the other one is Diamonds King.
 * 
 * Since Eigen uses consecutive memory to store matrixes, the card sequence in memory of the Cards_52_2d_Type(ColMajor)
 * is just the same as in the @ref Deck::STANDARD_CARDS first 52 cards.
 * 
 * Also see @ref Deck::STANDARD_CARDS
 * 
 */
typedef Eigen::Array<CARD_ARRAY_DATA_TYPE, 4, 13> Cards_52_2d_Type;

/**
 * @brief Represents 3 players cards and public cards.
 * 
 * First row is the cards of player[0] (land lord).
 * Second row is the cards of player[1].
 * Third row is the cards of player[2].
 * Fourth row is the 3 face up cards.
 * Fifth row is the public cards.
 * 
 */
typedef Eigen::Array<CARD_ARRAY_DATA_TYPE, 5, 54, Eigen::RowMajor> GameCards;

} // namespace doudizhu
} // namespace caishen

#endif