// CommandParser.h
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <Arduino.h>
#include <functional>

class CommandParser {
public:
    // Any callable taking the argument string
    using command_function_t = std::function<void(const String& args)>;

    struct Command {
        const char*            name;
        const char*            description;
        size_t                 arg_count;
        command_function_t     function;
    };

    struct CommandGroup {
        const char*            name;
        const Command*         commands;
        size_t                 command_count;
    };

    CommandParser() = default;

    // Set the available command groups
    void set_groups(const CommandGroup* groups, size_t group_count);

    // Parse and execute a raw input line
    void parse_and_execute(const String& input) const;

private:
    const CommandGroup* groups_       = nullptr;
    size_t               group_count_ = 0;
};

#endif // COMMAND_PARSER_H
