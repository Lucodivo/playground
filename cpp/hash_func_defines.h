#pragma once

#define HASH_FUNC_HASH(name) u64 name(void* key) // allows us to easily define HashSet hash function
typedef HASH_FUNC_HASH(hash_func_hash); // allows us to easily pass around function pointers of hash functions
HASH_FUNC_HASH(HashFuncHashStub) // creation of a stub hash function that is guaranteed to throw an error
{
  printf("ERROR: This hash func is just a stub function.\n");
  return ~0u;
}

#define HASH_FUNC_EQUALS(name) bool name(void* key1, void* key2) // allows us to easily define HashSet equals function
typedef HASH_FUNC_EQUALS(hash_func_equals); // allows us to easily pass around function pointers of equals functions
HASH_FUNC_EQUALS(HashFuncEqualsStub) // creation of a stub equals function that is guaranteed to throw an error
{
  printf("ERROR: This equals func is just a stub function.\n");
  return false;
}