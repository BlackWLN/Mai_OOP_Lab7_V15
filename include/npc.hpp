#pragma once
#include <string>
#include <vector>
#include <memory>

#include "entities.hpp"

class Observer;   

class NPC {
protected:
    NPCType type;
    float x, y;
    std::string name;
    bool alive = true;
    std::vector<std::weak_ptr<Observer>> observers;

public:
    NPC(NPCType t, float x_, float y_, std::string n)
        : type(t), x(x_), y(y_), name(std::move(n)) {}
    virtual ~NPC() = default;
    NPCType getType() const { return type; }
    std::string getName() const { return name; }
    float getX() const { return x; }
    float getY() const { return y; }
    bool isAlive() const { return alive; }
    void kill() { alive = false; }
    void attach(std::shared_ptr<Observer> obs) {
        observers.emplace_back(obs);
    }
    void notify(std::shared_ptr<NPC> victim);
    virtual bool canKill(const NPC& other) const = 0;
    virtual void accept(class FightVisitor& v) = 0;
};
