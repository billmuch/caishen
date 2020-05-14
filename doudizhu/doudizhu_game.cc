#include "doudizhu_game.h"
#include "doudizhu_action.h"
#include "doudizhu_utils.h"

#include <iostream>

namespace caishen
{
namespace doudizhu
{

const int Game::NUM_PLAYER;
const int Game::LAND_LORD_PLAYER_ID;

Game::Game(bool isTrain)
    : _playerId(LAND_LORD_PLAYER_ID),
      _gameCards(GameCards::Zero()),
      _isTrain(isTrain),
      _winnerId(-1),
      _playerHands{Eigen::Map<Cards_54_1d_Type>(_gameCards.data()),
                   Eigen::Map<Cards_54_1d_Type>(_gameCards.data() + 54 * sizeof(CARD_ARRAY_DATA_TYPE)),
                   Eigen::Map<Cards_54_1d_Type>(_gameCards.data() + 2 * 54 * sizeof(CARD_ARRAY_DATA_TYPE))},
      _staledCachedPlayerHandsByRank{true, true, true},
      _staledPlayerHandsChainIndexes{true, true, true},
      _faceUpCards(_gameCards.data() + 3 * 54 * sizeof(CARD_ARRAY_DATA_TYPE)),
      _publicCards(_gameCards.data() + 4 * 54 * sizeof(CARD_ARRAY_DATA_TYPE))
{}

std::unique_ptr<Game> Game::createNewGame(const std::array<int, 54> & cards, bool isTrain)
{
    std::unique_ptr<Game> game(new Game(isTrain));
    for (int i = 0; i < 17; i++)
    {
        game->_playerHands[0][cards[i]] = 1;
    }
    for (int i = 17; i < 20; i++)
    {
        game->_playerHands[0][cards[i]] = 1;
        game->_faceUpCards[cards[i]] = 1;
    }
    for (int i = 20; i < 37; i++)
    {
        game->_playerHands[1][cards[i]] = 1;
    }
    for (int i = 37; i < 54; i++)
    {
        game->_playerHands[2][cards[i]] = 1;
    }

    return game;
}

Game::Game(const int currPlayerId, const GameCards &gameCards, const HistoryActions &history, bool isTrain)
    : _playerId(currPlayerId), _gameCards(gameCards), _history(history), _isTrain(isTrain),
      _playerHands{Eigen::Map<Cards_54_1d_Type>(_gameCards.data()),
                   Eigen::Map<Cards_54_1d_Type>(_gameCards.data() + 54 * sizeof(CARD_ARRAY_DATA_TYPE)),
                   Eigen::Map<Cards_54_1d_Type>(_gameCards.data() + 2 * 54 *sizeof(CARD_ARRAY_DATA_TYPE))},
      _staledCachedPlayerHandsByRank{true, true, true},
      _staledPlayerHandsChainIndexes{true, true, true},
      _faceUpCards(_gameCards.data() + 3 * 54 * sizeof(CARD_ARRAY_DATA_TYPE)),
      _publicCards(_gameCards.data() + 4 * 54 * sizeof(CARD_ARRAY_DATA_TYPE))
{
    _winnerId = -1;

    for(int i = 0; i < NUM_PLAYER; i++)
    {
        if (_playerHands[i].sum() == 0)
        {
            _winnerId = i;
            break;
        }
    }
}

Game::~Game() {}

void Game::refreshCachedPlayerHandsByRank(int playerId)
{
    if (_staledCachedPlayerHandsByRank[_playerId])
    {
        Eigen::Map<Eigen::Array<CARD_ARRAY_DATA_TYPE, 1, 13>>(_cachedPlayerHandsByRank[playerId].data()) = Eigen::Map<Cards_52_2d_Type>(_playerHands[playerId].data()).colwise().sum();
        _cachedPlayerHandsByRank[playerId][13] = _playerHands[playerId][52];
        _cachedPlayerHandsByRank[playerId][14] = _playerHands[playerId][53];

        _staledCachedPlayerHandsByRank[playerId] = false;
    }
}

void Game::refreshChainIndexes(int _playerId)
{
    if (_staledPlayerHandsChainIndexes[_playerId])
    {
        refreshCachedPlayerHandsByRank(_playerId);

        getChainIndexes(Eigen::Map<const Eigen::Array<CARD_ARRAY_DATA_TYPE, 1, 12>>(_cachedPlayerHandsByRank[_playerId].data()), 
                _soloChainIndexes[_playerId], 
                _pairChainIndexes[_playerId], 
                _trioChainIndexes[_playerId]);

        _staledPlayerHandsChainIndexes[_playerId] = false;
    }
}

bool Game::isValidAction(const Action & action)
{
    Action_Type actionType;
    Action_Rank actionRank;

    if (!action.isValidTypeAndRank())
    {
        return false;
    }

    std::tie(actionType, actionRank) = action.getDetails();

    Action_Type roundActionType = Action_Type::ACTION_TYPE_PASS;
    Action_Rank roundActionRank = 0;
    if (!_history.empty())
    {
        Action_Type prevType;
        Action_Rank prevRank;
        std::tie(prevType, prevRank) = _history.back().getDetails();
        if (prevType == Action_Type::ACTION_TYPE_PASS)
        {
            if (_history.size() > 1)
            {
                std::tie(roundActionType, roundActionRank) = (_history.end() - 2)->getDetails();
            }
        }
        else
        {
            roundActionType = prevType;
            roundActionRank = prevRank;
        }
    }

    switch (roundActionType)
    {
    case Action_Type::ACTION_TYPE_PASS:
        if (actionType == Action_Type::ACTION_TYPE_PASS)
        {
            return false;
        }
        else
        {
            break;
        }
    case Action_Type::ACTION_TYPE_ROCKET:
        return false;
    case Action_Type::ACTION_TYPE_BOMB:
        if (actionType == Action_Type::ACTION_TYPE_ROCKET || (actionType == Action_Type::ACTION_TYPE_BOMB && actionRank > roundActionRank))
        {
            break;
        }
        else
        {
            return false;
        }
    default:
        if (actionType == Action_Type::ACTION_TYPE_PASS 
            || (actionType == roundActionType && actionRank > roundActionRank)
            || actionType == Action_Type::ACTION_TYPE_BOMB 
            || actionType == Action_Type::ACTION_TYPE_ROCKET)
        {
            break;
        }
        else
        {
            return false;
        }
    }

    //if all action cards are in player's hands
    if ((_playerHands[_playerId] < Eigen::Map<const Cards_54_1d_Type>(action.data())).any())
    {
        return false;
    }

    return true;
}

bool Game::play(Action &action)
{
    if (isOver())
    {
        return false;
    }

    if (!_isTrain && !isValidAction(action))
    {
        return false;
    }    

    _history.push_back(action);

    Action_Type actionType;
    Action_Rank actionRank;
    std::tie(actionType, actionRank) = action.getDetails();
    if (actionType != Action_Type::ACTION_TYPE_PASS)
    {
        Eigen::Map<const Cards_54_1d_Type> actionCards(action.data());
        _playerHands[_playerId] -= actionCards;
        _publicCards += actionCards;

        if (_playerHands[_playerId].sum() == 0)
        {
            _winnerId = _playerId;
        }

        _staledCachedPlayerHandsByRank[_playerId] = true;
        _staledPlayerHandsChainIndexes[_playerId] = true;
    }

    _playerId = (_playerId + 1) % NUM_PLAYER;

    return true;
}

bool Game::playBack()
{
    if (_history.empty())
    {
        return false;
    }

    Action lastAction = _history.back();
    _history.pop_back();

    _winnerId = -1;
    _playerId = (_playerId + 2) % NUM_PLAYER;

    if (lastAction.getDetails().first != Action_Type::ACTION_TYPE_PASS)
    {
        Eigen::Map<const Cards_54_1d_Type> actionCards(lastAction.data());
        _publicCards -= actionCards;
        _playerHands[_playerId] += actionCards;

        _staledCachedPlayerHandsByRank[_playerId] = true;
        _staledPlayerHandsChainIndexes[_playerId] = true;
    }

    return true;
}

const bool Game::getPayoff(Eigen::Ref<Game::Payoff> payoff)
{
    if (isOver())
    {
        if (_winnerId == 0)
        {
            payoff[0] = 1.0;
            payoff[1] = 0.0;
            payoff[2] = 0.0;
        }
        else
        {
            payoff[0] = 0.0;
            payoff[1] = 1.0;
            payoff[2] = 1.0;
        }

        return true;
    }
    else
    {
        return false;
    }
}

void Game::get_SOLO_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks)
{
    assert(startRank >= 0);

    availableRanks.clear();

    refreshCachedPlayerHandsByRank(_playerId);

    for (int i = startRank; i < 15; i++)
    {
        if (_cachedPlayerHandsByRank[playerId][i] > 0)
        {
            availableRanks.push_back(i);
        }
    }
}

void Game::get_SOLO_CHAIN_X_Actions(int X, int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks)
{
    assert(startRank >= 0);

    availableRanks.clear();

    refreshCachedPlayerHandsByRank(_playerId);
    refreshChainIndexes(_playerId);

    int start, len;
    for (auto indexes : _soloChainIndexes[_playerId])
    {
        std::tie(start, len) = indexes;
        if (len >= X)
        {
            for (int i = std::max(start, startRank); i <= (start + len) - X; i++)
            {
                availableRanks.push_back(i);
            }
        }
    }
}

void Game::get_PAIR_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks)
{
    assert(startRank >= 0);

    availableRanks.clear();

    refreshCachedPlayerHandsByRank(_playerId);

    for (int i = startRank; i <= 12; i++)
    {
        if (_cachedPlayerHandsByRank[playerId][i] > 1)
        {
            availableRanks.push_back(i);
        }
    }
}

