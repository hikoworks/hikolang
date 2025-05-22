cmake_minimum_required(VERSION 3.20)
project(hikolang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable vcpkg integration
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

# Find LLVM (required components: core, support, etc.)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Find ICU (i18n and uc components)
find_package(ICU REQUIRED COMPONENTS i18n uc)

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
        ICU::i18n
        ICU::uc
)

# Optionally, for LLVM dynamic libraries
llvm_map_components_to_libnames(llvm_libs core support)
target_link_libraries(hikolang PRIVATE ${llvm_libs})

