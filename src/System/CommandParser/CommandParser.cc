#include "CommandParser.h"

CommandParser::CommandParser()
    : stream(nullptr), groups(nullptr), groupCount(0) {}

void CommandParser::begin(Stream& inputStream) {
    this->stream = &inputStream;
    buffer = "";
}

void CommandParser::setGroups(const CommandGroup* groups, size_t groupCount) {
    this->groups = groups;
    this->groupCount = groupCount;
}

void CommandParser::update() {
    while (stream && stream->available()) {
        char c = stream->read();
        if (c == '\n') {
            handleLine(buffer);
            buffer = "";
        } else if (c != '\r') {
            buffer += c;
        }
    }
}

void CommandParser::handleLine(String line) {
    line.trim();
    int space1 = line.indexOf(' ');
    if (space1 == -1) {
        stream->println("Usage: <group> <command> [args]");
        return;
    }

    String group = line.substring(0, space1);
    line = line.substring(space1 + 1).trim();

    int space2 = line.indexOf(' ');
    String command = (space2 == -1) ? line : line.substring(0, space2);
    String args = (space2 == -1) ? "" : line.substring(space2 + 1);

    executeCommand(group, command, args);
}

void CommandParser::executeCommand(const String& group, const String& command, const String& args) {
    for (size_t i = 0; i < groupCount; ++i) {
        if (group.equalsIgnoreCase(groups[i].groupName)) {
            for (size_t j = 0; j < groups[i].commandCount; ++j) {
                if (command.equalsIgnoreCase(groups[i].commands[j].name)) {
                    groups[i].commands[j].func(args);
                    return;
                }
            }
        }
    }
    stream->println("Unknown command or group.");
}
