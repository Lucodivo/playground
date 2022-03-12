//
// Created by Connor on 3/11/2022.
//

#include "test.h"

#include "hash_func_defines.h"
#include "hash_set.cpp"
#include "hash_map.cpp"

struct TestKey_hs {
  u64 uniqueIndex;
  f32 aFloat;
  f64 aDouble;
  bool aBool;
  s16 aSignedInt16;
};

HASH_FUNC_HASH(hash_set_test_data_hash) {
        TestKey_hs* testKey = (TestKey_hs*)key;
        return testKey->uniqueIndex;
}

HASH_FUNC_EQUALS(hash_set_test_data_equals) {
        TestKey_hs* testKey1 = (TestKey_hs*)key1;
        TestKey_hs* testKey2 = (TestKey_hs*)key2;
        return testKey1->uniqueIndex == testKey2->uniqueIndex &&
        testKey1->aFloat == testKey2->aFloat &&
        testKey1->aDouble == testKey2->aDouble &&
        testKey1->aBool == testKey2->aBool &&
        testKey1->aSignedInt16 == testKey2->aSignedInt16;
}

class HashSetTest : public testing::Test {
public:
  const class_access u64 firstLevelCapacity = 16;
  const class_access u64 firstWaveInsertCount = firstLevelCapacity;
  const class_access u64 collisionKeysCount = 10;
  const class_access u64 notInsertedDataCount = firstWaveInsertCount;

  TestKey_hs firstWaveTestKeys[firstLevelCapacity];
  TestKey_hs collisionKeys[collisionKeysCount];
  TestKey_hs notInsertedTestKeys[notInsertedDataCount];
  HashSet* testDataHashSet;

  // Init HashSet and fill it up with data
  void SetUp() override { // runs immediately before a test starts
    testDataHashSet = new HashSet(sizeof(TestKey_hs), hash_set_test_data_hash, hash_set_test_data_equals, firstLevelCapacity);

    const TestKey_hs defaultInsertKey{
            0,
            1.0f,
            2.0,
            true,
            4
    };

    const TestKey_hs default_NOT_InsertKey{
            0,
            400.0f,
            300.0,
            false,
            100
    };

    for(u32 i = 0; i < firstWaveInsertCount; i++) {
      firstWaveTestKeys[i] = defaultInsertKey;
      firstWaveTestKeys[i].uniqueIndex = i;
    }

    const u64 halfCollisionCount = collisionKeysCount / 2;
    // collisions
    for(u64 i = 0; i < halfCollisionCount; i++) {
      collisionKeys[i] = defaultInsertKey;
      collisionKeys[i].aSignedInt16 = defaultInsertKey.aSignedInt16 + i + 1; // something to be unique
      collisionKeys[i].uniqueIndex = (firstLevelCapacity + i);
    }
    // double collisions to same indices as above
    for(u64 i = halfCollisionCount; i < collisionKeysCount; i++) {
      collisionKeys[i] = defaultInsertKey;
      collisionKeys[i].aSignedInt16 = defaultInsertKey.aSignedInt16 + i + 1; // something to be unique
      collisionKeys[i].uniqueIndex = (firstLevelCapacity + i - halfCollisionCount);
    }

    for(u32 i = 0; i < notInsertedDataCount; i++) {
      notInsertedTestKeys[i] = default_NOT_InsertKey;
      notInsertedTestKeys[i].uniqueIndex = i;
    }

    // fill up hash set
    for(TestKey_hs& testData : firstWaveTestKeys) {
      testDataHashSet->insert(&testData);
    }

    // cause some collisions
    for(TestKey_hs& testData : collisionKeys) {
      testDataHashSet->insert(&testData);
    }
  }

  void TearDown() override { // runs immediately after a test finishes
    delete testDataHashSet;
  }
};

TEST_F(HashSetTest, inserts_contains_collisions) {
  // assert first round of data was inserted
  for(TestKey_hs& testData : firstWaveTestKeys) {
  ASSERT_TRUE(testDataHashSet->contains(&testData));
  }

  // assert collision round of data was inserted
  for(TestKey_hs& testData : collisionKeys) {
  ASSERT_TRUE(testDataHashSet->contains(&testData));
  }

  // assert non-inserted data is properly reported as not contained
  for(TestKey_hs& testData : notInsertedTestKeys) {
  ASSERT_FALSE(testDataHashSet->contains(&testData));
  }

  ASSERT_EQ(collisionKeysCount, testDataHashSet->collisionsCount);
  ASSERT_EQ(collisionKeysCount + firstLevelCapacity, testDataHashSet->elementsCount);
}

