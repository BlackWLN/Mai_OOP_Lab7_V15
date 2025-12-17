#include "../include/npc.hpp"

NPC::NPC(NpcType t, int _x, int _y, const std::string &_name)
    : type(t), x(_x), y(_y), name(_name), alive(true) {}

NPC::NPC(NpcType t, std::istream &is) : type(t), alive(true) {
  is >> x >> y;
  is.ignore();
  std::getline(is, name);
}

void NPC::subscribe(std::shared_ptr<IFightObserver> observer) {
  observers.push_back(observer);
}

void NPC::fight_notify(const std::shared_ptr<NPC> defender, bool win) {
  for (auto &o : observers)
    o->on_fight(shared_from_this(), defender, win);
}

bool NPC::is_close(const std::shared_ptr<NPC> &other, size_t distance) const {
  std::shared_lock lock1(mutex);
  std::shared_lock lock2(other->mutex);
  
  int dx = x - other->x;
  int dy = y - other->y;
  return (dx * dx + dy * dy) <= (int)(distance * distance);
}

void NPC::save(std::ostream &os) {
  std::shared_lock lock(mutex);
  os << x << std::endl;
  os << y << std::endl;
  os << name << std::endl;
}

std::ostream &operator << (std::ostream & os, NPC &npc) {
  std::shared_lock lock(npc.mutex);
  os << "{ x:" << npc.x << ", y:" << npc.y << ", name: " << npc.name << " }";
  return os;
}