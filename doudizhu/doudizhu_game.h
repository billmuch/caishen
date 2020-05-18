#ifndef CAISHEN_DOUDIZHU_DOUDIZHU_GAME_H_
#define CAISHEN_DOUDIZHU_DOUDIZHU_GAME_H_

#include "doudizhu_types.h"
#include "doudizhu_action.h"

#include <vector>
#include <Eigen/Dense>
#include <memory>

namespace caishen
{
namespace doudizhu
{

/**
 * @brief A doudizhu game
 * 
 */
class Game
{
public:
    static const int NUM_PLAYER = 3;
    static const int LAND_LORD_PLAYER_ID = 0;

    /**
     * @brief Represents Payoff of the game. 
     * 
     * [1 0 0] if landlord wins or [0 1 1] for peasants win
     * 
     */
    typedef Eigen::Array<double, 3, 1> Payoff;

public:
    /**
     * @brief Create a new game with cards
     * 
     * @param cards int array in which are card indexes in the @ref Deck::STANDARD_DECK
     * @return std::unique_ptr<Game> 
     */
    static std::unique_ptr<Game> createNewGame(const std::array<int, 54> &cards, bool isTrain = false);

    /**
     * @brief Construct a middle stage Game
     * 
     * @param currPlayerId 
     * @param gameCards 
     * @param history 
     */
    Game(const int currPlayerId, const GameCards &gameCards, const HistoryActions &history, bool isTrain = false);
    virtual ~Game();

    /**
     * @brief If the action is a valid action
     * 
     * @param action 
     * @return true 
     * @return false 
     */
    bool isValidAction(const Action &action);

    /**
     * @brief Play an action
     * 
     * @param action 
     * @return true If the action is played successfully
     * @return false If game already over or action is invalid
     */
    bool play(Action &action);

    /**
     * @brief Play back the last action
     * 
     * @return true if the last action was played back
     * @return false If no actions can be played back
     */
    bool playBack();
    inline const bool isOver() { return _winnerId != -1; }

    /**
     * @brief Get the @ref Game::Payoff Payoff
     * 
     * @param [out] payoff 
     * @return true if game is over and payoff is return, bool train = trueed
     * @return false if game is not over
     */
    const bool getPayoff(Eigen::Ref<Game::Payoff> payoff);

    /**
     * @brief Get the current player id
     * 
     * @return const int playerId
     */
    inline const int getPlayerId() { return _playerId; }

    /**
     * @brief Get the current game cards (each players cards, public cards and 3 face up cards)
     * 
     * @return const GameCards& game cards
     */
    inline const GameCards &getCards() { return _gameCards; }

    /**
     * @brief Get the history 
     * 
     * @return const HistoryActions& 
     */
    inline const HistoryActions &getHistory() { return _history; }

    /**
     * @brief Get current player's all legal actions
     * 
     * @param [out] legalActions 2 dimention vector, legalActions[actionType] is all ranks the current player can play. It will
     * be an empty vector if the current player can not play the actionType action
     */
    void getLegalActions(std::vector<std::vector<Action_Rank>> & legalActions);

    /**
     * @brief Get current player's Action Cards of Action_Type type and Action_Rank rank
     * 
     * @param type 
     * @param rank 
     * @param [out] actionCards actionCards will be cleared before add result into it
     */
    void getActionCards(Action_Type type, Action_Rank rank, std::vector<std::unique_ptr<Action>> & actionCards);

    /**
     * @brief Get action type ACTION_TYPE_SOLO ranks the player[playerId]'s hand can play, start from startRank
     * 
     * @param playerId
     * @param startRank 
     * @param [out] availableRanks 
     */
    void get_SOLO_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks);

    /**
     * @brief Get action type ACTION_TYPE_SOLO_CHAIN_X ranks the player[playerId]'s hand can play, start from startRank
     * 
     * @param X 
     * @param playerId 
     * @param startRank 
     * @param [out] availableRanks 
     */
    void get_SOLO_CHAIN_X_Actions(int X, int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks);

    /**
     * @brief Get action type ACTION_TYPE_PAIR ranks the player[playerId]'s hand can play, start from startRank
     * 
     * @param playerId 
     * @param startRank 
     * @param availableRanks 
     */
    void get_PAIR_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks);

    /**
     * @brief Get action type ACTION_TYPE_PAIR_CHAIN_X ranks the player[playerId]'s hand can play, start from startRank
     * 
     * @param X 
     * @param playerId 
     * @param startRank 
     * @param availableRanks 
     */
    void get_PAIR_CHAIN_X_Actions(int X, int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks);

