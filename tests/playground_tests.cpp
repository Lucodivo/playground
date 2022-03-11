#include "test.h"

#include "linked_list.cpp"
#include "dictionary_trie.cpp"
#include "hash_func_defines.h"
#include "hash_set.cpp"
#include "hash_map.cpp"

// TEST_F is for using "fixtures"
TEST(Playground, strncpys) {
const char* butts = "butts";
char copy[6] = {'?','?','?','?','?','?'};
strncpy_s(copy, 6, butts, 5);
ASSERT_EQ(strcmp(copy, butts), 0); // assert that it adds the null character
}

TEST(Playground, pointerArithmatic) {
  const u32 arrayCount = 50;
  const u32 halfArrayCount = arrayCount / 2;
  struct PtrArithTest {
    s64 datum3;
    u32 datum1;
    char datum2;
  };
  PtrArithTest data[arrayCount];
  PtrArithTest* halfData = data + halfArrayCount;

  u32 ptrDiff = halfData - data;
  u32 charPtrDiff = (char*)halfData - (char*)data;

  ASSERT_EQ(ptrDiff, halfArrayCount);
  ASSERT_EQ(charPtrDiff, halfArrayCount * sizeof(PtrArithTest));
}

TEST(SinglyLinkedList, addRemove) {
  u32 capacity = 6;
  singly_linked_list list = SinglyLinkedList(capacity);

  addFront(list, 0);
  addFront(list, 1);
  addFront(list, 2);
  addBack(list, 3);
  addBack(list, 4);
  addBack(list, 5);

  ASSERT_EQ(list.count, capacity);
  ASSERT_EQ(list.capacity, capacity);

  remove(list, 1);
  remove(list, 3);
  remove(list, 5);
  ASSERT_TRUE(contains(list, 0));
  ASSERT_TRUE(contains(list, 2));
  ASSERT_TRUE(contains(list, 4));
  ASSERT_FALSE(contains(list, 1));
  ASSERT_FALSE(contains(list, 3));
  ASSERT_FALSE(contains(list, 5));

  ASSERT_EQ(list.count, capacity - 3);

  addFront(list, 1);
  addBack(list, 3);
  addBack(list, 5);
  ASSERT_TRUE(contains(list, 0));
  ASSERT_TRUE(contains(list, 1));
  ASSERT_TRUE(contains(list, 2));
  ASSERT_TRUE(contains(list, 3));
  ASSERT_TRUE(contains(list, 4));
  ASSERT_TRUE(contains(list, 5));
  ASSERT_EQ(list.count, capacity);
  ASSERT_EQ(list.capacity, capacity); // assert capacity hasn't changed

  destroy(list);
}

TEST(SinglyLinkedList, doubleCapacity) {
  const u32 originalCapacity = 12;
  singly_linked_list list = SinglyLinkedList(originalCapacity);

  // fill up list to capacity
  for(u32 i = 0; i < originalCapacity; i++) {
    addBack(list, i);
  }

  // save list before increasing capacity into vectors
  std::vector<u32> listDataBeforeDoubleCap(list.count);
  u32 beforeDoubleCapIndex = 0;
  noop_node_s* listIter = list.head;
  while(listIter != nullptr) {
    listDataBeforeDoubleCap[beforeDoubleCapIndex++] = listIter->data;
    listIter = listIter->next;
  }

  const u32 countBeforeDoubleCap = list.count;
  ASSERT_EQ(countBeforeDoubleCap, list.capacity); // assert that we are at capacity
  addBack(list, originalCapacity);
  ASSERT_GT(list.capacity, originalCapacity); // assert that the capacity has grown
  ASSERT_EQ(list.count, countBeforeDoubleCap + 1); // assert the count has not changed

  // assert the data is the same
  listIter = list.head;
  beforeDoubleCapIndex = 0;
  for(u32 i = 0; i < countBeforeDoubleCap; i++) {
    ASSERT_EQ(listIter->data, listDataBeforeDoubleCap[beforeDoubleCapIndex++]);
    listIter = listIter->next;
  }
  ASSERT_EQ(listIter->data, originalCapacity);
  destroy(list);
}

