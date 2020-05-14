#ifndef CAISHEN_DOUDIZHU_DOUDIZHU_DECK_H_
#define CAISHEN_DOUDIZHU_DOUDIZHU_DECK_H_

#include <ostream>
#include <memory>

namespace caishen
{
namespace doudizhu
{

/**
 * @brief Card Suit
 * 
 */
enum Suit
{
    SPADES = 0,
    HEARTS,
    CLUBS,
    DIAMONDS,

    JOKERS
};

/**
 * @brief Card Rank
 * 
 */
enum Rank
{
    THREE = 0,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    ACE,
    TWO,
    BLACK_JOKER,
    RED_JOKER
};


class Card
{
public:
    const Suit _s;
    const Rank _r;

    static const char s_SuitStrings[5];
    static const char s_RankStrings[15];

private:
    Card(Suit s, Rank r);
    ~Card();

    friend class Deck;
    friend std::ostream &operator<<(std::ostream &, const Card &);
};

class Deck
{
public:
    /**
     * @brief Represents a full deck. 
     * 
     * Cards sequence is 
     *      Spades 3, Hearts 3, Clubs 3, Diamonds 3, 
     *      Spades 4, Hearts 4, Clubs 4, Diamonds 4, 
     *      ...
     *      Spades King, Hearts King, Clubs King, Diamonds King, 
     *      Spades Ace, Hearts Ace, Clubs Ace, Diamonds Ace, 
     *      Spades 2, Hearts 2, Clubs 2, Diamonds 2, 
     *      Black Joker,
     *      Red Joker
     * 
     */
    static const Card STANDARD_DECK[54];

    static int indexInStandardDeck(Suit s, Rank r);

    /**
     * @brief Deal the deck.
     * 
     * @param cards [out] cards int array of length 54, to get the indexes in @ref Deck::STANDARD_DECK of shuffled deck of cards
     */
    static void deal(std::array<int, 54> & cards);

    /**
     * @brief Set the Random Seed object
     * 
     * @param seed 
     */
    static void setRandomSeed(unsigned seed);

private:
    static unsigned s_seed;
};

} // namespace doudizhu
} // namespace caishen

#endif
