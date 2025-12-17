#include "../include/dragon.hpp"
#include "../include/knight.hpp"
#include "../include/pegasus.hpp"

Pegasus::Pegasus(int x, int y, const std::string &name)
    : NPC(PegasusType, x, y, name) {}

Pegasus::Pegasus(std::istream &is) : NPC(PegasusType, is) {}

void Pegasus::print() { std::cout << *this; }

void Pegasus::save(std::ostream &os) {
  os << PegasusType << std::endl;
  NPC::save(os);
}

bool Pegasus::accept(std::shared_ptr<NPC> attacker) {
  std::shared_ptr<Visitor> visitor =
      std::dynamic_pointer_cast<Visitor>(attacker);
  if (visitor)
    return visitor->visit(*this);
  return false;
}

bool Pegasus::visit(Knight &other) {
  fight_notify(other.shared_from_this(), false);
  return false;
}

bool Pegasus::visit(Dragon &other) {
  fight_notify(other.shared_from_this(), false);
  return false;
}

bool Pegasus::visit(Pegasus &other) {
  fight_notify(other.shared_from_this(), false);
  return false;
}

std::ostream &operator<<(std::ostream &os, Pegasus &pegasus) {
  os << "pegasus: " << *static_cast<NPC *>(&pegasus) << std::endl;
  return os;
}