#pragma once
#include <iostream>
#include <fstream>
#include "npc.hpp"

class Observer {
public:
    virtual ~Observer() = default;
    virtual void onKill(const NPC& killer, const NPC& victim) = 0;
};

class ConsoleObserver : public Observer {
public:
    void onKill(const NPC& killer, const NPC& victim) override {
        std::cout << killer.getName() << " killed :| " << victim.getName() << "\n";
    }
};

class TextObserver : public Observer {
    std::ofstream file;
public:
    TextObserver() {
        file.open("log.txt", std::ios::app);
    }
    void onKill(const NPC& killer, const NPC& victim) override {
        file << killer.getName() << " killed :| " << victim.getName() << "\n";
    }
};
