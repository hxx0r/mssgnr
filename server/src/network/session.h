// server/src/session.h
#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <memory>
#include "protocol/messages.pb.h"  // Сгенерированный protobuf заголовок

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket);
    void start();
    
private:
    void do_read_header();
    void do_read_body(uint32_t body_length);
    void process_message(const std::vector<char>& buffer);
    void send_message(const messenger::Envelope& envelope);
    
    tcp::socket socket_;
    std::vector<char> read_buffer_;
};

#endif