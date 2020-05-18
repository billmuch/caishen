#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "doudizhu_card.h"
#include "doudizhu_game.h"
#include "doudizhu_utils.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include "json/json.h"
#include <unistd.h>

using namespace caishen::doudizhu;

TEST(STANDARD_DECK, STANDARD_DECK_CardPositionRight)
{
    for (int i = 0; i < 52; i++)
    {
        ASSERT_EQ(Deck::STANDARD_DECK[i]._r * 4 + Deck::STANDARD_DECK[i]._s, i);
    }

    ASSERT_EQ(Deck::STANDARD_DECK[52]._r, Rank::BLACK_JOKER);
    ASSERT_EQ(Deck::STANDARD_DECK[52]._s, Suit::JOKERS);

    ASSERT_EQ(Deck::STANDARD_DECK[53]._r, Rank::RED_JOKER);
    ASSERT_EQ(Deck::STANDARD_DECK[53]._s, Suit::JOKERS);
}

TEST(Deck, indexInStandardDeck)
{
    for (int i = 0; i < 54; i++)
    {
        ASSERT_EQ(Deck::indexInStandardDeck(Deck::STANDARD_DECK[i]._s, Deck::STANDARD_DECK[i]._r), i);
    }
}

TEST(Deck, STANDARD_DECK_CardStringRepresentation)
{

    std::string deckStr = "S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6D6S7H7C7D7S8H8C8D8S9H9C9D9STHTCTDTSJHJCJDJSQHQCQDQSKHKCKDKSAHACADAS2H2C2D2JBJR";
    std::ostringstream oss;

    for (int i = 0; i < 54; i++)
    {
        oss << Deck::STANDARD_DECK[i];
    }

    ASSERT_EQ(oss.str(), deckStr);
}

TEST(Deck, deal)
{
    std::array<int, 54> deck1;
    std::array<int, 54> deck2;

    Deck::setRandomSeed(0);
    Deck::deal(deck1);
    Deck::setRandomSeed(0);
    Deck::deal(deck2);

    for (int i = 0; i < 54; i++)
    {
        ASSERT_EQ(deck1[i], deck2[i]);
    }

    std::sort(deck1.begin(), deck1.end());
    for (int i = 0; i < 54; i++)
    {
        ASSERT_EQ(deck1[i], i);
    }
}

TEST(utils, cardsFromString)
{
    Cards_54_1d_Type cards;

    for (int i = 0; i < 54; i++)
    {
        const Card &c = Deck::STANDARD_DECK[i];
        std::string s = {Card::s_SuitStrings[c._s], Card::s_RankStrings[c._r]};
        ASSERT_TRUE(cardsFromString(cards, s));
        ASSERT_EQ(cards.sum(), 1);
        ASSERT_EQ(cards[i], 1);
        cards[i] = 0;
    }

    ASSERT_FALSE(cardsFromString(cards, "a3b4"));
    ASSERT_FALSE(cardsFromString(cards, "S34"));
    ASSERT_FALSE(cardsFromString(cards, "S3b"));
    ASSERT_FALSE(cardsFromString(cards, "S3S3"));

    ASSERT_TRUE(cardsFromString(cards, "S4JR"));
    ASSERT_EQ(cards.sum(), 2);
    ASSERT_EQ(cards[4], 1);
    ASSERT_EQ(cards[53], 1);

    ASSERT_TRUE(cardsFromString(cards, "S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6D6S7H7C7D7S8H8C8D8S9H9C9D9STHTCTDTSJHJCJDJSQHQCQDQSKHKCKDKSAHACADAS2H2C2D2JBJR"));
    ASSERT_TRUE(cards.isOnes());
}

TEST(utils, isValidActionTypeAndRank)
{
    std::shared_ptr<ActionData> ad(new ActionData(ActionData::Zero()));

    //PASS
    ad->setZero();
    (*ad)[54] = 1;
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_PASS, 0));
    (*ad)[54] = 0;

    //SOLO
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3");
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_BOMB, 0));
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_SOLO, 13));

    ad->setZero();
    for (int i = 0; i < 54; i++)
    {
        (*ad)[i] = 1;
        ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_SOLO, Deck::STANDARD_DECK[i]._r));
        (*ad)[i] = 0;
    }

    //SOLO_CHAIN
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3H4D5C6S7");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_SOLO_CHAIN_5, 0));
    (*ad)[53] = 1;
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_SOLO_CHAIN_5, 0));

    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "STHJDQCKCA");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_SOLO_CHAIN_5, 7));

    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "HJDQCKCAD2");
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_SOLO_CHAIN_5, 7));

    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3H4D5C6S7S8D9STHJDQCKCA");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_SOLO_CHAIN_12, 0));

    //PAIR
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3");
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_BOMB, 0));
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_PAIR, 13));

    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_PAIR, 0));

    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S2D2");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_PAIR, 12));
    (*ad)[53] = 1;
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_PAIR, 12));

    //PAIR_CHAIN
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3H4C4D5H5C6D6S7H7");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_PAIR_CHAIN_5, 0));
    (*ad)[53] = 1;
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_PAIR_CHAIN_5, 0));

    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "CQDQCKSKCADA");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_PAIR_CHAIN_3, 9));

    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "C2D2CKSKCADA");
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_PAIR_CHAIN_3, 9));

    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "D5H5C6D6S7H7C8H8D9S9STCTHJDJDQCQCKHKCASA");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_PAIR_CHAIN_10, 2));

    //TRIO
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3H3");
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_BOMB, 0));
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_PAIR, 13));

    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3H3");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO, 0));

    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S2D2H2");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO, 12));
    (*ad)[53] = 1;
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO, 12));

    //TRIO_CHAIN
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3H3H4C4D4D5H5C5C6D6S6S7H7D7");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO_CHAIN_5, 0));
    (*ad)[53] = 1;
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO_CHAIN_5, 0));

    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "CQDQSQHKCKSKSACADA");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO_CHAIN_3, 9));
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "C2D2S2QKCKSKSACADA");
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO_CHAIN_3, 9));

    //TRIO_SOLO
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3H3JR");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO_SOLO, 0));

    //TRIO_SOLO_CHAIN
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3H3S4D4H4JRD2");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO_SOLO_CHAIN_2, 0));
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3H3S4D4H4JRJB");
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO_SOLO_CHAIN_2, 0));

    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3S4D4H4S5D5H5S6D6H6S7D7H7S8JR");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO_SOLO_CHAIN_4, 1));
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3H3S4D4H4S5D5H5S6D6H6S7D7H7JR");
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO_SOLO_CHAIN_4, 1));
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S8D8H8S4D4H4S5D5H5S6D6H6S7D7H7JR");
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO_SOLO_CHAIN_4, 1));
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3S4D4H4S5D5H5S6D6H6S7D7H7JBJR");
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO_SOLO_CHAIN_4, 1));

    //TRIO_PAIR
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3H3D2H2");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO_PAIR, 0));
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3H3D2HA");
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO_PAIR, 0));

    //TRIO_PAIR_CHAIN
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3H3S4D4H4S2D2SADA");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO_PAIR_CHAIN_2, 0));
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3D3H3S4D4H4D2S2SASK");
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_TRIO_PAIR_CHAIN_2, 0));

    //FOUR_TWO_SOLO
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S4D4H4C4JBSA");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_FOUR_TWO_SOLO, 1));
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S4D4H4C4HASA");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_FOUR_TWO_SOLO, 1));
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S4D4H4C4JBJR");
    ASSERT_FALSE(isValidActionTypeAndRank(ad, ACTION_TYPE_FOUR_TWO_SOLO, 1));

    //FOUR_TWO_PAIR
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S4D4H4C4HASAH2S2");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_FOUR_TWO_PAIR, 1));

    //BOMB
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S4D4H4C4");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_BOMB, 1));
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S2D2H2C2");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_BOMB, 12));

    //ROCKET
    ad->setZero();
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "JBJR");
    ASSERT_TRUE(isValidActionTypeAndRank(ad, ACTION_TYPE_ROCKET, 0));
}

