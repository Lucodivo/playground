//
// Created by Connor on 3/23/2022.
//

#include "test.h"

#include <string>

/*
  Given an input string s and a pattern p, implement regular expression matching with support for '.' and '*' where:

    '.' Matches any single character.
    '*' Matches zero or more of the preceding element.

  The matching should cover the entire input string (not partial).
 */
class Solution {
public:

  // This function WILL NOT handle the ".*" case
  // Sending in a string with ".*" is undefined
  // if match found, returns one index after the last character in the found substring
  // if match not found, returns -1
  bool matchingSubstr(const std::string& s, size_t strBegin, size_t strEnd,
                      const std::string& p, size_t patBegin, size_t patEnd,
                      s32& outSubstrBegin, s32& outSubstrEnd) {

    outSubstrBegin = -1;
    outSubstrEnd = -1;

    s32 lastPatIndex = (s32)patEnd - 1;

    for(s32 startStrIndex = (s32)strBegin; startStrIndex < strEnd; ++startStrIndex) {
      s32 sIter = startStrIndex;
      for(s32 patIndex = (s32)patBegin; patIndex < patEnd; ++patIndex) {

        char patC = p[patIndex];
        bool zeroToMany = ((patIndex + 1) < patEnd) && p[patIndex + 1] == '*';

        if(zeroToMany) {
          ++patIndex;

          while(sIter < strEnd) {
            char c = s[sIter];
            if(c == patC) {
              ++sIter;
            } else {
              break;
            }
          }
        } else if(sIter >= strEnd) { // if string iteration has gone to far, no matching substring found
          return false;
        } else if(patC == '.') {
          // Keep going
          ++sIter;
        } else if(patC != s[sIter]) {
          break; // break this pattern match loop
        }

        if(patIndex == lastPatIndex) { // pattern has finished, end of matching substring is
          outSubstrBegin = startStrIndex;
          outSubstrEnd = sIter;
          return true;
        }
      }
    }

    return false;
  }

  // massage the pattern
  // a substring of "a*aaa" would preferably be "aaaa*", both say "three or more consecutive a's"
  // a substring of "aa*aaaa*aaa" would preferably be "aaaaaaaa*", "7 or more consecutive a's"
  // return result is the new length of the pattern
  s32 massagePattern(std::string& p) {
    size_t pLen = p.length();

    char repeatedChar;
    size_t moveBack = 0; // occurs when two '*' cancel out
    for(size_t i = 1; i < pLen; ++i) { // constraints mention that '*' will never be at index 0
      const char c = p[i];
      p[i - moveBack] = c;
      if(c == '*') {
        repeatedChar = p[i-1];
        size_t nextIndex = i + 1;
        while(nextIndex < pLen) {
          if (p[nextIndex] == '*') { // another star
            moveBack += 2;
          } else if(p[nextIndex] == repeatedChar) {
            p[nextIndex - moveBack - 1] = repeatedChar;
          } else {
            break;
          }
          ++nextIndex;
        }
        i = nextIndex - 1;
        p[i - moveBack] = '*';
      }
    }

    return pLen - moveBack;
  }

  bool isMatch(std::string s, std::string p) {
    u32 sLen = s.length();
    u32 pLen = massagePattern(p);

    s32 lastSubstrEnd = 0;
    size_t pBegin = 0;
    char prevPChar = '?';
    bool beginningMatch = true;
    for(u32 pIndex = 0; pIndex < pLen; ++pIndex) {
      const char pChar = p[pIndex];

      if(pChar == '*' && prevPChar == '.') { // special wild card case
        s32 substrBegin, substrEnd;
        bool matchFound = matchingSubstr(s, lastSubstrEnd, sLen,
                                         p, pBegin, pIndex - 1,
                                         substrBegin, substrEnd);
        if(!matchFound) {
          return false;
        }

        if(beginningMatch) {
          if(substrBegin != 0) {
            return false;
          }
          beginningMatch = false;
        }
        pBegin = pIndex + 1;
        lastSubstrEnd = substrEnd;
      }

      prevPChar = pChar;
    }

    s32 substrBegin, substrEnd;
    bool matchFound = matchingSubstr(s, lastSubstrEnd, sLen,
                                     p, pBegin, pLen,
                                     substrBegin, substrEnd);
    if(!matchFound) {
      return false;
    }

    if(beginningMatch) {
      if(substrBegin != 0) {
        return false;
      }
      beginningMatch = false;
    }

    return true;
  }
};

TEST(Playground, regex) {
  std::string str1 = "aaccccc";
  std::string pattern1 = "a*aab*cccc*cc*c";
  std::string str2 = "aa";
  std::string pattern2 = "a";
  const u32 pLen = pattern1.length();
  std::string expectedMassagedPattern = "aaa*b*cccccc*";
  const u32 expectedMassagedPLen = expectedMassagedPattern.length();

  Solution solution;

  bool substrMatch = solution.isMatch(str2, pattern2);

  ASSERT_TRUE(substrMatch);
}