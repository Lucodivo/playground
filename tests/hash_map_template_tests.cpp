//
// Created by Connor on 3/12/2022.
//

#include "test.h"
#include "hash_map_template.cpp"

struct TestKey_hm {
  u64 uniqueIndex;
  char fourCharCode[4];
};

struct TestData_hm {
  u32 anUnsignedInt32;
  s8 aSignedInt8;
  f64 aDouble;
};

struct TestEntry_hm {
  TestKey_hm key;
  TestData_hm datum;
};

u64 hashString_func(const TestKey_hm& key) {
  return key.uniqueIndex;
}

bool equalsString_func(const TestKey_hm& key1, const TestKey_hm& key2) {
  return key1.uniqueIndex == key2.uniqueIndex &&
          key1.fourCharCode[0] == key2.fourCharCode[0] &&
          key1.fourCharCode[1] == key2.fourCharCode[1] &&
          key1.fourCharCode[2] == key2.fourCharCode[2] &&
          key1.fourCharCode[3] == key2.fourCharCode[3];
}

class HashMapTemplateTest : public testing::Test {
public:
  const class_access u64 firstLevelCapacity = 16;
  const class_access u64 firstWaveInsertCount = firstLevelCapacity;
  const class_access u64 collisionsCount = 10;
  const class_access u64 notInsertedCount = firstWaveInsertCount;

  TestEntry_hm firstWaveTestEntries[firstLevelCapacity];
  TestEntry_hm collisionEntries[collisionsCount];
  TestEntry_hm notInsertedTestEntries[notInsertedCount];
  HashMapTemplate<TestKey_hm, TestData_hm>* testDataHashSet;

  // Init HashSetVoid and fill it up with data
  void SetUp() override { // runs immediately before a test starts

    auto hashString_lambda = [](const TestKey_hm& key) -> u64 {
      return key.uniqueIndex;
    };
    auto equalsString_lambda = [](const TestKey_hm& key1, const TestKey_hm& key2) -> bool {
      return key1.uniqueIndex == key2.uniqueIndex &&
             key1.fourCharCode[0] == key2.fourCharCode[0] &&
             key1.fourCharCode[1] == key2.fourCharCode[1] &&
             key1.fourCharCode[2] == key2.fourCharCode[2] &&
             key1.fourCharCode[3] == key2.fourCharCode[3];
    };

    //testDataHashSet = new HashMapTemplate<TestKey_hm, TestData_hm>(hashString_func, equalsString_func, firstLevelCapacity); // or use functions
    testDataHashSet = new HashMapTemplate<TestKey_hm, TestData_hm>(hashString_lambda, equalsString_lambda, firstLevelCapacity);

    for(u32 i = 0; i < firstWaveInsertCount; i++) {
      firstWaveTestEntries[i].key.uniqueIndex = i;
      firstWaveTestEntries[i].key.fourCharCode[0] = 'a' + i;
      firstWaveTestEntries[i].key.fourCharCode[1] = 'a' + i;
      firstWaveTestEntries[i].key.fourCharCode[2] = 'a' + i;
      firstWaveTestEntries[i].key.fourCharCode[3] = 'a' + i;
      firstWaveTestEntries[i].datum.anUnsignedInt32 = i;
      firstWaveTestEntries[i].datum.aSignedInt8 = (s8)i;
      firstWaveTestEntries[i].datum.aDouble = (f64)i;
    }

    const u64 halfCollisionCount = collisionsCount / 2;
    // collisions
    for(u64 i = 0; i < halfCollisionCount; i++) {
      collisionEntries[i].key.uniqueIndex = (firstLevelCapacity + i);
      collisionEntries[i].key.fourCharCode[0] = firstWaveTestEntries[i].key.fourCharCode[0] + 1;
      collisionEntries[i].key.fourCharCode[1] = firstWaveTestEntries[i].key.fourCharCode[1] + 2;
      collisionEntries[i].key.fourCharCode[2] = firstWaveTestEntries[i].key.fourCharCode[2] + 3;
      collisionEntries[i].key.fourCharCode[3] = firstWaveTestEntries[i].key.fourCharCode[3] + 4;
      u64 magicNumber = i + firstWaveInsertCount;
      collisionEntries[i].datum.anUnsignedInt32 = magicNumber;
      collisionEntries[i].datum.aSignedInt8 = (s8)magicNumber;
      collisionEntries[i].datum.aDouble = (f64)magicNumber;
    }
    // double collisions to same indices as above
    for(u64 i = halfCollisionCount; i < collisionsCount; i++) {
      collisionEntries[i].key.uniqueIndex = (firstLevelCapacity + i - halfCollisionCount);
      collisionEntries[i].key.fourCharCode[0] = firstWaveTestEntries[i].key.fourCharCode[0] + 1;
      collisionEntries[i].key.fourCharCode[1] = firstWaveTestEntries[i].key.fourCharCode[1] + 2;
      collisionEntries[i].key.fourCharCode[2] = firstWaveTestEntries[i].key.fourCharCode[2] + 3;
      collisionEntries[i].key.fourCharCode[3] = firstWaveTestEntries[i].key.fourCharCode[3] + 4;
      u64 magicNumber = i + firstWaveInsertCount;
      collisionEntries[i].datum.anUnsignedInt32 = magicNumber;
      collisionEntries[i].datum.aSignedInt8 = (s8)magicNumber;
      collisionEntries[i].datum.aDouble = (f64)magicNumber;
    }

    for(u32 i = 0; i < notInsertedCount; i++) {
      notInsertedTestEntries[i].key.uniqueIndex = i;
      notInsertedTestEntries[i].key.fourCharCode[0] = 'a' - i - 1;
      notInsertedTestEntries[i].key.fourCharCode[1] = 'a' - i - 1;
      notInsertedTestEntries[i].key.fourCharCode[2] = 'a' - i - 1;
      notInsertedTestEntries[i].key.fourCharCode[3] = 'a' - i - 1;
      firstWaveTestEntries[i].datum.anUnsignedInt32 = 4'000'000'000 - i - 1;
      firstWaveTestEntries[i].datum.aSignedInt8 = (s8)(100 - i - 1);
      firstWaveTestEntries[i].datum.aDouble = 400.0 - (f64)i - 1.0;
    }

    // fill up hash set
    for(TestEntry_hm& testEntry : firstWaveTestEntries) {
      testDataHashSet->insert(testEntry.key, testEntry.datum);
    }

    // cause some collisions
    for(TestEntry_hm& testEntry : collisionEntries) {
      testDataHashSet->insert(testEntry.key, testEntry.datum);
    }
  }