const char* wordFile4000 = "4000-most-common-english-words.txt";
const char* wordFileAlpha = "words_alpha.txt";
const char* wordFile = wordFileAlpha;

TEST(TrieDictionary, buildDictAndContains_LinkedTrie) {
  std::vector<char> fileCharacters;
  readFile(wordFile, fileCharacters);

  // == Trie with allocator ==
  Timer timer;
  StartTimer(timer);
  linked_trie_dictionary linkedTrieDictionary;
  buildDictionary(fileCharacters, linkedTrieDictionary);
  f64 timeToLoad = StopTimer(timer);
  printf("Time to load (linked trie): %5.5f ms\n", timeToLoad);

  StartTimer(timer);
  ASSERT_TRUE(contains(linkedTrieDictionary, "the"));
  ASSERT_TRUE(contains(linkedTrieDictionary, "and"));
  ASSERT_TRUE(contains(linkedTrieDictionary, "vacuum"));
  ASSERT_TRUE(contains(linkedTrieDictionary, "selected"));
  ASSERT_TRUE(contains(linkedTrieDictionary, "frustration"));
  ASSERT_FALSE(contains(linkedTrieDictionary, "thion"));
  ASSERT_FALSE(contains(linkedTrieDictionary, "anipol"));
  ASSERT_FALSE(contains(linkedTrieDictionary, "selectedz"));
  ASSERT_FALSE(contains(linkedTrieDictionary, "frustr"));
  f64 timeForContains = StopTimer(timer);
  printf("Time for 9 contains (linked trie): %5.5f ms\n", timeForContains);

  f64 totalMemoryAllocatedMBs = linkedTrieDictionary.allocator.totalMemoryAllocated / 1024.0 / 1024.0;
  printf("Total Memory (linked trie): %5.5f MBs\n", totalMemoryAllocatedMBs);
  printf("Malloc Count (linked trie): %llu\n", linkedTrieDictionary.allocator.mallocPtrs.size());

  freeDictionary(linkedTrieDictionary);
}

TEST(TrieDictionary, buildDictAndContains_LinkedTrie_NoAllocator) {
  std::vector<char> fileCharacters;
  readFile(wordFile, fileCharacters);

  // == Trie using new ==
  Timer timer;
  StartTimer(timer);
  linked_trie_dictionary_no_allocator linkedTrieDictionaryNoAllocator;
  buildDictionary(fileCharacters, linkedTrieDictionaryNoAllocator);
  f64 timeToLoad = StopTimer(timer);
  printf("Time to load (no allocator): %5.5f ms\n", timeToLoad);

  StartTimer(timer);
  ASSERT_TRUE(contains(linkedTrieDictionaryNoAllocator, "the"));
  ASSERT_TRUE(contains(linkedTrieDictionaryNoAllocator, "and"));
  ASSERT_TRUE(contains(linkedTrieDictionaryNoAllocator, "vacuum"));
  ASSERT_TRUE(contains(linkedTrieDictionaryNoAllocator, "selected"));
  ASSERT_TRUE(contains(linkedTrieDictionaryNoAllocator, "frustration"));
  ASSERT_FALSE(contains(linkedTrieDictionaryNoAllocator, "thion"));
  ASSERT_FALSE(contains(linkedTrieDictionaryNoAllocator, "anipol"));
  ASSERT_FALSE(contains(linkedTrieDictionaryNoAllocator, "selectedz"));
  ASSERT_FALSE(contains(linkedTrieDictionaryNoAllocator, "frustr"));
  f64 timeForContains = StopTimer(timer);
  printf("Time for 9 contains (no allocator): %5.5f ms\n", timeForContains);

  u64 totalMemoryAllocated = sizeof(linked_trie_dictionary_node) * linkedTrieDictionaryNoAllocator.nodeCount;
  f64 totalMemoryAllocatedMBs = totalMemoryAllocated / 1024.0 / 1024.0;
  printf("Total Memory (no allocator): %5.5f MBs\n", totalMemoryAllocatedMBs);
  printf("Malloc Count (no allocator): %llu\n", linkedTrieDictionaryNoAllocator.nodeCount);

  freeDictionary(linkedTrieDictionaryNoAllocator);
}

