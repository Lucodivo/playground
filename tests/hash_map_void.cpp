//
// Created by Connor on 3/9/2022.
// Generic hash map by just throwing pointers around
// HashMapVoid holds shallow copies (literally just memcpy) of data inserted
//

// TODO: Should first level hold a actual elements? or continue to just hold pointers to elements?
// TODO: Calculate collisions only when asked. No keeping track of them.
struct HashMapVoid {
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
  void** firstElementsPtrArray;
  void* unusedElementsArray;
  void* recyclingElementsList;
  u64 totalMallocSize;

  hash_func_hash* hashFunc = HashFuncHashStub;
  hash_func_equals* equalsFunc = HashFuncEqualsStub;

  HashMapVoid(u64 keySize_, u64 datumSize_, hash_func_hash* hashFunc_, hash_func_equals* equalsFunc_, u64 capacity = 1024) {
    if(capacity < 2) { // capacity is now allowed to be less than 2
      capacity = 2;
    }

    keySize = keySize_;
    datumSize = datumSize_;
    hashFunc = hashFunc_;
    equalsFunc = equalsFunc_;

    // pad the elements
    u64 minAlignment = 8;
    datumOffset = ((keySize + (minAlignment - 1)) / minAlignment) * minAlignment;
    nextElementOffset = datumOffset + (((datumSize + (minAlignment - 1)) / minAlignment) * minAlignment);

    firstLevelCapacity = capacity / 2;
    firstLevelMemSize = firstLevelCapacity * sizeof(void*);

    elementsCapacity = capacity;
    elementSize = nextElementOffset + sizeof(void**);
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
  }

  ~HashMapVoid() {
    free(mallocPtr);
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

  void clear() {
    unusedElementsCount = elementsCapacity;
    recyclingElementsCount = 0;
    elementsCount = 0;
    collisionsCount = 0;

    memset(mallocPtr, 0, totalMallocSize);
    firstElementsPtrArray = (void**)mallocPtr;
    unusedElementsArray = (void*)((char*)mallocPtr + firstLevelMemSize);
    recyclingElementsList = nullptr;
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

        insert(parseElementPtr_key(elementsIterator), parseElementPtr_datum(elementsIterator));
        elementsIterator = *parseElementPtr_next(elementsIterator);
      }

      ++firstElementPtrsIterator;
    }

    free(old_mallocPtr);
  }

  // Guarantees that the element returned has a next ptr set to nullptr
  // Key and datum in element have no such guarantees
  void* nextFreeElement() {

    void* nextFree = nullptr;

    // check recycled elements first
    if(recyclingElementsList != nullptr) {
      void** nextElementPtr = parseElementPtr_next(recyclingElementsList);
      nextFree = recyclingElementsList;
      recyclingElementsList = *nextElementPtr;
      *nextElementPtr = nullptr; // clean the pointer before returning
      --recyclingElementsCount;
    } else {
      nextFree = unusedElementsArray;
      unusedElementsArray = (char*)unusedElementsArray + elementSize;
      unusedElementsCount--;
    }

    return nextFree;
  }

  bool contains(void* key) const {
    u64 hash = hashFunc(key);
    u64 arrayIndex = hash % firstLevelCapacity;

    void** foundElementPtr = firstElementsPtrArray + arrayIndex;
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

  void insert(void* key, void* datum) {
    u64 hash = hashFunc(key);
    u64 arrayIndex = hash % firstLevelCapacity;
    void** firstElementPtr = firstElementsPtrArray + arrayIndex;

    // check for collisions, and for existing key in hashmap
    bool wasCollision = false;
    void** foundElementPtr = firstElementPtr;
    while(*foundElementPtr != nullptr) {
      wasCollision = true;
      if(equalsFunc(key, parseElementPtr_key(*foundElementPtr))) {
        memcpy(parseElementPtr_datum(*foundElementPtr), datum, datumSize);
        return;
      }
      foundElementPtr = parseElementPtr_next(*foundElementPtr);;
    }

    // if we need to add a new element and we are out of space, we need to resize the hash set
    if(elementsCount == elementsCapacity) {
      resize(elementsCapacity * 2);
      insert(key, datum);
      return;
    }

    void* newElement = nextFreeElement();
    writeElement(newElement, key, datum, *firstElementPtr);
    *firstElementPtr = newElement;
    ++elementsCount;

    if(wasCollision) {
      ++collisionsCount;
    }
  }

  // Returns a memcpy of stored data. In order to avoiding returning a pointer to stored data.
  bool retrieve(void* key, void* outDatum) const {
    u64 hash = hashFunc(key);
    u64 arrayIndex = hash % firstLevelCapacity;

    void** foundElementPtr = firstElementsPtrArray + arrayIndex;
    while(*foundElementPtr != nullptr) {
      void* foundKey = parseElementPtr_key(*foundElementPtr);

      if(equalsFunc(key, foundKey)) {
        void* foundDatum = parseElementPtr_datum(*foundElementPtr);
        memcpy(outDatum, foundDatum, datumSize);
        return true;
      }

      foundElementPtr = parseElementPtr_next(*foundElementPtr);
    }

    return false;
  }

  // TODO: ugly. redo.
  bool remove(void* key) {
    u64 hash = hashFunc(key);
    u64 arrayIndex = hash % firstLevelCapacity;

    void** firstLevelElementPtr = firstElementsPtrArray + arrayIndex;
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