TEST(utils, get_SOLO_ActionCards)
{
    Cards_54_1d_Type fullDeck;
    fullDeck.setOnes();

    for (Action_Rank r = 0; r <= 14; r++)
    {
        std::shared_ptr<ActionData> pad(new ActionData());
        pad->setZero();
        get_SOLO_ActionCards(fullDeck, r, pad);
        ASSERT_TRUE(isValidActionTypeAndRank(pad, ACTION_TYPE_SOLO, r));
    }
}

TEST(utils, get_TYPEX_CHAINX_ActionCards_ACTION_TYPE_SOLO_CHAIN)
{
    Cards_54_1d_Type fullDeck;
    fullDeck.setOnes();

    std::shared_ptr<ActionData> pad(new ActionData());

    for (Action_Rank r = 0; r <= 7; r++)
    {
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(1, 5, fullDeck, r, pad);
        ASSERT_TRUE(isValidActionTypeAndRank(pad, ACTION_TYPE_SOLO_CHAIN_5, r));
    }

    for (Action_Rank r = 0; r <= 0; r++)
    {
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(1, 12, fullDeck, r, pad);
        ASSERT_TRUE(isValidActionTypeAndRank(pad, ACTION_TYPE_SOLO_CHAIN_12, r));
    }
}

TEST(utils, get_TYPEX_CHAINX_ActionCards_ACTION_TYPE_PAIR)
{
    Cards_54_1d_Type fullDeck;
    fullDeck.setOnes();

    std::shared_ptr<ActionData> pad(new ActionData());

    for (Action_Rank r = 0; r <= 12; r++)
    {
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(2, 1, fullDeck, r, pad);
        ASSERT_TRUE(isValidActionTypeAndRank(pad, ACTION_TYPE_PAIR, r));
    }
}

TEST(utils, get_TYPEX_CHAINX_ActionCards_ACTION_TYPE_PAIR_CHAIN)
{
    Cards_54_1d_Type fullDeck;
    fullDeck.setOnes();

    std::shared_ptr<ActionData> pad(new ActionData());

    for (Action_Rank r = 0; r <= 9; r++)
    {
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(2, 3, fullDeck, r, pad);
        ASSERT_TRUE(isValidActionTypeAndRank(pad, ACTION_TYPE_PAIR_CHAIN_3, r));
    }

    for (Action_Rank r = 0; r <= 2; r++)
    {
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(2, 10, fullDeck, r, pad);
        ASSERT_TRUE(isValidActionTypeAndRank(pad, ACTION_TYPE_PAIR_CHAIN_10, r));
    }
}

TEST(utils, get_TYPEX_CHAINX_ActionCards_ACTION_TYPE_TRIO)
{
    Cards_54_1d_Type fullDeck;
    fullDeck.setOnes();

    std::shared_ptr<ActionData> pad(new ActionData());

    for (Action_Rank r = 0; r <= 12; r++)
    {
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(3, 1, fullDeck, r, pad);
        ASSERT_TRUE(isValidActionTypeAndRank(pad, ACTION_TYPE_TRIO, r));
    }
}

TEST(utils, get_TYPEX_CHAINX_ActionCards_ACTION_TYPE_TRIO_CHAIN)
{
    Cards_54_1d_Type fullDeck;
    fullDeck.setOnes();

    std::shared_ptr<ActionData> pad(new ActionData());

    for (Action_Rank r = 0; r <= 10; r++)
    {
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(3, 2, fullDeck, r, pad);
        ASSERT_TRUE(isValidActionTypeAndRank(pad, ACTION_TYPE_TRIO_CHAIN_2, r));
    }

    for (Action_Rank r = 0; r <= 6; r++)
    {
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(3, 6, fullDeck, r, pad);
        ASSERT_TRUE(isValidActionTypeAndRank(pad, ACTION_TYPE_TRIO_CHAIN_6, r));
    }
}

class AttachmentCardsTest : public ::testing::Test
{
protected:
    static std::unique_ptr<Json::Value> pRoot;
    static void loadTypeCardsJson()
    {
        std::ifstream in;
        in.open("./doudizhu/type_card.json");

        if (!in.is_open())
        {
            assert(false);
            return;
        }

        pRoot = std::unique_ptr<Json::Value>(new Json::Value());

        Json::CharReaderBuilder rbuilder;
        rbuilder["collectComments"] = false;
        std::string errs;
        bool ok = Json::parseFromStream(rbuilder, in, &(*pRoot), &errs);
        assert(ok);
        in.close();
    }

    static std::vector<std::string> avaliableCardsStr(std::string type, std::string rank)
    {
        if (!pRoot)
        {
            loadTypeCardsJson();
        }

        std::vector<std::string> result;
        for (auto s : (*pRoot)[type][rank])
        {
            result.push_back(s.asString());
        }
        return result;
    }

    static std::vector<std::string> actionCardsStr(std::vector<std::shared_ptr<ActionData>> &ads)
    {
        std::vector<std::string> result;
        for (auto pad : ads)
        {
            std::ostringstream oss;
            for (int i = 0; i < 54; i++)
            {
                if ((*pad)[i])
                {
                    oss << Card::s_RankStrings[Deck::STANDARD_DECK[i]._r];
                }
            }
            result.push_back(oss.str());
        }
        return result;
    }
};
std::unique_ptr<Json::Value> AttachmentCardsTest::pRoot;

