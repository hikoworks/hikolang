cmake_minimum_required(VERSION 3.20)
project(hikolang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable vcpkg integration
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(ERROR "CMAKE_TOOLCHAIN_FILE is not set. Please set it to the vcpkg toolchain file.")
endif()

# Find LLVM (required components: core, support, etc.)
find_package(llvm REQUIRED)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Find ICU (i18n and uc components)
find_package(icu REQUIRED COMPONENTS i18n uc)

include_directories(
    ${LLVM_INCLUDE_DIRS}
    ${ICU_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
)

add_definitions(${LLVM_DEFINITIONS})

add_executable(hikolang main.cpp)

target_link_libraries(hikolang
    PRIVATE
        LLVMCore
        LLVMSupport
        icu::i18n
        icu::uc
)

# Optionally, for LLVM dynamic libraries
llvm_map_components_to_libnames(llvm_libs core support)
target_link_libraries(hikolang PRIVATE ${llvm_libs})