void Game::get_PAIR_CHAIN_X_Actions(int X, int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks)
{
    assert(startRank >= 0);

    availableRanks.clear();

    refreshCachedPlayerHandsByRank(_playerId);
    refreshChainIndexes(_playerId);

    int start, len;
    for (auto indexes : _pairChainIndexes[_playerId])
    {
        std::tie(start, len) = indexes;
        if (len >= X)
        {
            for (int i = std::max(start, startRank); i <= (start + len) - X; i++)
            {
                availableRanks.push_back(i);
            }
        }
    }
}

void Game::get_TRIO_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks)
{
    assert(startRank >= 0);

    availableRanks.clear();

    refreshCachedPlayerHandsByRank(_playerId);

    for (int i = startRank; i <= 12; i++)
    {
        if (_cachedPlayerHandsByRank[playerId][i] > 2)
        {
            availableRanks.push_back(i);
        }
    }
}

void Game::get_TRIO_CHAIN_X_Actions(int X, int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks)
{
    assert(startRank >= 0);

    availableRanks.clear();

    refreshCachedPlayerHandsByRank(_playerId);
    refreshChainIndexes(_playerId);

    int start, len;
    for (auto indexes : _trioChainIndexes[_playerId])
    {
        std::tie(start, len) = indexes;
        if (len >= X)
        {
            for (int i = std::max(start, startRank); i <= (start + len) - X; i++)
            {
                availableRanks.push_back(i);
            }
        }
    }
}

