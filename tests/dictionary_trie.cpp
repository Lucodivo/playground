//
// Created by Connor on 3/4/2022.
//

struct linked_trie_dictionary_node {
  char character;
  linked_trie_dictionary_node* firstChild;
  linked_trie_dictionary_node* nextSibling;
};

struct linked_trie_dictionary_allocator {
  linked_trie_dictionary_node* freeNodes;
  u32 remainingNodes;
  std::vector<void*> mallocPtrs;
  u32 nodeCountPerMalloc;
  u64 totalMemoryAllocated;
};

struct linked_trie_dictionary {
  linked_trie_dictionary_node root;
  linked_trie_dictionary_allocator allocator;
};

linked_trie_dictionary_node* nextFree(linked_trie_dictionary_allocator& allocator) {
  if(allocator.remainingNodes == 0) {
    const u32 newMallocSize = sizeof(linked_trie_dictionary_node) * allocator.nodeCountPerMalloc;
    void* newMallocPtr = malloc(newMallocSize);
    allocator.mallocPtrs.push_back(newMallocPtr);
    allocator.freeNodes = (linked_trie_dictionary_node*)newMallocPtr;
    allocator.remainingNodes = allocator.nodeCountPerMalloc;
    allocator.totalMemoryAllocated += newMallocSize;
  }

  allocator.remainingNodes--;
  return allocator.freeNodes++;
}

void freeDictionary(linked_trie_dictionary& dict) {
  dict.root.character = '*';
  dict.root.firstChild = nullptr;
  dict.root.nextSibling = nullptr;
  for(void* mallocPtr : dict.allocator.mallocPtrs) {
    free(mallocPtr);
  }
  dict.allocator = {};
}

bool contains(const linked_trie_dictionary& dict, const std::string& word) {
  if(dict.root.character != '*' || dict.root.nextSibling != nullptr) {
    printf("Dictionary root is borked.");
    return false;
  }

  const linked_trie_dictionary_node* parent = &dict.root;
  u32 wordIndex = 0;
  u32 wordCharCount = word.size();
  while(parent != nullptr && wordIndex < wordCharCount) {
    char character = word[wordIndex++];

    if(parent->firstChild == nullptr) {
      printf("A non-null termination node should not have null child. It should either contain the next letters or a null termination node.");
      return false;
    }

    linked_trie_dictionary_node* child = parent->firstChild;
    while(child != nullptr) {
      if(child->character == character) {
        parent = child;
        break;
      }
      child = child->nextSibling;
    }
    if(parent != child) { // if character not found
      return false;
    }
  }

  // at this point all characters have been found and just have to check for a termination character
  linked_trie_dictionary_node* child = parent->firstChild;
  while(child != nullptr) {
    if(child->character == '.') {
      return true;
    }
    child = child->nextSibling;
  }

  return false;
}

void buildDictionary(const std::vector<char>& fileCharacters, linked_trie_dictionary& outDict) {
  // initialize allocator
  outDict.allocator = {};
  outDict.allocator.mallocPtrs.reserve(100);
  const u64 initialNodeCountEstimate = fileCharacters.size() / 4; // The average word length in the English dictionary is 4.7
  const u64 initialMemoryAllocated = initialNodeCountEstimate * sizeof(linked_trie_dictionary_node);
  void* initialMallocPtr = malloc(initialMemoryAllocated);
  outDict.allocator.mallocPtrs.push_back(initialMallocPtr);
  outDict.allocator.freeNodes = (linked_trie_dictionary_node*)initialMallocPtr;
  outDict.allocator.remainingNodes = initialNodeCountEstimate;
  outDict.allocator.totalMemoryAllocated = initialMemoryAllocated;
  outDict.allocator.nodeCountPerMalloc = initialNodeCountEstimate / 50; // increase by 2% each malloc

  // initialize root
  outDict.root.character = '*';
  outDict.root.nextSibling = nullptr;
  outDict.root.firstChild = nullptr;

  u32 fileCharactersCount = fileCharacters.size();
  for(u32 fileCharacterIndex = 0; fileCharacterIndex < fileCharactersCount; fileCharacterIndex++) {
    char fileCharacter = fileCharacters[fileCharacterIndex];

    // TODO: handle uppercase
    if((fileCharacter < 'a' || fileCharacter > 'z') && fileCharacter != '-') {
      continue;
    }

    linked_trie_dictionary_node* parent = &outDict.root;
    while(fileCharacterIndex < fileCharactersCount) {
      fileCharacter = fileCharacters[fileCharacterIndex];
      if((fileCharacter < 'a' || fileCharacter > 'z') && fileCharacter != '-') {
        break;
      }

      if(parent->firstChild == nullptr) {
        parent->firstChild = nextFree(outDict.allocator);
        parent->firstChild->character = fileCharacter;
        parent->firstChild->nextSibling = nullptr;
        parent->firstChild->firstChild = nullptr;
        parent = parent->firstChild;
      } else {
        linked_trie_dictionary_node* child = parent->firstChild;
        while(child != nullptr) {
          if(child->character == fileCharacter) {
            parent = child;
            break;
          }
          child = child->nextSibling;
        }
        if(parent != child) { // if character wasn't found
          child = nextFree(outDict.allocator);
          child->character = fileCharacter;
          child->nextSibling = parent->firstChild;
          parent->firstChild = child;
          child->firstChild = nullptr;
          parent = child;
        }
      }
    }

    // NOTE: word is completed by placing '.' as a child
    // NOTE: Should I assume that this is an error?
    bool isWordAlready = false;
    linked_trie_dictionary_node* child = parent->firstChild;
    while(child != nullptr) {
      if(child->character == '.') {
        isWordAlready = true;
        break;
      }
      child = child->nextSibling;
    }

    if(!isWordAlready) {
      linked_trie_dictionary_node* terminationNode = nextFree(outDict.allocator);
      terminationNode->character = '.';
      terminationNode->nextSibling = parent->firstChild;
      parent->firstChild = terminationNode;
      terminationNode->firstChild = nullptr;
    }
  }
}

