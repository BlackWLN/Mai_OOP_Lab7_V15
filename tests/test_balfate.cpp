#include "../include/entities.hpp"
#include "../include/factory.hpp"
#include "../include/fight.hpp"
#include "../include/npc.hpp"
#include "../include/observer.hpp"
#include <gtest/gtest.h>
#include <memory>

TEST(NPCTest, BearCanKill) {
auto bear = NPCFactory::createNPC(KhightType, 0, 0, "Khight");
auto werewolf = NPCFactory::createNPC(DragonType, 0, 0, "Dragon");
auto bandit = NPCFactory::createNPC(PegasType, 0, 0, "Pegas");
auto anotherBear = NPCFactory::createNPC(KhightType, 0, 0, "Khight!");
// Медведь убивает оборотней
EXPECT_TRUE(bear->canKill(*werewolf));

// Медведь НЕ убивает разбойников
EXPECT_FALSE(bear->canKill(*bandit));

// Медведь НЕ убивает других медведей
EXPECT_FALSE(bear->canKill(*anotherBear));
}

TEST(NPCTest, WerewolfCanKill) {
auto werewolf = NPCFactory::createNPC(DragonType, 0, 0, "Dragon");
auto bandit = NPCFactory::createNPC(PegasType, 0, 0, "Pegas");
auto bear = NPCFactory::createNPC(KhightType, 0, 0, "Khight");
auto anotherWerewolf = NPCFactory::createNPC(DragonType, 0, 0, "Dragon!");

// Оборотень убивает разбойников
EXPECT_TRUE(werewolf->canKill(*bandit));

// Оборотень НЕ убивает медведей
EXPECT_FALSE(werewolf->canKill(*bear));

// Оборотень НЕ убивает других оборотней
EXPECT_FALSE(werewolf->canKill(*anotherWerewolf));
}

TEST(NPCTest, BanditCannotKill) {
auto bandit = NPCFactory::createNPC(PegasType, 0, 0, "Pegas");
auto bear = NPCFactory::createNPC(KhightType, 0, 0, "Khight");
auto werewolf = NPCFactory::createNPC(DragonType, 0, 0, "Dragon");
auto anotherBandit = NPCFactory::createNPC(PegasType, 0, 0, "Pegas!");

// Разбойник НИКОГО не убивает
EXPECT_FALSE(bandit->canKill(*bear));
EXPECT_FALSE(bandit->canKill(*werewolf));
EXPECT_FALSE(bandit->canKill(*anotherBandit));
}

// Тесты для боевой системы
TEST(FightTest, BearKillsWerewolfInRange) {
auto bear = NPCFactory::createNPC(KhightType, 0, 0, "Khight");
auto werewolf = NPCFactory::createNPC(DragonType, 10, 0, "Dragon");

// Создаем наблюдатель для отслеживания убийств
auto observer = std::make_shared<ConsoleObserver>();
bear->attach(observer);
werewolf->attach(observer);

FightVisitor visitor(bear, 15.0f); // Дистанция 10, диапазон 15
visitor.visit(werewolf);

EXPECT_FALSE(werewolf->isAlive()); // Оборотень должен быть убит
EXPECT_TRUE(bear->isAlive());      // Медведь должен остаться жив
}

TEST(FightTest, WerewolfKillsBanditInRange) {
auto werewolf = NPCFactory::createNPC(DragonType, 0, 0, "Dragon");
auto bandit = NPCFactory::createNPC(PegasType, 5, 5, "Pegas");

FightVisitor visitor(werewolf, 10.0f); // Дистанция ~7.07, диапазон 10
visitor.visit(bandit);

EXPECT_FALSE(bandit->isAlive());   // Разбойник должен быть убит
EXPECT_TRUE(werewolf->isAlive());  // Оборотень должен остаться жив
}

TEST(FightTest, BanditCannotKillBear) {
auto bandit = NPCFactory::createNPC(PegasType, 0, 0, "Pegas");
auto bear = NPCFactory::createNPC(KhightType, 1, 1, "Khight");

FightVisitor visitor(bandit, 10.0f); // Дистанция ~1.41, диапазон 10
visitor.visit(bear);

EXPECT_TRUE(bear->isAlive());    // Медведь должен остаться жив
EXPECT_TRUE(bandit->isAlive());  // Разбойник также жив
}

TEST(FightTest, NoKillOutOfRange) {
auto bear = NPCFactory::createNPC(KhightType, 0, 0, "Khight");
auto werewolf = NPCFactory::createNPC(DragonType, 100, 100, "Dragon");

FightVisitor visitor(bear, 10.0f); // Дистанция ~141, диапазон 10
visitor.visit(werewolf);

EXPECT_TRUE(werewolf->isAlive()); // Оборотень жив, так как вне диапазона
}

TEST(FightTest, DeadNPCsCannotFight) {
auto bear = NPCFactory::createNPC(KhightType, 0, 0, "Khight");
auto werewolf = NPCFactory::createNPC(DragonType, 10, 10, "Dragon");

// Убиваем медведя
bear->kill();

FightVisitor visitor(bear, 20.0f);
visitor.visit(werewolf);

EXPECT_TRUE(werewolf->isAlive()); // Оборотень жив, так как медведь мертв
}

