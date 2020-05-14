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
    // next player id is currPlayerId
    // action history is historyActions
    Cards_54_1d_Type cards;
    GameCards gameCards(GameCards::Zero());

    gameCards.row(4) = Cards_54_1d_Type::Ones();

    cardsFromString("S4JR", landlordCards);
    gameCards.row(0) += cards;
    gameCards.row(4) -= cards;

    cardsFromString("S3DA", player1Cards);
    gameCards.row(1) += cards;
    gameCards.row(4) -= cards;

    cardsFromString("D2", player2Cards);
    gameCards.row(2) += cards;
    gameCards.row(4) -= cards;

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
    bool result = game->playback();
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
    bool result = game->getPayoff(payoff);
```