TEST(HashSet, insert_remove) {
  const u64 firstLevelCapacity = 16;
  TestKey_hs insertKey{
          0,
          1.0f,
          2.0,
          true,
          4
  };
  TestKey_hs insertThenRemoveKey1{
          insertKey.uniqueIndex,
          2.0f,
          3.0,
          false,
          5
  };
  TestKey_hs insertThenRemoveKey2 = insertThenRemoveKey1;
  ++insertThenRemoveKey2.aSignedInt16; // make unique
  TestKey_hs insertThenRemoveKey3 = insertThenRemoveKey2;
  ++insertThenRemoveKey2.aSignedInt16; // make unique

  HashSet testDataHashSet = HashSet(sizeof(TestKey_hs), hash_set_test_data_hash, hash_set_test_data_equals, firstLevelCapacity);

  testDataHashSet.insert(&insertThenRemoveKey1); // to test removing from front
  testDataHashSet.insert(&insertKey);
  testDataHashSet.insert(&insertThenRemoveKey2); // to test removing from middle
  testDataHashSet.insert(&insertThenRemoveKey3); // to test removing from end

  ASSERT_EQ(testDataHashSet.elementsCount, 4);
  ASSERT_EQ(testDataHashSet.collisionsCount, 3);
  ASSERT_EQ(testDataHashSet.recyclingElementsCount, 0);
  ASSERT_TRUE(testDataHashSet.contains(&insertKey));
  ASSERT_TRUE(testDataHashSet.contains(&insertThenRemoveKey1));
  ASSERT_TRUE(testDataHashSet.contains(&insertThenRemoveKey2));
  ASSERT_TRUE(testDataHashSet.contains(&insertThenRemoveKey3));

  // remove first element added
  testDataHashSet.remove(&insertThenRemoveKey1);

  ASSERT_EQ(testDataHashSet.elementsCount, 3);
  ASSERT_EQ(testDataHashSet.collisionsCount, 2);
  ASSERT_EQ(testDataHashSet.recyclingElementsCount, 1);
  ASSERT_TRUE(testDataHashSet.contains(&insertKey));
  ASSERT_FALSE(testDataHashSet.contains(&insertThenRemoveKey1));
  ASSERT_TRUE(testDataHashSet.contains(&insertThenRemoveKey2));
  ASSERT_TRUE(testDataHashSet.contains(&insertThenRemoveKey3));

  // remove middle element added
  testDataHashSet.remove(&insertThenRemoveKey2);

  ASSERT_EQ(testDataHashSet.elementsCount, 2);
  ASSERT_EQ(testDataHashSet.collisionsCount, 1);
  ASSERT_EQ(testDataHashSet.recyclingElementsCount, 2);
  ASSERT_TRUE(testDataHashSet.contains(&insertKey));
  ASSERT_FALSE(testDataHashSet.contains(&insertThenRemoveKey1));
  ASSERT_FALSE(testDataHashSet.contains(&insertThenRemoveKey2));
  ASSERT_TRUE(testDataHashSet.contains(&insertThenRemoveKey3));

  // remove last element added
  testDataHashSet.remove(&insertThenRemoveKey3);

  ASSERT_EQ(testDataHashSet.elementsCount, 1);
  ASSERT_EQ(testDataHashSet.collisionsCount, 0);
  ASSERT_EQ(testDataHashSet.recyclingElementsCount, 3);
  ASSERT_TRUE(testDataHashSet.contains(&insertKey));
  ASSERT_FALSE(testDataHashSet.contains(&insertThenRemoveKey1));
  ASSERT_FALSE(testDataHashSet.contains(&insertThenRemoveKey2));
  ASSERT_FALSE(testDataHashSet.contains(&insertThenRemoveKey3));
}

struct TestKey_hm {
  u64 uniqueIndex;
  char fourCharCode[4]; // key
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

class HashMapTest : public testing::Test {
public:
  const class_access u64 firstLevelCapacity = 16;
  const class_access u64 firstWaveInsertCount = firstLevelCapacity;
  const class_access u64 collisionsCount = 10;
  const class_access u64 notInsertedCount = firstWaveInsertCount;

  TestEntry_hm firstWaveTestEntries[firstLevelCapacity];
  TestEntry_hm collisionEntries[collisionsCount];
  TestEntry_hm notInsertedTestEntries[notInsertedCount];
  HashMap* testDataHashSet;

  // Init HashSet and fill it up with data
  void SetUp() override { // runs immediately before a test starts
    testDataHashSet = new HashMap(sizeof(TestKey_hm), sizeof(TestData_hm), hash_map_test_data_hash, hash_map_test_data_equals, firstLevelCapacity);

    const TestData_hm default_NOT_InsertDatum {
            6,
            6,
            6.0
    };

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
      testDataHashSet->insert(&testEntry.key, &testEntry.datum);
    }

    // cause some collisions
    for(TestEntry_hm& testEntry : collisionEntries) {
      testDataHashSet->insert(&testEntry.key, &testEntry.datum);
    }
  }

  void TearDown() override { // runs immediately after a test finishes
    delete testDataHashSet;
  }
};

