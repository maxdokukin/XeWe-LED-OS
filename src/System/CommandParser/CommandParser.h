#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

class CommandParser {
public:
    typedef void (*CommandFunction)(String args);

    struct Command {
        const char* name;
        CommandFunction func;
    };

    struct CommandGroup {
        const char* groupName;
        const Command* commands;
        size_t commandCount;
    };

    CommandParser();
    void begin(Stream& inputStream);
    void update(); // Call this in loop()

    void setGroups(const CommandGroup* groups, size_t groupCount);

private:
    Stream* stream;
    String buffer;

    const CommandGroup* groups;
    size_t groupCount;

    void handleLine(String line);
    void executeCommand(const String& group, const String& command, const String& args);
};

#endif
