#include <map>
#include <string>
#include <iostream>

std::string readCommand(const std::string& userInput, std::string& currentChannel) {
    // Predefined map of words/phrases to commands
    std::string escape = std::string(1, char(1));
    std::map<std::string, std::string> commands = {
        {"/join ", "JOIN {CHANNEL}"},
        {"/part ", "PART {CHANNEL}"},
        {"/quit", "QUIT :{MESSAGE}"},
        {"/me ", "PRIVMSG {CHANNEL} :" + escape + "ACTION {MESSAGE}" + escape},
        {"/ping", "PRIVMSG {CHANNEL} :P-I-N-G"},
        {"/set ", "PRIVMSG {CHANNEL} :{MESSAGE}"}
    };

    // Look for the word in the predefined commands
    for (const auto& [word, command] : commands) {
        if (userInput.find(word) != std::string::npos) {
            // Extract the remainder of the userInput after the command word
            std::string message = userInput.substr(word.size());

            std::string newChannel = currentChannel;  // Start with current channel

            // Special handling for `/join #channel` and `/part #channel`
            if (word == "/join " || word == "/part ") {
                // The new channel is the message itself (after /join # or /part #)
                newChannel = message;
                currentChannel = newChannel;  // Update the current channel to the new one
            }

            // Special handling for `/set #channel message`
            if (word == "/set ") {
                // Find the first space after the channel name
                size_t spacePos = message.find(' ', 1); // Start searching after "#"
                if (spacePos != std::string::npos) {
                    // Extract the new channel name
                    newChannel = message.substr(0, spacePos);
                    // Extract the message after the space
                    message = message.substr(spacePos + 1);
                } else {
                    // Only a channel was provided, no message
                    newChannel = message;
                    message = "";
                }
                // Update the current channel to the new channel
                currentChannel = newChannel;
            }

            // Create a new command string by replacing placeholders
            std::string newCommand = command;

            // Replace {CHANNEL} with the newChannel (the updated channel)
            size_t pos = newCommand.find("{CHANNEL}");
            if (pos != std::string::npos) {
                newCommand.replace(pos, 9, newChannel); // Replace {CHANNEL} with newChannel
            }

            // Replace {MESSAGE} with the user message, if any
            pos = newCommand.find("{MESSAGE}");
            if (pos != std::string::npos) {
                newCommand.replace(pos, 9, message); // Replace {MESSAGE} with the remainder of the input
            }

            // Return the modified command string
            return newCommand;
        }
    }

    // If no command matched, return an empty string
    return "";
}
