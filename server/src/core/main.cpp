// server/src/main.cpp
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include "session.h"  // Подключаем наш новый заголовок

using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        std::cout << "Server started on port " << port << std::endl;
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::cout << "New client connected" << std::endl;
                    std::make_shared<Session>(std::move(socket))->start();
                } else {
                    std::cout << "Accept error: " << ec.message() << std::endl;
                }
                do_accept();
            });
    }
    
    tcp::acceptor acceptor_;
};

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: server <port>\n";
            return 1;
        }
        
        boost::asio::io_context io_context;
        Server server(io_context, std::atoi(argv[1]));
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    
    return 0;
}