// ==== TRIE USING ARRAY OF 27 POINTERS
const u32 supportedLetterCount = 27; // a-z, -

struct trie_dictionary_node {
  bool endOfWord;
  trie_dictionary_node* children[supportedLetterCount];
};

struct trie_dictionary_allocator {
  trie_dictionary_node* freeNodes;
  u64 remainingNodes;
  std::vector<void*> mallocPtrs;
  u32 nodeCountPerMalloc;
  u64 totalMemoryAllocated;
};

struct trie_dictionary {
  trie_dictionary_node root;
  trie_dictionary_allocator allocator;
};

trie_dictionary_node* nextFree(trie_dictionary_allocator& allocator) {
  if(allocator.remainingNodes == 0) {
    const u32 newMallocSize = sizeof(trie_dictionary_node) * allocator.nodeCountPerMalloc;
    void* newMallocPtr = malloc(newMallocSize);
    memset(newMallocPtr, 0, newMallocSize);
    allocator.mallocPtrs.push_back(newMallocPtr);
    allocator.freeNodes = (trie_dictionary_node*)newMallocPtr;
    allocator.remainingNodes = allocator.nodeCountPerMalloc;
    allocator.totalMemoryAllocated += newMallocSize;
  }

  allocator.remainingNodes--;
  return allocator.freeNodes++;
}

void freeDictionary(trie_dictionary& dict) {
  for(u32 i = 0; i < supportedLetterCount; i++) {
    dict.root.children[i] = nullptr;
  }
  dict.root.endOfWord = false;

  for(void* mallocPtr : dict.allocator.mallocPtrs) {
    free(mallocPtr);
  }
  dict.allocator = {};
  dict.allocator.mallocPtrs.clear();
}

bool contains(const trie_dictionary& dict, const std::string& word) {
  const trie_dictionary_node* parent = &dict.root;
  for(const char& c : word) {
    u32 letterIndex;
    if(c >= 'a' && c <= 'z') {
      letterIndex = c - 'a';
    } else if(c == '-') {
      letterIndex = 26;
    } else {
      return false;
    }

    const trie_dictionary_node* child = parent->children[letterIndex];
    if(child == nullptr) {
      return false;
    }
    parent = child;
  }

  return parent->endOfWord;
}

void buildDictionary(const std::vector<char>& fileCharacters, trie_dictionary& outDict) {
  outDict.root = {};
  outDict.allocator = {};
  outDict.allocator.mallocPtrs.reserve(100);
  const u64 initialNodeCountEstimate = fileCharacters.size() / 4; // The average word length in the English dictionary is 4.7
  const u64 initialMemoryAllocated = initialNodeCountEstimate * sizeof(trie_dictionary_node);
  void* initialMallocPtr = malloc(initialMemoryAllocated);
  memset(initialMallocPtr, 0, initialMemoryAllocated);
  outDict.allocator.mallocPtrs.push_back(initialMallocPtr);
  outDict.allocator.freeNodes = (trie_dictionary_node*)initialMallocPtr;
  outDict.allocator.remainingNodes = initialNodeCountEstimate;
  outDict.allocator.totalMemoryAllocated = initialMemoryAllocated;
  outDict.allocator.nodeCountPerMalloc = initialNodeCountEstimate / 50; // increase by 2% each malloc

  u32 fileCharactersCount = fileCharacters.size();
  for(u32 fileCharacterIndex = 0; fileCharacterIndex < fileCharactersCount; fileCharacterIndex++) {
    char fileCharacter = fileCharacters[fileCharacterIndex];

    // TODO: handle uppercase
    if((fileCharacter < 'a' || fileCharacter > 'z') && fileCharacter != '-') {
      continue;
    }

    u32 wordHeadIndex = fileCharacterIndex;
    u32 wordTailIndex = wordHeadIndex;
    while(++fileCharacterIndex < fileCharactersCount) {
      fileCharacter = fileCharacters[fileCharacterIndex];
      if((fileCharacter < 'a' || fileCharacter > 'z') && fileCharacter != '-') {
        break;
      }
      wordTailIndex = fileCharacterIndex;
    }

    trie_dictionary_node* parent = &outDict.root;
    for(u32 i = wordHeadIndex; i <= wordTailIndex; i++) {
      char character = fileCharacters[i];

      s32 letterIndex = -1;
      if(character >= 'a' && character <= 'z') {
        letterIndex = character - 'a';
      } else if(character == '-') {
        letterIndex = 26;
      } else {
        printf("Error: Character outside the set {a-z,-}");
      }

      trie_dictionary_node* child = parent->children[letterIndex];
      if(child == nullptr) {
        child = nextFree(outDict.allocator);
        parent->children[letterIndex] = child;
      }
      parent = child;
    }
    parent->endOfWord = true;
  }
}

