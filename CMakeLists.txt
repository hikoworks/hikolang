cmake_minimum_required(VERSION 3.25)
project(hikolang LANGUAGES C CXX)
configure_file("src/test_utilities/paths.cpp.in" "src/test_utilities/paths.cpp" @ONLY)

include(FetchContent)

# disable adding CTest build targets like "NightlyStart" (before ctest include)
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CMake/CTestCustom.cmake.in" "CTestCustom.cmake" @ONLY)
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
    FetchContent_Declare(hikotest GIT_REPOSITORY https://github.com/hikoworks/hikotest.git GIT_TAG main)
    FetchContent_MakeAvailable(hikotest)
endif()

# Find LLVM (required components: core, support, etc.)
# vcpkg.json dependency example:
#     { "name": "llvm", "default-features": false, "features": ["default-targets"] }
find_package(LLVM CONFIG REQUIRED)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
# Optionally, for LLVM dynamic libraries
llvm_map_components_to_libnames(llvm_libs core support)


find_package(ICU REQUIRED COMPONENTS i18n uc data)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(libgit2 CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

add_library(hk_objects OBJECT
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ast/expression_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ast/import_library_declaration_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ast/import_module_declaration_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ast/import_repository_declaration_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ast/library_declaration_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ast/library_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ast/module_declaration_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ast/module_node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ast/module_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ast/node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ast/program_declaration_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ast/program_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ast/top_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/error/hkc_error.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/error/hkc_error.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/error/error_item.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/error/error_item.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/error/error_list.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/error/error_location.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/error/errors.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/consume.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/consume.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_context.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_fqname.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_fqname.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_import_module_declaration.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_import_module_declaration.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_import_repository_declaration.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_import_repository_declaration.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_library_declaration.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_library_declaration.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_module_declaration.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_module_declaration.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_module.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_module.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_program_declaration.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_program_declaration.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_result.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_top.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse_top.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/repository/source.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/repository/source.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/repository/repository.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/repository/repository.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/char_category.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/char_category.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/line_table.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/line_table.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/parse_block_comment.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/parse_bracketed_string.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/parse_context_arg.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/parse_identifier.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/parse_line_comment.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/parse_line_directive.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/parse_number.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/parse_operator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/parse_position_arg.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/parse_superscript_integer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/parse_string.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/parse_tag.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/token_parsers.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/token_vector.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/token.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/token.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/tokenizer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/tokenizer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/base32.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/base32.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/command_line.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/command_line.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/defer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/enum_variant.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/file_buffer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/file_buffer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/file_ifstream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/file_ifstream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/file.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/file.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/fixed_fifo.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/fixed_string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/fqname.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/generator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/git_error.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/git_error.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/git.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/git.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/interned.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/lazy_vector.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/log.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/log.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/module.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/module.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/path.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/path.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/read_file.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/read_file.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/repository_flags.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/repository_url.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/repository_url.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/semantic_version.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/semantic_version.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/sha.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/sha.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/strings.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/strings.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/unicode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/unicode.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/vector_map.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/vector_set.hpp"
)

target_include_directories(hk_objects PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_include_directories(hk_objects PRIVATE "${LLVM_INCLUDE_DIRS}")
target_include_directories(hk_objects PRIVATE "${ICU_INCLUDE_DIRS}")


if(WIN32)
    target_compile_definitions(hk_objects PRIVATE "_WIN32_WINNT=0x0601") # Windows 7
    target_compile_definitions(hk_objects PRIVATE "_CRT_SECURE_NO_WARNINGS") # Disable warnings for unsafe functions
    target_compile_definitions(hk_objects PRIVATE "NOMINMAX") # Disable min/max macros
    target_compile_definitions(hk_objects PRIVATE "WIN32_LEAN_AND_MEAN") # Exclude rarely-used Windows headers
    target_compile_definitions(hk_objects PRIVATE "WIN32_NO_STATUS") # Disable Windows status codes
    target_compile_definitions(hk_objects PRIVATE "UNICODE") # Use Unicode
    target_compile_definitions(hk_objects PRIVATE "_UNICODE") # Use Unicode
endif()


# The hic compiler.
add_executable(hkc)
target_sources(hkc PRIVATE
    $<TARGET_OBJECTS:hk_objects>
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hkc/command.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hkc/hkc.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hkc/options.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hkc/options.hpp"
)

target_include_directories(hkc PRIVATE "${CMAKE_SOURCE_DIR}/src")
#target_compile_definitions(hkc PRIVATE ${LLVM_DEFINITIONS})
#target_link_libraries(hkc PRIVATE ${llvm_libs})
target_link_libraries(hkc PRIVATE Microsoft.GSL::GSL)
target_link_libraries(hkc PRIVATE ICU::i18n)
target_link_libraries(hkc PRIVATE ICU::uc)
target_link_libraries(hkc PRIVATE ICU::data)
target_link_libraries(hkc PRIVATE libgit2::libgit2package)
target_link_libraries(hkc PRIVATE OpenSSL::SSL)
target_link_libraries(hkc PRIVATE OpenSSL::Crypto)

if(BUILD_TESTING)
    add_executable(hktests)
    target_sources(hktests PRIVATE
        $<TARGET_OBJECTS:hk_objects>
        "${CMAKE_CURRENT_SOURCE_DIR}/src/repository/repository_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer/tokenizer_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/git_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/read_file_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/strings_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/unicode_tests.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/src/test_utilities/paths.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/test_utilities/paths.hpp"
    )
    #target_compile_definitions(hktests PRIVATE ${LLVM_DEFINITIONS})
    #target_link_libraries(hktests PRIVATE ${llvm_libs})
    target_link_libraries(hktests PRIVATE Microsoft.GSL::GSL)
    target_link_libraries(hktests PRIVATE ICU::i18n)
    target_link_libraries(hktests PRIVATE ICU::uc)
    target_link_libraries(hktests PRIVATE ICU::data)
    target_link_libraries(hktests PRIVATE libgit2::libgit2package)
    target_link_libraries(hktests PRIVATE OpenSSL::SSL)
    target_link_libraries(hktests PRIVATE OpenSSL::Crypto)
    target_link_libraries(hktests PRIVATE hikotest)
    target_include_directories(hktests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

    set_target_properties(hktests PROPERTIES DEBUG_POSTFIX "-dbg")
    set_target_properties(hktests PROPERTIES RELEASE_POSTFIX "-rel")
    set_target_properties(hktests PROPERTIES RELWITHDEBINFO_POSTFIX "-rdi")

    # Sanitizers do not work on Windows.
    if (NOT WIN32)
        target_compile_options(hktests PRIVATE -fsanitize=address)
        target_link_options(hktests PRIVATE -fsanitize=address)
    endif()

    add_test(NAME hktests COMMAND hktests)
endif()
