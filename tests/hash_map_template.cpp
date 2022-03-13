//
// Created by Connor on 3/12/2022.
//

// TODO: clear() function?
// TODO: malloc more elements when capacity is met
template<typename S /*key*/, typename T/*value*/>
struct HashMapTemplate {

  struct Element {
    S key;
    T value;
    Element* next;
  };

  struct FirstElement {
    u32 count;
    Element element;
  };

  u64 firstLevelCapacity;
  u64 additionalElementCapacity;
  u64 totalMallocSize;

  u64 elementCount;
  u64 collisionCount;
  u64 freeElementCount;
  u64 recyclingElementCount;

  FirstElement* firstLevel;
  Element* freeElements;
  Element* recyclingElements;
  void* mallocPtr;

  typedef u64 hash_func_hash(const S& key);
  typedef bool hash_func_equals(const S& key1, const S& key2);

  hash_func_hash* hashFunc;
  hash_func_equals* equalsFunc;

  HashMapTemplate(hash_func_hash* hash, hash_func_equals* equals, u64 firstLevelCapacity_) {
    hashFunc = hash;
    equalsFunc = equals;
    firstLevelCapacity = firstLevelCapacity_;
    additionalElementCapacity = firstLevelCapacity_;

    u64 firstLevelMallocSize = firstLevelCapacity * sizeof(FirstElement);
    u64 additionalElementsMallocSize = additionalElementCapacity * sizeof(Element);

    totalMallocSize = firstLevelMallocSize + additionalElementsMallocSize;
    mallocPtr = malloc(totalMallocSize);
    memset(mallocPtr, 0, totalMallocSize);
    firstLevel = (FirstElement*)mallocPtr;
    freeElements = (Element*)(firstLevel + firstLevelCapacity);
    recyclingElements = nullptr;

    elementCount = 0;
    collisionCount = 0;
    freeElementCount = additionalElementCapacity;
    recyclingElementCount = 0;
  }

  ~HashMapTemplate() {
    free(mallocPtr);
  }

  Element* nextFreeElement() {
    Element* freeElement;

    if(recyclingElementCount > 0) {
      freeElement = recyclingElements;
      recyclingElements = freeElement->next;
      freeElement->next = nullptr;
      --recyclingElementCount;
    } else {
      if(freeElementCount == 0) {
        // TODO: allocate more elements
        freeElement = nullptr;
      } else {
        freeElement = freeElements++;
        --freeElementCount;
      }
    }

    return freeElement;
  }

  // NOTE: Does not check for duplicate values being inserted
  void insert(const S& key, const T& value) {
    u64 hashIndex = hashFunc(key) % firstLevelCapacity;

    FirstElement& firstElement = firstLevel[hashIndex];
    if(firstElement.count == 0) { // write to first element
      firstElement.element.key = key;
      firstElement.element.value = value;
      firstElement.element.next = nullptr;
    } else {
      Element* element = nextFreeElement();
      element->key = key;
      element->value = value;
      element->next = firstElement.element.next;
      firstElement.element.next = element;
      ++collisionCount;
    }

    ++firstElement.count;
    ++elementCount;
  }

  // NOTE: Only removes the first found matching entry, leaves any duplicates that may remain in the hash map
  bool remove(const S& key) {
    u64 hashIndex = hashFunc(key) % firstLevelCapacity;

    FirstElement& firstElement = firstLevel[hashIndex];
    if(firstElement.count == 0) {
      return false;
    }

    Element* element = &firstElement.element;
    Element** prevElementsNextPtr = nullptr;
    bool elementPartOfCollision = false;
    while(element != nullptr) {
      if(equalsFunc(key, element->key)) {
        if(prevElementsNextPtr == nullptr) { // removing first element
          if(element->next == nullptr) { // and it's the only element
            // No work needed, just reduce counter
          } else {
            if(element->next != nullptr) { elementPartOfCollision = true; }
            Element* elementToRecycle = element->next;
            element->key = element->next->key;
            element->value = element->next->value;
            element->next = element->next->next;
            elementToRecycle->next = recyclingElements;
            recyclingElements = elementToRecycle;
            ++recyclingElementCount;
          }
        } else {
          // remove from linked list
          *prevElementsNextPtr = element->next;
          // add to recycling list
          element->next = recyclingElements;
          recyclingElements = element;
          ++recyclingElementCount;
        }
        if(elementPartOfCollision) { --collisionCount; }
        --firstElement.count;
        --elementCount;
        return true;
      }
      elementPartOfCollision = true;
      prevElementsNextPtr = &element->next;
      element = element->next;
    }

    return false;
  }

  bool contains(const S& key) {
    u64 hashIndex = hashFunc(key) % firstLevelCapacity;
    FirstElement& firstElement = firstLevel[hashIndex];

    if(firstElement.count == 0) { return false; }
    Element* element = &firstElement.element;
    while(element != nullptr) {
      if(equalsFunc(key, element->key)) {
        return true;
      }
      element = element->next;
    }
    return false;
  }

  bool retrieve(const S& key, T& outValue) {
    u64 hashIndex = hashFunc(key) % firstLevelCapacity;
    FirstElement& firstElement = firstLevel[hashIndex];

    if(firstElement.count == 0) { return false; }
    Element* element = &firstElement.element;
    while(element != nullptr) {
      if(equalsFunc(key, element->key)) {
        outValue = element->value;
        return true;
      }
      element = element->next;
    }
    return false;
  }
};