TEST_F(AttachmentCardsTest, get_SOLO_AttachmentCards_ACTION_TYPE_TRIO_SOLO)
{
    Cards_54_1d_Type fullDeck;
    fullDeck.setOnes();

    for (Action_Rank r = 0; r <= 12; r++)
    {
        std::vector<std::shared_ptr<ActionData>> ads;
        std::shared_ptr<ActionData> pad(new ActionData());
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(3, 1, fullDeck, r, pad);
        get_SOLO_AttachmentCards(fullDeck, pad, 1, -1, -1, ads);

        std::vector<std::string> cs1 = avaliableCardsStr("trio_solo", std::to_string(r+1));
        std::vector<std::string> cs2 = actionCardsStr(ads);
        EXPECT_THAT(cs1, cs2);
    }
}

TEST_F(AttachmentCardsTest, get_SOLO_AttachmentCards_ACTION_TYPE_TRIO_SOLO_CHAIN)
{
    Cards_54_1d_Type fullDeck;
    fullDeck.setOnes();

    for (Action_Rank r = 0; r <= 10; r++)
    {
        std::vector<std::shared_ptr<ActionData>> ads;
        std::shared_ptr<ActionData> pad(new ActionData());
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(3, 2, fullDeck, r, pad);
        get_SOLO_AttachmentCards(fullDeck, pad, 2, -1, -1, ads);

        std::vector<std::string> cs1 = avaliableCardsStr("trio_solo_chain_2", std::to_string(r+1));
        std::vector<std::string> cs2 = actionCardsStr(ads);
        std::sort(cs1.begin(), cs1.end());
        std::sort(cs2.begin(), cs2.end());
        EXPECT_THAT(cs1, cs2);
    }

    for (Action_Rank r = 0; r <= 7; r++)
    {
        std::vector<std::shared_ptr<ActionData>> ads;
        std::shared_ptr<ActionData> pad(new ActionData());
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(3, 5, fullDeck, r, pad);
        get_SOLO_AttachmentCards(fullDeck, pad, 5, 
                r - 1 < 0 ? -1 : r - 1, 
                r + 5 < 12 ? r + 5 : -1, 
                ads);

        std::vector<std::string> cs1 = avaliableCardsStr("trio_solo_chain_5", std::to_string(r+1));
        std::vector<std::string> cs2 = actionCardsStr(ads);
        std::sort(cs1.begin(), cs1.end());
        std::sort(cs2.begin(), cs2.end());
        EXPECT_THAT(cs1, cs2);
    }
}

TEST_F(AttachmentCardsTest, get_PAIR_AttachmentCards_ACTION_TYPE_TRIO_PAIR)
{
    Cards_54_1d_Type fullDeck;
    fullDeck.setOnes();

    for (Action_Rank r = 0; r <= 12; r++)
    {
        std::vector<std::shared_ptr<ActionData>> ads;
        std::shared_ptr<ActionData> pad(new ActionData());
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(3, 1, fullDeck, r, pad);
        get_PAIR_AttachmentCards(fullDeck, pad, 1, ads);

        std::vector<std::string> cs1 = avaliableCardsStr("trio_pair", std::to_string(r+1));
        std::vector<std::string> cs2 = actionCardsStr(ads);
        EXPECT_THAT(cs1, cs2);
    }
}

TEST_F(AttachmentCardsTest, get_PAIR_AttachmentCards_ACTION_TYPE_TRIO_PAIR_CHAIN)
{
    Cards_54_1d_Type fullDeck;
    fullDeck.setOnes();

    for (Action_Rank r = 0; r <= 10; r++)
    {
        std::vector<std::shared_ptr<ActionData>> ads;
        std::shared_ptr<ActionData> pad(new ActionData());
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(3, 2, fullDeck, r, pad);
        get_PAIR_AttachmentCards(fullDeck, pad, 2, ads);

        std::vector<std::string> cs1 = avaliableCardsStr("trio_pair_chain_2", std::to_string(r+1));
        std::vector<std::string> cs2 = actionCardsStr(ads);
        std::sort(cs1.begin(), cs1.end());
        std::sort(cs2.begin(), cs2.end());
        EXPECT_THAT(cs1, cs2);
    }

    for (Action_Rank r = 0; r <= 8; r++)
    {
        std::vector<std::shared_ptr<ActionData>> ads;
        std::shared_ptr<ActionData> pad(new ActionData());
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(3, 4, fullDeck, r, pad);
        get_PAIR_AttachmentCards(fullDeck, pad, 4, ads);

        std::vector<std::string> cs1 = avaliableCardsStr("trio_pair_chain_4", std::to_string(r+1));
        std::vector<std::string> cs2 = actionCardsStr(ads);
        std::sort(cs1.begin(), cs1.end());
        std::sort(cs2.begin(), cs2.end());
        EXPECT_THAT(cs1, cs2);
    }
}

TEST_F(AttachmentCardsTest, get_SOLO_AttachmentCards_ACTION_TYPE_FOUR_TWO_SOLO)
{
    Cards_54_1d_Type fullDeck;
    fullDeck.setOnes();

    for (Action_Rank r = 0; r <= 12; r++)
    {
        std::vector<std::shared_ptr<ActionData>> ads;
        std::shared_ptr<ActionData> pad(new ActionData());
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(4, 1, fullDeck, r, pad);
        get_SOLO_AttachmentCards(fullDeck, pad, 2, -1, -1, ads);

        std::vector<std::string> cs1 = avaliableCardsStr("four_two_solo", std::to_string(r+1));
        std::vector<std::string> cs2 = actionCardsStr(ads);
        EXPECT_THAT(cs1, cs2);
    }
}

TEST_F(AttachmentCardsTest, get_PAIR_AttachmentCards_ACTION_TYPE_FOUR_TWO_PAIR)
{
    Cards_54_1d_Type fullDeck;
    fullDeck.setOnes();

    for (Action_Rank r = 0; r <= 12; r++)
    {
        std::vector<std::shared_ptr<ActionData>> ads;
        std::shared_ptr<ActionData> pad(new ActionData());
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(4, 1, fullDeck, r, pad);
        get_PAIR_AttachmentCards(fullDeck, pad, 2, ads);

        std::vector<std::string> cs1 = avaliableCardsStr("four_two_pair", std::to_string(r+1));
        std::vector<std::string> cs2 = actionCardsStr(ads);
        EXPECT_THAT(cs1, cs2);
    }
}

TEST(utils, get_TYPEX_CHAINX_ActionCards_ACTION_TYPE_BOMB)
{
    Cards_54_1d_Type fullDeck;
    fullDeck.setOnes();

    std::shared_ptr<ActionData> pad(new ActionData());

    for (Action_Rank r = 0; r <= 12; r++)
    {
        pad->setZero();
        get_TYPEX_CHAINX_ActionCards(4, 1, fullDeck, r, pad);
        ASSERT_TRUE(isValidActionTypeAndRank(pad, ACTION_TYPE_BOMB, r));
    }
}

