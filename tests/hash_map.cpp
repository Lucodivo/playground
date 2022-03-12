//
// Created by Connor on 3/9/2022.
// Generic hash map by just throwing pointers around
// HashMap holds shallow copies (literally just memcpy) of data inserted
//

struct HashMap {
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
  u64 datumSize;

  const class_access u64 keyOffset = 0;
  u64 datumOffset;
  u64 nextElementOffset;

  void* mallocPtr;
  void** firstLevelPtrs; // TODO: Consider changing the first level array from holding a pointer to holding an actual element
  void* unusedElements;
  void* recyclingElements;
  u64 totalMallocSize;

  hash_func_hash* hashFunc = HashFuncHashStub;
  hash_func_equals* equalsFunc = HashFuncEqualsStub;

  HashMap(u64 keySize_, u64 datumSize_, hash_func_hash* hashFunc_, hash_func_equals* equalsFunc_, u64 firstLevelCapacity_ = 1024) {
    keySize = keySize_;
    datumSize = datumSize_;
    hashFunc = hashFunc_;
    equalsFunc = equalsFunc_;

    firstLevelCapacity = firstLevelCapacity_;
    firstLevelMemSize = firstLevelCapacity * sizeof(void*);

    // pad the elements
    u64 minAlignment = 8;
    datumOffset = ((keySize + (minAlignment - 1)) / minAlignment) * minAlignment;
    nextElementOffset = datumOffset + (((datumSize + (minAlignment - 1)) / minAlignment) * minAlignment);

    elementsCapacity = firstLevelCapacity * 2;
    elementSize = nextElementOffset + sizeof(void**);
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

  ~HashMap() {
    free(mallocPtr);
    mallocPtr = nullptr;
    firstLevelPtrs = nullptr;
    unusedElements = nullptr;
    recyclingElements = nullptr;
    totalMallocSize = 0;

    keySize = 0;
    datumSize = 0;
    hashFunc = HashFuncHashStub;
    equalsFunc = HashFuncEqualsStub;

    firstLevelCapacity = 0;
    firstLevelMemSize = 0;

    elementsCapacity = 0;
    elementSize = 0;
    elementsMemSize = 0;

    unusedElementsCount = 0;
    recyclingElementsCount = 0;
    elementsCount = 0;
    collisionsCount = 0;
  }

  void* parseElementPtr_key(void* elementPtr) const {
    return elementPtr; // + keyOffset (which is zero)
  }

  void* parseElementPtr_datum(void* elementPtr) const {
    return ((char*)elementPtr + datumOffset);
  }

  void** parseElementPtr_next(void* elementPtr) const {
    return (void**)((char*)elementPtr + nextElementOffset);
  }

  // Guarantees that the element returned has a next ptr set to nullptr
  // Key and datum in element have no such guarantees
  void* nextFreeElement() {

    void* nextFree;

    // check recycled elements first
    if(recyclingElements != nullptr) {
      void** nextElementPtr = parseElementPtr_next(recyclingElements);
      nextFree = recyclingElements;
      recyclingElements = *nextElementPtr;
      *nextElementPtr = nullptr; // clean the pointer before returning
      --recyclingElementsCount;
    } else if(unusedElementsCount > 0){
      nextFree = unusedElements;
      unusedElements = (char*)unusedElements + elementSize;
      unusedElementsCount--;
    } else {
      // TODO: malloc more elements
      printf("Error: HashMap at capacity.\n");
      nextFree = nullptr;
    }

    return nextFree;
  }

  bool contains(void* key) const {
    u64 hash = hashFunc(key);
    u64 arrayIndex = hash % firstLevelCapacity;

    void** foundElementPtr = firstLevelPtrs + arrayIndex;
    while(*foundElementPtr != nullptr) {
      void* foundKey = parseElementPtr_key(*foundElementPtr);
      if(equalsFunc(key, foundKey)) {
        return true;
      }
      foundElementPtr = parseElementPtr_next(*foundElementPtr);
    }
    return false;
  }

  void writeElement(void* element, void* key, void* datum, void* next) {
    void* newKey = parseElementPtr_key(element);
    void* newDatum = parseElementPtr_datum(element);
    void** newNextElementPtr = parseElementPtr_next(element);
    memcpy(newKey, key, keySize);
    memcpy(newDatum, datum, datumSize);
    *newNextElementPtr = next;
  }

  // TODO: Can just insert at the front of the list if we don't care about duplicate values
  void insert(void* key, void* datum) {
    u64 hash = hashFunc(key);
    u64 arrayIndex = hash % firstLevelCapacity;

    bool wasCollision = false;
    void** foundElementPtr = firstLevelPtrs + arrayIndex;
    while(*foundElementPtr != nullptr) {
      wasCollision = true;
      void* foundKey = parseElementPtr_key(*foundElementPtr);
      if(equalsFunc(key, foundKey)) {
        printf("Attempting to insert same item twice.\n");
        return;
      }
      foundElementPtr = parseElementPtr_next(*foundElementPtr);;
    }

    void* newElement = nextFreeElement();
    writeElement(newElement, key, datum, nullptr);
    *foundElementPtr = newElement;
    ++elementsCount;

    if(wasCollision) {
      ++collisionsCount;
    }
  }

  void* retrieve(void* key) const {
    u64 hash = hashFunc(key);
    u64 arrayIndex = hash % firstLevelCapacity;

    void** foundElementPtr = firstLevelPtrs + arrayIndex;
    while(*foundElementPtr != nullptr) {
      void* foundKey = parseElementPtr_key(*foundElementPtr);

      if(equalsFunc(key, foundKey)) {
        void* foundDatum = parseElementPtr_datum(*foundElementPtr);
        return foundDatum;
      }

      foundElementPtr = parseElementPtr_next(*foundElementPtr);
    }

    return nullptr;
  }

  bool remove(void* key) {
    u64 hash = hashFunc(key);
    u64 arrayIndex = hash % firstLevelCapacity;

    void** firstLevelElementPtr = firstLevelPtrs + arrayIndex;
    void** prevNextElementPtr = nullptr;
    void** foundElementPtr = firstLevelElementPtr;
    while(*foundElementPtr != nullptr) {
      void* foundKey = parseElementPtr_key(*foundElementPtr);
      void** foundNextPtr = parseElementPtr_next(*foundElementPtr);

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