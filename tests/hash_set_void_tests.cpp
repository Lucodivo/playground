//
// Created by Connor on 3/16/2022.
//

#include "test.h"

#include "hash_func_defines.h"
#include "hash_set_void.cpp"

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

class HashSetVoidTest_F : public testing::Test {
public:
  const class_access u64 firstLevelCapacity = 16;
  const class_access u64 firstWaveInsertCount = firstLevelCapacity;
  const class_access u64 collisionKeysCount = 10;
  const class_access u64 notInsertedDataCount = firstWaveInsertCount;

  TestKey_hs firstWaveTestKeys[firstLevelCapacity];
  TestKey_hs collisionKeys[collisionKeysCount];
  TestKey_hs notInsertedTestKeys[notInsertedDataCount];
  HashSetVoid* testDataHashSet;

  // Init HashSetVoid and fill it up with data
  void SetUp() override { // runs immediately before a test starts
    testDataHashSet = new HashSetVoid(sizeof(TestKey_hs), hash_set_test_data_hash, hash_set_test_data_equals, firstLevelCapacity);

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
    for(TestKey_hs& testData: firstWaveTestKeys) {
      testDataHashSet->insert(&testData);
    }

    // cause some collisions
    for(TestKey_hs& testData: collisionKeys) {
      testDataHashSet->insert(&testData);
    }
  }

  void TearDown() override { // runs immediately after a test finishes
    delete testDataHashSet;
  }
};

TEST_F(HashSetVoidTest_F, inserts_contains_collisions) {
// assert first round of data was inserted
  for(TestKey_hs& testData: firstWaveTestKeys) {
    ASSERT_TRUE(testDataHashSet->contains(&testData));
  }

// assert collision round of data was inserted
  for(TestKey_hs& testData: collisionKeys) {
    ASSERT_TRUE(testDataHashSet->contains(&testData));
  }

// assert non-inserted data is properly reported as not contained
  for(TestKey_hs& testData: notInsertedTestKeys) {
    ASSERT_FALSE(testDataHashSet->contains(&testData));
  }

  ASSERT_EQ(collisionKeysCount, testDataHashSet->collisionsCount);
  ASSERT_EQ(collisionKeysCount + firstLevelCapacity, testDataHashSet->elementsCount);
}

TEST_F(HashSetVoidTest_F, clear) {
// Ensure all entries are initially contained as expected
  for(TestKey_hs& testKey: firstWaveTestKeys) {
    ASSERT_TRUE(testDataHashSet->contains(&testKey));
  }
  for(TestKey_hs& testKey: collisionKeys) {
    ASSERT_TRUE(testDataHashSet->contains(&testKey));
  }
  for(TestKey_hs& testKey: notInsertedTestKeys) {
    ASSERT_FALSE(testDataHashSet->contains(&testKey));
  }
  ASSERT_EQ(testDataHashSet->elementsCount, firstWaveInsertCount + collisionKeysCount);
  ASSERT_EQ(testDataHashSet->collisionsCount, collisionKeysCount);

  testDataHashSet->clear();

// Assert that nothing remains
  for(TestKey_hs& testKey: firstWaveTestKeys) {
    ASSERT_FALSE(testDataHashSet->contains(&testKey));
  }
  for(TestKey_hs& testKey: collisionKeys) {
    ASSERT_FALSE(testDataHashSet->contains(&testKey));
  }
  for(TestKey_hs& testKey: notInsertedTestKeys) {
    ASSERT_FALSE(testDataHashSet->contains(&testKey));
  }
  ASSERT_EQ(testDataHashSet->elementsCount, 0);
  ASSERT_EQ(testDataHashSet->collisionsCount, 0);
}