TEST(utils, get_ROCKET_ActionCards)
{
    Cards_54_1d_Type fullDeck;
    fullDeck.setOnes();

    std::shared_ptr<ActionData> pad(new ActionData());
    pad->setZero();

    get_ROCKET_ActionCards(fullDeck, 0, pad);
    ASSERT_TRUE(isValidActionTypeAndRank(pad, ACTION_TYPE_ROCKET, 0));
}

TEST(utils, get_PASS_ActionCards)
{
    Cards_54_1d_Type fullDeck;
    fullDeck.setOnes();

    std::shared_ptr<ActionData> pad(new ActionData());
    pad->setZero();

    get_PASS_ActionCards(fullDeck, 0, pad);
    ASSERT_TRUE(isValidActionTypeAndRank(pad, ACTION_TYPE_PASS, 0));
}

TEST(Game, createNewGame)
{

    std::array<int, 54> deck;

    Deck::setRandomSeed(100);
    Deck::deal(deck);

    std::unique_ptr<Game> game = Game::createNewGame(deck);
    ASSERT_EQ(game->getPlayerId(), Game::LAND_LORD_PLAYER_ID);
    ASSERT_TRUE(game->getHistory().empty());
    Game::Payoff payoff;
    ASSERT_FALSE(game->getPayoff(payoff));
    ASSERT_FALSE(game->isOver());

    GameCards gameCards = game->getCards();
    Cards_54_1d_Type sum = gameCards.row(LANDLORD_CARDS_ROW);
    ASSERT_EQ(gameCards.row(LANDLORD_CARDS_ROW).sum(), 20);
    sum += gameCards.row(PLAYER_1_CARDS_ROW);
    ASSERT_EQ(gameCards.row(PLAYER_1_CARDS_ROW).sum(), 17);
    sum += gameCards.row(PLAYER_2_CARDS_ROW);
    ASSERT_EQ(gameCards.row(PLAYER_2_CARDS_ROW).sum(), 17);
    sum += gameCards.row(PUBLIC_CARDS_ROW);
    ASSERT_TRUE(sum.isOnes());
    ASSERT_EQ(gameCards.row(FACEUP_CARDS_ROW).sum(), 3);
    //if all 3 faceup cards are in land_lord's hands
    ASSERT_FALSE((gameCards.row(LANDLORD_CARDS_ROW) < gameCards.row(FACEUP_CARDS_ROW)).any());
}

std::unique_ptr<Game> createGame(std::string landlordCards, std::string player1Cards, std::string player2Cards, std::string faceupCards = "")
{
    Cards_54_1d_Type cards;

    GameCards gameCards(GameCards::Zero());

    gameCards.row(PUBLIC_CARDS_ROW) = Cards_54_1d_Type::Ones();

    cardsFromString(cards, landlordCards);
    gameCards.row(LANDLORD_CARDS_ROW) += cards;
    gameCards.row(PUBLIC_CARDS_ROW) -= cards;

    cardsFromString(cards, player1Cards);
    gameCards.row(PLAYER_1_CARDS_ROW) += cards;
    gameCards.row(PUBLIC_CARDS_ROW) -= cards;

    cardsFromString(cards, player2Cards);
    gameCards.row(PLAYER_2_CARDS_ROW) += cards;
    gameCards.row(PUBLIC_CARDS_ROW) -= cards;

    if (faceupCards != "")
    {
        cardsFromString(cards, faceupCards);
        gameCards.row(FACEUP_CARDS_ROW) += cards;
    }

    return std::unique_ptr<Game>(new Game(0, gameCards, HistoryActions()));
}

TEST(Game, playAndPlayBack)
{
    std::unique_ptr<Game> game = createGame("S4JR", "S3DA", "D2");

    ASSERT_FALSE(game->playBack());

    ASSERT_FALSE(game->isOver());

    //landlord play Spades 4
    std::shared_ptr<ActionData> ad(new ActionData(ActionData::Zero()));
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S4");
    Action action = Action(ad, ACTION_TYPE_SOLO, 1);

    ASSERT_TRUE(game->play(action));

    ASSERT_EQ(game->getCards()(0, 4), 0);
    ASSERT_EQ(game->getCards()(4, 4), 1);
    ASSERT_FALSE(game->isOver());
    ASSERT_EQ(game->getHistory().back(), action);
    ASSERT_EQ(game->getPlayerId(), 1);

    //player1 play Spades 3
    std::shared_ptr<ActionData> ad1(new ActionData(ActionData::Zero()));
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad1->data()), "S3");
    Action action1 = Action(ad1, ACTION_TYPE_SOLO, 0);
    ASSERT_FALSE(game->play(action1));

    //player1 play PASS
    (*ad1).setZero();
    (*ad1)[54] = 1;
    Action action11 = Action(ad1, ACTION_TYPE_PASS, 0);
    ASSERT_TRUE(game->play(action11));

    ASSERT_EQ(game->getPlayerId(), 2);
    ASSERT_FALSE(game->isOver());

    GameCards cardsBeforePlayBack = game->getCards();
    HistoryActions historyBeforePlayBack = game->getHistory();

    //player2 play PASS
    std::shared_ptr<ActionData> ad2(new ActionData(ActionData::Zero()));
    (*ad2)[54] = 1;
    Action action2 = Action(ad2, ACTION_TYPE_PASS, 0);
    ASSERT_TRUE(game->play(action2));

    ASSERT_EQ(game->getPlayerId(), 0);
    ASSERT_FALSE(game->isOver());

    //landlord play red joker
    std::shared_ptr<ActionData> ad3(new ActionData(ActionData::Zero()));
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad3->data()), "JR");
    Action action3 = Action(ad3, ACTION_TYPE_SOLO, 14);
    ASSERT_TRUE(game->play(action3));

    ASSERT_EQ(game->getCards()(0, 3), 0);
    ASSERT_EQ(game->getCards()(4, 53), 1);
    ASSERT_TRUE(game->isOver());
    Game::Payoff payoff;
    ASSERT_TRUE(game->getPayoff(payoff));
    ASSERT_TRUE(payoff.isApprox(Game::Payoff({1.0, 0.0, 0.0})));

    //playBack(landlord)
    ASSERT_TRUE(game->playBack());
    ASSERT_FALSE(game->isOver());
    ASSERT_EQ(game->getPlayerId(), 0);

    //playback(player2)
    ASSERT_TRUE(game->playBack());
    ASSERT_EQ(game->getPlayerId(), 2);
    ASSERT_TRUE((game->getCards() - cardsBeforePlayBack).isZero());
    HistoryActions currHistory = game->getHistory();
    ASSERT_EQ(currHistory.size(), historyBeforePlayBack.size());
    for (std::size_t i = 0; i < historyBeforePlayBack.size(); i++)
    {
        ASSERT_EQ(currHistory[i], historyBeforePlayBack[i]);
    }

    //player2 play Diamonds 2
    std::shared_ptr<ActionData> ad4(new ActionData(ActionData::Zero()));
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad4->data()), "D2");
    Action action4 = Action(ad4, ACTION_TYPE_SOLO, 12);
    ASSERT_TRUE(game->play(action4));

    ASSERT_TRUE(game->isOver());
    Game::Payoff payoff1;
    ASSERT_TRUE(game->getPayoff(payoff1));
    ASSERT_TRUE(payoff1.isApprox(Game::Payoff({0.0, 1.0, 1.0})));
}