    /**
     * @brief Get action type ACTION_TYPE_TRIO ranks the player[playerId]'s hand can play, start from startRank
     * 
     * @param playerId 
     * @param startRank 
     * @param availableRanks 
     */
    void get_TRIO_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks);

    /**
     * @brief Get action type ACTION_TYPE_TRIO_CHAIN_X ranks the player[playerId]'s hand can play, start from startRank
     * 
     * @param X 
     * @param playerId 
     * @param startRank 
     * @param availableRanks 
     */
    void get_TRIO_CHAIN_X_Actions(int X, int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks);

    /**
     * @brief Get action type ACTION_TYPE_TRIO_SOLO ranks the player[playerId]'s hand can play, start from startRank
     * 
     * @param playerId 
     * @param startRank 
     * @param availableRanks 
     */
    void get_TRIO_SOLO_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks);

    /**
     * @brief Get action type ACTION_TYPE_TRIO_CHAIN_X ranks the player[playerId]'s hand can play, start from startRank
     * 
     * @param X 
     * @param playerId 
     * @param startRank 
     * @param availableRanks 
     */
    void get_TRIO_SOLO_CHAIN_X_Actions(int X, int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks);

    /**
     * @brief Get action type ACTION_TYPE_PAIR ranks the player[playerId]'s hand can play, start from startRank
     * 
     * @param playerId 
     * @param startRank 
     * @param availableRanks 
     */
    void get_TRIO_PAIR_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks);

    /**
     * @brief Get action type ACTION_TYPE_PAIR_CHAIN_X ranks the player[playerId]'s hand can play, start from startRank
     * 
     * @param X 
     * @param playerId 
     * @param startRank 
     * @param availableRanks 
     */
    void get_TRIO_PAIR_CHAIN_X_Actions(int X, int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks);

    /**
     * @brief Get action type ACTION_TYPE_FOUR_TWO_SOLO ranks the player[playerId]'s hand can play, start from startRank
     * 
     * @param playerId 
     * @param startRank 
     * @param availableRanks 
     */
    void get_FOUR_TWO_SOLO_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks);

    /**
     * @brief Get action type ACTION_TYPE_FOUR_TWO_PAIR ranks the player[playerId]'s hand can play, start from startRank
     * 
     * @param playerId 
     * @param startRank 
     * @param availableRanks 
     */
    void get_FOUR_TWO_PAIR_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks);

    /**
     * @brief Get action type ACTION_TYPE_BOMB ranks the player[playerId]'s hand can play, start from startRank
     * 
     * @param playerId 
     * @param startRank 
     * @param availableRanks 
     */
    void get_BOMB_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks);

    /**
     * @brief Get action type ACTION_TYPE_ROCKET ranks the player[playerId]'s hand can play, start from startRank
     * 
     * @param playerId 
     * @param startRank 
     * @param availableRanks 
     */
    void get_ROCKET_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks);

    /**
     * @brief Get current player's all legal actions
     * 
     
     */
    /**
     * @brief Get all action type and all ranks the player[playerId]'s hand can play
     * 
     * @param playerId 
     * @param [out] actionRanks 2 dimention vector, actionRanks[actionType] is all ranks the player[playerId]'s hand can play
     * for the action type actionType. It will be an empty vector if the player can not play the actionType action
     */
    void getAllActions(int playerId, std::vector<std::vector<Action_Rank>> & actionRanks);

private:
    Game(bool isTrain = false);

    inline void refreshCachedPlayerHandsByRank(int playerId);
    inline void refreshChainIndexes(int _playerId);

private:
    int _playerId;
    GameCards _gameCards;
    HistoryActions _history;

    bool _isTrain;

    int _winnerId;

    /**
     * @brief reference to the first 3 rows of _gameCards
     * 
     */
    Eigen::Map<Cards_54_1d_Type> _playerHands[3];

    /**
     * @brief cache of the player hands by rank
     * 
     */
    Eigen::Array<CARD_ARRAY_DATA_TYPE, 1, 15> _cachedPlayerHandsByRank[3];
    /**
     * @brief if the cache of player hands by rank need to be refresh
     * 
     */
    bool _staledCachedPlayerHandsByRank[3];

    std::vector<std::pair<int, int>> _soloChainIndexes[3];
    std::vector<std::pair<int, int>> _pairChainIndexes[3];
    std::vector<std::pair<int, int>> _trioChainIndexes[3];
    bool _staledPlayerHandsChainIndexes[3];

    /**
     * @brief reference to the 4th row of _gameCards
     * 
     */
    Eigen::Map<Cards_54_1d_Type> _faceUpCards;

    /**
     * @brief reference to the 5th row of _gameCards
     * 
     */
    Eigen::Map<Cards_54_1d_Type> _publicCards;
};

} // namespace doudizhu
} // namespace caishen

#endif // CAISHEN_DOUDIZHU_DOUDIZHU_GAME_H_