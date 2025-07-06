
#include "command_line.hpp"
#include <cassert>
#include <string>
#include <string_view>
#include <format>
#include <print>
#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <utility>
#include <vector>
#include <functional>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <cctype>
#include <gsl/gsl>

#if defined(_WIN32)
#include <windows.h>
#include <stringapiset.h>
#include <shellapi.h>
#endif

namespace hk {

command_line::option_type::option_type(
    std::string name,
    char short_name,
    std::string description,
    std::function<std::string(std::string_view)> update) :
    name(std::move(name)),
    short_name(short_name),
    description(std::move(description)),
    update(update)
{
    if (has_value = this->name.ends_with('='); has_value) {
        this->name.pop_back();
    }
}

void command_line::option_type::print_help() const
{
    if (short_name != '\0') {
        std::println("  -{}, --{}: {}", short_name,name, description);
    } else {
        std::println("  --{}: {}", name, description);
    }
}

void command_line::print_help() const
{
    std::println("Usage:");
    std::println("{}", _synopsis);

    std::println("\nOptions:");
    for (auto const &option : _options) {
        option.print_help();
    }

    std::println("\nDescription:");
    std::println("{}", _manual);
}

[[nodiscard]] command_line::option_type const* command_line::find(std::string const& name) const
{
    auto it = std::lower_bound(_options.cbegin(), _options.cend(), name,
        [](option_type const& opt, std::string const& name) {
            return opt.name < name;
        });

    return it != _options.cend() && it->name == name ? std::addressof(*it) : nullptr;
}

[[nodiscard]] command_line::option_type const* command_line::find(char name) const
{
    auto it = std::find_if(_options.cbegin(), _options.cend(),
        [name](option_type const& opt) { return opt.short_name == name; });

    return it != _options.cend() ? std::addressof(*it) : nullptr;
}

void command_line::add(
    std::string name, char short_name, std::string description,
    std::function<std::string(std::string_view)> update)
{
    auto it = std::lower_bound(_options.begin(), _options.end(), name,
        [](option_type const& opt, std::string const& name) {
            return opt.name < name;
        });
    
    if (it == _options.end()) {
        _options.emplace_back(std::move(name), short_name, std::move(description), std::move(update));

    } else if (it->name == name) {
        throw std::invalid_argument(std::format("Option '{}' already exists.", name));

    } else {
        _options.insert(it, option_type{std::move(name), short_name, std::move(description), std::move(update)});
    }
}

[[nodiscard]] command_line::result_type command_line::parse(std::vector<std::string> const &arguments) const
{
    auto it = arguments.cbegin();
    auto last = arguments.cend();

    for (; it != last; ++it) {
        if (*it == "--") {
            // End of options, all remaining arguments are positional.
            ++it;
            break;

        } else if (*it == "-") {
            // Single dash, treat as a positional argument (stdin/stdout).
            break;

        } else if (it->starts_with("@")) {
            auto filename = it->substr(1);
            if (filename.empty()) {
                std::println("Error: Filename for '@' option is empty.");
                return result_type::argument_error;
            }
            if (auto tmp = parse(filename); tmp != result_type::success) {
                std::println("Error parsing arguments from file: '{}'", filename);
                return tmp;
            }

        } else if (it->starts_with("--")) {
            auto const pos = it->find('=');
            auto const has_value = pos != std::string_view::npos;

            auto [name, value] = [&] {
                if (has_value) {
                    return std::pair{it->substr(2, pos - 2), it->substr(pos + 1)};
                } else {
                    return std::pair{it->substr(2), std::string{}};
                }
            }();

            auto *option = find(name);
            if (not option) {
                std::println("Unknown option: '{}'", name);
                return result_type::argument_error;
            }

            if (option->has_value and not has_value) {
                std::println("Option '{}' requires a value.", name);
                return result_type::argument_error;

            } else if (not option->has_value and has_value) {
                std::println("Option '{}' does not take a value.", name);
                return result_type::argument_error;
            }

            option->update(value);

        } else if (it->starts_with("-")) {
            auto str = it->substr(1);

            while (not str.empty()) {
                auto short_option = str[0];
                str = it->substr(1);

                auto *option = find(short_option);
                if (not option) {
                    std::println("Unknown short option: '-{}'", short_option);
                    return result_type::argument_error;
                }

                if (option->has_value) {
                    if (not str.empty()) {
                        // The option has a value, and the next character is part of the value.
                        option->update(str);
                        str.clear();

                    } else {
                        // The option has a value, and the next argument is the value.
                        if (++it == last) {
                            std::println("Option '-{}' requires a value, but no value was provided.", short_option);
                            return result_type::argument_error;
                        }
                        option->update(*it);
                        break;
                    }
                } else {
                    // The option does not have a value, just call the update function.
                    option->update(std::string{});
                }
            }

        } else {
            // The first positional argument.
            break;
        }
    }
    
    // Positional arguments processing.
    for (; it != last; ++it) {
        _update(*it);
    }

    return result_type::success;
}

[[nodiscard]] command_line::result_type command_line::parse(std::string_view filename) const
{
    assert(not filename.empty());

    auto file_path = std::filesystem::path{filename};

    if (not std::filesystem::exists(file_path)) {
        std::println("Respone file '{}' does not exist.", file_path.string());
        return result_type::file_error;
    }

    if (not std::filesystem::is_regular_file(file_path)) {
        std::println("Response file '{}' is not a regular file.", file_path.string());
        return result_type::file_error;
    }

    auto file_size = std::filesystem::file_size(file_path);

    auto file = std::ifstream{file_path, std::ios::in | std::ios::binary};
    if (not file.is_open()) {
        std::println("Failed to open response file '{}': {}", file_path.string(), std::strerror(errno));
        return result_type::file_error;
    }

    auto data = std::string{};
    data.resize_and_overwrite(
        file_size,
        [&file](char *p, std::size_t count) {
            file.read(p, gsl::narrow_cast<std::streamsize>(count));
            return file.gcount();
        });

    file.close();

    auto arguments = std::vector<std::string>{};

    enum class state_type {
        normal,
        in_quotes,
        escaped,
    };
    
    auto state = state_type::normal;
    auto argument = std::string{};
    for (auto const c : data) {
        if (state == state_type::escaped) {
            // If we are in an escaped state, just add the character to the argument.
            argument.push_back(c);
            state = state_type::normal;

        } else if (c == '\\') {
            // If we encounter a backslash, we enter the escaped state.
            state = state_type::escaped;

        } else if (c == '\"') {
            // If we encounter a double quote, we toggle the in_quotes state.
            state = state == state_type::normal ? state_type::in_quotes : state_type::normal;

        } else if (std::isspace(c) and state == state_type::normal) {
            // If we encounter a whitespace character, we only add the argument if it is not empty.
            if (not argument.empty()) {
                arguments.push_back(std::move(argument));
                argument.clear();
            }

        } else {
            // Otherwise, we just add the character to the argument.
            argument.push_back(c);
        }
    }

    if (not argument.empty()) {
        // If we have a non-empty argument at the end of the line, add it.
        arguments.push_back(std::move(argument));
    }

    return parse(arguments);
}

[[nodiscard]] static std::vector<std::string> argv_to_command_line_arguments(int argc, char const *const *argv)
{
    assert(argc > 0 and argv != nullptr);

    auto arguments = std::vector<std::string>{};
    arguments.reserve(static_cast<std::size_t>(argc - 1));

    for (int i = 1; i != argc; ++i) {
        arguments.emplace_back(argv[i]);
    }

    return arguments;
}

#if defined(_WIN32)
[[nodiscard]] static std::vector<string_string> win32_get_command_line_arguments()
{
auto w_line = GetCommandLineW();
    assert(w_line != nullptr);

    int w_argc = 0;
    auto w_args = CommandLineToArgvW(w_line, &w_argc);
    if (w_args == nullptr) {
        std::println("Failed to parse command line: {}", GetLastError());
        return result_type::argument_error;
    }
    assert(w_argc > 0);

    auto arguments = std::vector<std::string>{};
    arguments.resize(static_cast<std::size_t>(w_argc - 1));

    for (int i = 1; i < w_argc; ++i) {
        auto w_arg = std::wstring_view{w_args[i]};
        auto &argument = arguments[static_cast<std::size_t>(i - 1)];

        // Reserve enough space for UTF-8 conversion.
        argument.resize_and_overwrite(
            w_arg.size() * 4,
            [&w_arg](char *p, std::size_t count) {
                return WideCharToMultiByte(
                    CP_UTF8, 0,
                    w_arg.data(), static_cast<int>(w_arg.size()),
                    p, gsl::narrow_cast<int>(count),
                    nullptr,
                    nullptr);
            });

        if (argument.empty()) {
            std::println("Failed to convert argument {} to UTF-8: {}", i, GetLastError());
            LocalFree(w_args);
            return result_type::argument_error;
        }
    }

    LocalFree(w_args);
    return arguments;
}
#endif

[[nodiscard]] command_line::result_type command_line::parse(int argc, char const *const *argv) const
{
#ifdef _WIN32
    auto arguments = win32_get_command_line_arguments();
#else
    auto arguments = argv_to_command_line_arguments(argc, argv);
#endif

    return parse(arguments);
}

} // namespace hk