TEST(HashSetVoidTest, insert_remove) {
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

  HashSetVoid testDataHashSet = HashSetVoid(sizeof(TestKey_hs), hash_set_test_data_hash, hash_set_test_data_equals, firstLevelCapacity);

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

TEST_F(HashSetVoidTest_F, resize) {
  // Ensure all entries are initially contained as expected
  for(TestKey_hs& testKey : firstWaveTestKeys) {
    ASSERT_TRUE(testDataHashSet->contains(&testKey));
  }
  for(TestKey_hs& testKey : collisionKeys) {
    ASSERT_TRUE(testDataHashSet->contains(&testKey));
  }
  for(TestKey_hs& testKey : notInsertedTestKeys) {
    ASSERT_FALSE(testDataHashSet->contains(&testKey));
  }
  ASSERT_EQ(testDataHashSet->elementsCount, firstWaveInsertCount + collisionKeysCount);
  ASSERT_EQ(testDataHashSet->collisionsCount, collisionKeysCount);
  u64 newCapacity = testDataHashSet->elementsCapacity * 2;

  testDataHashSet->resize(newCapacity);

  // Ensure all entries are initially contained as expected
  for(TestKey_hs& testKey : firstWaveTestKeys) {
    ASSERT_TRUE(testDataHashSet->contains(&testKey));
  }
  for(TestKey_hs& testKey : collisionKeys) {
    ASSERT_TRUE(testDataHashSet->contains(&testKey));
  }
  for(TestKey_hs& testKey : notInsertedTestKeys) {
    ASSERT_FALSE(testDataHashSet->contains(&testKey));
  }
  ASSERT_EQ(testDataHashSet->elementsCount, firstWaveInsertCount + collisionKeysCount);
  ASSERT_EQ(testDataHashSet->elementsCapacity, newCapacity);
}

TEST(HashMapVoidTest, resize_from_insert) {

  const u64 beginCapacity = 4;
  const u64 expectedEndCapacity = beginCapacity * 2;
  HashSetVoid hashMapVoid = HashSetVoid(sizeof(TestKey_hs), hash_set_test_data_hash, hash_set_test_data_equals, 4);

  const u64 testKeysCount = beginCapacity + 1;
  TestKey_hs testKeys[testKeysCount];

  TestKey_hs defaultTestKey;
  defaultTestKey.uniqueIndex = 0;
  defaultTestKey.aSignedInt16 = 1;
  defaultTestKey.aDouble = 2.2;
  defaultTestKey.aBool = true;
  defaultTestKey.aFloat = 3.3f;
  TestKey_hs notInsertedKey = defaultTestKey;
  notInsertedKey.uniqueIndex = ~0u;

  for(u64 i = 0; i < testKeysCount; ++i) {
    TestKey_hs& testKey = testKeys[i];
    testKey = defaultTestKey;
    testKey.uniqueIndex = i;
  }

  for(u64 i = 0; i < beginCapacity; ++i) {
    TestKey_hs& testKey = testKeys[i];
    hashMapVoid.insert(&testKey);
  }

  // assert beginning assumptions are correct
  ASSERT_EQ(hashMapVoid.elementsCount, hashMapVoid.elementsCapacity);
  ASSERT_EQ(hashMapVoid.elementsCapacity, beginCapacity);
  for(u64 i = 0; i < beginCapacity; ++i) {
    TestKey_hs& testKey = testKeys[i];
    ASSERT_TRUE(hashMapVoid.contains(&testKey));
  }
  TestKey_hs& camelsBackKey = testKeys[beginCapacity];
  ASSERT_FALSE(hashMapVoid.contains(&camelsBackKey));

  hashMapVoid.insert(&camelsBackKey);

  // assert all elements and counts are as expected
  ASSERT_EQ(hashMapVoid.elementsCount, testKeysCount);
  ASSERT_GT(hashMapVoid.elementsCapacity, beginCapacity);
  for(TestKey_hs& testKey : testKeys) {
    ASSERT_TRUE(hashMapVoid.contains(&testKey));
  }
  ASSERT_FALSE(hashMapVoid.contains(&notInsertedKey));
}