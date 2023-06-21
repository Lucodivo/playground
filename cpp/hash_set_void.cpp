//
// Created by Connor on 3/8/2022.
// Generic hash set by just throwing around void pointers
//

// TODO: Should first level hold actual keys? or continue to hold pointers to elements?
// TODO: Calculate collisions only when asked. No keeping track of them.
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

  const class_access u64 keyOffset = 0;
  u64 nextElementOffset;

  void* mallocPtr;
  void** firstElementsPtrArray;
  void* unusedElementsArray;
  void* recyclingElementsList;
  u64 totalMallocSize;

  hash_func_hash* hashFunc = HashFuncHashStub;
  hash_func_equals* equalsFunc = HashFuncEqualsStub;

  HashSetVoid(u64 keySize_, hash_func_hash* hashFunc_, hash_func_equals* equalsFunc_, u64 capacity = 1024) {
    if(capacity < 2) { // capacity is now allowed to be less than 2
      capacity = 2;
    }

    keySize = keySize_;
    hashFunc = hashFunc_;
    equalsFunc = equalsFunc_;

    // pad the elements
    u64 minAlignment = 8;
    nextElementOffset = ((keySize + (minAlignment - 1)) / minAlignment) * minAlignment;

    firstLevelCapacity = capacity / 2;
    firstLevelMemSize = firstLevelCapacity * sizeof(void**);

    elementsCapacity = capacity;
    elementSize = keySize + sizeof(void*); // key + next ptr
    elementsMemSize = elementsCapacity * elementSize;

    unusedElementsCount = elementsCapacity;
    recyclingElementsCount = 0;
    elementsCount = 0;
    collisionsCount = 0;

    totalMallocSize = firstLevelMemSize + elementsMemSize;
    mallocPtr = malloc(totalMallocSize);
    memset(mallocPtr, 0, totalMallocSize);
    firstElementsPtrArray = (void**)mallocPtr;
    unusedElementsArray = firstElementsPtrArray + firstLevelCapacity;
    recyclingElementsList = nullptr;
  }

  ~HashSetVoid() {
    free(mallocPtr);
  }

  void* parseElementPtr_key(void* elementPtr) const {
    return elementPtr; // + keyOffset (which is zero)
  }

  void** parseElementPtr_next(void* elementPtr) const {
    return (void**)((char*)elementPtr + nextElementOffset);
  }

  void clear() {
    unusedElementsCount = elementsCapacity;
    recyclingElementsCount = 0;
    elementsCount = 0;
    collisionsCount = 0;

    memset(mallocPtr, 0, totalMallocSize);
    firstElementsPtrArray = (void**)mallocPtr;
    unusedElementsArray = firstElementsPtrArray + firstLevelCapacity;
    recyclingElementsList = nullptr;
  }

  void* nextFreeElement() {

    void* nextFree;

    // check recycled elements first
    if(recyclingElementsList != nullptr) {
      nextFree = recyclingElementsList;
      recyclingElementsList = *parseElementPtr_next(nextFree);
      --recyclingElementsCount;
    } else if(unusedElementsCount > 0){
      nextFree = unusedElementsArray;
      unusedElementsArray = (char*)unusedElementsArray + elementSize;
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

    void** foundElementPtr = firstElementsPtrArray + arrayIndex;
    while(*foundElementPtr != nullptr) {
      if(equalsFunc(key, parseElementPtr_key(*foundElementPtr))) {
        return true;
      }
      foundElementPtr = parseElementPtr_next(*foundElementPtr);
    }
    return false;
  }

  void resize(u64 newCapacity){
    if(newCapacity < 2) { // capacity is not allowed to be less than 2
      newCapacity = 2;
    }

    // keep old member variables accessible
    u64 old_firstLevelCapacity = firstLevelCapacity;
    void* old_mallocPtr = mallocPtr;
    void** old_firstElementsPtrArray = firstElementsPtrArray;

    // update new member variables
    firstLevelCapacity = newCapacity / 2;
    firstLevelMemSize = firstLevelCapacity * sizeof(void*);

    elementsCapacity = newCapacity;
    elementsMemSize = elementsCapacity * elementSize;

    unusedElementsCount = elementsCapacity;
    recyclingElementsCount = 0;
    elementsCount = 0;
    collisionsCount = 0;

    totalMallocSize = firstLevelMemSize + elementsMemSize;
    mallocPtr = malloc(totalMallocSize);
    memset(mallocPtr, 0, totalMallocSize);
    firstElementsPtrArray = (void**)mallocPtr;
    unusedElementsArray = (void*)((char*)mallocPtr + firstLevelMemSize);
    recyclingElementsList = nullptr;

    // traverse the old data and use insert to put it into the new array
    void** firstElementPtrsIterator = old_firstElementsPtrArray;
    for(u64 i = 0; i < old_firstLevelCapacity; ++i) {

      void* elementsIterator = *firstElementPtrsIterator;
      while(elementsIterator != nullptr) {
        if(elementsCount == elementsCapacity) {
          // If the hash map was resized to be smaller, we don't want to insert more elements than the current capacity.
          break;
        }

        insert(parseElementPtr_key(elementsIterator));
        elementsIterator = *parseElementPtr_next(elementsIterator);
      }

      ++firstElementPtrsIterator;
    }

    free(old_mallocPtr);
  }

  void insert(void* key) {

    u64 hash = hashFunc(key);
    u64 arrayIndex = hash % firstLevelCapacity;

    bool wasCollision = false;
    void** foundElementPtr = firstElementsPtrArray + arrayIndex;
    while(*foundElementPtr != nullptr) {
      wasCollision = true;
      if(equalsFunc(key, parseElementPtr_key(*foundElementPtr))) {
        // NOTE: Key inserted into set more than once. Do nothing.
        return;
      }
      foundElementPtr = parseElementPtr_next(*foundElementPtr);
    }

    // if we need to add a new element and we are out of space, we need to resize the hash set
    if(elementsCount == elementsCapacity) {
      resize(elementsCapacity * 2);
      insert(key);
      return;
    }

    void* newKey = nextFreeElement();
    memcpy(newKey, key, keySize);
    *foundElementPtr = newKey;
    ++elementsCount;

    if(wasCollision) {
      ++collisionsCount;
    }
  }

  bool remove(void* key) {
    u64 hash = hashFunc(key);
    u64 arrayIndex = hash % firstLevelCapacity;

    void** firstLevelElementPtr = firstElementsPtrArray + arrayIndex;
    void** prevNextElementPtr = nullptr;
    void** foundElementPtr = firstLevelElementPtr;
    while(*foundElementPtr != nullptr) {

      void** foundNextPtr = parseElementPtr_next(*foundElementPtr);
      if(equalsFunc(key, parseElementPtr_key(*foundElementPtr))) {
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
        *foundNextPtr = recyclingElementsList;
        recyclingElementsList = *foundElementPtr;
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