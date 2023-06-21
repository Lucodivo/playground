//
// Created by Connor on 3/11/2022.
//

#include "test.h"
#include "linked_list.cpp"

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