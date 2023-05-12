project(division_engine_core_tests)

include(FetchContent)
enable_testing()

set(DIVISION_TESTS_SOURCES division_unordered_id_table_tests.cpp division_ordered_id_table_tests.cpp)
add_executable(division_engine_core_tests ${DIVISION_TESTS_SOURCES})

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2
        GIT_TAG v3.1.0
)
FetchContent_MakeAvailable(Catch2)

target_link_libraries(division_engine_core_tests PRIVATE division_engine_core Catch2::Catch2WithMain)


list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)
catch_discover_tests(division_engine_core_tests)