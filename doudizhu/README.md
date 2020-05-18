# Usage of Doudizhu Game Engine

## Create a new game

```cpp
    std::array<int, 54> deck;

    Deck::setRandomSeed(100);
    Deck::deal(deck);

    std::unique_ptr<Game> game = Game::createNewGame(deck);
```

## Create a middle state game

```cpp
    // landlord (player 0) has Spade 4 and Red Joker
    // player 1 has Spade 3 and Diamond A
    // player 2 has Diamond 2
    // faceup cards are S3S4S5
    // next player id is currPlayerId
    // action history is historyActions
    Cards_54_1d_Type cards;
    GameCards gameCards(GameCards::Zero());

    gameCards.row(PUBLIC_CARDS_ROW) = Cards_54_1d_Type::Ones();

    cardsFromString(cards, "S4JR");
    gameCards.row(LANDLORD_CARDS_ROW) += cards;
    gameCards.row(PUBLIC_CARDS_ROW) -= cards;

    cardsFromString(cards, "S3DA");
    gameCards.row(PLAYER_1_CARDS_ROW) += cards;
    gameCards.row(PUBLIC_CARDS_ROW) -= cards;

    cardsFromString(cards, "D2");
    gameCards.row(PLAYER_2_CARDS_ROW) += cards;
    gameCards.row(PUBLIC_CARDS_ROW) -= cards;

    cardsFromString(cards, "S3S4S5");
    gameCards.row(FACEUP_CARDS_ROW) += cards;

    // setup historyActions ...

    std::unique_ptr<Game> game(new Game(currPlayerId, gameCards, historyActions));
```

## Play an action

```cpp
    //play Spades 3
    std::shared_ptr<ActionData> ad1(new ActionData(ActionData::Zero()));
    cardsFromString(Eigen::Map<Cards_54_1d_Type>(ad1->data()), "S3"); //(*ad1)[0] = 1;
    Action action1 = Action(ad1, ACTION_TYPE_SOLO, 0);
    bool result = game->play(action1);

    //play PASS
    (*ad1).setZero();
    (*ad1)[54] = 1;
    Action action11 = Action(ad1, ACTION_TYPE_PASS, 0);
    bool result = game->play(action11);
```

## Play back

```cpp
    bool result = game->playBack();
```

## Get game status

```cpp
    // get game cards
    GameCards gameCards = game->getCards();

    // get game history
    HistoryActions history = game->getHistory();
```

## If game is over and get the payoff

```cpp
    // if game is over
    bool result = game->isOver();

    // get payoff
    Game::Payoff payoff;
    result = game->getPayoff(payoff);
```

## Get current player's all legal actions for play

```cpp
    std::vector<std::vector<Action_Rank>> legalGameTypeAndRanks;
    // get all legal action type and ranks
    game->getLegalActions(legalGameTypeAndRanks);
    for (int type = 0; type < ACTION_TYPE_SIZE; ++type)
    {
        auto ranks = legalGameTypeAndRanks[type];
        if (!ranks.empty())
        {
            for (Action_Rank rank : ranks)
            {
                // get actions of the type and rank
                std::vector<std::unique_ptr<Action>> actions;
                game->getActionCards(Action_Type(type), rank, actions);

                // user can play some action now
                // game->play(actions[i]);
            }
        }
    }
```
