// CommandParser.cc
#include "CommandParser.h"
#include <Arduino.h>  // for Serial

void CommandParser::set_groups(const CommandGroup* groups, size_t group_count) {
    groups_      = groups;
    group_count_ = group_count;
}

void CommandParser::parse_and_execute(const String& input) const {
    String line = input;
    line.trim();

    // Require leading '$'
    if (!line.startsWith("$")) {
        Serial.println("Error: commands must start with '$'");
        return;
    }

    // Strip off the '$'
    line = line.substring(1);
    line.trim();

    // Split off the group name
    int space_idx = line.indexOf(' ');
    if (space_idx < 0) {
        Serial.println("Usage: $<group> <command> [args...]");
        return;
    }

    String group_name = line.substring(0, space_idx);
    String rest       = line.substring(space_idx + 1);
    rest.trim();

    // Split off the command name
    int space_idx2    = rest.indexOf(' ');
    String command    = (space_idx2 < 0 ? rest : rest.substring(0, space_idx2));
    String arguments  = (space_idx2 < 0 ? String() : rest.substring(space_idx2 + 1));

    // Find and execute
    for (size_t i = 0; i < group_count_; ++i) {
        if (group_name.equalsIgnoreCase(groups_[i].name)) {
            for (size_t j = 0; j < groups_[i].command_count; ++j) {
                const Command& cmd = groups_[i].commands[j];
                if (command.equalsIgnoreCase(cmd.name)) {
                    cmd.function(arguments);
                    return;
                }
            }
        }
    }

    Serial.println("Unknown command or group.");
}