// ==== FOR ALLOCATOR PERFORMANCE TESTING =====
struct linked_trie_dictionary_no_allocator {
  linked_trie_dictionary_node root;
  u64 nodeCount;
};

void freeDictionary(linked_trie_dictionary_no_allocator& dict) {
  // TODO: no thanks
}

bool contains(const linked_trie_dictionary_no_allocator& dict, const std::string& word) {
  if(dict.root.character != '*' || dict.root.nextSibling != nullptr) {
    printf("Dictionary root is borked.");
    return false;
  }

  const linked_trie_dictionary_node* parent = &dict.root;
  u32 wordIndex = 0;
  u32 wordCharCount = word.size();
  while(parent != nullptr && wordIndex < wordCharCount) {
    char character = word[wordIndex++];

    if(parent->firstChild == nullptr) {
      printf("A non-null termination node should not have null child. It should either contain the next letters or a null termination node.");
      return false;
    }

    linked_trie_dictionary_node* child = parent->firstChild;
    while(child != nullptr) {
      if(child->character == character) {
        parent = child;
        break;
      }
      child = child->nextSibling;
    }
    if(parent != child) { // if character not found
      return false;
    }
  }

  // at this point all characters have been found and just have to check for a termination character
  linked_trie_dictionary_node* child = parent->firstChild;
  while(child != nullptr) {
    if(child->character == '.') {
      return true;
    }
    child = child->nextSibling;
  }

  return false;
}

void buildDictionary(const std::vector<char>& fileCharacters, linked_trie_dictionary_no_allocator& outDict) {
  // init dictionary to hold nothing
  outDict.root.character = '*';
  outDict.root.nextSibling = nullptr;
  outDict.root.firstChild = nullptr;
  outDict.nodeCount = 0;

  u32 fileCharactersCount = fileCharacters.size();
  for(u32 fileCharacterIndex = 0; fileCharacterIndex < fileCharactersCount; fileCharacterIndex++) {
    char fileCharacter = fileCharacters[fileCharacterIndex];

    // TODO: handle uppercase
    if((fileCharacter < 'a' || fileCharacter > 'z') && fileCharacter != '-') {
      continue;
    }

    u32 wordHeadIndex = fileCharacterIndex++;
    u32 wordTailIndex = wordHeadIndex;
    while(fileCharacterIndex < fileCharactersCount) {
      fileCharacter = fileCharacters[fileCharacterIndex];
      if((fileCharacter < 'a' || fileCharacter > 'z') && fileCharacter != '-') {
        break;
      }
      wordTailIndex = fileCharacterIndex++;
    }

    linked_trie_dictionary_node* parent = &outDict.root;
    for(u32 i = wordHeadIndex; i <= wordTailIndex; i++) {
      char character = fileCharacters[i];

      if(parent->firstChild == nullptr) {
        parent->firstChild = new linked_trie_dictionary_node;
        outDict.nodeCount++;
        parent->firstChild->character = character;
        parent->firstChild->nextSibling = nullptr;
        parent->firstChild->firstChild = nullptr;
        parent = parent->firstChild;
      } else {
        linked_trie_dictionary_node* child = parent->firstChild;
        while(child != nullptr) {
          if(child->character == character) {
            parent = child;
            break;
          }
          child = child->nextSibling;
        }
        if(parent != child) { // if character wasn't found
          child = new linked_trie_dictionary_node;
          outDict.nodeCount++;
          child->character = character;
          child->nextSibling = parent->firstChild;
          parent->firstChild = child;
          child->firstChild = nullptr;
          parent = child;
        }
      }
    }

    // NOTE: word is completed by placing '.' as a child
    bool isWordAlready = false;
    linked_trie_dictionary_node* child = parent->firstChild;
    while(child != nullptr) {
      if(child->character == '.') {
        isWordAlready = true;
        break;
      }
      child = child->nextSibling;
    }
    if(!isWordAlready) {
      linked_trie_dictionary_node* terminationNode = new linked_trie_dictionary_node;
      outDict.nodeCount++;
      terminationNode->character = '.';
      terminationNode->nextSibling = parent->firstChild;
      parent->firstChild = terminationNode;
      terminationNode->firstChild = nullptr;
    }
  }
}
