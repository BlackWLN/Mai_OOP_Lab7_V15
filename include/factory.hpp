#pragma once
#include <memory>
#include <string>

#include "npc.hpp"
#include "entities.hpp"

class NPCFactory {
public:
    static std::shared_ptr<NPC> createNPC(
        NPCType type,
        float x, float y,
        const std::string& name
    );
};
