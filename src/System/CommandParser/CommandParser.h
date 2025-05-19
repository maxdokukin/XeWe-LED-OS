// CommandParser.h
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <Arduino.h>
#include <functional>

class CommandParser {
public:
    // Allow any callable, including lambdas capturing `this`
    using CommandFunction = std::function<void(const String& args)>;

    struct Command {
        const char*      name;
        CommandFunction  function;
    };

    struct CommandGroup {
        const char*      name;
        const Command*   commands;
        size_t           commandCount;
    };

    CommandParser() = default;

    // Set your groups once...
    void setGroups(const CommandGroup* groups, size_t groupCount);

    // ...then call parse() for each incoming line
    void parse(const String& inputLine) const;

private:
    const CommandGroup* groups     = nullptr;
    size_t               groupCount = 0;

    void parseAndExecute(const String& input) const;
};

#endif // COMMAND_PARSER_H