  void TearDown() override { // runs immediately after a test finishes
    delete testDataHashSet;
  }
};

TEST_F(HashMapTemplateTest, insert_retrieve) {
  // assert first round of data was inserted
  for(TestEntry_hm& testEntry : firstWaveTestEntries) {
    ASSERT_TRUE(testDataHashSet->contains(testEntry.key));
    TestData_hm retrievedTestData;
    ASSERT_TRUE(testDataHashSet->retrieve(testEntry.key, retrievedTestData));
    ASSERT_EQ(testEntry.datum.aDouble, retrievedTestData.aDouble);
    ASSERT_EQ(testEntry.datum.aSignedInt8, retrievedTestData.aSignedInt8);
    ASSERT_EQ(testEntry.datum.anUnsignedInt32, retrievedTestData.anUnsignedInt32);
  }

  // assert collision round of data was inserted and can be retrieved
  for(TestEntry_hm& testEntry : collisionEntries) {
    ASSERT_TRUE(testDataHashSet->contains(testEntry.key));
    TestData_hm retrievedTestData;
    ASSERT_TRUE(testDataHashSet->retrieve(testEntry.key, retrievedTestData));
    ASSERT_EQ(testEntry.datum.aDouble, retrievedTestData.aDouble);
    ASSERT_EQ(testEntry.datum.aSignedInt8, retrievedTestData.aSignedInt8);
    ASSERT_EQ(testEntry.datum.anUnsignedInt32, retrievedTestData.anUnsignedInt32);
  }

  // assert non-inserted data is properly reported as not contained
  for(TestEntry_hm& testEntry : notInsertedTestEntries) {
    ASSERT_FALSE(testDataHashSet->contains(testEntry.key));
    TestData_hm retrievedTestData;
    ASSERT_FALSE(testDataHashSet->retrieve(testEntry.key, retrievedTestData));
  }

  ASSERT_EQ(collisionsCount, testDataHashSet->collisionCount);
  ASSERT_EQ(collisionsCount + firstLevelCapacity, testDataHashSet->elementCount);
}

