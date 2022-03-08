#include "test.h"

#include "linked_list.cpp"
#include "dictionary_trie.cpp"

class PlaygroundTest : public testing::Test {
protected:
  void SetUp() override { } // run immediately before a test starts
  void TearDown() override { } // invoked immediately after a test finishes
};

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