TEST(TrieDictionary, buildDictAndContains_Trie) {
  std::vector<char> fileCharacters;
  readFile(wordFile, fileCharacters);

  // == Trie using new ==
  Timer timer;
  StartTimer(timer);
  trie_dictionary trieDictionary;
  buildDictionary(fileCharacters, trieDictionary);
  f64 timeToLoad = StopTimer(timer);
  printf("Time to load (trie): %5.5f ms\n", timeToLoad);

  StartTimer(timer);
  ASSERT_TRUE(contains(trieDictionary, "the"));
  ASSERT_TRUE(contains(trieDictionary, "and"));
  ASSERT_TRUE(contains(trieDictionary, "vacuum"));
  ASSERT_TRUE(contains(trieDictionary, "selected"));
  ASSERT_TRUE(contains(trieDictionary, "frustration"));
  ASSERT_FALSE(contains(trieDictionary, "thion"));
  ASSERT_FALSE(contains(trieDictionary, "anipol"));
  ASSERT_FALSE(contains(trieDictionary, "selectedz"));
  ASSERT_FALSE(contains(trieDictionary, "frustr"));
  f64 timeForContains = StopTimer(timer);
  printf("Time for 9 contains (trie): %5.5f ms\n", timeForContains);

  f64 totalMemoryAllocatedMBs = trieDictionary.allocator.totalMemoryAllocated / 1024.0 / 1024.0;
  printf("Total Memory (trie): %5.5f MBs\n", totalMemoryAllocatedMBs);
  printf("Malloc Count (trie): %llu\n", trieDictionary.allocator.mallocPtrs.size());

  freeDictionary(trieDictionary);
}

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
  const u32 normalizeRange = 4 * 'a';
  u64 hash = testData->fourCharCode[0] +
          testData->fourCharCode[1] +
          testData->fourCharCode[2] +
          testData->fourCharCode[3];
  hash -= normalizeRange;
  return hash;
}

HASH_FUNC_EQUALS(hash_map_test_data_equals) {
  TestKey_hm* testData1 = (TestKey_hm*)key1;
  TestKey_hm* testData2 = (TestKey_hm*)key2;
  return testData1->fourCharCode[0] == testData2->fourCharCode[0] &&
         testData1->fourCharCode[1] == testData2->fourCharCode[1] &&
         testData1->fourCharCode[2] == testData2->fourCharCode[2] &&
         testData1->fourCharCode[3] == testData2->fourCharCode[3];
}

TEST(HashMap, doesnt_immediately_crash) {
  const u64 firstLevelCapacity = 16;
  HashMap testDataHashMap = HashMap(sizeof(TestKey_hm), sizeof(TestData_hm), hash_map_test_data_hash, hash_map_test_data_equals, firstLevelCapacity);

  TestEntry_hm entry{};
  entry.key.fourCharCode[0] = 'a';
  entry.key.fourCharCode[1] = 'b';
  entry.key.fourCharCode[2] = 'c';
  entry.key.fourCharCode[3] = 'd';
  entry.datum.anUnsignedInt32 = 1;
  entry.datum.aSignedInt8 = 2;
  entry.datum.aDouble = 3.0;

  testDataHashMap.insert(&entry.key, &entry.datum);

  ASSERT_TRUE(testDataHashMap.contains(&entry.key));

  TestData_hm* dataRetrieved = (TestData_hm*)testDataHashMap.retrieve(&entry.key);
  ASSERT_EQ(entry.datum.anUnsignedInt32, dataRetrieved->anUnsignedInt32);
  ASSERT_EQ(entry.datum.aSignedInt8, dataRetrieved->aSignedInt8);
  ASSERT_EQ(entry.datum.aDouble, dataRetrieved->aDouble);
}