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

    // Extract group, command, and arguments
    String group_name;
    String command_name;
    String arguments;

    int space_idx = line.indexOf(' ');
    if (space_idx < 0) {
        // Only group was provided
        group_name = line;
    } else {
        group_name   = line.substring(0, space_idx);
        String rest  = line.substring(space_idx + 1);
        rest.trim();

        int space_idx2 = rest.indexOf(' ');
        if (space_idx2 < 0) {
            command_name = rest;
        } else {
            command_name = rest.substring(0, space_idx2);
            arguments    = rest.substring(space_idx2 + 1);
        }
    }

    // Find the group
    for (size_t i = 0; i < group_count_; ++i) {
        const CommandGroup& grp = groups_[i];
        if (group_name.equalsIgnoreCase(grp.name)) {
            // If no command given, run the first one (if available)
            if (command_name.length() == 0) {
                if (grp.command_count > 0) {
                    grp.commands[0].function(String());
                } else {
                    Serial.println("Error: no commands available for group.");
                }
                return;
            }

            // Otherwise, look for a matching command
            for (size_t j = 0; j < grp.command_count; ++j) {
                const Command& cmd = grp.commands[j];
                if (command_name.equalsIgnoreCase(cmd.name)) {
                    cmd.function(arguments);
                    return;
                }
            }

            Serial.println("Unknown command in group.");
            return;
        }
    }

    Serial.println("Unknown command group.");
}