TEST(Game, get_SOLO_Actions)
{
    std::unique_ptr<Game> game;
    std::vector<Action_Rank> ranks;

    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6S7S8STSJSQSKSAJBJR", "S2", "C2");
    game->get_SOLO_Actions(0, 0, ranks);
    ASSERT_EQ(ranks, std::vector<int>({0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 13, 14}));
    game->get_SOLO_Actions(0, 6, ranks);
    ASSERT_EQ(ranks, std::vector<int>({7, 8, 9, 10, 11, 13, 14}));
}

TEST(Game, get_SOLO_CHAIN_X_Actions)
{
    std::unique_ptr<Game> game;
    std::vector<Action_Rank> ranks;

    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6S7S8S9STSJSQSKSAD2JBJR", "S2", "C2");
    game->get_SOLO_CHAIN_X_Actions(5, 0, 2, ranks);
    ASSERT_EQ(ranks, std::vector<int>({2, 3, 4, 5, 6, 7}));

    for (int i = 5; i <= 12; i++)
    {
        std::vector<int> expect;
        for (int j = 0; j <= 7 - (i - 5); j++)
        {
            expect.push_back(j);
        }
        game->get_SOLO_CHAIN_X_Actions(i, 0, 0, ranks);
        ASSERT_EQ(ranks, expect);
    }
}

