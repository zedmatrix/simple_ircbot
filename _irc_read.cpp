#include <iostream>
#include <map>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

void writeToServer(tcp::socket &socket, const std::string &message);

// Function to handle responses based on keywords
std::string getResponse(const std::string& userMessage, const std::string& userNick) {
    // Predefined map of words/phrases to responses
    std::map<std::string, std::string> responses = {
        {"Hello", "Hi There ${NICK}!"},
        {"LFS", "Oh, Linux From Scratch?"},
        {"!ping", "Pong!"},
        {"google", "Search? "},
        {"GUI", "gooey?"}
    };

    // Look for the word in the predefined responses
    for (const auto& [word, response] : responses) {
        if (userMessage.find(word) != std::string::npos) {
            std::string responseWithNick = response;
            // Replace ${NICK} with the actual user's nickname
            size_t pos = responseWithNick.find("${NICK}");
            if (pos != std::string::npos) {
                responseWithNick.replace(pos, 7, userNick); // Replace ${NICK} with userNick
            }
            return responseWithNick;
        }
    }

    // Default response if no match found
    return "";
}

void readFromServer(tcp::socket &socket) {
    char data[512];

    while (true) {
        boost::system::error_code error;
        size_t len = socket.read_some(boost::asio::buffer(data), error);

        if (error == boost::asio::error::eof) {
            break; // Connection closed by the server
        } else if (error) {
            throw boost::system::system_error(error);
        }

        std::string message(data, len);
        std::cout << message; // Print the incoming server message

        // Respond to server PING to keep the connection alive
        if (message.find("PING") == 0) {
            std::string serverPingResponse = "PONG " + message.substr(5);
            writeToServer(socket, serverPingResponse);
        }

        // Detect and respond to messages
        size_t pos = message.find("PRIVMSG");
        if (pos != std::string::npos) {
            // Extract the actual user message (after PRIVMSG)
            size_t msgStart = message.find(":", pos);
            if (msgStart != std::string::npos) {
                std::string userMessage = message.substr(msgStart + 1);

                // Extract the user's nickname (this is simplistic and assumes a specific format)
                size_t nickEnd = message.find('!');
                std::string userNick = message.substr(1, nickEnd - 1);

                // Get the appropriate response
                std::string response = getResponse(userMessage, userNick);

                // If a response is found, send it to the channel or user
                if (!response.empty()) {
                    size_t channelPos = message.find('#');
                    if (channelPos != std::string::npos) {
                        std::string channel = message.substr(channelPos, message.find(' ', channelPos) - channelPos);
                        writeToServer(socket, "PRIVMSG " + channel + " :" + response);
                    }
                }
            }
        }
    }
}

