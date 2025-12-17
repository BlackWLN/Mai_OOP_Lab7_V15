#include "../include/npc.hpp"
#include "../include/observer.hpp"
#include "../include/fight.hpp"
#include "../include/factory.hpp"

void NPC::notify(std::shared_ptr<NPC> victim) {
    for (auto& w : observers) {
        if (auto obs = w.lock()) {
            obs->onKill(*this, *victim);
        }
    }
}


class Khight : public NPC {
public:
    Khight(float x, float y, std::string name)
        : NPC(KhightType, x, y, std::move(name)) {}

    bool canKill(const NPC& other) const override {
        return other.getType() == DragonType;
    }

    void accept(FightVisitor& v) override {
        v.visit(std::shared_ptr<NPC>(this, [](NPC*){}));
    }
};

class Dragon : public NPC {
public:
    Dragon(float x, float y, std::string name)
        : NPC(DragonType, x, y, std::move(name)) {}

    bool canKill(const NPC& other) const override {
        return other.getType() == PegasType;
    }

    void accept(FightVisitor& v) override {
        v.visit(std::shared_ptr<NPC>(this, [](NPC*){}));
    }
};

class Pegas : public NPC {
public:
    Pegas(float x, float y, std::string name)
        : NPC(PegasType, x, y, std::move(name)) {}

    bool canKill(const NPC& other) const override {
        return false;
    }

    void accept(FightVisitor& v) override {
        v.visit(std::shared_ptr<NPC>(this, [](NPC*){}));
    }
};

std::shared_ptr<NPC> NPCFactory::createNPC(
    NPCType type, float x, float y, const std::string& name
) {
    switch (type) {
        case KhightType: return std::make_shared<Khight>(x, y, name);
        case DragonType: return std::make_shared<Dragon>(x, y, name);
        case PegasType: return std::make_shared<Pegas>(x, y, name);
        default: return nullptr;
    }
}

std::string typeToString(NPCType t) {
    switch (t) {
        case KhightType: return "Khight";
        case DragonType: return "Dragon";
        case PegasType: return "Pegas";
        default: return "Unknown";
    }
}

std::istream& operator>>(std::istream& is, NPCType& t) {
    int value = 0;
    if (is >> value) {
        if (value >= KhightType && value <= PegasType)
            t = static_cast<NPCType>(value);
        else
            t = Unknown;
    }
    return is;
}