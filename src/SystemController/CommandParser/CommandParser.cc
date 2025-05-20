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

    // 1) Require leading '$'
    if (!line.startsWith("$")) {
        Serial.println("Error: commands must start with '$'");
        return;
    }

    // 2) Strip off the '$'
    line = line.substring(1);
    line.trim();

    // 3) Split off <group>
    int sp1 = line.indexOf(' ');
    if (sp1 < 0) {
        Serial.println("Usage: $<group> <command> [args...]");
        return;
    }
    String group_name = line.substring(0, sp1);

    // 4) Split off <command> and [args...]
    String rest = line.substring(sp1 + 1);
    rest.trim();
    int sp2 = rest.indexOf(' ');
    String command_name;
    String arguments;
    if (sp2 < 0) {
        // no args
        command_name = rest;
        arguments    = String();
    } else {
        command_name = rest.substring(0, sp2);
        arguments    = rest.substring(sp2 + 1);
        arguments.trim();
    }

    // 5) Look up group
    for (size_t i = 0; i < group_count_; ++i) {
        const CommandGroup& grp = groups_[i];
        if (group_name.equalsIgnoreCase(grp.name)) {
            // 6) Look up command
            for (size_t j = 0; j < grp.command_count; ++j) {
                const Command& cmd = grp.commands[j];
                if (command_name.equalsIgnoreCase(cmd.name)) {
                    // 7) Optional: require args if arg_count>0
                    if (cmd.arg_count > 0 && arguments.length() == 0) {
                        Serial.print("Error: '");
                        Serial.print(cmd.name);
                        Serial.println("' requires arguments.");
                        return;
                    }
                    // 8) Dispatch!
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