TEST(Game, get_PAIR_Actions)
{
    std::unique_ptr<Game> game;
    std::vector<Action_Rank> ranks;

    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6S7H7S8H8S9H9STHTSJHJSQHQSKHKSAHAH2D2JBJR", "S2", "C2");
    game->get_PAIR_Actions(0, 0, ranks);
    ASSERT_EQ(ranks, std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    game->get_PAIR_Actions(0, 6, ranks);
    ASSERT_EQ(ranks, std::vector<int>({6, 7, 8, 9, 10, 11, 12}));
}

TEST(Game, get_PAIR_CHAIN_X_Actions)
{
    std::unique_ptr<Game> game;
    std::vector<Action_Rank> ranks;

    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6S7H7S8H8S9H9STHTSJHJSQHQSKHKSAHAH2D2JBJR", "S2", "C2");
    game->get_PAIR_CHAIN_X_Actions(3, 0, 2, ranks);
    ASSERT_EQ(ranks, std::vector<int>({2, 3, 4, 5, 6, 7, 8, 9}));
    for (int i = 3; i <= 10; i++)
    {
        std::vector<int> expect;
        for (int j = 0; j <= 9 - (i - 3); j++)
        {
            expect.push_back(j);
        }
        game->get_PAIR_CHAIN_X_Actions(i, 0, 0, ranks);
        ASSERT_EQ(ranks, expect);
    }
}

TEST(Game, get_TRIO_Actions)
{
    std::unique_ptr<Game> game;
    std::vector<Action_Rank> ranks;

    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6S7H7C7S8H8C8S9H9C9STHTCTSJHJCJSQHQCQSKHKCKSAHACAS2H2C2JBJR", "DA", "D2");
    game->get_TRIO_Actions(0, 0, ranks);
    ASSERT_EQ(ranks, std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    game->get_TRIO_Actions(0, 6, ranks);
    ASSERT_EQ(ranks, std::vector<int>({6, 7, 8, 9, 10, 11, 12}));
}

TEST(Game, get_TRIO_CHAIN_X_Actions)
{
    std::unique_ptr<Game> game;
    std::vector<Action_Rank> ranks;

    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6S7H7C7S8H8C8S9H9C9STHTCTSJHJCJSQHQCQSKHKCKSAHACAS2H2C2JBJR", "DA", "D2");
    game->get_TRIO_CHAIN_X_Actions(2, 0, 2, ranks);
    ASSERT_EQ(ranks, std::vector<int>({2, 3, 4, 5, 6, 7, 8, 9, 10}));
    for (int i = 2; i <= 6; i++)
    {
        std::vector<int> expect;
        for (int j = 0; j <= 10 - (i - 2); j++)
        {
            expect.push_back(j);
        }
        game->get_TRIO_CHAIN_X_Actions(i, 0, 0, ranks);
        ASSERT_EQ(ranks, expect);
    }
}

TEST(Game, get_TRIO_SOLO_Actions)
{
    std::unique_ptr<Game> game;
    std::vector<Action_Rank> ranks;

    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6S7H7C7S8H8C8S9H9C9STHTCTSJHJCJSQHQCQSKHKCKSAHACAS2H2C2JBJR", "DA", "D2");
    game->get_TRIO_SOLO_Actions(0, 0, ranks);
    ASSERT_EQ(ranks, std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    game->get_TRIO_SOLO_Actions(0, 2, ranks);
    ASSERT_EQ(ranks, std::vector<int>({2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));

    // 33333
    game = createGame("S3H3C3D3", "DA", "D2");
    game->get_TRIO_SOLO_Actions(0, 0, ranks);
    ASSERT_TRUE(ranks.empty());
}

TEST(Game, get_TRIO_SOLO_CHAIN_X_Actions)
{
    std::unique_ptr<Game> game;
    std::vector<Action_Rank> ranks;

    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6S7H7C7S8H8C8S9H9C9STHTCTSJHJCJSQHQCQSKHKCKSAHACAS2H2C2JBJR", "DA", "D2");
    game->get_TRIO_SOLO_CHAIN_X_Actions(2, 0, 2, ranks);
    ASSERT_EQ(ranks, std::vector<int>({2, 3, 4, 5, 6, 7, 8, 9, 10}));
    for (int i = 2; i <= 5; i++)
    {
        std::vector<int> expect;
        for (int j = 0; j <= 10 - (i - 2); j++)
        {
            expect.push_back(j);
        }
        game->get_TRIO_SOLO_CHAIN_X_Actions(i, 0, 0, ranks);
        ASSERT_EQ(ranks, expect);
    }

    // 333444_34, 333444_BR
    game = createGame("S3H3C3D3S4H4C4D4JBJR", "DA", "D2");
    game->get_TRIO_SOLO_CHAIN_X_Actions(2, 0, 0, ranks);
    ASSERT_TRUE(ranks.empty());

    // 444555666_333
    game = createGame("S3H3C3D3S4H4C4S5H5C5S6H6C6", "DA", "D2");
    game->get_TRIO_SOLO_CHAIN_X_Actions(3, 0, 0, ranks);
    ASSERT_TRUE(ranks.empty());

    // 444555666_777
    game = createGame("S4H4C4S5H5C5S6H6C6S7H7C7D7", "DA", "D2");
    game->get_TRIO_SOLO_CHAIN_X_Actions(3, 0, 0, ranks);
    ASSERT_TRUE(ranks.empty());

    // 444555666777_9999
    game = createGame("S4H4C4S5H5C5S6H6C6S7H7C7S9H9C9D9", "DA", "D2");
    game->get_TRIO_SOLO_CHAIN_X_Actions(4, 0, 0, ranks);
    ASSERT_TRUE(ranks.empty());
}

TEST(Game, get_TRIO_PAIR_Actions)
{
    std::unique_ptr<Game> game;
    std::vector<Action_Rank> ranks;

    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6S7H7C7S8H8C8S9H9C9STHTCTSJHJCJSQHQCQSKHKCKSAHACAS2H2C2JBJR", "DA", "D2");
    game->get_TRIO_PAIR_Actions(0, 0, ranks);
    ASSERT_EQ(ranks, std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    game->get_TRIO_PAIR_Actions(0, 2, ranks);
    ASSERT_EQ(ranks, std::vector<int>({2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
}

TEST(Game, get_TRIO_PAIR_CHAIN_X_Actions)
{
    std::unique_ptr<Game> game;
    std::vector<Action_Rank> ranks;

    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6S7H7C7S8H8C8S9H9C9STHTCTSJHJCJSQHQCQSKHKCKSAHACAS2H2C2JBJR", "DA", "D2");
    game->get_TRIO_PAIR_CHAIN_X_Actions(2, 0, 2, ranks);
    ASSERT_EQ(ranks, std::vector<int>({2, 3, 4, 5, 6, 7, 8, 9, 10}));
    for (int i = 2; i <= 4; i++)
    {
        std::vector<int> expect;
        for (int j = 0; j <= 10 - (i - 2); j++)
        {
            expect.push_back(j);
        }
        game->get_TRIO_PAIR_CHAIN_X_Actions(i, 0, 0, ranks);
        ASSERT_EQ(ranks, expect);
    }

    // 33334444_8888
    game = createGame("S3H3C3D3S4H4C4D4S8H8C8D8JBJR", "DA", "D2");
    game->get_TRIO_PAIR_CHAIN_X_Actions(2, 0, 0, ranks);
    ASSERT_TRUE(ranks.empty());
}

TEST(Game, get_FOUR_TWO_SOLO_Actions)
{
    std::unique_ptr<Game> game;
    std::vector<Action_Rank> ranks;

    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6D6S7H7C7D7S8H8C8D8S9H9C9D9STHTCTDTSJHJCJDJSQHQCQDQSKHKCKDKSAHACADAS2H2C2D2", "JB", "JR");
    game->get_FOUR_TWO_SOLO_Actions(0, 0, ranks);
    ASSERT_EQ(ranks, std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    game->get_FOUR_TWO_SOLO_Actions(0, 2, ranks);
    ASSERT_EQ(ranks, std::vector<int>({2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));

    // 3333_BR
    game = createGame("S3H3C3D3JBJR", "DA", "D2");
    game->get_FOUR_TWO_SOLO_Actions(0, 0, ranks);
    ASSERT_TRUE(ranks.empty());
}

TEST(Game, get_FOUR_TWO_PAIR_Actions)
{
    std::unique_ptr<Game> game;
    std::vector<Action_Rank> ranks;

    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6D6S7H7C7D7S8H8C8D8S9H9C9D9STHTCTDTSJHJCJDJSQHQCQDQSKHKCKDKSAHACADAS2H2C2D2", "JB", "JR");
    game->get_FOUR_TWO_PAIR_Actions(0, 0, ranks);
    ASSERT_EQ(ranks, std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    game->get_FOUR_TWO_PAIR_Actions(0, 2, ranks);
    ASSERT_EQ(ranks, std::vector<int>({2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));

    // 3333_4444
    game = createGame("S3H3C3D3S4H4C4D4", "DA", "D2");
    game->get_FOUR_TWO_PAIR_Actions(0, 0, ranks);
    ASSERT_TRUE(ranks.empty());
}

TEST(Game, get_BOMB_Actions)
{
    std::unique_ptr<Game> game;
    std::vector<Action_Rank> ranks;

    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6D6S7H7C7D7S8H8C8D8S9H9C9D9STHTCTDTSJHJCJDJSQHQCQDQSKHKCKDKSAHACADAS2H2C2D2", "JB", "JR");
    game->get_BOMB_Actions(0, 0, ranks);
    ASSERT_EQ(ranks, std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    game->get_BOMB_Actions(0, 2, ranks);
    ASSERT_EQ(ranks, std::vector<int>({2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
}

TEST(Game, get_ROCKET_Actions)
{
    std::unique_ptr<Game> game;
    std::vector<Action_Rank> ranks;

    game = createGame("JBJR", "DA", "D2");
    game->get_ROCKET_Actions(0, 0, ranks);
    ASSERT_EQ(ranks, std::vector<int>({0}));

    game = createGame("JB", "DA", "D2");
    game->get_ROCKET_Actions(0, 0, ranks);
    ASSERT_TRUE(ranks.empty());
}

TEST(Game, getAllActions)
{
    std::unique_ptr<Game> game;
    std::vector<std::vector<Action_Rank>> ranks;

    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6D6S7H7C7D7S8H8C8D8S9H9C9D9STHTCTDTSJHJCJDJSQHQCQDQSKHKCKDKSAHACADAS2H2C2D2JBJR", "", "");
    game->getAllActions(0, ranks);

    ASSERT_EQ(ranks[ACTION_TYPE_SOLO], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}));
    ASSERT_EQ(ranks[ACTION_TYPE_SOLO_CHAIN_5], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7}));
    ASSERT_EQ(ranks[ACTION_TYPE_SOLO_CHAIN_6], std::vector<int>({0, 1, 2, 3, 4, 5, 6}));
    ASSERT_EQ(ranks[ACTION_TYPE_SOLO_CHAIN_7], std::vector<int>({0, 1, 2, 3, 4, 5}));
    ASSERT_EQ(ranks[ACTION_TYPE_SOLO_CHAIN_8], std::vector<int>({0, 1, 2, 3, 4}));
    ASSERT_EQ(ranks[ACTION_TYPE_SOLO_CHAIN_9], std::vector<int>({0, 1, 2, 3}));
    ASSERT_EQ(ranks[ACTION_TYPE_SOLO_CHAIN_10], std::vector<int>({0, 1, 2}));
    ASSERT_EQ(ranks[ACTION_TYPE_SOLO_CHAIN_11], std::vector<int>({0, 1}));
    ASSERT_EQ(ranks[ACTION_TYPE_SOLO_CHAIN_12], std::vector<int>({0}));
    ASSERT_EQ(ranks[ACTION_TYPE_PAIR], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    ASSERT_EQ(ranks[ACTION_TYPE_PAIR_CHAIN_3], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
    ASSERT_EQ(ranks[ACTION_TYPE_PAIR_CHAIN_4], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8}));
    ASSERT_EQ(ranks[ACTION_TYPE_PAIR_CHAIN_5], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7}));
    ASSERT_EQ(ranks[ACTION_TYPE_PAIR_CHAIN_6], std::vector<int>({0, 1, 2, 3, 4, 5, 6}));
    ASSERT_EQ(ranks[ACTION_TYPE_PAIR_CHAIN_7], std::vector<int>({0, 1, 2, 3, 4, 5}));
    ASSERT_EQ(ranks[ACTION_TYPE_PAIR_CHAIN_8], std::vector<int>({0, 1, 2, 3, 4}));
    ASSERT_EQ(ranks[ACTION_TYPE_PAIR_CHAIN_9], std::vector<int>({0, 1, 2, 3}));
    ASSERT_EQ(ranks[ACTION_TYPE_PAIR_CHAIN_10], std::vector<int>({0, 1, 2}));
    ASSERT_EQ(ranks[ACTION_TYPE_TRIO], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    ASSERT_EQ(ranks[ACTION_TYPE_TRIO_CHAIN_2], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
    ASSERT_EQ(ranks[ACTION_TYPE_TRIO_CHAIN_3], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
    ASSERT_EQ(ranks[ACTION_TYPE_TRIO_CHAIN_4], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8}));
    ASSERT_EQ(ranks[ACTION_TYPE_TRIO_CHAIN_5], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7}));
    ASSERT_EQ(ranks[ACTION_TYPE_TRIO_CHAIN_6], std::vector<int>({0, 1, 2, 3, 4, 5, 6}));
    ASSERT_EQ(ranks[ACTION_TYPE_TRIO_SOLO], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    ASSERT_EQ(ranks[ACTION_TYPE_TRIO_SOLO_CHAIN_2], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
    ASSERT_EQ(ranks[ACTION_TYPE_TRIO_SOLO_CHAIN_3], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
    ASSERT_EQ(ranks[ACTION_TYPE_TRIO_SOLO_CHAIN_4], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8}));
    ASSERT_EQ(ranks[ACTION_TYPE_TRIO_SOLO_CHAIN_5], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7}));
    ASSERT_EQ(ranks[ACTION_TYPE_TRIO_PAIR], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    ASSERT_EQ(ranks[ACTION_TYPE_TRIO_PAIR_CHAIN_2], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
    ASSERT_EQ(ranks[ACTION_TYPE_TRIO_PAIR_CHAIN_3], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
    ASSERT_EQ(ranks[ACTION_TYPE_TRIO_PAIR_CHAIN_4], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8}));
    ASSERT_EQ(ranks[ACTION_TYPE_FOUR_TWO_SOLO], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    ASSERT_EQ(ranks[ACTION_TYPE_FOUR_TWO_PAIR], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    ASSERT_EQ(ranks[ACTION_TYPE_BOMB], std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    ASSERT_EQ(ranks[ACTION_TYPE_ROCKET], std::vector<int>({0}));
    ASSERT_EQ(ranks[ACTION_TYPE_PASS], std::vector<int>({0}));

    game = createGame("S3H3C3D3S4H4C4D4", "", "");
    game->getAllActions(0, ranks);
    ASSERT_TRUE(ranks[ACTION_TYPE_FOUR_TWO_PAIR].empty());

    game = createGame("S3H3C3D3S4H4C4D4", "", "");
    game->getAllActions(0, ranks);
    ASSERT_TRUE(ranks[ACTION_TYPE_TRIO_SOLO_CHAIN_2].empty());
    ASSERT_TRUE(ranks[ACTION_TYPE_TRIO_PAIR_CHAIN_2].empty());
}

TEST(Game, getLegalActions)
{
    std::unique_ptr<Game> game;
    std::vector<std::vector<Action_Rank>> legalActions;

    // game over
    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6D6S7H7C7D7S8H8C8D8S9H9C9D9STHTCTDTSJHJCJDJSQHQCQDQSKHKCKDKSAHACADAS2H2C2D2JBJR", "", "");
    game->getLegalActions(legalActions);

    ASSERT_TRUE(game->isOver());
    EXPECT_THAT(legalActions, std::vector<std::vector<Action_Rank>>(ACTION_TYPE_SIZE, std::vector<Action_Rank>()));
    
    // full deck 52
    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6D6S7H7C7D7S8H8C8D8S9H9C9D9STHTCTDTSJHJCJDJSQHQCQDQSKHKCKDKSAHACADAS2H2C2D2", "JB", "JR");
    game->getLegalActions(legalActions);

    std::vector<int> v({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14});
    // no JB, JR for type solo
    EXPECT_THAT(legalActions[0], std::vector<Action_Rank>(v.begin(), v.begin() + 12 + 1));
    for (int i = 1; i <= 35; i++)
    {
        EXPECT_THAT(legalActions[i], std::vector<Action_Rank>(v.begin(), v.begin() + s_actionTypeMaxRank[i] + 1));
    }

    // landlord play SOLO, test SOLO, PASS, ROCKET and BOMB
    {
        game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6D6", "S7H7C7D7S8H8C8D8S9H9C9D9STHTCTDTSJHJCJDJSQHQCQDQJBJR", "SKHKCKDKSAHACADAS2H2C2D2");
        std::shared_ptr<ActionData> ad(new ActionData(ActionData::Zero()));
        cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S4");
        Action action = Action(ad, ACTION_TYPE_SOLO, 1); 
        game->play(action); // landlord play Spades 4
        game->getLegalActions(legalActions);
        EXPECT_THAT(legalActions[ACTION_TYPE_SOLO], std::vector<Action_Rank>({4, 5, 6, 7, 8, 9, 13, 14}));
        EXPECT_THAT(legalActions[ACTION_TYPE_BOMB], std::vector<Action_Rank>({4, 5, 6, 7, 8, 9}));
        EXPECT_THAT(legalActions[ACTION_TYPE_ROCKET], std::vector<Action_Rank>({0}));
        EXPECT_THAT(legalActions[ACTION_TYPE_PASS], std::vector<Action_Rank>({0}));
        for (int i = 1; i <= 34; i++)
        {
            EXPECT_THAT(legalActions[i], std::vector<Action_Rank>());
        }

        std::shared_ptr<ActionData> ad1(new ActionData(ActionData::Zero()));
        cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad1->data()), "S7H7C7D7");
        Action action1 = Action(ad1, ACTION_TYPE_BOMB, 4);
        game->play(action1); // player1 play bomb, test bomb, rocket and pass
        game->getLegalActions(legalActions);
        EXPECT_THAT(legalActions[ACTION_TYPE_BOMB], std::vector<Action_Rank>({10, 11, 12}));
        EXPECT_THAT(legalActions[ACTION_TYPE_ROCKET], std::vector<Action_Rank>());
        EXPECT_THAT(legalActions[ACTION_TYPE_PASS], std::vector<Action_Rank>({0}));
        for (int i = 1; i <= 34; i++)
        {
            EXPECT_THAT(legalActions[i], std::vector<Action_Rank>());
        }
    }

    {
        game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6D6", "S7H7C7D7S8H8C8D8S9H9C9D9STHTCTDTSJHJCJDJSQHQCQDQJBJR", "SKHKCKDKSAHACADAS2H2C2D2");
        std::shared_ptr<ActionData> ad(new ActionData(ActionData::Zero()));
        cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad->data()), "S3H3C3D3");
        Action action = Action(ad, ACTION_TYPE_BOMB, 0);
        game->play(action); // landlord play BOMB, test PASS and ROCKET
        game->getLegalActions(legalActions);
        EXPECT_THAT(legalActions[ACTION_TYPE_BOMB], std::vector<Action_Rank>({4, 5, 6, 7, 8, 9}));
        EXPECT_THAT(legalActions[ACTION_TYPE_ROCKET], std::vector<Action_Rank>({0}));
        EXPECT_THAT(legalActions[ACTION_TYPE_PASS], std::vector<Action_Rank>({0}));
        for (int i = 0; i <= 34; i++)
        {
            EXPECT_THAT(legalActions[i], std::vector<Action_Rank>());
        }

        std::shared_ptr<ActionData> ad1(new ActionData(ActionData::Zero()));
        cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad1->data()), "JBJR");
        Action action1 = Action(ad1, ACTION_TYPE_ROCKET, 0);
        game->play(action1); // player1 play rocket, test pass
        game->getLegalActions(legalActions);
        EXPECT_THAT(legalActions[ACTION_TYPE_PASS], std::vector<Action_Rank>({0}));
        for (int i = 0; i <= 36; i++)
        {
            EXPECT_THAT(legalActions[i], std::vector<Action_Rank>());
        }

        std::shared_ptr<ActionData> passAd(new ActionData(ActionData::Zero()));
        (*passAd)[54] = 1;
        Action passAction = Action(passAd, ACTION_TYPE_PASS, 0);
        game->play(passAction); // player2 play pass, test pass
        game->getLegalActions(legalActions);
        EXPECT_THAT(legalActions[ACTION_TYPE_PASS], std::vector<Action_Rank>({0}));
        for (int i = 0; i <= 36; i++)
        {
            EXPECT_THAT(legalActions[i], std::vector<Action_Rank>());
        }

        game->play(passAction); // landlord play pass, test getAllActions
        game->getLegalActions(legalActions);
        std::vector<std::vector<Action_Rank>> player1AllActions;
        game->getAllActions(1, player1AllActions);
        EXPECT_THAT(legalActions, player1AllActions);
    }
}

TEST(Game, getActionCards)
{
    std::unique_ptr<Game> game;
    std::vector<std::unique_ptr<Action>> actionCards;

    // no faceup cards
    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6D6S7H7C7D7S8H8C8D8S9H9C9D9STHTCTDTSJHJCJDJSQHQCQDQSKHKCKDKSAHACADAS2H2C2D2JBJR", "", "", "S3H3C3");
    game->getActionCards(ACTION_TYPE_SOLO, 0, actionCards);
    ASSERT_EQ(actionCards.size(), 1);
    ASSERT_EQ(actionCards[0]->getActionData()->sum(), 1);
    ASSERT_EQ(actionCards[0]->data()[0], 0); //S3
    ASSERT_EQ(actionCards[0]->data()[1], 0); //H3
    ASSERT_EQ(actionCards[0]->data()[2], 0); //C3
    ASSERT_EQ((*actionCards[0]->getActionData())[3], 1);

    game->getActionCards(ACTION_TYPE_PAIR, 0, actionCards);
    ASSERT_EQ(actionCards.size(), 1);
    Eigen::Array<CARD_ARRAY_DATA_TYPE, 1, 13> cards_13_1d = Eigen::Map<const Cards_52_2d_Type>(actionCards[0]->getActionData()->data()).colwise().sum();
    ASSERT_EQ(cards_13_1d.sum(), 2);
    ASSERT_EQ(cards_13_1d[0], 2);
    ASSERT_EQ((*actionCards[0]->getActionData())[3], 1);

    game = createGame("S3H3C3D3S4H4C4D4S5H5C5D5S6H6C6D6S7H7C7D7S8H8C8D8S9H9C9D9STHTCTDTSJHJCJDJSQHQCQDQSKHKCKDKSAHACADAS2H2C2D2JBJR", "", "", "D3D4D5");
    game->getActionCards(ACTION_TYPE_SOLO, 0, actionCards);
    ASSERT_EQ(actionCards.size(), 1);
    cards_13_1d = Eigen::Map<const Cards_52_2d_Type>(actionCards[0]->getActionData()->data()).colwise().sum();
    ASSERT_EQ(cards_13_1d.sum(), 1);
    ASSERT_EQ(cards_13_1d[0], 1);
    ASSERT_EQ(actionCards[0]->data()[3], 0); //D3
    ASSERT_EQ(actionCards[0]->data()[7], 0); //D4
    ASSERT_EQ(actionCards[0]->data()[11], 0); //D5

    // ACTION_TYPE_TRIO_SOLO_CHAIN_X
    game = createGame("S5H5C5D5S6H6C6D6S7H7C7D7S8H8C8D8S9H9C9D9STHTCTDT", "", "", "");
    game->getActionCards(ACTION_TYPE_TRIO_SOLO_CHAIN_4, 3, actionCards);
    ASSERT_EQ(actionCards.size(), 1);
    cards_13_1d = Eigen::Map<const Cards_52_2d_Type>(actionCards[0]->getActionData()->data()).colwise().sum();
    ASSERT_EQ(cards_13_1d.sum(), s_actionTypeSumCards[ACTION_TYPE_TRIO_SOLO_CHAIN_4]);
    ASSERT_EQ(cards_13_1d[2], 2);
    ASSERT_EQ(cards_13_1d[7], 2);

    // ACTION_TYPE_FOUR_TWO_PAIR
    game = createGame("S3H3C3D3S4H4C4D4", "", "", "");
    EXPECT_DEATH(game->getActionCards(ACTION_TYPE_FOUR_TWO_PAIR, 0, actionCards), ".*");
}
