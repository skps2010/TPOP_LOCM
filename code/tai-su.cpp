#include "tai-su.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#pragma region STATIC_METHOD
static double p2(double a)
{
    return a * a;
}
#pragma endregion
#pragma region Card_IMPLEMENTATION
Card::Card(
    int cardNumber,
    int instanceId,
    int location,
    int cardType,
    int cost,
    int atk,
    int def,
    std::string abilities,
    int myhealthChange,
    int opponentHealthChange,
    int cardDraw,
    int lane,
    int index)
    : cardNumber_(cardNumber),
      instanceId_(instanceId),
      location_(location),
      cardType_(cardType),
      cost_(cost),
      attack_(atk),
      defense_(def),
      myHealthChange_(myhealthChange),
      opponentHealthChange_(opponentHealthChange),
      cardDraw_(cardDraw),
      lane_(lane),
      index_(index)
{
    for (int i = 0; i < 7;
         ++i) {  // the last ability "P" indicates that if it
                 // is the player himself
        if (i < abilities.length() && abilities[i] != '-')
            abilities_[i] = true;
        else
            abilities_[i] = false;
    }
}

int Card::instanceID() const
{
    return instanceId_;
}

int Card::location() const
{
    return location_;
}

int Card::cardType() const
{
    return cardType_;
}

int Card::cost() const
{
    return cost_;
}

int Card::attack() const
{
    return attack_;
}

int Card::defense() const
{
    return defense_;
}

int Card::lane() const
{
    return lane_;
}

int Card::ability(int index) const
{
    return abilities_[index];
}

void Card::atkDiff(int atk)
{
    attack_ += atk;
}

void Card::defDiff(int def)
{
    defense_ += def;
}

bool Card::operator<(const Card &rhs) const
{
    return score_ < rhs.score_;
}

bool Card::operator==(const Card &rhs) const
{
    return score_ == rhs.score_;
}

void Card::pick(std::string &action) const
{
    action += "PICK " + std::to_string(index_);
}

void Card::getDamaged(int amount)
{
    if (amount > 0 && abilities_[WARD])
        abilities_[WARD] = false;
    else
        defDiff(-amount);
}
#pragma endregion
#pragma region CreatureCard_IMPLEMENTATION
CreatureCard::CreatureCard(
    int cardNumber,
    int instanceId,
    int location,
    int cardType,
    int cost,
    int atk,
    int def,
    std::string abilities,
    int myhealthChange,
    int opponentHealthChange,
    int cardDraw,
    int lane,
    int index)
    : Card(
          cardNumber,
          instanceId,
          location,
          cardType,
          cost,
          atk,
          def,
          abilities,
          myhealthChange,
          opponentHealthChange,
          cardDraw,
          lane,
          index)
{
}

void CreatureCard::summon(std::string &action, int lane) const
{
    action += "SUMMON " + std::to_string(instanceID()) + " " +
              std::to_string(lane) + ";";
}

bool CreatureCard::canKill(const CreatureCard &c)
{
    return c.defense() <= this->attack();
}

void CreatureCard::attackTo(
    std::string &action,
    CreatureCard &target)
{
    target.getDamaged(
        abilities_[LETHAL] && !target.ability(PLAYER) ? 999
                                                      : attack_);
    getDamaged(target.attack());
    action += "ATTACK " + std::to_string(this->instanceID()) +
              " " + std::to_string(target.instanceID()) + ";";
}


#pragma endregion
#pragma region ItemCard_IMPLEMENTATION
ItemCard::ItemCard(
    int cardNumber,
    int instanceId,
    int location,
    int cardType,
    int cost,
    int atk,
    int def,
    std::string abilities,
    int myhealthChange,
    int opponentHealthChange,
    int cardDraw,
    int lane,
    int index)
    : Card(
          cardNumber,
          instanceId,
          location,
          cardType,
          cost,
          atk,
          def,
          abilities,
          myhealthChange,
          opponentHealthChange,
          cardDraw,
          lane,
          index){};
