#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

const int VERSION = 1;

#pragma region MAGIC_VARS
enum CardType { CREATURE, GREENITEM, REDITEM, BLUEITEM };

enum Location { IN_HAND, MY_SIDE, OPPONENT_SIDE = -1 };

enum Lane { LEFT, RIGHT, NOT_IN = -1 };

enum BoardNumber { MYLEFT, MYRIGHT, OPPONENTLEFT, OPPONENTRIGHT };

enum Ability {  // BCDGLWP
    BREAKTHROUGH,
    CHARGE,
    DRAIN,
    GUARD,
    LETHAL,
    WARD,
    PLAYER
};
#pragma endregion
#pragma region CARD_TYPES
class Card
{
public:
    Card(
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
        int index);
    int instanceID() const;
    int location() const;
    int cardType() const;
    int cost() const;
    int attack() const;
    int defense() const;
    int lane() const;
    int ability(int index) const;
    void atkDiff(int atk);
    void defDiff(int def);
    bool operator<(const Card &rhs) const;
    bool operator==(const Card &rhs) const;
    void calculateGetScore();
    void pick(std::string &action) const;
    void getDamaged(int amount);

private:
    int cardNumber_, instanceId_, location_, cardType_, lane_,
        index_;
    double myHealthChange_, opponentHealthChange_, cardDraw_,
        cost_, attack_, defense_;
    double abilities_[7];  // BCDGLWP
    double score_ = -1;
    friend class CreatureCard;
    friend class ItemCard;
};

class CreatureCard : public Card
{
public:
    CreatureCard(
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
        int index);
    void summon(std::string &action, int lane) const;
    bool canKill(const CreatureCard &c);
    void attackTo(std::string &action, CreatureCard &target);
    void calculateUseScore(
        int enemyTotalHP,
        int ownTotalHP,
        int enemyTotalAttack,
        int ownTotalAttack);
    void calculateAttackScore(const CreatureCard &attacker);
    void calculateRedItemScore(const Card &attacker);
    void calculateAttackOrderScore();

private:
};

class ItemCard : public Card
{
public:
    ItemCard(
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
        int index);
    void use(std::string &action, CreatureCard &target) const;
    void calculateUseScore();

private:
};

class GameManager
{
public:
    std::vector<Card> cardOptions;
    std::vector<CreatureCard> board[4]; // myLeft myRight opponentLeft opponentRight
    int enemyTotalHP[2] = {0}, enemyTotalAttack[2] = {0}, ownTotalHP[2] = {0}, ownTotalAttack[2] = {0};
    // game information
    int playerHealth, playerMana, playerDeck, playerRune, playerDraw;
    int opponentHealth, opponentMana, opponentDeck, opponentRune, opponentDraw;
    int opponentHand;
    int opponentActions;
    std::string actions;

    void setUp();
    void draw();
    void battle();
    void print();
};
#pragma endregion