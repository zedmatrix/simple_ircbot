#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <map>
#include <string>
#include "_irc_read.cpp"
#include "_irc_write.cpp"

using boost::asio::ip::tcp;
std::string readCommand(const std::string& userInput, const std::string& currentChannel);
std::string getResponse(const std::string& userMessage, const std::string& userNick);
void readFromServer(tcp::socket &socket);

void writeToServer(tcp::socket &socket, const std::string &message) {
    boost::asio::write(socket, boost::asio::buffer(message + "\r\n"));
}


int main() {
    boost::asio::io_context io_context;
    tcp::socket socket(io_context);

    tcp::resolver resolver(io_context);
    boost::asio::connect(socket, resolver.resolve("irc.libera.chat", "6667"));

    std::string nick = "wIZzLe";
    std::string user = "cppBot 0 * :My IRC Bot";

    // Send NICK and USER to IRC server
    writeToServer(socket, "NICK " + nick);
    writeToServer(socket, "USER " + user);

    // Optionally join a channel
    std::string currentChannel = "#Qt6";
    writeToServer(socket, "JOIN " + currentChannel);

    // Launch a thread to read from the server
    std::thread reader(readFromServer, std::ref(socket));

    // Main loop: send messages or commands
    std::string input;
    while (getline(std::cin, input)) {
        std::string command = readCommand(input, currentChannel);
        if (!command.empty()) {
            writeToServer(socket, command);
        } else {
            writeToServer(socket, "PRIVMSG " + currentChannel + " :" + input);
        }
    }

    socket.close();
    reader.join();
    return 0;
}
