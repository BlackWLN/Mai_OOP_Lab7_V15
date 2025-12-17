#pragma once
#include "npc.hpp"

class Knight : public NPC, public Visitor
{
public:
    Knight(int x, int y, const std::string& name);
    Knight(std::istream& is);

    void print() override;
    void save(std::ostream& os) override;
    bool accept(std::shared_ptr<NPC> attacker) override;

    bool visit(Knight& other) override;
    bool visit(Dragon& other) override;
    bool visit(Pegasus& other) override;
    
    int get_move_distance() const override { return 30; }
    int get_kill_distance() const override { return 10; }

    friend std::ostream& operator<<(std::ostream& os, Knight& knight);
};