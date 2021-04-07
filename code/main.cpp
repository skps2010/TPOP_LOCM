#include "tai-su.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

int main() {
  //    std::string action;
  //    CreatureCard c(1, 1, 1, 1, 1, 1, 1, "G", 1, 1, 1, 1), d(1, 2, 1, 1, 1,
  //    1, 1, "G", 1, 1, 1, 1); c.summon(action, 0); d.summon(action, 1);
  //    std::cout << c.canKill(d) << std::endl;
  //    c.attackTo(action, d);

  //    ItemCard i(1, 1, 1, 1, 1, 1, 1, "G", 1, 1, 1, 0);
  //    i.use(action, d.instanceID());

  //    std::cout << action << std::endl;
  //    std::cout << (i == d)<< std::endl;
  while (1) {
    std::vector<Card> cardOptions;
    std::vector<CreatureCard>
        board[4]; // myLeft myRight opponentLeft opponentRight
    int enemyTotalHP[2] = {0}, enemyTotalAttack[2] = {0}, ownTotalHP[2] = {0},
        ownTotalAttack[2] = {0};
    // game information
    int playerHealth, playerMana, playerDeck, playerRune, playerDraw;
    int opponentHealth, opponentMana, opponentDeck, opponentRune, opponentDraw;

    std::cin >> playerHealth >> playerMana >> playerDeck >> playerRune >>
        playerDraw;
    std::cin >> opponentHealth >> opponentMana >> opponentDeck >>
        opponentRune >> opponentDraw;

    CreatureCard player(-1, -1, MY_SIDE, CREATURE, 0, 0, playerHealth,
                        "------P", 0, 0, 0, -1, -1);
    CreatureCard opponent(-1, -1, OPPONENT_SIDE, CREATURE, 0, 0, opponentHealth,
                          "------P", 0, 0, 0, -1, -1);

    int opponentHand;
    int opponentActions;
    std::cin >> opponentHand >> opponentActions;
    std::cin.ignore();

    for (int i = 0; i < opponentActions; i++) {
      std::string cardNumberAndAction;
      getline(std::cin, cardNumberAndAction);
    }
    int cardCount;
    std::cin >> cardCount;
    std::cin.ignore();

    for (int i = 0; i < cardCount; i++) {
      int cardNumber, instanceID, location, cardType, cost, attack, defense,
          myHealthChange, opponentHealthChange, cardDraw, lane = LEFT;
      std::string abilities;
      std::cin >> cardNumber >> instanceID >> location >> cardType >> cost >>
          attack >> defense >> abilities >> myHealthChange >>
          opponentHealthChange >> cardDraw;
      if (VERSION == 2)
        std::cin >> lane;
      std::cin.ignore();

      if (playerMana == 0) { // Draw Phase
        Card card(cardNumber, instanceID, IN_HAND, cardType, cost, attack,
                  defense, abilities, myHealthChange, opponentHealthChange,
                  cardDraw, NOT_IN, i);
        cardOptions.push_back(card);
      } else {                     // Battle Phase
        if (location == IN_HAND) { // in my hand
          if (cardType == CREATURE) {
            CreatureCard summonLeft(cardNumber, instanceID, IN_HAND, cardType,
                                    cost, attack, defense, abilities,
                                    myHealthChange, opponentHealthChange,
                                    cardDraw, LEFT, i);
            cardOptions.push_back(summonLeft);
            if (VERSION == 2) {
              CreatureCard summonRight(
                  cardNumber, instanceID, IN_HAND, cardType, cost, attack,
                  defense, abilities, myHealthChange, opponentHealthChange,
                  cardDraw, RIGHT, i);
              cardOptions.push_back(summonRight);
            }
          } else {
            ItemCard item(cardNumber, instanceID, IN_HAND, cardType, cost,
                          attack, defense, abilities, myHealthChange,
                          opponentHealthChange, cardDraw, lane, i);
            cardOptions.push_back(item);
          }
        } else if (location == OPPONENT_SIDE) { // in opponent side of board
          CreatureCard tmp(cardNumber, instanceID, OPPONENT_SIDE, CREATURE,
                           cost, attack, defense, abilities, myHealthChange,
                           opponentHealthChange, cardDraw, lane, i);
          board[2 + lane].push_back(tmp);
          enemyTotalAttack[lane] += attack;
          enemyTotalHP[lane] += defense;
        } else if (location == MY_SIDE) { // in my side of board
          CreatureCard tmp(cardNumber, instanceID, IN_HAND, CREATURE, cost,
                           attack, defense, abilities, myHealthChange,
                           opponentHealthChange, cardDraw, lane, i);
          board[lane].push_back(tmp);
          ownTotalAttack[lane] += attack;
          ownTotalHP[lane] += defense;
        }
      }
    }

    std::string actions;

    // Draw Phase
    if (playerMana == 0) {
      for (auto &option : cardOptions) {
        option.calculateGetScore();
      }
      std::sort(cardOptions.begin(), cardOptions.end());
      cardOptions[0].pick(actions);
    }
    // Battle Phase
    else {
      // Summon
      for (auto &option : cardOptions) {
        if (option.cardType() == CREATURE) {
          ((CreatureCard *)&option)
              ->calculateUseScore(enemyTotalHP[option.lane()],
                                  ownTotalHP[option.lane()],
                                  enemyTotalAttack[option.lane()],
                                  ownTotalAttack[option.lane()]);
        } else { // Item
          ((ItemCard *)&option)->calculateUseScore();
        }
      }

      std::sort(cardOptions.begin(), cardOptions.end());
      for (const auto &option : cardOptions) {
        if (playerMana < option.cost())
          continue;
        playerMana -= option.cost();
        int boardNumber = -1;

        switch (option.cardType()) {
        case CREATURE:
          ((CreatureCard *)&option)->summon(actions, option.lane());
          if (option.ability(CHARGE))
            board[option.lane()].push_back(*(CreatureCard *)&option);
          break;

        // TODO better strategy
        case GREENITEM:
          if (board[0].size() > 0) {
            ((ItemCard *)&option)->use(actions, board[0][0]);
          } else if (board[1].size() > 0) {
            ((ItemCard *)&option)->use(actions, board[1][0]);
          }
          break;

        case REDITEM:
          for (auto &enemy : board[2]) {
            enemy.calculateRedItemScore(option);
          }
          std::sort(board[2].begin(), board[2].end());
          std::reverse(board[2].begin(), board[2].end());

          for (auto &enemy : board[3]) {
            enemy.calculateRedItemScore(option);
          }
          std::sort(board[3].begin(), board[3].end());
          std::reverse(board[3].begin(), board[3].end());

          if (board[2].size() > 0 && board[3].size() > 0) {
            boardNumber = board[2].back() < board[3].back() ? 2 : 3;
          } else if (board[2].size() > 0) {
            boardNumber = 2;
          } else if (board[3].size() > 0) {
            boardNumber = 3;
          }

          if (boardNumber != -1) {
            ((ItemCard *)&option)->use(actions, board[boardNumber].back());
            if (board[boardNumber].back().defense() <= 0)
              board[boardNumber].pop_back();
          }
          break;

        case BLUEITEM:
          ((ItemCard *)&option)->use(actions, opponent);
          break;
        }
      }

      board[OPPONENTLEFT].push_back(opponent);
      board[OPPONENTRIGHT].push_back(opponent);

      // Attack
      for (int i = 0; i < 2; i++) {
        for (auto &creature : board[i]) {
          creature.calculateAttackOrderScore();
        }
        std::sort(board[i].begin(), board[i].end());

        for (auto &creature : board[i]) {
          auto &targets = board[2 + i];
          if (targets.size() <= 0)
            break;
          for (auto &enemy : targets) {
            enemy.calculateAttackScore(creature);
          }
          std::sort(targets.begin(), targets.end());
          std::reverse(targets.begin(), targets.end());
          creature.attackTo(actions, targets.back());
          if (targets.back().defense() <= 0)
            targets.pop_back();
        }
      }
    }
    // do actions
    if (actions.size())
      std::cout << actions << std::endl;
    else
      std::cout << "PASS" << std::endl;
  }
  return 0;
}
