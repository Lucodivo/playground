include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # For Windows: Prevent overriding parent project's compiler/linker settings
FetchContent_MakeAvailable(googletest)

enable_testing() #Enables testing for this directory and below.

set(LIBS gtest_main)

# shader_reflect_test
add_executable(
        playground_tests
        playground_tests.cpp
)
target_link_libraries(playground_tests ${LIBS})
#add_dependencies(shader_reflect_test _____)

include(GoogleTest)
gtest_discover_tests(
        playground_tests
)