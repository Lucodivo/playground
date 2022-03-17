//
// Created by Connor on 3/11/2022.
//

#include "test.h"

#include "hash_func_defines.h"
#include "hash_map_void.cpp"

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

HASH_FUNC_HASH(hash_map_test_data_hash) {
  TestKey_hm* testData = (TestKey_hm*)key;
  // Note: more fun hash but harder to test collisions
//  const u32 normalizeRange = 4 * 'a';
//  u64 hash = testData->fourCharCode[0] +
//  testData->fourCharCode[1] +
//  testData->fourCharCode[2] +
//  testData->fourCharCode[3];
//  hash -= normalizeRange;
  return testData->uniqueIndex;
}

HASH_FUNC_EQUALS(hash_map_test_data_equals) {
  TestKey_hm* testData1 = (TestKey_hm*)key1;
  TestKey_hm* testData2 = (TestKey_hm*)key2;
  return testData1->uniqueIndex == testData2->uniqueIndex &&
  testData1->fourCharCode[0] == testData2->fourCharCode[0] &&
  testData1->fourCharCode[1] == testData2->fourCharCode[1] &&
  testData1->fourCharCode[2] == testData2->fourCharCode[2] &&
  testData1->fourCharCode[3] == testData2->fourCharCode[3];
}

class HashMapVoidTest_F : public testing::Test {
public:
  const class_access u64 capacity = 32;
  const class_access u64 firstWaveInsertCount = capacity / 2;
  const class_access u64 collisionsCount = 10;
  const class_access u64 notInsertedCount = firstWaveInsertCount;

  TestEntry_hm firstWaveTestEntries[firstWaveInsertCount];
  TestEntry_hm collisionEntries[collisionsCount];
  TestEntry_hm notInsertedTestEntries[notInsertedCount];
  HashMapVoid* testDataHashMap;

  // Init HashSetVoid and fill it up with data
  void SetUp() override { // runs immediately before a test starts
    testDataHashMap = new HashMapVoid(sizeof(TestKey_hm), sizeof(TestData_hm), hash_map_test_data_hash, hash_map_test_data_equals, capacity);

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
      collisionEntries[i].key.uniqueIndex = i;
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
      collisionEntries[i].key.uniqueIndex = i - halfCollisionCount;
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
      testDataHashMap->insert(&testEntry.key, &testEntry.datum);
    }

    // cause some collisions
    for(TestEntry_hm& testEntry : collisionEntries) {
      testDataHashMap->insert(&testEntry.key, &testEntry.datum);
    }
  }

  void TearDown() override { // runs immediately after a test finishes
    delete testDataHashMap;
  }
};

TEST_F(HashMapVoidTest_F, insert_retrieve) {
  // assert first round of data was inserted
  for(TestEntry_hm& testEntry : firstWaveTestEntries) {
    ASSERT_TRUE(testDataHashMap->contains(&testEntry.key));
    TestData_hm testDataPtr;
    ASSERT_TRUE(testDataHashMap->retrieve(&testEntry.key, &testDataPtr));
    ASSERT_EQ(testEntry.datum.aDouble, testDataPtr.aDouble);
    ASSERT_EQ(testEntry.datum.aSignedInt8, testDataPtr.aSignedInt8);
    ASSERT_EQ(testEntry.datum.anUnsignedInt32, testDataPtr.anUnsignedInt32);
  }

  // assert collision round of data was inserted and can be retrieved
  for(TestEntry_hm& testEntry : collisionEntries) {
    ASSERT_TRUE(testDataHashMap->contains(&testEntry.key));
    TestData_hm testDataPtr;
    ASSERT_TRUE(testDataHashMap->retrieve(&testEntry.key, &testDataPtr));
    ASSERT_EQ(testEntry.datum.aDouble, testDataPtr.aDouble);
    ASSERT_EQ(testEntry.datum.aSignedInt8, testDataPtr.aSignedInt8);
    ASSERT_EQ(testEntry.datum.anUnsignedInt32, testDataPtr.anUnsignedInt32);
  }

  // assert non-inserted data is properly reported as not contained
  for(TestEntry_hm& testEntry : notInsertedTestEntries) {
    ASSERT_FALSE(testDataHashMap->contains(&testEntry.key));
    TestData_hm testDataPtr;
    ASSERT_FALSE(testDataHashMap->retrieve(&testEntry.key, &testDataPtr));
  }

  ASSERT_EQ(collisionsCount, testDataHashMap->collisionsCount);
  ASSERT_EQ(collisionsCount + firstWaveInsertCount, testDataHashMap->elementsCount);
}