TEST_F(HashMapTemplateTest, insert_remove) {
  u32 halfFirstWaveInsertCount = firstWaveInsertCount / 2;

  // remove half of the first wave
  for(u32 i = 0; i < halfFirstWaveInsertCount; ++i){
    TestEntry_hm& testEntry = firstWaveTestEntries[i];
    ASSERT_TRUE(testDataHashSet->remove(testEntry.key));
  }

  // assert that the first half is gone
  for(u32 i = 0; i < halfFirstWaveInsertCount; ++i){
    TestEntry_hm& testEntry = firstWaveTestEntries[i];
    ASSERT_FALSE(testDataHashSet->contains(testEntry.key));
    TestData_hm retrievedTestData;
    ASSERT_FALSE(testDataHashSet->retrieve(testEntry.key, retrievedTestData));
  }

  // assert that the second half remains unchanged
  for(u32 i = halfFirstWaveInsertCount; i < firstWaveInsertCount; ++i){
    TestEntry_hm& testEntry = firstWaveTestEntries[i];
    ASSERT_TRUE(testDataHashSet->contains(testEntry.key));
    TestData_hm retrievedTestData;
    ASSERT_TRUE(testDataHashSet->retrieve(testEntry.key, retrievedTestData));
    ASSERT_EQ(testEntry.datum.aDouble, retrievedTestData.aDouble);
    ASSERT_EQ(testEntry.datum.aSignedInt8, retrievedTestData.aSignedInt8);
    ASSERT_EQ(testEntry.datum.anUnsignedInt32, retrievedTestData.anUnsignedInt32);
  }

  u32 halfCollisionInsertCount = collisionsCount / 2;

  // remove half of the collisions
  for(u32 i = 0; i < halfCollisionInsertCount; ++i){
    TestEntry_hm& testEntry = collisionEntries[i];
    ASSERT_TRUE(testDataHashSet->remove(testEntry.key));
  }

  // assert that the first half is gone
  for(u32 i = 0; i < halfCollisionInsertCount; ++i){
    TestEntry_hm& testEntry = collisionEntries[i];
    ASSERT_FALSE(testDataHashSet->contains(testEntry.key));
    TestData_hm retrievedTestData;
    ASSERT_FALSE(testDataHashSet->retrieve(testEntry.key, retrievedTestData));
  }

  // assert that the second half remains unchanged
  for(u32 i = halfCollisionInsertCount; i < collisionsCount; ++i){
    TestEntry_hm& testEntry = collisionEntries[i];
    ASSERT_TRUE(testDataHashSet->contains(testEntry.key));
    TestData_hm retrievedTestData;
    ASSERT_TRUE(testDataHashSet->retrieve(testEntry.key, retrievedTestData));
    ASSERT_EQ(testEntry.datum.aDouble, retrievedTestData.aDouble);
    ASSERT_EQ(testEntry.datum.aSignedInt8, retrievedTestData.aSignedInt8);
    ASSERT_EQ(testEntry.datum.anUnsignedInt32, retrievedTestData.anUnsignedInt32);
  }

  // The collisions are initialized to double up for the hash values of 0-halfCollisionCount
  // 0-halfFirstWaveCount first wave entries have been removed, and the collisions have been reduced to single collisions
  // the overall result should be no collisions
  ASSERT_EQ(testDataHashSet->collisionCount, 0);

  u32 remainingCollisionsEntries = collisionsCount - halfCollisionInsertCount;
  u32 remainingFirstWaveEntries = firstWaveInsertCount - halfFirstWaveInsertCount;
  ASSERT_EQ(testDataHashSet->elementCount, remainingFirstWaveEntries + remainingCollisionsEntries);
  ASSERT_EQ(testDataHashSet->recyclingElementCount, collisionsCount);
}