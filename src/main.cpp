#include <iostream>
#include <vector>
#include <memory>
#include <fstream>

#include "../include/entities.hpp"
#include "../include/factory.hpp"
#include "../include/fight.hpp"
#include "../include/npc.hpp"
#include "../include/observer.hpp"

static void saveToFile(const std::vector<std::shared_ptr<NPC>>& npcs, const std::string& filename)
{
    std::ofstream file(filename);
    for (auto& npc : npcs) {
        file << npc->getType() << " "
             << npc->getX() << " "
             << npc->getY() << " "
             << npc->getName() << "\n";
    }
}

static void loadFromFile(std::vector<std::shared_ptr<NPC>>& npcs, const std::string& filename)
{
    std::ifstream file(filename);

    NPCType type;
    float x, y;
    std::string name;

    npcs.clear();

    while (file >> type >> x >> y >> name) {
        auto npc = NPCFactory::createNPC(type, x, y, name);
        if (npc) {
            npcs.push_back(npc);
        }
    }
}

int main()
{
    std::vector<std::shared_ptr<NPC>> npcs;

    std::shared_ptr<Observer> consoleObs = std::make_shared<ConsoleObserver>();
    std::shared_ptr<Observer> fileObs    = std::make_shared<TextObserver>();

    while (true) {
        std::cout << "\nDungeon Editor\n";
        std::cout << "1 - Add NPC\n";
        std::cout << "2 - List\n";
        std::cout << "3 - Save\n";
        std::cout << "4 - Load\n";
        std::cout << "5 - Fight\n";
        std::cout << "6 - Exit\n";
        std::cout << "> ";

        int cmd;
        std::cin >> cmd;

        if (cmd == 1) {
            int t;
            float x, y;
            std::string name;

            std::cout << "Type (0-Knight, 1-Dragon, 2-Pegas): ";
            std::cin >> t;
            NPCType type = static_cast<NPCType>(t);

            std::cout << "X Y: ";
            std::cin >> x >> y;

            std::cout << "Name: ";
            std::cin >> name;

            auto npc = NPCFactory::createNPC(type, x, y, name);

            if (npc) {
                npc->attach(consoleObs);
                npc->attach(fileObs);

                npcs.push_back(npc);
                std::cout << "NPC added.\n";
            }
            else {
                std::cout << "Unknown type.\n";
            }
        }

        else if (cmd == 2) {
            std::cout << "\n LIST \n";
            for (auto& n : npcs) {
                std::cout << typeToString(n->getType())
                          << " " << n->getName()
                          << " (" << n->getX()
                          << ", " << n->getY() << ") ";

                if (!n->isAlive()) std::cout << "[dead]";
                std::cout << "\n";
            }
        }

        else if (cmd == 3) { 
            saveToFile(npcs, "map.txt");
            std::cout << "Saved to map.txt\n";
        }

        else if (cmd == 4) { 
            loadFromFile(npcs, "map.txt");

            for (auto& npc : npcs) {
                npc->attach(consoleObs);
                npc->attach(fileObs);
            }

            std::cout << "Loaded from map.txt\n";
        }

        else if (cmd == 5) { 

            if (npcs.size() < 2) {
                std::cout << "Need at least 2 NPCs\n";
                continue;
            }

            float range;
            std::cout << "Fight range: ";
            std::cin >> range;

            for (size_t i = 0; i < npcs.size(); ++i) {
                for (size_t j = 0; j < npcs.size(); ++j) {
                    if (i == j) continue;
                    if (!npcs[i]->isAlive() || !npcs[j]->isAlive()) continue;

                    FightVisitor visitor(npcs[i], range);
                    npcs[j]->accept(visitor);
                }
            }

            std::cout << "Fight finished.\n";
        }

        else if (cmd == 6) {
            break;
        }

        else {
            std::cout << "Unknown command.\n";
        }
    }

    return 0;
}
