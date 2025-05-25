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
find_package(ICU REQUIRED COMPONENTS i18n uc)

# Find MS-GSL
find_package(Microsoft.GSL CONFIG REQUIRED)

add_library(hic_objects OBJECT
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utf8.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utf8.hpp"
)




# The hic compiler.
add_executable(hic)
target_sources(hic PRIVATE
    $<TARGET_OBJECTS:hic_objects>
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hic.cpp"
)

target_compile_definitions(hic PRIVATE ${LLVM_DEFINITIONS})
if(WIN32)
    target_compile_definitions(hic PRIVATE "_WIN32_WINNT=0x0601") # Windows 7
    target_compile_definitions(hic PRIVATE "_CRT_SECURE_NO_WARNINGS") # Disable warnings for unsafe functions
    target_compile_definitions(hic PRIVATE "NOMINMAX") # Disable min/max macros
    target_compile_definitions(hic PRIVATE "WIN32_LEAN_AND_MEAN") # Exclude rarely-used Windows headers
    target_compile_definitions(hic PRIVATE "WIN32_NO_STATUS") # Disable Windows status codes
    target_compile_definitions(hic PRIVATE "UNICODE") # Use Unicode
    target_compile_definitions(hic PRIVATE "_UNICODE") # Use Unicode
endif()

target_include_directories(hic PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_include_directories(hic PRIVATE "${LLVM_INCLUDE_DIRS}")
target_include_directories(hic PRIVATE "${ICU_INCLUDE_DIRS}")

target_link_libraries(hic PRIVATE ${llvm_libs})
target_link_libraries(hic PRIVATE Microsoft.GSL::GSL)
target_link_libraries(hic PRIVATE ICU::i18n)
target_link_libraries(hic PRIVATE ICU::uc)

if(BUILD_TESTING)
    add_executable(hic_tests)
    target_sources(hic_tests PRIVATE
        $<TARGET_OBJECTS:hic_objects>
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utf8_tests.cpp"
    )

    target_link_libraries(hic_tests PRIVATE hikotest)
    target_include_directories(hic_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

    set_target_properties(hic_tests PROPERTIES DEBUG_POSTFIX "-dbg")
    set_target_properties(hic_tests PROPERTIES RELEASE_POSTFIX "-rel")
    set_target_properties(hic_tests PROPERTIES RELWITHDEBINFO_POSTFIX "-rdi")
    if(WIN32)
        target_compile_definitions(hic_tests PRIVATE "_WIN32_WINNT=0x0601") # Windows 7
        target_compile_definitions(hic_tests PRIVATE "_CRT_SECURE_NO_WARNINGS") # Disable warnings for unsafe functions
        target_compile_definitions(hic_tests PRIVATE "NOMINMAX") # Disable min/max macros
        target_compile_definitions(hic_tests PRIVATE "WIN32_LEAN_AND_MEAN") # Exclude rarely-used Windows headers
        target_compile_definitions(hic_tests PRIVATE "WIN32_NO_STATUS") # Disable Windows status codes
        target_compile_definitions(hic_tests PRIVATE "UNICODE") # Use Unicode
        target_compile_definitions(hic_tests PRIVATE "_UNICODE") # Use Unicode
    endif()

    # Sanitizers do not work on Windows.
    if (NOT WIN32)
        target_compile_options(hic_tests PRIVATE -fsanitize=address)
        target_link_options(hic_tests PRIVATE -fsanitize=address)
    endif()

    add_test(NAME hic_tests COMMAND hic_tests)
endif()