TEST(FightTest, DeadVictimCannotBeKilledAgain) {
auto bear = NPCFactory::createNPC(KhightType, 0, 0, "Khight");
auto werewolf = NPCFactory::createNPC(DragonType, 5, 5, "Dragon");

// Убиваем оборотня
werewolf->kill();

FightVisitor visitor(bear, 10.0f);
visitor.visit(werewolf);

// Проверяем, что оборотень остается мертвым
EXPECT_FALSE(werewolf->isAlive());
}

// Тесты для фабрики
TEST(FactoryTest, CreateAllTypes) {
auto bear = NPCFactory::createNPC(KhightType, 0, 0, "Khight");
EXPECT_NE(bear, nullptr);
EXPECT_EQ(bear->getType(), KhightType);

auto werewolf = NPCFactory::createNPC(DragonType, 1, 1, "Dragon");
EXPECT_NE(werewolf, nullptr);
EXPECT_EQ(werewolf->getType(), DragonType);

auto bandit = NPCFactory::createNPC(PegasType, 2, 2, "Pegas");
EXPECT_NE(bandit, nullptr);
EXPECT_EQ(bandit->getType(), PegasType);

// Неизвестный тип
auto unknown = NPCFactory::createNPC(Unknown, 3, 3, "Unknown");
EXPECT_EQ(unknown, nullptr);
}

TEST(FactoryTest, NPCProperties) {
auto npc = NPCFactory::createNPC(KhightType, 10.5f, 20.5f, "TestKhight");

EXPECT_EQ(npc->getX(), 10.5f);
EXPECT_EQ(npc->getY(), 20.5f);
EXPECT_EQ(npc->getName(), "TestKhight");
EXPECT_TRUE(npc->isAlive()); // NPC должен быть жив при создании

npc->kill();
EXPECT_FALSE(npc->isAlive()); // После убийства должен быть мертв
}

// Тесты для наблюдателей
TEST(ObserverTest, ConsoleObserver) {
auto bear = NPCFactory::createNPC(KhightType, 0, 0, "Khight");
auto werewolf = NPCFactory::createNPC(DragonType, 1, 1, "Dragon");

auto consoleObserver = std::make_shared<ConsoleObserver>();
bear->attach(consoleObserver);

// Перенаправляем вывод cout для проверки
std::stringstream buffer;
std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

bear->notify(werewolf);

std::cout.rdbuf(old); // Восстанавливаем cout
std::string output = buffer.str();

// Проверяем, что в выводе есть имена убийцы и жертвы
EXPECT_NE(output.find("Khight"), std::string::npos);
EXPECT_NE(output.find("Dragon"), std::string::npos);
}

// Тесты для цепочки убийств
TEST(ChainTest, CompleteKillChain) {
std::vector<std::shared_ptr<NPC>> npcs;

// Создаем NPC всех типов
npcs.push_back(NPCFactory::createNPC(KhightType, 0, 0, "Knight1"));
npcs.push_back(NPCFactory::createNPC(DragonType, 10, 0, "Dragon1"));
npcs.push_back(NPCFactory::createNPC(PegasType, 20, 0, "Pegas1"));

// Добавляем наблюдателей
auto consoleObserver = std::make_shared<ConsoleObserver>();
for (auto& npc : npcs) {
    npc->attach(consoleObserver);
}

// Медведь убивает оборотня
FightVisitor visitor1(npcs[0], 15.0f); // Khight -> Dragon
visitor1.visit(npcs[1]);
EXPECT_FALSE(npcs[1]->isAlive());
EXPECT_TRUE(npcs[0]->isAlive());

// Оборотень уже мертв, не может никого убить

// Разбойник не может убить медведя
FightVisitor visitor2(npcs[2], 25.0f); // Pegas -> Khight
visitor2.visit(npcs[0]);
EXPECT_TRUE(npcs[0]->isAlive()); // Медведь должен остаться жив

// Оборотень убил бы разбойника, если бы был жив
// Но он мертв, поэтому ничего не происходит
}

TEST(ChainTest, NoCyclicKills) {
auto bear = NPCFactory::createNPC(KhightType, 0, 0, "Khight");
auto werewolf = NPCFactory::createNPC(DragonType, 5, 0, "Dragon");
auto bandit = NPCFactory::createNPC(PegasType, 10, 0, "Pegas");

// Проверяем, что нет цикла убийств
EXPECT_TRUE(bear->canKill(*werewolf));
EXPECT_TRUE(werewolf->canKill(*bandit));
EXPECT_FALSE(bandit->canKill(*bear));

// Это линейная цепочка: Khight -> Dragon -> Pegas
// Pegas никого не убивает, замыкая цепочку
}

int main(int argc, char** argv) {
::testing::InitGoogleTest(&argc, argv);
return RUN_ALL_TESTS();
}