void Game::get_TRIO_SOLO_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks)
{
    assert(startRank >= 0);

    availableRanks.clear();

    refreshCachedPlayerHandsByRank(_playerId);

    int sum = _cachedPlayerHandsByRank[playerId].sum();

    for (int i = startRank; i <= 12; i++)
    {
        if ((_cachedPlayerHandsByRank[playerId][i] > 2) && (sum - _cachedPlayerHandsByRank[playerId][i] > 0))
        {
            availableRanks.push_back(i);
        }
    }
}

void Game::get_TRIO_SOLO_CHAIN_X_Actions(int X, int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks)
{
    assert(startRank >= 0);

    availableRanks.clear();

    refreshCachedPlayerHandsByRank(_playerId);
    refreshChainIndexes(_playerId);

    int sum = _cachedPlayerHandsByRank[playerId].sum();
    //no 333444_BR
    if (_cachedPlayerHandsByRank[_playerId][13] && _cachedPlayerHandsByRank[_playerId][14])
    {
        sum -= 1;
    }

    int start, len;
    for (auto indexes : _trioChainIndexes[_playerId])
    {
        std::tie(start, len) = indexes;
        if (len >= X)
        {
            for (int i = std::max(start, startRank); i <= (start + len) - X; i++)
            {
                int sumCopy = sum;
                for(int j = i; j < i + X; j++)
                {
                    //no 333444_34
                    sumCopy -= _cachedPlayerHandsByRank[_playerId][j];
                }

                //no 444555666_333
                if (X > 2 && i > 0 && _cachedPlayerHandsByRank[_playerId][i - 1] > 2)
                {
                    sumCopy -= (_cachedPlayerHandsByRank[_playerId][i - 1] - 2);
                }

                //no 444555666_777
                if (X > 2 && (i + X < 12) && _cachedPlayerHandsByRank[_playerId][i + X] > 2)
                {
                    sumCopy -= (_cachedPlayerHandsByRank[_playerId][i + X] - 2);
                }

                //no 444555666777_9999
                if (X > 3)
                {
                    for (int j = 0; j <= 12; j++)
                    {
                        if (j >= i - 1 and j <= i + X)
                        {
                            continue;
                        }
                        if (_cachedPlayerHandsByRank[_playerId][j] == 4)
                        {
                            sumCopy -= 1;
                        }
                    }
                }

                if (sumCopy >= X)
                {
                    availableRanks.push_back(i);
                }
            }
        }
    }
}

