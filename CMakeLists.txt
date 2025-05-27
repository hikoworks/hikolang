cmake_minimum_required(VERSION 3.25)
project(hikolang LANGUAGES C CXX)
include(FetchContent)

# disable adding CTest build targets like "NightlyStart" (before ctest include)
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CTestCustom.cmake.in" "CTestCustom.cmake" @ONLY)
include(CTest)

# Set the C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable vcpkg integration
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(ERROR "CMAKE_TOOLCHAIN_FILE is not set. Please set it to the vcpkg toolchain file.")
endif()

# Get the hikotest library for unit-testing if BUILD_TESTING is enabled.
if(BUILD_TESTING)
    FetchContent_Declare(hikotest GIT_REPOSITORY https://github.com/hikogui/hikotest.git GIT_TAG main)
    FetchContent_MakeAvailable(hikotest)
endif()

# Find LLVM (required components: core, support, etc.)
find_package(LLVM CONFIG REQUIRED)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
# Optionally, for LLVM dynamic libraries
llvm_map_components_to_libnames(llvm_libs core support)

# Find ICU (i18n and uc components)
find_package(ICU REQUIRED COMPONENTS i18n uc data)

# Find MS-GSL
find_package(Microsoft.GSL CONFIG REQUIRED)

add_library(hic_objects OBJECT
    "${CMAKE_CURRENT_SOURCE_DIR}/src/char_category.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/char_category.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/char_lookahead.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/module.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/module.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer_parse_number.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utf8.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utf8.hpp"
)

target_include_directories(hic_objects PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_include_directories(hic_objects PRIVATE "${LLVM_INCLUDE_DIRS}")
target_include_directories(hic_objects PRIVATE "${ICU_INCLUDE_DIRS}")


if(WIN32)
    target_compile_definitions(hic_objects PRIVATE "_WIN32_WINNT=0x0601") # Windows 7
    target_compile_definitions(hic_objects PRIVATE "_CRT_SECURE_NO_WARNINGS") # Disable warnings for unsafe functions
    target_compile_definitions(hic_objects PRIVATE "NOMINMAX") # Disable min/max macros
    target_compile_definitions(hic_objects PRIVATE "WIN32_LEAN_AND_MEAN") # Exclude rarely-used Windows headers
    target_compile_definitions(hic_objects PRIVATE "WIN32_NO_STATUS") # Disable Windows status codes
    target_compile_definitions(hic_objects PRIVATE "UNICODE") # Use Unicode
    target_compile_definitions(hic_objects PRIVATE "_UNICODE") # Use Unicode
endif()


# The hic compiler.
add_executable(hic)
target_sources(hic PRIVATE
    $<TARGET_OBJECTS:hic_objects>
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hic.cpp"
)

target_compile_definitions(hic PRIVATE ${LLVM_DEFINITIONS})
target_link_libraries(hic PRIVATE ${llvm_libs})
target_link_libraries(hic PRIVATE Microsoft.GSL::GSL)
target_link_libraries(hic PRIVATE ICU::i18n)
target_link_libraries(hic PRIVATE ICU::uc)
target_link_libraries(hic PRIVATE ICU::data)


if(BUILD_TESTING)
    add_executable(hic_tests)
    target_sources(hic_tests PRIVATE
        $<TARGET_OBJECTS:hic_objects>
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utf8_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer_tests.cpp"
    )
    target_compile_definitions(hic_tests PRIVATE ${LLVM_DEFINITIONS})
    target_link_libraries(hic_tests PRIVATE ${llvm_libs})
    target_link_libraries(hic_tests PRIVATE Microsoft.GSL::GSL)
    target_link_libraries(hic_tests PRIVATE ICU::i18n)
    target_link_libraries(hic_tests PRIVATE ICU::uc)
    target_link_libraries(hic_tests PRIVATE ICU::data)
    target_link_libraries(hic_tests PRIVATE hikotest)
    target_include_directories(hic_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

    set_target_properties(hic_tests PROPERTIES DEBUG_POSTFIX "-dbg")
    set_target_properties(hic_tests PROPERTIES RELEASE_POSTFIX "-rel")
    set_target_properties(hic_tests PROPERTIES RELWITHDEBINFO_POSTFIX "-rdi")

    # Sanitizers do not work on Windows.
    if (NOT WIN32)
        target_compile_options(hic_tests PRIVATE -fsanitize=address)
        target_link_options(hic_tests PRIVATE -fsanitize=address)
    endif()

    add_test(NAME hic_tests COMMAND hic_tests)
endif()