void ItemCard::use(std::string &action, CreatureCard &target)
    const
{
    target.atkDiff(attack_);
    target.getDamaged(-defense_);
    for (int i = 0; i < 6; i++) {
        switch (cardType_) {
        case GREENITEM:
            target.abilities_[i] =
                abilities_[i] || target.abilities_[i];
            break;
        case REDITEM:
            target.abilities_[i] =
                !abilities_[i] && target.abilities_[i];
            break;
        }
    }

    action += "USE " + std::to_string(instanceID()) + " " +
              std::to_string(target.instanceID()) + ";";
}
#pragma endregion

#pragma region GameManager_IMPLEMENTATION
void GameManager::setUp()
{
    std::cin >> playerHealth >> playerMana >> playerDeck >>
        playerRune >> playerDraw;
    std::cin >> opponentHealth >> opponentMana >> opponentDeck >>
        opponentRune >> opponentDraw;

    std::cin >> opponentHand >> opponentActions;
    std::cin.ignore();
    actions = "";

    for (int i = 0; i < opponentActions; i++) {
        std::string cardNumberAndAction;
        getline(std::cin, cardNumberAndAction);
    }
    int cardCount;
    std::cin >> cardCount;
    std::cin.ignore();

    for (int i = 0; i < cardCount; i++) {
        int cardNumber, instanceID, location, cardType, cost,
            attack, defense, myHealthChange, opponentHealthChange,
            cardDraw, lane = LEFT;
        std::string abilities;
        std::cin >> cardNumber >> instanceID >> location >>
            cardType >> cost >> attack >> defense >> abilities >>
            myHealthChange >> opponentHealthChange >> cardDraw;
        if (VERSION == 2)
            std::cin >> lane;
        std::cin.ignore();

        if (playerMana == 0) {  // Draw Phase
            Card card(
                cardNumber, instanceID, IN_HAND, cardType, cost,
                attack, defense, abilities, myHealthChange,
                opponentHealthChange, cardDraw, NOT_IN, i);
            cardOptions.push_back(card);
        } else {                        // Battle Phase
            if (location == IN_HAND) {  // in my hand
                if (cardType == CREATURE) {
                    CreatureCard summonLeft(
                        cardNumber, instanceID, IN_HAND, cardType,
                        cost, attack, defense, abilities,
                        myHealthChange, opponentHealthChange,
                        cardDraw, LEFT, i);
                    cardOptions.push_back(summonLeft);
                    if (VERSION == 2) {
                        CreatureCard summonRight(
                            cardNumber, instanceID, IN_HAND,
                            cardType, cost, attack, defense,
                            abilities, myHealthChange,
                            opponentHealthChange, cardDraw, RIGHT,
                            i);
                        cardOptions.push_back(summonRight);
                    }
                } else {
                    ItemCard item(
                        cardNumber, instanceID, IN_HAND, cardType,
                        cost, attack, defense, abilities,
                        myHealthChange, opponentHealthChange,
                        cardDraw, lane, i);
                    cardOptions.push_back(item);
                }
            } else if (location == OPPONENT_SIDE) {  // in
                                                     // opponent
                                                     // side of
                                                     // board
                CreatureCard tmp(
                    cardNumber, instanceID, OPPONENT_SIDE,
                    CREATURE, cost, attack, defense, abilities,
                    myHealthChange, opponentHealthChange,
                    cardDraw, lane, i);
                board[2 + lane].push_back(tmp);
                enemyTotalAttack[lane] += attack;
                enemyTotalHP[lane] += defense;
            } else if (location == MY_SIDE) {  // in my side of
                                               // board
                CreatureCard tmp(
                    cardNumber, instanceID, IN_HAND, CREATURE,
                    cost, attack, defense, abilities,
                    myHealthChange, opponentHealthChange,
                    cardDraw, lane, i);
                board[lane].push_back(tmp);
                ownTotalAttack[lane] += attack;
                ownTotalHP[lane] += defense;
            }
        }
    }
}
void GameManager::draw()
{
    for (auto &option : cardOptions) {
        option.calculateGetScore();
    }
    std::sort(cardOptions.begin(), cardOptions.end());
    cardOptions[0].pick(actions);
}
void GameManager::battle()
{
    CreatureCard opponent(
        -1, -1, OPPONENT_SIDE, CREATURE, 0, 0, opponentHealth,
        "------P", 0, 0, 0, -1, -1);

    // Summon
    for (auto &option : cardOptions) {
        if (option.cardType() == CREATURE) {
            ((CreatureCard *) &option)
                ->calculateUseScore(
                    enemyTotalHP[option.lane()],
                    ownTotalHP[option.lane()],
                    enemyTotalAttack[option.lane()],
                    ownTotalAttack[option.lane()]);
        } else {  // Item
            ((ItemCard *) &option)->calculateUseScore();
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
            ((CreatureCard *) &option)
                ->summon(actions, option.lane());
            if (option.ability(CHARGE))
                board[option.lane()].push_back(
                    *(CreatureCard *) &option);
            break;

        // TODO better strategy
        case GREENITEM:
            if (board[0].size() > 0) {
                ((ItemCard *) &option)->use(actions, board[0][0]);
            } else if (board[1].size() > 0) {
                ((ItemCard *) &option)->use(actions, board[1][0]);
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
                boardNumber =
                    board[2].back() < board[3].back() ? 2 : 3;
            } else if (board[2].size() > 0) {
                boardNumber = 2;
            } else if (board[3].size() > 0) {
                boardNumber = 3;
            }

            if (boardNumber != -1) {
                ((ItemCard *) &option)
                    ->use(actions, board[boardNumber].back());
                if (board[boardNumber].back().defense() <= 0)
                    board[boardNumber].pop_back();
            }
            break;

        case BLUEITEM:
            ((ItemCard *) &option)->use(actions, opponent);
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
void GameManager::print()
{
    // do actions
    if (actions.size())
        std::cout << actions << std::endl;
    else
        std::cout << "PASS" << std::endl;
}
#pragma endregion

#pragma region User_IMPLEMENTATION

/*************************************************************/
/*                         以下可編輯                          */
/*************************************************************/

void Card::calculateGetScore()
{
    switch (cardType_) {
    case CREATURE:
    case GREENITEM:
        score_ = -attack_ / cost_;
        break;
    case REDITEM:
        score_ = defense_ / cost_ / 2;
        break;
    case BLUEITEM:
        score_ =
            (defense_ - myHealthChange_ + opponentHealthChange_) /
            cost_ / 2;
        break;
    }
}

void CreatureCard::calculateUseScore(
    int enemyTotalHP,
    int ownTotalHP,
    int enemyTotalAttack,
    int ownTotalAttack)
{
    score_ = -attack_ / cost_;
}

void CreatureCard::calculateAttackScore(
    const CreatureCard &attacker)
{
    if (abilities_[PLAYER])  // if there are no cards with Guard,
                             // attack the player first.
        score_ = -500;
    else if (attacker.ability(
                 BREAKTHROUGH))  // if it has Breakthrough, kill
                                 // the one with the less health
        score_ = defense_;
    else if (attacker.ability(
                 LETHAL))  // if it has Lethal, kill the one with
                           // the most health
        score_ = -defense_;
    else {  // if it can kill, kill the one with the most health,
            // if not, attack the one with the least health
        score_ =
            attacker.attack() - (abilities_[WARD] ? 0 : defense_);
        if (score_ < 0)
            score_ = defense_;
    }
    if (abilities_[GUARD])  // the one with Guard needs to be
                            // damaged first
        score_ -= 1000;
}

void CreatureCard::calculateRedItemScore(const Card &attacker)
{
    // if it can kill, kill the one with the most health, if not,
    // attack the one with the least health
    score_ =
        -attacker.defense() - (abilities_[WARD] ? 0 : defense_);
    if (score_ < 0)
        score_ = defense_;
    if (abilities_[GUARD])  // the one with Guard needs to be
                            // damaged first
        score_ -= 1000;
}

void CreatureCard::calculateAttackOrderScore()
{
    // The more damage it has, the later it will attack
    score_ = attack_;
    // If it has Lethal, it should attack first
    if (abilities_[LETHAL])
        score_ -= 1000;
}

void ItemCard::calculateUseScore()
{
    switch (cardType_) {
    case GREENITEM:
        score_ = (double) -attack_ / cost_;
        break;
    case REDITEM:
        score_ = (double) defense_ / cost_ / 2;
        break;
    case BLUEITEM:
        score_ =
            (double) (defense_ - myHealthChange_ + opponentHealthChange_) /
            cost_ / 2;
        break;
    }
}

#pragma endregion