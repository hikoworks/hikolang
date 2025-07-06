
#include "options.hpp"
#include "utility/command_line.hpp"
#include <print>

static void default_options(options& o)
{
    o.force_download_packages = false;
    o.compile_directory = std::filesystem::current_path();;
}

[[nodiscard]] static std::string fixup_options(options& o)
{
    if (o.cmd == command::none) {
        return std::string{"Requires a command, e.g. 'build'."};
    }

    // Create an absolute path for the compile directory.
    auto error = std::error_code{};
    o.compile_directory = std::filesystem::absolute(o.compile_directory, error);
    if (error) {
        return std::format("Invalid compile directory '{}': {}", o.compile_directory.string(), error.message());
    }

    // Check if the compile directory exists.
    if (not std::filesystem::exists(o.compile_directory)) {
        return std::format("Compile directory '{}' does not exist.", o.compile_directory.string());
    }

    return std::string{};
}

[[nodiscard]] std::expected<options, int> parse_options(int argc, char const* const* argv)
{
    auto parser = hk::command_line{"hkc [options] <command> [directory]", "Hikolang compiler"};

    auto num_positional_arguments = 0;
    auto r = options{};

    parser.add(
        "--force-download-packages",
        "Force download packages from the internet, to make sure that the latest version is used.",
        [&r](std::string_view) {
            r.force_download_packages = true;
            return std::string{};
        });

    parser.add([&r, &num_positional_arguments](std::string_view value) {
        if (num_positional_arguments == 0) {
            if (auto tmp = parse_command(value)) {
                r.cmd = *tmp;
            } else {
                return tmp.error();
            }

        } else if (num_positional_arguments == 1) {
            if (r.cmd == command::help) {
                if (auto tmp = parse_command(value)) {
                    r.cmd_help = *tmp;
                } else {
                    return tmp.error();
                }

            } else {
                try {
                    r.compile_directory = std::filesystem::path{value};
                } catch (std::exception const& e) {
                    return std::format("Invalid compile directory '{}': {}", value, e.what());
                }
            }

        } else {
            return std::format("Unexpected {}th positional argument '{}', expected at most 2 positional arguments.", num_positional_arguments, value);
        }

        ++num_positional_arguments;
        return std::string{};
    });

    default_options(r);

    switch (parser.parse(argc, argv)) {
    case hk::command_line::result_type::success:
        break;

    case hk::command_line::result_type::help_requested:
        parser.print_help();
        return std::unexpected{0};

    case hk::command_line::result_type::argument_error:
    case hk::command_line::result_type::file_error:
        std::println("");
        parser.print_help();
        return std::unexpected{2};
    }

    if (auto tmp = fixup_options(r); not tmp.empty()) {
        std::println("Error: {}\n", tmp);
        parser.print_help();
        return std::unexpected{2};
    }

    return r;
}

