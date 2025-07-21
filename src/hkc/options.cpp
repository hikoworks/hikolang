
#include "options.hpp"
#include "utility/command_line.hpp"
#include "utility/strings.hpp"
#include <print>

options::options()
{
    compile_directory = std::filesystem::current_path();;
}

[[nodiscard]] std::string options::finish()
{


    // Create an absolute path for the compile directory.
    auto error = std::error_code{};
    compile_directory = std::filesystem::absolute(compile_directory, error);
    if (error) {
        return std::format("Invalid compile directory '{}': {}", compile_directory.string(), error.message());
    }

    // Check if the compile directory exists.
    if (not std::filesystem::exists(compile_directory)) {
        return std::format("Compile directory '{}' does not exist.", compile_directory.string());
    }

    return std::string{};
}

[[nodiscard]] int options::parse(int argc, char const* const* argv)
{
    auto parser = hk::command_line{"hkc [options] <command> [directory]", "Hikolang compiler"};

    auto num_positional_arguments = 0;

    parser.add("--version", 'v', "Show the version of the compiler.", [this](std::string_view) {
        std::println("Hikolang Compiler (hkc) version 1.0.0");
        return std::string{};
    });

    parser.add("--help", 'h', "Show this help message.", [this](std::string_view) {
        help = true;
        return std::string{};
    });

    parser.add("--list", "List all repositories, packages, applications, libraries and dependencies.", [this](std::string_view) {
        command |= command_type::list;
        return std::string{};
    });

    parser.add(
        "--clone",
        "Clone the specified remote repositories.\n"
        "Many commands will automatically clone the required repositories.",
        [this](std::string_view) {
            command |= command_type::clone;
            return std::string{};
        });

    parser.add("--build", "Compile applications or libraries.", [this](std::string_view) {
        command |= command_type::build;
        return std::string{};
    });

    parser.add("--test", "Run tests.", [this](std::string_view) {
        command |= command_type::test;
        return std::string{};
    });

    parser.add("--sign", "Sign the compiled applications with a digital signature.", [this](std::string_view) {
        command |= command_type::sign;
        return std::string{};
    });

    parser.add("--distribute", "Package the compiled applications into a distributable format.", [this](std::string_view) {
        command |= command_type::distribute;
        return std::string{};
    });

    parser.add("--build-type=", "Specify the build type (e.g., `debug`, `release`).", [this](std::string_view value) {
        if (value == "debug") {
            build_type = build_type_t::debug;
            return std::string{};
        } else if (value == "release") {
            build_type = build_type_t::release;
            return std::string{};
        } else if (value.empty()) {
            return std::string{"Build type is required."};
        } else {
            return std::format("Invalid build type '{}'.", value);
        }
    });

    parser.add("--optimize=", 'O', "Enable optimizations during compilation.", [this](std::string_view value) {
        if (value == "0") {
            optimize_level = optimize_level_type::none;
        } else if (value == "1") {
            optimize_level = optimize_level_type::basic;
        } else if (value == "2") {
            optimize_level = optimize_level_type::aggressive;
        } else if (value == "s") {
            optimize_level = optimize_level_type::size;
        } else {
            return std::format("Invalid optimization level '{}'.", value);
        }
        return std::string{};
    });

    parser.add(
        "--repository=",
        "Specify local repositories to include in the build. Packages in these "
        "repositories will override packages in remote repositories, ignoring version "
        "numbers. Multiple `--repository` options may be specified.\n"
        "Format: `--repository=<path>[:<path>]` where `<path>`",
        [this](std::string_view value) {
            if (value.empty()) {
                return std::string{"Repository path is required."};
            }

            for (auto path_str : hk::split(value, ':')) {
                auto path = std::filesystem::path{};

                try {
                    path = std::filesystem::path{path_str};
                } catch (std::exception const& e) {
                    return std::format("Invalid repository path '{}': {}", path_str, e.what());
                }

                auto error = std::error_code{};
                path = std::filesystem::absolute(path, error);
                if (error) {
                    return std::format("Invalid repository path '{}': {}", path_str, error.message());
                }

                if (not std::filesystem::exists(path)) {
                    return std::format("Repository path '{}' does not exist.", path.string());
                }

                if (not std::filesystem::is_directory(path)) {
                    return std::format("Repository path '{}' is not a directory.", path.string());
                }

                auto it = local_repositories.lower_bound(path);
                if (it != local_repositories.end() and *it == path) {
                    return std::format("Repository path '{}' already exists.", path.string());
                }
                local_repositories.emplace_hint(it, path);
            }
            return std::string{};
        });

    parser.add("--test-recursive", "Run tests recursively in all cloned repositories.", [this](std::string_view) {
        test_recursive = true;
        return std::string{};
    });

    parser.add("--build-recursive", "Compile all applications recursively in all cloned repositories.", [this](std::string_view) {
        build_recursive = true;
        return std::string{};
    });

    parser.add("--fetch", "Fetch the latest changes from remote repositories before building.", [this](std::string_view) {
        fetch = true;
        return std::string{};
    });

    parser.add("--library-path=", 'L', "Specify a path to search for FFI libraries.", [this](std::string_view value) {
        if (value.empty()) {
            return std::string{"Library path is required."};
        }

        auto paths = hk::split(value, ':');
        for (auto path_str : paths) {
            auto path = std::filesystem::path{};

            try {
                path = std::filesystem::path{path_str};
            } catch (std::exception const& e) {
                return std::format("Invalid library path '{}': {}", path_str, e.what());
            }

            auto error = std::error_code{};
            path = std::filesystem::absolute(path, error);
            if (error) {
                return std::format("Invalid library path '{}': {}", path_str, error.message());
            }

            if (not std::filesystem::exists(path)) {
                return std::format("Repository library '{}' does not exist.", path.string());
            }

            if (not std::filesystem::is_directory(path)) {
                return std::format("Repository library '{}' is not a directory.", path.string());
            }

            auto it = library_paths.lower_bound(path);
            if (it != library_paths.end() and *it == path) {
                // Duplicate paths are not an error.
                return std::string{};
            }
            library_paths.emplace_hint(it, path);
        }
        return std::string{};
    });

    parser.add(
        "--define=",
        'D',
        "Define a constant that can be used in the source code.\n"
        "Format: `<name>:<value>[,<name>:<value>]`.\n"
        "Multiple --define options are allowed.", [this](std::string_view value) {
            if (value.empty()) {
                return std::string{"Define value is required."};
            }

            for (auto const& def : hk::split(value, ',')) {
                auto parts = hk::split(def, ':');
                if (parts.size() != 2) {
                    return std::format("Invalid define format '{}'. Expected '<name>:<value>'.", def);
                }

                auto it = defines.lower_bound(parts[0]);
                if (it != defines.end() and it->first == parts[0]) {
                    return std::format("Define '{}' already exists with value '{}'.", parts[0], it->second);
                }

                defines.emplace_hint(it, parts[0], parts[1]);
            }
            return std::string{};
        });

    parser.add([this, &num_positional_arguments](std::string_view value) {
        if (num_positional_arguments == 0) {
            try {
                compile_directory = std::filesystem::path{value};
            } catch (std::exception const& e) {
                return std::format("Invalid compile directory '{}': {}", value, e.what());
            }

        } else {
            return std::format("Unexpected {}th positional argument '{}', expected at most 2 positional arguments.", num_positional_arguments, value);
        }

        ++num_positional_arguments;
        return std::string{};
    });

    switch (parser.parse(argc, argv)) {
    case hk::command_line::result_type::success:
        break;

    case hk::command_line::result_type::argument_error:
    case hk::command_line::result_type::file_error:
        std::println("");
        parser.print_help();
        return 2;
    }

    if (auto tmp = finish(); not tmp.empty()) {
        std::println("Error: {}\n", tmp);
        parser.print_help();
        return 2;
    }

    if (help) {
        parser.print_help();
        return 0;
    }

    return -1;
}

