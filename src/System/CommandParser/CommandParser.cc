// CommandParser.cc
#include "CommandParser.h"
#include <Arduino.h>  // for Serial

void CommandParser::setGroups(const CommandGroup* groupList, size_t count) {
    groups     = groupList;
    groupCount = count;
}

void CommandParser::parse(const String& inputLine) const {
    parseAndExecute(inputLine);
}

void CommandParser::parseAndExecute(const String& input) const {
    String line = input;
    line.trim();

    // require a leading '$'
    if (!line.startsWith("$")) {
        Serial.println("Error: commands must start with '$'");
        return;
    }
    // strip off the '$'
    line = line.substring(1);
    line.trim();

    int sp1 = line.indexOf(' ');
    if (sp1 < 0) {
        Serial.println("Usage: $<group> <command> [args...]");
        return;
    }

    String grp  = line.substring(0, sp1);
    String rest = line.substring(sp1 + 1);
    rest.trim();

    int sp2     = rest.indexOf(' ');
    String cmd  = (sp2 < 0 ? rest : rest.substring(0, sp2));
    String args = (sp2 < 0 ? String() : rest.substring(sp2 + 1));

    for (size_t i = 0; i < groupCount; ++i) {
        if (grp.equalsIgnoreCase(groups[i].name)) {
            for (size_t j = 0; j < groups[i].commandCount; ++j) {
                if (cmd.equalsIgnoreCase(groups[i].commands[j].name)) {
                    groups[i].commands[j].function(args);
                    return;
                }
            }
        }
    }

    Serial.println("Unknown command or group.");
}
