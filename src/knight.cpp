#include "../include/dragon.hpp"
#include "../include/knight.hpp"
#include "../include/pegasus.hpp"

Knight::Knight(int x, int y, const std::string &name)
    : NPC(KnightType, x, y, name) {}

Knight::Knight(std::istream &is) : NPC(KnightType, is) {}

void Knight::print() { std::cout << *this; }

void Knight::save(std::ostream &os) {
  os << KnightType << std::endl;
  NPC::save(os);
}

bool Knight::accept(std::shared_ptr<NPC> attacker) {
  std::shared_ptr<Visitor> visitor =
      std::dynamic_pointer_cast<Visitor>(attacker);
  if (visitor)
    return visitor->visit(*this);
  return false;
}

bool Knight::visit(Knight &other) {
  fight_notify(other.shared_from_this(), false);
  return false;
}

bool Knight::visit(Dragon &other) {
  fight_notify(other.shared_from_this(), true);
  return true;
}

bool Knight::visit(Pegasus &other) {
  fight_notify(other.shared_from_this(), false);
  return false;
}

std::ostream &operator<<(std::ostream &os, Knight &knight) {
  os << "knight: " << *static_cast<NPC *>(&knight) << std::endl;
  return os;
}