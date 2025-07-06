
#include <string>
#include <expected>
#include <format>

enum class command {
    none,
    help,
    build,
};

[[nodiscard]] constexpr std::expected<command, std::string> parse_command(std::string_view cmd)
{
    if (cmd == "help") {
        return command::help;
    } else if (cmd == "build") {
        return command::build;
    } else {
        return std::unexpected{std::format("Unknown command '{}'.", cmd)};
    }
}
