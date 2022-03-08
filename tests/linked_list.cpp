//
// Created by Connor on 3/4/2022.
//

struct noop_node_s {
  u32 data;
  noop_node_s* next;
};

struct noop_node_d {
  u32 data;
  noop_node_d* next;
  noop_node_d* prev;
};

struct singly_linked_list {
  noop_node_s* head;
  noop_node_s* tail;
  noop_node_s* recycledNodes;
  noop_node_s* nodePool;
  void* mallocPtr;
  u32 capacity;
  u32 count;
};

void printList(const singly_linked_list& list) {
  printf("singly linked list: \n");
  if(list.head == nullptr) {
    printf("-> empty\n");
  } else {
    noop_node_s* iter = list.head;
    while(iter != nullptr) {
      printf("-> %u\n", iter->data);
      iter = iter->next;
    }
  }
}

void printRecycled(const singly_linked_list& list) {
  printf("singly linked recycled: \n");
  if(list.recycledNodes == nullptr) {
    printf("-> empty\n");
  } else {
    noop_node_s* iter = list.recycledNodes;
    while(iter != nullptr) {
      printf("-> %u\n", iter->data);
      iter = iter->next;
    }
  }
}

void doubleCapacity(singly_linked_list& list) {
  singly_linked_list newList;
  newList.count = list.count;
  newList.capacity = list.capacity * 2;
  newList.mallocPtr = malloc(newList.capacity * sizeof(noop_node_s));

  // == memcpy whole list ==
  u32 copySize = (char*)list.nodePool - (char*)list.mallocPtr;
  memcpy(newList.mallocPtr, list.mallocPtr, copySize);

  // == fix pointers ==
  u32 addrBaseDiff = (noop_node_s*)newList.mallocPtr - (noop_node_s*)list.mallocPtr;
  newList.head = list.head == nullptr ? nullptr : list.head + addrBaseDiff;
  newList.tail = list.tail == nullptr ? nullptr : list.tail + addrBaseDiff;
  newList.recycledNodes = list.recycledNodes == nullptr ? nullptr : list.recycledNodes + addrBaseDiff;
  newList.nodePool = list.nodePool == nullptr ? nullptr : list.nodePool + addrBaseDiff;

  // fix list pointers
  noop_node_s* iter = newList.head;
  while(iter != nullptr) {
    iter->next = iter->next == nullptr ? nullptr : iter->next + addrBaseDiff;
    iter = iter->next;
  }

  // fix recycled counters
  iter = newList.recycledNodes;
  while(iter != nullptr) {
    iter->next = iter->next == nullptr ? nullptr : iter->next + addrBaseDiff;
    iter = iter->next;
  }

  // == free ==
  free(list.mallocPtr);
  list = newList;
}

void destroy(singly_linked_list& list) {
  free(list.mallocPtr);
  list.head = nullptr;
  list.tail = nullptr;
  list.recycledNodes = nullptr;
  list.nodePool = nullptr;
  list.capacity = 0;
  list.count = 0;
}

singly_linked_list SinglyLinkedList(u32 capacity = 32) {
  singly_linked_list result{};
  result.head = nullptr;
  result.tail = nullptr;
  result.recycledNodes = nullptr;
  result.mallocPtr = malloc(sizeof(noop_node_s) * capacity);
  result.nodePool = (noop_node_s*)result.mallocPtr;
  result.capacity = capacity;
  result.count = 0;
  return result;
}

void addBack(singly_linked_list& list, u32 data) {
  if(list.count == list.capacity) {
    doubleCapacity(list);
  }

  noop_node_s* newNode;
  if(list.recycledNodes != nullptr) {
    newNode = list.recycledNodes;
    list.recycledNodes = list.recycledNodes->next;
  } else {
    newNode = list.nodePool;
    list.nodePool++;
  }

  newNode->data = data;
  newNode->next = nullptr;

  if(list.count == 0) {
    list.head = newNode;
    list.tail = newNode;
  } else {
    list.tail->next = newNode;
    list.tail = newNode;
  }

  list.count++;
}

void addFront(singly_linked_list& list, u32 data) {
  if(list.count == list.capacity) {
    doubleCapacity(list);
  }

  noop_node_s* newNode;
  if(list.recycledNodes != nullptr) {
    newNode = list.recycledNodes;
    list.recycledNodes = list.recycledNodes->next;
  } else {
    newNode = list.nodePool;
    list.nodePool++;
  }

  newNode->data = data;
  newNode->next = list.head;
  list.head = newNode;

  if(list.count == 0) {
    list.tail = list.head;
  }

  list.count++;
}

bool remove(singly_linked_list& list, u32 data) {

  noop_node_s* iter = list.head;
  noop_node_s* prevIter = nullptr;
  while(iter != nullptr) {
    if(iter->data == data) {
      if(prevIter == nullptr) { // if node is head
        list.head = iter->next;
      } else {
        prevIter->next = iter->next;
      }

      if(iter == list.tail) { // if node was tail
        list.tail = prevIter;
      }

      // add removed node to recycle list
      iter->next = list.recycledNodes;
      list.recycledNodes = iter;

      list.count--;
      return true;
    }
    prevIter = iter;
    iter = iter->next;
  }

  return false;
}

bool contains(singly_linked_list& list, u32 data) {
  noop_node_s* iter = list.head;
  while(iter != nullptr) {
    if(iter->data == data) {
      return true;
    }
    iter = iter->next;
  }
  return false;
}