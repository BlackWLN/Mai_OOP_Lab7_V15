#pragma once
#include <memory>
#include <cmath>
#include "npc.hpp"

class FightVisitor {
    std::shared_ptr<NPC> attacker;
    float range;

public:
    FightVisitor(std::shared_ptr<NPC> a, float r)
        : attacker(std::move(a)), range(r) {}

    void visit(std::shared_ptr<NPC> victim) {
        if (!attacker->isAlive() || !victim->isAlive()) return;

        float dx = attacker->getX() - victim->getX();
        float dy = attacker->getY() - victim->getY();
        float dist = std::sqrt(dx*dx + dy*dy);

        if (dist > range) return;

        if (attacker->canKill(*victim)) {
            victim->kill();
            attacker->notify(victim);
        }
    }
};