void Game::get_TRIO_PAIR_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks)
{
    assert(startRank >= 0);

    availableRanks.clear();

    refreshCachedPlayerHandsByRank(_playerId);

    for (int i = startRank; i <= 12; i++)
    {
        if (_cachedPlayerHandsByRank[playerId][i] > 2)
        {
            for (int j = 0; j <= 12; j++)
            {
                if (j == i) continue;
                if (_cachedPlayerHandsByRank[playerId][j] >= 2)
                {
                    availableRanks.push_back(i);
                    break;
                }
            }
        }
    }
}

void Game::get_TRIO_PAIR_CHAIN_X_Actions(int X, int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks)
{
    assert(startRank >= 0);

    availableRanks.clear();

    refreshCachedPlayerHandsByRank(_playerId);
    refreshChainIndexes(_playerId);

    int start, len;
    for (auto indexes : _trioChainIndexes[_playerId])
    {
        std::tie(start, len) = indexes;
        if (len >= X)
        {
            for (int i = std::max(start, startRank); i <= (start + len) - X; i++)
            {
                int pairs = 0;
                for (int j = 0; j <= 12; j++)
                {
                    if (j >= i && j < i + X) continue;
                    if (_cachedPlayerHandsByRank[playerId][j] >= 2)
                    {
                        if (++pairs >= X) 
                        {
                            availableRanks.push_back(i);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void Game::get_FOUR_TWO_SOLO_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks)
{
    assert(startRank >= 0);

    availableRanks.clear();

    refreshCachedPlayerHandsByRank(_playerId);

    int sum = _cachedPlayerHandsByRank[playerId].sum();
    //no 3333_BR
    if (_cachedPlayerHandsByRank[_playerId][13] && _cachedPlayerHandsByRank[_playerId][14])
    {
        sum -= 1;
    }

    for (int i = startRank; i <= 12; i++)
    {
        if ((_cachedPlayerHandsByRank[playerId][i] == 4) && (sum - 4 >= 2))
        {
            availableRanks.push_back(i);
        }
    }
}

void Game::get_FOUR_TWO_PAIR_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks)
{
    assert(startRank >= 0);

    availableRanks.clear();

    refreshCachedPlayerHandsByRank(_playerId);

    for (int i = startRank; i <= 12; i++)
    {
        if (_cachedPlayerHandsByRank[playerId][i] == 4)
        {
            int pairs = 0;
            for (int j = 0; j <= 12; j++)
            {
                if (j == i) continue;
                if (_cachedPlayerHandsByRank[playerId][j] >= 2)
                {
                    if (++pairs >= 2) 
                    {
                        availableRanks.push_back(i);
                        break;
                    }
                }
            }
        }
    }
}

void Game::get_BOMB_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks)
{
    assert(startRank >= 0);

    availableRanks.clear();

    refreshCachedPlayerHandsByRank(_playerId);

    for (int i = startRank; i <= 12; i++)
    {
        if (_cachedPlayerHandsByRank[playerId][i] == 4)
        {
            availableRanks.push_back(i);
        }
    }
}

void Game::get_ROCKET_Actions(int playerId, Action_Rank startRank, std::vector<Action_Rank> & availableRanks)
{
    assert(startRank >= 0);

    availableRanks.clear();

    refreshCachedPlayerHandsByRank(_playerId);

    if (startRank == 0 && _playerHands[playerId][52] && _playerHands[playerId][53])
    {
        availableRanks.push_back(0);
    }
}

void Game::getAllActions(int playerId, std::vector<std::vector<Action_Rank>> & ranks)
{
     ranks.clear();

    for (int i = 0; i <= int(Action_Type::ACTION_TYPE_PASS); i++)
    {
        ranks.push_back(std::vector<Action_Rank>());
    }

    refreshCachedPlayerHandsByRank(_playerId);
    refreshChainIndexes(_playerId);

    int sum = _cachedPlayerHandsByRank[playerId].sum();

    //PASS
    ranks[ACTION_TYPE_PASS].push_back(0);

    //ROCKET
    if (_cachedPlayerHandsByRank[playerId][13] && _cachedPlayerHandsByRank[playerId][14])
    {
        ranks[ACTION_TYPE_ROCKET].push_back(0);
    }

    for (int i = 0; i <= 12; i++)
    {
        std::vector<Action_Rank> & soloRanks = ranks[ACTION_TYPE_SOLO];
        std::vector<Action_Rank> & pairRanks = ranks[ACTION_TYPE_PAIR];
        std::vector<Action_Rank> & trioRanks = ranks[ACTION_TYPE_TRIO];
        std::vector<Action_Rank> & bombRanks = ranks[ACTION_TYPE_BOMB];
        std::vector<Action_Rank> & trioSoloRanks = ranks[ACTION_TYPE_TRIO_SOLO];
        std::vector<Action_Rank> & trioPairRanks = ranks[ACTION_TYPE_TRIO_PAIR];
        std::vector<Action_Rank> & fourTwoSoloRanks = ranks[ACTION_TYPE_FOUR_TWO_SOLO];
        std::vector<Action_Rank> & fourTwoPairRanks = ranks[ACTION_TYPE_FOUR_TWO_PAIR];

        //BOMB
        if (_cachedPlayerHandsByRank[playerId][i] == 4)
        {
            bombRanks.push_back(i);

            //FOUR TWO SOLO
            //- ranks[ACTION_TYPE_ROCKET].size() for no 3333_BR type
            if (sum - 4 - ranks[ACTION_TYPE_ROCKET].size() >= 2)
            {
                fourTwoSoloRanks.push_back(i);
            }

            //FOUR TWO PAIR
            int pairs = 0;
            for (int j = 0; j <= 12; j++)
            {
                if (j == i) continue;
                if (_cachedPlayerHandsByRank[playerId][j] >= 2)
                {
                    if (++pairs >= 2) 
                    {
                        fourTwoPairRanks.push_back(i);
                        break;
                    }
                }
            }
        }
        
        //TRIO
        if (_cachedPlayerHandsByRank[playerId][i] >= 3)
        {
            trioRanks.push_back(i);
            //TRIO SOLO
            if (sum - _cachedPlayerHandsByRank[playerId][i] > 0)
            {
                trioSoloRanks.push_back(i);
            }
            //TRIO PAIR
            for (int j = 0; j <= 12; j++)
            {
                if (j == i) continue;
                if (_cachedPlayerHandsByRank[playerId][j] >= 2)
                {
                    trioPairRanks.push_back(i);
                    break;
                }
            }
        }

        //PAIR
        if (_cachedPlayerHandsByRank[playerId][i] >= 2 )
        {
            pairRanks.push_back(i);
        }

        //SOLO without BJ RJ
        if (_cachedPlayerHandsByRank[playerId][i] >= 1 )
        {
            soloRanks.push_back(i);
        }
    }

    //SOLO BJ
    if (_cachedPlayerHandsByRank[playerId][13])
    {
        ranks[ACTION_TYPE_SOLO].push_back(13);
    }
    //SOLO RJ
    if (_cachedPlayerHandsByRank[playerId][14])
    {
        ranks[ACTION_TYPE_SOLO].push_back(14);
    }
    
    //SOLO CHAIN
    for (auto indexes : _soloChainIndexes[_playerId])
    {
        int start, len;
        std::tie(start, len) = indexes;
        for (int i = 0; i <= len - 5; i++)
        {
            for (int l = 5; l <= len - i; l++)
            {
                assert(l <= 12);
                //ACTION_TYPE_SOLO_CHAIN_(l), start from (i + start)
                ranks[l - 5 + ACTION_TYPE_SOLO_CHAIN_5].push_back(i + start);
            }
        }
    }

    //PAIR CHAIN
    for (auto indexes : _pairChainIndexes[_playerId])
    {
        int start, len;
        std::tie(start, len) = indexes;
        for (int i = 0; i <= len - 3; i++)
        {
            for (int l = 3; l <= std::min(len - i, 10); l++)
            {
                assert(l <= 10);
                ranks[l - 3 + ACTION_TYPE_PAIR_CHAIN_3].push_back(i + start);
            }
        }
    }

    //TRIO CHAIN
    for (auto indexes : _trioChainIndexes[_playerId])
    {
        int start, len;
        std::tie(start, len) = indexes;
        for (int i = 0; i <= len - 2; i++)
        {
            for (int l = 2; l <= std::min(len - i, 6); l++)
            {
                assert(l <= 6);
                ranks[l - 2 + ACTION_TYPE_TRIO_CHAIN_2].push_back(i + start);
            }
        }
    }

    //TRIO SOLO CHAIN
    for (int i = 0; i < 4; i++)
    {
        get_TRIO_SOLO_CHAIN_X_Actions(i + 2, playerId, 0, ranks[i + ACTION_TYPE_TRIO_SOLO_CHAIN_2]);
    }

    //TRIO PAIR CHAIN
    for (int i = 0; i < 3; i++)
    {
        get_TRIO_SOLO_CHAIN_X_Actions(i + 2, playerId, 0, ranks[i + ACTION_TYPE_TRIO_PAIR_CHAIN_2]);
    }
}

void Game::getLegalActions(std::vector<std::vector<Action_Rank>> & legalActions)
{
    legalActions.clear();
    legalActions.assign(38, std::vector<Action_Rank>());

    if (isOver())
    {
        return;
    }

    Action_Type roundType = Action_Type::ACTION_TYPE_PASS;
    Action_Rank roundRank = 0;

    if (!_history.empty())
    {
        std::tie(roundType, roundRank) = _history.back().getDetails();
        if (roundType == Action_Type::ACTION_TYPE_PASS)
        {
            std::tie(roundType, roundRank) = _history[_history.size() - 2].getDetails();
        }
    }

    if (roundType != Action_Type::ACTION_TYPE_PASS)
    {
        switch(roundType)
        {
        case ACTION_TYPE_SOLO:
            get_SOLO_Actions(_playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_SOLO_CHAIN_5:
            get_SOLO_CHAIN_X_Actions(5, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_SOLO_CHAIN_6:
            get_SOLO_CHAIN_X_Actions(6, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_SOLO_CHAIN_7:
            get_SOLO_CHAIN_X_Actions(7, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_SOLO_CHAIN_8:
            get_SOLO_CHAIN_X_Actions(8, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_SOLO_CHAIN_9:
            get_SOLO_CHAIN_X_Actions(9, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_SOLO_CHAIN_10:
            get_SOLO_CHAIN_X_Actions(10, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_SOLO_CHAIN_11:
            get_SOLO_CHAIN_X_Actions(11, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_SOLO_CHAIN_12:
            get_SOLO_CHAIN_X_Actions(12, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_PAIR:
            get_PAIR_Actions(_playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_PAIR_CHAIN_3:
            get_PAIR_CHAIN_X_Actions(3, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_PAIR_CHAIN_4:
            get_PAIR_CHAIN_X_Actions(4, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_PAIR_CHAIN_5:
            get_PAIR_CHAIN_X_Actions(5, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_PAIR_CHAIN_6:
            get_PAIR_CHAIN_X_Actions(6, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_PAIR_CHAIN_7:
            get_PAIR_CHAIN_X_Actions(7, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_PAIR_CHAIN_8:
            get_PAIR_CHAIN_X_Actions(8, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_PAIR_CHAIN_9:
            get_PAIR_CHAIN_X_Actions(9, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_PAIR_CHAIN_10:
            get_PAIR_CHAIN_X_Actions(10, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_TRIO:
            get_TRIO_Actions(_playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_TRIO_CHAIN_2:
            get_TRIO_CHAIN_X_Actions(2, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_TRIO_CHAIN_3:
            get_TRIO_CHAIN_X_Actions(3, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_TRIO_CHAIN_4:
            get_TRIO_CHAIN_X_Actions(4, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_TRIO_CHAIN_5:
            get_TRIO_CHAIN_X_Actions(5, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_TRIO_CHAIN_6:
            get_TRIO_CHAIN_X_Actions(6, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_TRIO_SOLO:
            get_TRIO_SOLO_Actions(_playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_TRIO_SOLO_CHAIN_2:
            get_TRIO_SOLO_CHAIN_X_Actions(2, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_TRIO_SOLO_CHAIN_3:
            get_TRIO_SOLO_CHAIN_X_Actions(3, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_TRIO_SOLO_CHAIN_4:
            get_TRIO_SOLO_CHAIN_X_Actions(4, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_TRIO_SOLO_CHAIN_5:
            get_TRIO_SOLO_CHAIN_X_Actions(5, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_TRIO_PAIR:
            get_TRIO_PAIR_Actions(_playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_TRIO_PAIR_CHAIN_2:
            get_TRIO_PAIR_CHAIN_X_Actions(2, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_TRIO_PAIR_CHAIN_3:
            get_TRIO_PAIR_CHAIN_X_Actions(3, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_TRIO_PAIR_CHAIN_4:
            get_TRIO_PAIR_CHAIN_X_Actions(4, _playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_FOUR_TWO_SOLO:
            get_FOUR_TWO_SOLO_Actions(_playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_FOUR_TWO_PAIR:
            get_FOUR_TWO_PAIR_Actions(_playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_BOMB:
            get_BOMB_Actions(_playerId, roundRank + 1, legalActions[roundType]);
            break;
        case ACTION_TYPE_ROCKET:
            // get_ROCKET_Actions(_playerId, roundRank + 1, legalActions[roundType]);
            assert(false);
            break;
        default:
            assert(false);
        }

        legalActions[Action_Type::ACTION_TYPE_PASS].push_back(0);
        if (roundType != Action_Type::ACTION_TYPE_ROCKET)
        {
            get_ROCKET_Actions(_playerId, 0, legalActions[roundType]);
            if (roundType != Action_Type::ACTION_TYPE_BOMB)
            {
                get_BOMB_Actions(_playerId, 0, legalActions[roundType]);
            }
        }
    }
    else
    {
        getAllActions(_playerId, legalActions);
    }
}

} // namespace doudizhu
} // namespace caishen
