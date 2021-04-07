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

void CreatureCard::calculateUseScore(
    int enemyTotalHP,
    int ownTotalHP,
    int enemyTotalAttack,
    int ownTotalAttack)
{
    // score_ = -attack_ / cost_;
    // score_ = {{exprs[0]}};
    score_ =
        ((((enemyTotalHP /
            sqrt(exp(
                ((defense_ +
                  sqrt(exp(
                      ((enemyTotalHP /
                        sqrt(exp(
                            (defense_ +
                             sqrt(exp(
                                 ((cardDraw_ +
                                   sqrt(exp(ownTotalHP))) +
                                  p2((enemyTotalHP /
                                      sqrt(exp(
                                          (defense_ -
                                           sqrt(exp(
                                               (enemyTotalHP /
                                                sqrt(exp(
                                                    (defense_ +
                                                     sqrt(exp(
                                                         ((cardDraw_ +
                                                           sqrt(exp(
                                                               ownTotalHP))) /
                                                          enemyTotalHP) -
                                                         p2(exp(exp(
                                                             ownTotalHP)))))) -
                                                    exp(myHealthChange_)))) -
                                               p2(p2(exp(
                                                   cost_)))))) -
                                          exp(myHealthChange_)))) +
                                     sqrt(exp(
                                         (defense_ *
                                          sqrt(exp(ownTotalHP))) -
                                         exp(cost_))))) -
                                 p2(p2(
                                     exp(sqrt(exp(cost_)))))))) -
                            exp(myHealthChange_)))) +
                       sqrt(sqrt(abilities_[3]))) -
                      p2(exp(exp(log(sqrt(exp(cost_))))))))) +
                 sqrt(
                     exp(((abilities_[5] +
                           sqrt(exp(exp(ownTotalHP)))) +
                          sqrt(sqrt(log(sqrt(exp(cost_)))))) -
                         p2(exp(exp(log(sqrt(exp(cost_))))))))) -
                exp(myHealthChange_)))) +
           sqrt(
               exp((defense_ * sqrt(exp(ownTotalHP))) -
                   exp(cost_)))) -
          exp(cost_)) -
         myHealthChange_);
    // score_ = p2(p2(log(enemyTotalHP + exp(attack_ *
    // log(abilities_[3]))))) * abilities_[4];
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