include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # For Windows: Prevent overriding parent project's compiler/linker settings
FetchContent_MakeAvailable(googletest)

enable_testing() #Enables testing for this directory and below.

set(LIBS gtest_main)

add_executable(
        playground_tests
        playground_tests.cpp
)
target_link_libraries(playground_tests ${LIBS})

add_executable(
        linked_list_tests
        linked_list_tests.cpp
)
target_link_libraries(linked_list_tests ${LIBS})

add_executable(
        dictionary_trie_tests
        dictionary_trie_tests.cpp
)
target_link_libraries(dictionary_trie_tests ${LIBS})

add_executable(
        hash_set_and_map_tests
        hash_set_and_map_tests.cpp
)
target_link_libraries(hash_set_and_map_tests ${LIBS})

include(GoogleTest)
gtest_discover_tests(
        playground_tests
        linked_list_tests
        dictionary_trie_tests
        hash_set_and_map_tests
)