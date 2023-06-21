//
// Created by Connor on 3/11/2022.
//

#include "test.h"
#include "dictionary_trie.cpp"

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