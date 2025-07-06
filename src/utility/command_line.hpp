
#pragma once

#include <vector>
#include <map>
#include <string>
#include <string_view>
#include <variant>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace hk {


class command_line {
public:
    struct option_type {
    public:
        /** The long name of the option, e.g. `--help`.
         * 
         */
        std::string name = {};

        /** The short name of the option, e.g. `-h` for `--help`.
         */
        char short_name = '\0';

        /** Whether the option has an value.
         */
        bool has_value = false;

        /** The description shown in the help message. 
         */
        std::string description = {};

        /** Function called when the option is encountered. 
         */
        std::function<std::string(std::string_view)> update = {};

        option_type(
            std::string name,
            char short_name,
            std::string description,
            std::function<std::string(std::string_view)> update);

        void print_help() const;
    };

    enum class result_type {
        success,
        help_requested,
        argument_error,
        file_error
    };

    /** Create a cmd_arguments_options object.
     * 
     * @param synopsis The synopsis of the command line arguments, e.g. `hlc [options] <file>`.
     * @param manual A short manual for the tool, shown after the command line argument description.
     * @param update Function called when a positional argument is encountered.
     */
    command_line(
        std::string synopsis = {},
        std::string manual = {},
        std::function<std::string(std::string_view)> update = {}
    ) :
        _synopsis(std::move(synopsis)),
        _manual(std::move(manual)),
        _update(std::move(update))
    {}

    /** Add a command line option.
     * 
     * @param name The long name of the option, e.g. `--help`.
     * @param short_name The short name of the option, e.g. `-h`.
     * @param description The description of the option, shown in the help message.
     * @param update The function to call when the option is encountered.
     */
    void add(std::string name, char short_name, std::string description, std::function<std::string(std::string_view)> update);

    /** Add a command line option.
     * 
     * @param name The long name of the option, e.g. `--help`.
     * @param description The description of the option, shown in the help message.
     * @param update The function to call when the option is encountered.
     */
    void add(std::string name, std::string description, std::function<std::string(std::string_view)> update)
    {
        add(std::move(name), '\0', std::move(description), std::move(update));
    }

    /** Add a function to be called when a positional argument is encountered.
     * 
     * @param update The function to call with the positional argument as a string view.
     */
    void add(std::function<std::string(std::string_view)> update)
    {
        _update = std::move(update);
    }

    /** Print the help message to the standard output.
     */
    void print_help() const;

    /** Parse command line arguments from argc and argv.
     * 
     * @note On Windows the @a argc and @a argv parameters are ignored and the windows API is
     *       used to retrieve the command line arguments decoded as UTF-8.
     * @param argc The number of command line arguments.
     * @param argv The command line arguments.
     * @return An error code indicating the result of the parsing.
     */
    [[nodiscard]] result_type parse(int argc, char const *const *argv) const;

private:
    /** List of command line options.
     */
    std::vector<option_type> _options = {};

    /** Synopsis of the command line arguments.
     */
    std::string _synopsis = {};

    /** Short manual for the tool 
     * 
     * This text will appear after the command-line argument description.
     */
    std::string _manual = {};

    /** Function to be called when a position argument is encountered. 
     * 
     * This function is called with the positional argument as a string view.
     * The return value is an error message or an empty string if the argument was processed successfully.
     */
    std::function<std::string(std::string_view)> _update = {};

    /** Find an option by its long-name. 
     * 
     * @param name The long name of the option to find.
     * @return A pointer to the option if found, or nullptr if not found.
     */
    [[nodiscard]] option_type const* find(std::string const& name) const;

    /** Find an option by its short-name.
     * 
     * @param name The short name of the option to find.
     * @return A pointer to the option if found, or nullptr if not found.
     */
    [[nodiscard]] option_type const* find(char name) const;

    /** Parse command line arguments.
     * 
     * @param arguments The command line arguments to parse.
     * @return An error code indicating the result of the parsing.
     */
    [[nodiscard]] result_type parse(std::vector<std::string> const &arguments) const;

    /** Parse command line arguments from a file. 
     * 
     * The file is called a response file and contains command line arguments.
     *  - The file is encoded in UTF-8 and can contain multiple lines.
     *  - Each argument is separated by whitespace, including newlines.
     *  - You can escape any character in the file by preceding it with a backslash `\`.
     *  - You can escape whitespace in arguments by enclosing them in double quotes `"`
     * 
     * @param filename The name of the file containing command line arguments.
     * @return An error code indicating the result of the parsing.
     */
    [[nodiscard]] result_type parse(std::string_view filename) const;
};



}