TEST_F(HashMapTest, insert_retrieve) {
  // assert first round of data was inserted
  for(TestEntry_hm& testEntry : firstWaveTestEntries) {
    ASSERT_TRUE(testDataHashSet->contains(&testEntry.key));
    TestData_hm* testDataPtr = (TestData_hm*)testDataHashSet->retrieve(&testEntry.key);
    ASSERT_NE(testDataPtr, nullptr);
    ASSERT_EQ(testEntry.datum.aDouble, testDataPtr->aDouble);
    ASSERT_EQ(testEntry.datum.aSignedInt8, testDataPtr->aSignedInt8);
    ASSERT_EQ(testEntry.datum.anUnsignedInt32, testDataPtr->anUnsignedInt32);
  }

  // assert collision round of data was inserted and can be retrieved
  for(TestEntry_hm& testEntry : collisionEntries) {
    ASSERT_TRUE(testDataHashSet->contains(&testEntry.key));
    TestData_hm* testDataPtr = (TestData_hm*)testDataHashSet->retrieve(&testEntry.key);
    ASSERT_NE(testDataPtr, nullptr);
    ASSERT_EQ(testEntry.datum.aDouble, testDataPtr->aDouble);
    ASSERT_EQ(testEntry.datum.aSignedInt8, testDataPtr->aSignedInt8);
    ASSERT_EQ(testEntry.datum.anUnsignedInt32, testDataPtr->anUnsignedInt32);
  }

  // assert non-inserted data is properly reported as not contained
  for(TestEntry_hm& testEntry : notInsertedTestEntries) {
    ASSERT_FALSE(testDataHashSet->contains(&testEntry.key));
    ASSERT_EQ(testDataHashSet->retrieve(&testEntry.key), nullptr);
  }

  ASSERT_EQ(collisionsCount, testDataHashSet->collisionsCount);
  ASSERT_EQ(collisionsCount + firstLevelCapacity, testDataHashSet->elementsCount);
}

TEST_F(HashMapTest, insert_remove) {
  u32 halfFirstWaveInsertCount = firstWaveInsertCount / 2;

  // remove half of the first wave
  for(u32 i = 0; i < halfFirstWaveInsertCount; ++i){
    TestEntry_hm& testEntry = firstWaveTestEntries[i];
    ASSERT_TRUE(testDataHashSet->remove(&testEntry.key));
  }

  // assert that the first half is gone
  for(u32 i = 0; i < halfFirstWaveInsertCount; ++i){
    TestEntry_hm& testEntry = firstWaveTestEntries[i];
    ASSERT_FALSE(testDataHashSet->contains(&testEntry.key));
    ASSERT_EQ(testDataHashSet->retrieve(&testEntry.key), nullptr);
  }

  // assert that the second half remains unchanged
  for(u32 i = halfFirstWaveInsertCount; i < firstWaveInsertCount; ++i){
    TestEntry_hm& testEntry = firstWaveTestEntries[i];
    ASSERT_TRUE(testDataHashSet->contains(&testEntry.key));
    TestData_hm* testDataPtr = (TestData_hm*)testDataHashSet->retrieve(&testEntry.key);
    ASSERT_NE(testDataPtr, nullptr);
    ASSERT_EQ(testEntry.datum.aDouble, testDataPtr->aDouble);
    ASSERT_EQ(testEntry.datum.aSignedInt8, testDataPtr->aSignedInt8);
    ASSERT_EQ(testEntry.datum.anUnsignedInt32, testDataPtr->anUnsignedInt32);
  }

  u32 halfCollisionInsertCount = collisionsCount / 2;

  // remove half of the collisions
  for(u32 i = 0; i < halfCollisionInsertCount; ++i){
    TestEntry_hm& testEntry = collisionEntries[i];
    ASSERT_TRUE(testDataHashSet->remove(&testEntry.key));
  }

  // assert that the first half is gone
  for(u32 i = 0; i < halfCollisionInsertCount; ++i){
    TestEntry_hm& testEntry = collisionEntries[i];
    ASSERT_FALSE(testDataHashSet->contains(&testEntry.key));
    ASSERT_EQ(testDataHashSet->retrieve(&testEntry.key), nullptr);
  }

  // assert that the second half remains unchanged
  for(u32 i = halfCollisionInsertCount; i < collisionsCount; ++i){
    TestEntry_hm& testEntry = collisionEntries[i];
    ASSERT_TRUE(testDataHashSet->contains(&testEntry.key));
    TestData_hm* testDataPtr = (TestData_hm*)testDataHashSet->retrieve(&testEntry.key);
    ASSERT_NE(testDataPtr, nullptr);
    ASSERT_EQ(testEntry.datum.aDouble, testDataPtr->aDouble);
    ASSERT_EQ(testEntry.datum.aSignedInt8, testDataPtr->aSignedInt8);
    ASSERT_EQ(testEntry.datum.anUnsignedInt32, testDataPtr->anUnsignedInt32);
  }

  // The collisions are initialized to double up for the hash values of 0-halfCollisionCount
  // 0-halfFirstWaveCount first wave entries have been removed, and the collisions have been reduced to single collisions
  // the overall result should be no collisions
  ASSERT_EQ(testDataHashSet->collisionsCount, 0);

  u32 remainingCollisionsEntries = collisionsCount - halfCollisionInsertCount;
  u32 remainingFirstWaveEntries = firstWaveInsertCount - halfFirstWaveInsertCount;
  ASSERT_EQ(testDataHashSet->elementsCount, remainingFirstWaveEntries + remainingCollisionsEntries);
  ASSERT_EQ(testDataHashSet->recyclingElementsCount, halfFirstWaveInsertCount + halfCollisionInsertCount);
}