TEST_F(HashMapVoidTest_F, insert_remove) {
  u32 halfFirstWaveInsertCount = firstWaveInsertCount / 2;

  // remove half of the first wave
  for(u32 i = 0; i < halfFirstWaveInsertCount; ++i){
    TestEntry_hm& testEntry = firstWaveTestEntries[i];
    ASSERT_TRUE(testDataHashMap->remove(&testEntry.key));
  }

  // assert that the first half is gone
  for(u32 i = 0; i < halfFirstWaveInsertCount; ++i){
    TestEntry_hm& testEntry = firstWaveTestEntries[i];
    ASSERT_FALSE(testDataHashMap->contains(&testEntry.key));
    TestData_hm testDataPtr;
    ASSERT_FALSE(testDataHashMap->retrieve(&testEntry.key, &testDataPtr));
  }

  // assert that the second half remains unchanged
  for(u32 i = halfFirstWaveInsertCount; i < firstWaveInsertCount; ++i){
    TestEntry_hm& testEntry = firstWaveTestEntries[i];
    ASSERT_TRUE(testDataHashMap->contains(&testEntry.key));
    TestData_hm testDataPtr;
    ASSERT_TRUE(testDataHashMap->retrieve(&testEntry.key, &testDataPtr));
    ASSERT_EQ(testEntry.datum.aDouble, testDataPtr.aDouble);
    ASSERT_EQ(testEntry.datum.aSignedInt8, testDataPtr.aSignedInt8);
    ASSERT_EQ(testEntry.datum.anUnsignedInt32, testDataPtr.anUnsignedInt32);
  }

  u32 halfCollisionInsertCount = collisionsCount / 2;

  // remove half of the collisions
  for(u32 i = 0; i < halfCollisionInsertCount; ++i){
    TestEntry_hm& testEntry = collisionEntries[i];
    ASSERT_TRUE(testDataHashMap->remove(&testEntry.key));
  }

  // assert that the first half is gone
  for(u32 i = 0; i < halfCollisionInsertCount; ++i){
    TestEntry_hm& testEntry = collisionEntries[i];
    ASSERT_FALSE(testDataHashMap->contains(&testEntry.key));
    TestData_hm testDataPtr;
    ASSERT_FALSE(testDataHashMap->retrieve(&testEntry.key, &testDataPtr));
  }

  // assert that the second half remains unchanged
  for(u32 i = halfCollisionInsertCount; i < collisionsCount; ++i){
    TestEntry_hm& testEntry = collisionEntries[i];
    ASSERT_TRUE(testDataHashMap->contains(&testEntry.key));
    TestData_hm testDataPtr;
    ASSERT_TRUE(testDataHashMap->retrieve(&testEntry.key, &testDataPtr));
    ASSERT_EQ(testEntry.datum.aDouble, testDataPtr.aDouble);
    ASSERT_EQ(testEntry.datum.aSignedInt8, testDataPtr.aSignedInt8);
    ASSERT_EQ(testEntry.datum.anUnsignedInt32, testDataPtr.anUnsignedInt32);
  }

  // The collisions are initialized to double up for the hash values of 0-halfCollisionCount
  // 0-halfFirstWaveCount first wave entries have been removed, and the collisions have been reduced to single collisions
  // the overall result should be no collisions
  ASSERT_EQ(testDataHashMap->collisionsCount, 0);

  u32 remainingCollisionsEntries = collisionsCount - halfCollisionInsertCount;
  u32 remainingFirstWaveEntries = firstWaveInsertCount - halfFirstWaveInsertCount;
  ASSERT_EQ(testDataHashMap->elementsCount, remainingFirstWaveEntries + remainingCollisionsEntries);
}

TEST_F(HashMapVoidTest_F, clear) {
  // Ensure all entries are initially contained as expected
  for(TestEntry_hm& testEntry : firstWaveTestEntries) {
    ASSERT_TRUE(testDataHashMap->contains(&testEntry.key));
  }
  for(TestEntry_hm& testEntry : collisionEntries) {
    ASSERT_TRUE(testDataHashMap->contains(&testEntry.key));
  }
  for(TestEntry_hm& testEntry : notInsertedTestEntries) {
    ASSERT_FALSE(testDataHashMap->contains(&testEntry.key));
  }
  ASSERT_EQ(testDataHashMap->elementsCount, firstWaveInsertCount + collisionsCount);
  ASSERT_EQ(testDataHashMap->collisionsCount, collisionsCount);

  testDataHashMap->clear();

  // Assert that nothing remains
  for(TestEntry_hm& testEntry : firstWaveTestEntries) {
    ASSERT_FALSE(testDataHashMap->contains(&testEntry.key));
  }
  for(TestEntry_hm& testEntry : collisionEntries) {
    ASSERT_FALSE(testDataHashMap->contains(&testEntry.key));
  }
  for(TestEntry_hm& testEntry : notInsertedTestEntries) {
    ASSERT_FALSE(testDataHashMap->contains(&testEntry.key));
  }
  ASSERT_EQ(testDataHashMap->elementsCount, 0);
  ASSERT_EQ(testDataHashMap->collisionsCount, 0);
}

