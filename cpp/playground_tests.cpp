#include "test.h"

int romanToInt(std::string s) {
  size_t count = s.size();
  int value = 0;

  size_t iterator = count - 1;

  while(iterator >= 0 && s[iterator] == 'I') {
    value += 1;
    --iterator;
  }

  while(iterator >= 0 && s[iterator] == 'V') {
    value += 5;
    --iterator;
  }

  if(iterator >= 0 && s[iterator] == 'I') {
    value -= 1;
    --iterator;
  }

  while(iterator >= 0 && s[iterator] == 'X') {
    value += 10;
    --iterator;
  }

  if(iterator >= 0 && s[iterator] == 'I') {
    value -= 1;
    --iterator;
  }

  while(iterator >= 0 && s[iterator] == 'L') {
    value += 50;
    --iterator;
  }

  if(iterator >= 0 && s[iterator] == 'X') {
    value -= 10;
    --iterator;
  }

  while(iterator >= 0 && s[iterator] == 'C') {
    value += 100;
    --iterator;
  }

  if(iterator >= 0 && s[iterator] == 'X') {
    value -= 50;
    --iterator;
  }

  while(iterator >= 0 && s[iterator] == 'D') {
    value += 500;
    --iterator;
  }

  if(iterator >= 0 && s[iterator] == 'C') {
    value -= 100;
    --iterator;
  }

  while(iterator >= 0 && s[iterator] == 'M') {
    value += 1000;
    --iterator;
  }

  if(iterator >= 0 && s[iterator] == 'C') {
    value -= 100;
  }

  return value;
}

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

TEST(Playground, roman) {
  std::string s = "III";
  s32 value = romanToInt(s);
  ASSERT_EQ(value, 3);
}