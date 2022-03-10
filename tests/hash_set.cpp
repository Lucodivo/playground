//
// Created by Connor on 3/8/2022.
// Generic hash set by just throwing around void pointers
//

#define HASH_SET_HASH(name) u64 name(void* voidPtr) // allows us to easily define HashSet hash function
typedef HASH_SET_HASH(hash_set_hash); // allows us to easily pass around function pointers of hash funcions
HASH_SET_HASH(HashSetHashStub) // creation of a stub hash function that is guaranteed to throw an error
{
  printf("ERROR: Hash map hash using stub function\n");
  return ~0u;
}

#define HASH_SET_EQUALS(name) bool name(void* voidPtr1, void* voidPtr2) // allows us to easily define HashSet equals function
typedef HASH_SET_EQUALS(hash_set_equals); // allows us to easily pass around function pointers of equals functions
HASH_SET_EQUALS(HashSetEqualsStub) // creation of a stub equals function that is guaranteed to throw an error
{
  printf("ERROR: Hash map equals using stub function\n");
  return false;
}

struct HashSet {
  u64 firstLevelCapacity;
  u64 firstLevelMemSize;

  u64 elementsCapacity;
  u64 elementSize;
  u64 elementsMemSize;

  u64 remainingUnusedElements;
  u64 elementsCount;
  u64 collisionsCount;

  u64 datumSize;

  void* mallocPtr;
  void** firstLevelPtrs;
  void* unusedElements;
  u64 totalMallocSize;

  hash_set_hash* hashFunc = HashSetHashStub;
  hash_set_equals* equalsFunc = HashSetEqualsStub;

  HashSet(u64 datumSize_, hash_set_hash* hashFunc_, hash_set_equals* equalsFunc_, u64 firstLevelCapacity_ = 1024) {
    datumSize = datumSize_;
    hashFunc = hashFunc_;
    equalsFunc = equalsFunc_;

    firstLevelCapacity = firstLevelCapacity_;
    firstLevelMemSize = firstLevelCapacity * sizeof(void*);

    elementsCapacity = firstLevelCapacity * 2;
    elementSize = datumSize + sizeof(void*); // data + next ptr
    elementsMemSize = elementsCapacity * elementSize;

    remainingUnusedElements = elementsCapacity;
    elementsCount = 0;
    collisionsCount = 0;

    totalMallocSize = firstLevelMemSize + elementsMemSize;
    mallocPtr = malloc(totalMallocSize);
    memset(mallocPtr, 0, totalMallocSize);
    firstLevelPtrs = (void**)mallocPtr;
    unusedElements = firstLevelPtrs + firstLevelCapacity;
  }

  ~HashSet() {
    free(mallocPtr);
    mallocPtr = nullptr;
    firstLevelPtrs = nullptr;
    unusedElements = nullptr;
    totalMallocSize = 0;

    datumSize = 0;
    hashFunc = HashSetHashStub;
    equalsFunc = HashSetEqualsStub;

    firstLevelCapacity = 0;
    firstLevelMemSize = 0;

    elementsCapacity = 0;
    elementSize = 0;
    elementsMemSize = 0;

    remainingUnusedElements = 0;
    elementsCount = 0;
    collisionsCount = 0;
  }

  void* nextFreeElement() {
    if(remainingUnusedElements == 0) {
      // TODO: expand hashmap
      printf("Error: HashSet at capacity.\n");
      return nullptr;
    }

    void* nextFree = unusedElements;
    unusedElements = (char*)unusedElements + elementSize;
    remainingUnusedElements--;
    return nextFree;
  }

  bool contains(void* queryItemPtr) {
    u64 hash = hashFunc(queryItemPtr);
    u64 arrayIndex = hash % firstLevelCapacity;

    void** foundItemPtr = firstLevelPtrs + arrayIndex;
    while(*foundItemPtr != nullptr) {
      void* foundItem = *foundItemPtr;
      if(equalsFunc(queryItemPtr, foundItem)) {
        return true;
      }
      void** nextPtr = (void**)((char*)foundItem + datumSize);
      foundItemPtr = nextPtr;
    }
    return false;
  }

  void insert(void* insertItemPtr) {
    u64 hash = hashFunc(insertItemPtr);
    u64 arrayIndex = hash % firstLevelCapacity;

    bool wasCollision = false;
    void** foundItemPtr = firstLevelPtrs + arrayIndex;
    while(*foundItemPtr != nullptr) {
      wasCollision = true;
      void* founditem = *foundItemPtr;
      if(equalsFunc(insertItemPtr, founditem)) {
        printf("Attempting to insert same item twice.\n");
        return;
      }
      void** nextPtr = (void**)((char*)founditem + datumSize);
      foundItemPtr = nextPtr;
    }

    void* newItem = nextFreeElement();
    memcpy(newItem, insertItemPtr, datumSize);
    *foundItemPtr = newItem;
    elementsCount++;

    if(wasCollision) {
      collisionsCount++;
    }
  }
};