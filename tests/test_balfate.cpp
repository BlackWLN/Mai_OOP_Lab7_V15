#include "../include/dragon.hpp"
#include "../include/knight.hpp"
#include "../include/pegasus.hpp"
#include "../include/npc.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <sstream>

TEST(NPCTest, KnightCreation) {
  Knight k(100, 200, "TestKnight");
  EXPECT_EQ(k.get_x(), 100);
  EXPECT_EQ(k.get_y(), 200);
  EXPECT_EQ(k.get_name(), "TestKnight");
}

TEST(NPCTest, DragonCreation) {
  Dragon d(50, 75, "TestDragon");
  EXPECT_EQ(d.get_x(), 50);
  EXPECT_EQ(d.get_y(), 75);
  EXPECT_EQ(d.get_name(), "TestDragon");
}

TEST(NPCTest, PegasusCreation) {
  Pegasus p(300, 400, "TestPegasus");
  EXPECT_EQ(p.get_x(), 300);
  EXPECT_EQ(p.get_y(), 400);
  EXPECT_EQ(p.get_name(), "TestPegasus");
}

TEST(NPCTest, IsCloseTrue) {
  auto k1 = std::make_shared<Knight>(0, 0, "K1");
  auto k2 = std::make_shared<Knight>(3, 4, "K2");
  EXPECT_TRUE(k1->is_close(k2, 5));
}

TEST(NPCTest, IsCloseFalse) {
  auto k1 = std::make_shared<Knight>(0, 0, "K1");
  auto k2 = std::make_shared<Knight>(10, 10, "K2");
  EXPECT_FALSE(k1->is_close(k2, 5));
}

TEST(NPCTest, IsCloseExactDistance) {
  auto k1 = std::make_shared<Knight>(0, 0, "K1");
  auto k2 = std::make_shared<Knight>(3, 4, "K2");
  EXPECT_TRUE(k1->is_close(k2, 5));
  EXPECT_FALSE(k1->is_close(k2, 4));
}

TEST(FightTest, KnightKillsDragon) {
  auto knight = std::make_shared<Knight>(0, 0, "K");
  auto dragon = std::make_shared<Dragon>(0, 0, "D");

  bool result = dragon->accept(knight);
  EXPECT_TRUE(result);
}

TEST(FightTest, DragonKillsPegasus) {
  auto dragon = std::make_shared<Dragon>(0, 0, "D");
  auto pegasus = std::make_shared<Pegasus>(0, 0, "P");

  bool result = pegasus->accept(dragon);
  EXPECT_TRUE(result);
}

TEST(FightTest, PegasusDoesNotKillKnight) {
  auto pegasus = std::make_shared<Pegasus>(0, 0, "P");
  auto knight = std::make_shared<Knight>(0, 0, "K");

  bool result = knight->accept(pegasus);
  EXPECT_FALSE(result);
}

TEST(FightTest, KnightDoesNotKillKnight) {
  auto k1 = std::make_shared<Knight>(0, 0, "K1");
  auto k2 = std::make_shared<Knight>(0, 0, "K2");

  bool result = k2->accept(k1);
  EXPECT_FALSE(result);
}

TEST(FightTest, DragonDoesNotKillDragon) {
  auto s1 = std::make_shared<Dragon>(0, 0, "D1");
  auto s2 = std::make_shared<Dragon>(0, 0, "D2");

  bool result = s2->accept(s1);
  EXPECT_FALSE(result);
}

TEST(FightTest, PegasusDoesNotKillPegasus) {
  auto p1 = std::make_shared<Pegasus>(0, 0, "P1");
  auto p2 = std::make_shared<Pegasus>(0, 0, "P2");

  bool result = p2->accept(p1);
  EXPECT_FALSE(result);
}

TEST(SaveLoadTest, KnightSaveLoad) {
  std::stringstream ss;
  Knight k1(100, 200, "TestKnight");
  k1.save(ss);

  int type;
  ss >> type;
  EXPECT_EQ(type, KnightType);

  Knight k2(ss);
  EXPECT_EQ(k2.get_x(), 100);
  EXPECT_EQ(k2.get_y(), 200);
  EXPECT_EQ(k2.get_name(), "TestKnight");
}

TEST(SaveLoadTest, DragonSaveLoad) {
  std::stringstream ss;
  Dragon d1(50, 75, "TestDragon");
  d1.save(ss);

  int type;
  ss >> type;
  EXPECT_EQ(type, DragonType);

  Dragon d2(ss);
  EXPECT_EQ(d2.get_x(), 50);
  EXPECT_EQ(d2.get_y(), 75);
  EXPECT_EQ(d2.get_name(), "TestDragon");
}

TEST(SaveLoadTest, PegasusSaveLoad) {
  std::stringstream ss;
  Pegasus p1(300, 400, "TestPegasus");
  p1.save(ss);

  int type;
  ss >> type;
  EXPECT_EQ(type, PegasusType);

  Pegasus p2(ss);
  EXPECT_EQ(p2.get_x(), 300);
  EXPECT_EQ(p2.get_y(), 400);
  EXPECT_EQ(p2.get_name(), "TestPegasus");
}

class MockObserver : public IFightObserver {
public:
  int fight_count = 0;
  int win_count = 0;

  void on_fight(const std::shared_ptr<NPC> attacker,
                const std::shared_ptr<NPC> defender, bool win) override {
    fight_count++;
    if (win)
      win_count++;
  }
};

TEST(ObserverTest, ObserverNotification) {
  auto observer = std::make_shared<MockObserver>();
  auto knight = std::make_shared<Knight>(0, 0, "K");
  auto dragon = std::make_shared<Dragon>(0, 0, "D");

  knight->subscribe(observer);
  dragon->accept(knight);

  EXPECT_EQ(observer->fight_count, 1);
  EXPECT_EQ(observer->win_count, 1);
}

TEST(ObserverTest, MultipleObservers) {
  auto obs1 = std::make_shared<MockObserver>();
  auto obs2 = std::make_shared<MockObserver>();

  auto knight = std::make_shared<Knight>(0, 0, "K");
  auto dragon = std::make_shared<Dragon>(0, 0, "D");

  knight->subscribe(obs1);
  knight->subscribe(obs2);

  dragon->accept(knight);

  EXPECT_EQ(obs1->fight_count, 1);
  EXPECT_EQ(obs2->fight_count, 1);
}

TEST(BattleTest, ChainReaction) {
  auto knight = std::make_shared<Knight>(0, 0, "K");
  auto dragon = std::make_shared<Dragon>(5, 0, "D");
  auto pegasus = std::make_shared<Pegasus>(10, 0, "P");

  EXPECT_TRUE(dragon->accept(knight));
  EXPECT_TRUE(pegasus->accept(dragon));
  EXPECT_FALSE(knight->accept(pegasus));
}

TEST(DistanceTest, ExactlyAtBorder) {
  auto k1 = std::make_shared<Knight>(0, 0, "K1");
  auto k2 = std::make_shared<Knight>(5, 0, "K2");

  EXPECT_TRUE(k1->is_close(k2, 5));
  EXPECT_FALSE(k1->is_close(k2, 4));
}

TEST(DistanceTest, DiagonalDistance) {
  auto k1 = std::make_shared<Knight>(0, 0, "K1");
  auto k2 = std::make_shared<Knight>(30, 40, "K2");

  EXPECT_TRUE(k1->is_close(k2, 50));
  EXPECT_FALSE(k1->is_close(k2, 49));
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}