#include "../include/dragon.hpp"
#include "../include/knight.hpp"
#include "../include/pegasus.hpp"
#include "../include/npc.hpp"

#include <thread>
#include <mutex>
#include <queue>
#include <chrono>
#include <atomic>

std::mutex print_mutex;
std::mutex queue_mutex;

struct FightEvent {
  std::shared_ptr<NPC> attacker;
  std::shared_ptr<NPC> defender;
};

std::queue<FightEvent> fight_queue;
std::atomic<bool> game_running{true};

class ConsoleObserver : public IFightObserver {
private:
  ConsoleObserver() {}

public:
  static std::shared_ptr<IFightObserver> get() {
    static ConsoleObserver instance;
    return std::shared_ptr<IFightObserver>(&instance, [](IFightObserver *) {});
  }

  void on_fight(const std::shared_ptr<NPC> attacker,
                const std::shared_ptr<NPC> defender, bool win) override {
    if (win) {
      std::lock_guard<std::mutex> lock(print_mutex);
      std::cout << "Murder: " << attacker->get_name() << " killed " 
                << defender->get_name() << std::endl;
    }
  }
};

class FileObserver : public IFightObserver {
private:
  std::ofstream log_file;
  std::mutex file_mutex;

  FileObserver() : log_file("log.txt", std::ios::app) {}

public:
  static std::shared_ptr<IFightObserver> get() {
    static FileObserver instance;
    return std::shared_ptr<IFightObserver>(&instance, [](IFightObserver *) {});
  }

  void on_fight(const std::shared_ptr<NPC> attacker,
                const std::shared_ptr<NPC> defender, bool win) override {
    if (win) {
      std::lock_guard<std::mutex> lock(file_mutex);
      if (log_file.is_open()) {
        log_file << "Murder: " << attacker->get_name() << " killed " 
                 << defender->get_name() << std::endl;
      }
    }
  }

  ~FileObserver() {
    if (log_file.is_open())
      log_file.close();
  }
};

std::shared_ptr<NPC> factory(NpcType type, int x, int y, const std::string &name) {
  std::shared_ptr<NPC> result;
  switch (type) {
  case KnightType:
    result = std::make_shared<Knight>(x, y, name);
    break;
  case DragonType:
    result = std::make_shared<Dragon>(x, y, name);
    break;
  case PegasusType:
    result = std::make_shared<Pegasus>(x, y, name);
    break;
  default:
    return nullptr;
  }

  if (result) {
    result->subscribe(ConsoleObserver::get());
    result->subscribe(FileObserver::get());
  }

  return result;
}

std::string generate_name(NpcType type, int index) {
  switch (type) {
  case KnightType:
    return "Knight_" + std::to_string(index);
  case DragonType:
    return "Dragon_" + std::to_string(index);
  case PegasusType:
    return "Pegasus_" + std::to_string(index);
  default:
    return "Unknown_" + std::to_string(index);
  }
}

// Поток движения
void movement_thread(std::vector<std::shared_ptr<NPC>>& npcs, int max_x, int max_y) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dir_dist(-1, 1);

  while (game_running) {
    for (size_t i = 0; i < npcs.size(); ++i) {
      auto& npc1 = npcs[i];
      if (!npc1->is_alive()) continue;

      // Движение NPC
      int move_dist = npc1->get_move_distance();
      int dx = dir_dist(gen) * move_dist;
      int dy = dir_dist(gen) * move_dist;
      npc1->move(dx, dy, max_x, max_y);

      // Проверка на возможность боя
      for (size_t j = 0; j < npcs.size(); ++j) {
        if (i == j) continue;
        
        auto& npc2 = npcs[j];
        if (!npc2->is_alive()) continue;

        int kill_dist = npc1->get_kill_distance();
        if (npc1->is_close(npc2, kill_dist)) {
          std::lock_guard<std::mutex> lock(queue_mutex);
          fight_queue.push({npc1, npc2});
        }
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

// Поток боёв
void fight_thread(std::vector<std::shared_ptr<NPC>>& npcs) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dice(1, 6);

  while (game_running) {
    FightEvent event;
    bool has_event = false;

    {
      std::lock_guard<std::mutex> lock(queue_mutex);
      if (!fight_queue.empty()) {
        event = fight_queue.front();
        fight_queue.pop();
        has_event = true;
      }
    }

    if (has_event) {
      if (!event.attacker->is_alive() || !event.defender->is_alive()) {
        continue;
      }

      int attack_roll = dice(gen);
      int defense_roll = dice(gen);

      if (attack_roll > defense_roll) {
        bool can_kill = event.defender->accept(event.attacker);
        if (can_kill) {
          event.defender->set_alive(false);
        }
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

// Печать карты
void print_map(const std::vector<std::shared_ptr<NPC>>& npcs, int max_x, int max_y) {
  std::lock_guard<std::mutex> lock(print_mutex);
  
  std::cout << "\n====== MAP ======" << std::endl;
  for (const auto& npc : npcs) {
    if (npc->is_alive()) {
      std::cout << npc->get_name() << " at (" << npc->get_x() << ", " 
                << npc->get_y() << ")" << std::endl;
    }
  }
  
  int alive_count = 0;
  for (const auto& npc : npcs) {
    if (npc->is_alive()) alive_count++;
  }
  std::cout << "Alive: " << alive_count << "/" << npcs.size() << std::endl;
  std::cout << "======================\n" << std::endl;
}

int main() {
  const int MAX_X = 100;
  const int MAX_Y = 100;
  const int NPC_COUNT = 50;
  const int GAME_DURATION = 30; // секунд

  std::vector<std::shared_ptr<NPC>> npcs;

  std::cout << "Generating " << NPC_COUNT << " NPCs..." << std::endl;
  std::srand(std::time(nullptr));
  
  for (int i = 0; i < NPC_COUNT; ++i) {
    NpcType type = NpcType(std::rand() % 3 + 1);
    std::string name = generate_name(type, i);
    int x = std::rand() % (MAX_X + 1);
    int y = std::rand() % (MAX_Y + 1);
    npcs.push_back(factory(type, x, y, name));
  }

  std::cout << "Starting game for " << GAME_DURATION << " seconds..." << std::endl;

  // Запуск потоков
  std::thread move_thread(movement_thread, std::ref(npcs), MAX_X, MAX_Y);
  std::thread combat_thread(fight_thread, std::ref(npcs));

  // Главный поток - печать карты каждую секунду
  auto start_time = std::chrono::steady_clock::now();
  while (true) {
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        current_time - start_time).count();

    if (elapsed >= GAME_DURATION) {
      break;
    }

    print_map(npcs, MAX_X, MAX_Y);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  game_running = false;

  move_thread.join();
  combat_thread.join();

  // Финальный отчёт
  std::cout << "\n===== GAME OVER =====" << std::endl;
  std::cout << "Survivors:" << std::endl;
  for (const auto& npc : npcs) {
    if (npc->is_alive()) {
      std::cout << "  " << npc->get_name() << " at (" << npc->get_x() 
                << ", " << npc->get_y() << ")" << std::endl;
    }
  }

  int alive_count = 0;
  for (const auto& npc : npcs) {
    if (npc->is_alive()) alive_count++;
  }
  
  std::cout << "\nTotal survived: " << alive_count << "/" << NPC_COUNT << std::endl;

  return 0;
}