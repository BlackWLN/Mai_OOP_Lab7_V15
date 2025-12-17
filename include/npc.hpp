#pragma once

#include <cstring>
#include <fstream>
#include <iostream>
#include <math.h>
#include <memory>
#include <random>
#include <set>
#include <string>
#include <vector>
#include <mutex>
#include <shared_mutex>

class NPC;
class Knight;
class Dragon;
class Pegasus;

using set_t = std::set<std::shared_ptr<NPC>>;

enum NpcType { Unknown = 0, KnightType = 1, DragonType = 2, PegasusType = 3 };

class IFightObserver {
public:
  virtual void on_fight(const std::shared_ptr<NPC> attacker,
                        const std::shared_ptr<NPC> defender, bool win) = 0;
  virtual ~IFightObserver() = default;
};

class Visitor {
public:
  virtual bool visit(Knight &knight) = 0;
  virtual bool visit(Dragon &dragon) = 0;
  virtual bool visit(Pegasus &pegasus) = 0;
  virtual ~Visitor() = default;
};

class NPC : public std::enable_shared_from_this<NPC> {
protected:
  NpcType type;
  int x;
  int y;
  std::string name;
  std::vector<std::shared_ptr<IFightObserver>> observers;
  bool alive;
  mutable std::shared_mutex mutex;

  NPC(NpcType t, int _x, int _y, const std::string &_name);
  NPC(NpcType t, std::istream &is);

public:
  virtual ~NPC() = default;

  void subscribe(std::shared_ptr<IFightObserver> observer);
  void fight_notify(const std::shared_ptr<NPC> defender, bool win);
  bool is_close(const std::shared_ptr<NPC> &other, size_t distance) const;

  virtual bool accept(std::shared_ptr<NPC> attacker) = 0;
  virtual void print() = 0;
  virtual void save(std::ostream &os);
  
  virtual int get_move_distance() const = 0;
  virtual int get_kill_distance() const = 0;

  int get_x() const { 
    std::shared_lock lock(mutex);
    return x; 
  }
  
  int get_y() const { 
    std::shared_lock lock(mutex);
    return y; 
  }
  
  std::string get_name() const { 
    std::shared_lock lock(mutex);
    return name; 
  }
  
  bool is_alive() const {
    std::shared_lock lock(mutex);
    return alive;
  }
  
  void set_alive(bool status) {
    std::lock_guard lock(mutex);
    alive = status;
  }
  
  void move(int dx, int dy, int max_x, int max_y) {
    std::lock_guard lock(mutex);
    if (!alive) return;
    
    x = std::max(0, std::min(max_x, x + dx));
    y = std::max(0, std::min(max_y, y + dy));
  }

  friend std::ostream &operator<<(std::ostream &os, NPC &npc);
};