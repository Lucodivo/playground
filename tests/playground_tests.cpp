#include "test.h"

// TEST_F is for using "fixtures"
TEST(Playground, strncpys) {
  const char* butts = "butts";
  char copy[6] = {'?','?','?','?','?','?'};
  strncpy_s(copy, 6, butts, 5);
  ASSERT_EQ(strcmp(copy, butts), 0); // assert that it adds the null character
}

TEST(Playground, pointerArithmatic) {
  const u32 arrayCount = 50;
  const u32 halfArrayCount = arrayCount / 2;
  struct PtrArithTest {
    s64 datum3;
    u32 datum1;
    char datum2;
  };
  PtrArithTest data[arrayCount];
  PtrArithTest* halfData = data + halfArrayCount;

  u32 ptrDiff = halfData - data;
  u32 charPtrDiff = (char*)halfData - (char*)data;

  ASSERT_EQ(ptrDiff, halfArrayCount);
  ASSERT_EQ(charPtrDiff, halfArrayCount * sizeof(PtrArithTest));
}