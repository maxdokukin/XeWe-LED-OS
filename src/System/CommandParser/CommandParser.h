#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

class CommandParser {
public:
    typedef void (*CommandFunction)(const String& args);

    struct Command {
        const char* name;
        CommandFunction function;
    };

    struct CommandGroup {
        const char* name;
        const Command* commands;
        size_t commandCount;
    };

    // No streams or buffering here—just set up your groups,
    // then call parse() on each full line you read.
    CommandParser() = default;
    void setGroups(const CommandGroup* groups, size_t groupCount);
    void parse(const String& inputLine) const;

private:
    const CommandGroup* groups = nullptr;
    size_t groupCount = 0;

    void parseAndExecute(const String& line) const;
};

#endif // COMMAND_PARSER_H
