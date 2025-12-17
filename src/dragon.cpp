#include "../include/dragon.hpp"
#include "../include/knight.hpp"
#include "../include/pegasus.hpp"

Dragon::Dragon(int x, int y, const std::string& name) 
    : NPC(DragonType, x, y, name) {}

Dragon::Dragon(std::istream& is) 
    : NPC(DragonType, is) {}

void Dragon::print()
{
    std::cout << *this;
}

void Dragon::save(std::ostream& os)
{
    os << DragonType << std::endl;
    NPC::save(os);
}

bool Dragon::accept(std::shared_ptr<NPC> attacker)
{
    std::shared_ptr<Visitor> visitor = std::dynamic_pointer_cast<Visitor>(attacker);
    if (visitor)
        return visitor->visit(*this);
    return false;
}

bool Dragon::visit(Knight& other)
{
    fight_notify(other.shared_from_this(), false);
    return false;
}

bool Dragon::visit(Dragon& other)
{
    fight_notify(other.shared_from_this(), false);
    return false;
}

bool Dragon::visit(Pegasus& other)
{
    fight_notify(other.shared_from_this(), true);
    return true;
}

std::ostream& operator<<(std::ostream& os, Dragon& dragon)
{
    os << "dragon: " << *static_cast<NPC*>(&dragon) << std::endl;
    return os;
}