TEST_F(HashMapVoidTest_F, resize) {
  // Ensure all entries are initially contained as expected
  for(TestEntry_hm& testEntry : firstWaveTestEntries) {
    ASSERT_TRUE(testDataHashMap->contains(&testEntry.key));
  }
  for(TestEntry_hm& testEntry : collisionEntries) {
    ASSERT_TRUE(testDataHashMap->contains(&testEntry.key));
  }
  for(TestEntry_hm& testEntry : notInsertedTestEntries) {
    ASSERT_FALSE(testDataHashMap->contains(&testEntry.key));
  }
  ASSERT_EQ(testDataHashMap->elementsCount, firstWaveInsertCount + collisionsCount);
  ASSERT_EQ(testDataHashMap->collisionsCount, collisionsCount);
  u64 newCapacity = testDataHashMap->elementsCapacity * 2;

  testDataHashMap->resize(newCapacity);

  // Ensure all entries are initially contained as expected
  for(TestEntry_hm& testEntry : firstWaveTestEntries) {
    ASSERT_TRUE(testDataHashMap->contains(&testEntry.key));
  }
  for(TestEntry_hm& testEntry : collisionEntries) {
    ASSERT_TRUE(testDataHashMap->contains(&testEntry.key));
  }
  for(TestEntry_hm& testEntry : notInsertedTestEntries) {
    ASSERT_FALSE(testDataHashMap->contains(&testEntry.key));
  }
  ASSERT_EQ(testDataHashMap->elementsCount, firstWaveInsertCount + collisionsCount);
  ASSERT_EQ(testDataHashMap->elementsCapacity, newCapacity);
}

TEST(HashMapVoidTest, resize_from_insert) {

  const u64 beginCapacity = 4;
  const u64 expectedEndCapacity = beginCapacity * 2;
  HashMapVoid hashMapVoid = HashMapVoid(sizeof(TestKey_hm), sizeof(TestData_hm), hash_map_test_data_hash, hash_map_test_data_equals, 4);

  const u64 testEntriesCount = beginCapacity + 1;
  TestEntry_hm testEntries[testEntriesCount];

  TestEntry_hm defaultTestEntry;
  defaultTestEntry.key.uniqueIndex = 0;
  defaultTestEntry.key.fourCharCode[0] = '0';
  defaultTestEntry.key.fourCharCode[1] = '1';
  defaultTestEntry.key.fourCharCode[2] = '2';
  defaultTestEntry.key.fourCharCode[3] = '3';
  defaultTestEntry.datum = {};
  TestEntry_hm notInsertedEntry = defaultTestEntry;
  notInsertedEntry.key.uniqueIndex = ~0u;

  for(u64 i = 0; i < testEntriesCount; ++i) {
    TestEntry_hm& testEntry = testEntries[i];
    testEntry = defaultTestEntry;
    testEntry.key.uniqueIndex = i;
  }

  for(u64 i = 0; i < beginCapacity; ++i) {
    TestEntry_hm& testEntry = testEntries[i];
    hashMapVoid.insert(&testEntry.key, &testEntry.datum);
  }

  // assert beginning assumptions are correct
  ASSERT_EQ(hashMapVoid.elementsCount, hashMapVoid.elementsCapacity);
  ASSERT_EQ(hashMapVoid.elementsCapacity, beginCapacity);
  for(u64 i = 0; i < beginCapacity; ++i) {
    TestEntry_hm& testEntry = testEntries[i];
    ASSERT_TRUE(hashMapVoid.contains(&testEntry.key));
  }
  TestEntry_hm& camelsBackEntry = testEntries[beginCapacity];
  ASSERT_FALSE(hashMapVoid.contains(&camelsBackEntry.key));

  hashMapVoid.insert(&camelsBackEntry.key, &camelsBackEntry.datum);

  // assert all elements and counts are as expected
  ASSERT_EQ(hashMapVoid.elementsCount, testEntriesCount);
  ASSERT_GT(hashMapVoid.elementsCapacity, beginCapacity);
  for(TestEntry_hm& testEntry : testEntries) {
    ASSERT_TRUE(hashMapVoid.contains(&testEntry.key));
  }
  ASSERT_FALSE(hashMapVoid.contains(&notInsertedEntry.key));
}