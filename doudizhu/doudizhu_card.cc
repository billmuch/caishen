#include "doudizhu_card.h"

#include <ctime>
#include <ostream>
#include <bits/stdc++.h>
#include <iostream>

namespace caishen
{
namespace doudizhu
{

const char Card::s_SuitStrings[5] = {
    'S',
    'H',
    'C',
    'D',

    'J'};

const char Card::s_RankStrings[15] = {
    '3', '4', '5', '6', '7', '8', '9', 'T',
    'J', 'Q', 'K', 'A', '2', 'B', 'R'
};


Card::Card(Suit s, Rank r) : _s(s), _r(r) {}

Card::~Card() {}

std::ostream& operator<<(std::ostream &strm, const Card &c) {
  return strm << Card::s_SuitStrings[c._s] << Card::s_RankStrings[c._r];
}

unsigned Deck::s_seed = time(0);

const Card Deck::STANDARD_DECK[54] = 
{
    Card(Suit::SPADES, Rank::THREE), Card(Suit::HEARTS, Rank::THREE), Card(Suit::CLUBS, Rank::THREE), Card(Suit::DIAMONDS, Rank::THREE), 
    Card(Suit::SPADES, Rank::FOUR), Card(Suit::HEARTS, Rank::FOUR), Card(Suit::CLUBS, Rank::FOUR), Card(Suit::DIAMONDS, Rank::FOUR), 
    Card(Suit::SPADES, Rank::FIVE), Card(Suit::HEARTS, Rank::FIVE), Card(Suit::CLUBS, Rank::FIVE), Card(Suit::DIAMONDS, Rank::FIVE), 
    Card(Suit::SPADES, Rank::SIX), Card(Suit::HEARTS, Rank::SIX), Card(Suit::CLUBS, Rank::SIX), Card(Suit::DIAMONDS, Rank::SIX), 
    Card(Suit::SPADES, Rank::SEVEN), Card(Suit::HEARTS, Rank::SEVEN), Card(Suit::CLUBS, Rank::SEVEN), Card(Suit::DIAMONDS, Rank::SEVEN), 
    Card(Suit::SPADES, Rank::EIGHT), Card(Suit::HEARTS, Rank::EIGHT), Card(Suit::CLUBS, Rank::EIGHT), Card(Suit::DIAMONDS, Rank::EIGHT), 
    Card(Suit::SPADES, Rank::NINE), Card(Suit::HEARTS, Rank::NINE), Card(Suit::CLUBS, Rank::NINE), Card(Suit::DIAMONDS, Rank::NINE), 
    Card(Suit::SPADES, Rank::TEN), Card(Suit::HEARTS, Rank::TEN), Card(Suit::CLUBS, Rank::TEN), Card(Suit::DIAMONDS, Rank::TEN), 
    Card(Suit::SPADES, Rank::JACK), Card(Suit::HEARTS, Rank::JACK), Card(Suit::CLUBS, Rank::JACK), Card(Suit::DIAMONDS, Rank::JACK), 
    Card(Suit::SPADES, Rank::QUEEN), Card(Suit::HEARTS, Rank::QUEEN), Card(Suit::CLUBS, Rank::QUEEN), Card(Suit::DIAMONDS, Rank::QUEEN), 
    Card(Suit::SPADES, Rank::KING), Card(Suit::HEARTS, Rank::KING), Card(Suit::CLUBS, Rank::KING), Card(Suit::DIAMONDS, Rank::KING), 
    Card(Suit::SPADES, Rank::ACE), Card(Suit::HEARTS, Rank::ACE), Card(Suit::CLUBS, Rank::ACE), Card(Suit::DIAMONDS, Rank::ACE), 
    Card(Suit::SPADES, Rank::TWO), Card(Suit::HEARTS, Rank::TWO), Card(Suit::CLUBS, Rank::TWO), Card(Suit::DIAMONDS, Rank::TWO), 
    Card(Suit::JOKERS, Rank::BLACK_JOKER),
    Card(Suit::JOKERS, Rank::RED_JOKER)
};

int Deck::indexInStandardDeck(Suit s, Rank r)
{
    if (r == RED_JOKER)
    {
        return 53;
    }
    else if (r == BLACK_JOKER)
    {
        return 52;
    }
    else
    {
        return s + r * 4;
    }
}

void Deck::deal(std::array<int, 54> & cards)
{
    for (int i = 0; i < 54; i++)
    {
        cards[i] = i;
    }
    std::shuffle(std::begin(cards), std::end(cards), std::default_random_engine(s_seed)); 
}

void Deck::setRandomSeed(unsigned seed)
{
    s_seed = seed;
}

} // namespace doudizhu
} // namespace caishen
