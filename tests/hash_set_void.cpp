//
// Created by Connor on 3/8/2022.
// Generic hash set by just throwing around void pointers
//

// TODO: clear() function?
// TODO: Should first level hold actual keys? or continue to hold pointers to elements?
// TODO: malloc more elements when capacity is met
// TODO: Inserting at the front of a slot is faster, if we don't care about duplicate values
struct HashSetVoid {
  u64 firstLevelCapacity;
  u64 firstLevelMemSize;

  u64 elementsCapacity;
  u64 elementSize;
  u64 elementsMemSize;

  u64 unusedElementsCount;
  u64 recyclingElementsCount;
  u64 elementsCount;
  u64 collisionsCount; // current collisions, not total collisions ever

  u64 keySize;

  void* mallocPtr;
  void** firstLevelPtrs;
  void* unusedElements;
  void* recyclingElements;
  u64 totalMallocSize;

  hash_func_hash* hashFunc = HashFuncHashStub;
  hash_func_equals* equalsFunc = HashFuncEqualsStub;

  HashSetVoid(u64 keySize_, hash_func_hash* hashFunc_, hash_func_equals* equalsFunc_, u64 firstLevelCapacity_ = 1024) {
    keySize = keySize_;
    hashFunc = hashFunc_;
    equalsFunc = equalsFunc_;

    firstLevelCapacity = firstLevelCapacity_;
    firstLevelMemSize = firstLevelCapacity * sizeof(void**);

    elementsCapacity = firstLevelCapacity * 2;
    elementSize = keySize + sizeof(void*); // key + next ptr
    elementsMemSize = elementsCapacity * elementSize;

    unusedElementsCount = elementsCapacity;
    recyclingElementsCount = 0;
    elementsCount = 0;
    collisionsCount = 0;

    totalMallocSize = firstLevelMemSize + elementsMemSize;
    mallocPtr = malloc(totalMallocSize);
    memset(mallocPtr, 0, totalMallocSize);
    firstLevelPtrs = (void**)mallocPtr;
    unusedElements = firstLevelPtrs + firstLevelCapacity;
    recyclingElements = nullptr;
  }

  ~HashSetVoid() {
    free(mallocPtr);
  }

  void parseVoidPtrData(void* elementPtr, void*& outKey, void**& outNext){
    outKey = elementPtr;
    outNext = (void**)((char*)elementPtr + keySize);
  }

  void* nextFreeElement() {

    void* nextFree;

    // check recycled elements first
    if(recyclingElements != nullptr) {
      void* key;
      void** nextElementPtr;
      parseVoidPtrData(recyclingElements, key, nextElementPtr);
      nextFree = key;
      recyclingElements = *nextElementPtr;
      --recyclingElementsCount;
    } else if(unusedElementsCount > 0){
      nextFree = unusedElements;
      unusedElements = (char*)unusedElements + elementSize;
      unusedElementsCount--;
    } else {
      printf("Error: HashSetVoid at capacity.\n");
      nextFree = nullptr;
    }

    return nextFree;
  }

  bool contains(void* key) {
    u64 hash = hashFunc(key);
    u64 arrayIndex = hash % firstLevelCapacity;

    void** foundElementPtr = firstLevelPtrs + arrayIndex;
    while(*foundElementPtr != nullptr) {
      void* foundKey;
      void** foundNextPtr;
      parseVoidPtrData(*foundElementPtr, foundKey, foundNextPtr);
      if(equalsFunc(key, foundKey)) {
        return true;
      }
      foundElementPtr = foundNextPtr;
    }
    return false;
  }

  void insert(void* key) {
    u64 hash = hashFunc(key);
    u64 arrayIndex = hash % firstLevelCapacity;

    bool wasCollision = false;
    void** foundKeyPtr = firstLevelPtrs + arrayIndex;
    while(*foundKeyPtr != nullptr) {
      wasCollision = true;
      void* foundKey;
      void** foundNextPtr;
      parseVoidPtrData(*foundKeyPtr, foundKey, foundNextPtr);
      if(equalsFunc(key, foundKey)) {
        printf("Attempting to insert same item twice.\n");
        return;
      }
      foundKeyPtr = foundNextPtr;
    }

    void* newKey = nextFreeElement();
    memcpy(newKey, key, keySize);
    *foundKeyPtr = newKey;
    ++elementsCount;

    if(wasCollision) {
      ++collisionsCount;
    }
  }

  bool remove(void* key) {
    u64 hash = hashFunc(key);
    u64 arrayIndex = hash % firstLevelCapacity;

    void** firstLevelElementPtr = firstLevelPtrs + arrayIndex;
    void** prevNextElementPtr = nullptr;
    void** foundElementPtr = firstLevelElementPtr;
    while(*foundElementPtr != nullptr) {
      void* foundKey;
      void** foundNextPtr;
      parseVoidPtrData(*foundElementPtr, foundKey, foundNextPtr);

      if(equalsFunc(key, foundKey)) {
        bool removedCollision = false;
        if(foundElementPtr == firstLevelElementPtr) { // first element
          *firstLevelElementPtr = *foundNextPtr;

          // if the next ptr wasn't null, we're removing a collision
          if(*foundNextPtr != nullptr) {
            removedCollision = true;
          }
        } else { // not the first element
          *prevNextElementPtr = *foundNextPtr;
          removedCollision = true;
        }

        // add removed element to recycling
        *foundNextPtr = recyclingElements;
        recyclingElements = *foundElementPtr;
        if(removedCollision) { --collisionsCount; }
        --elementsCount;
        ++recyclingElementsCount;
        return true;
      }

      prevNextElementPtr = foundNextPtr;
      foundElementPtr = foundNextPtr;
    }

    return false;
  }
};