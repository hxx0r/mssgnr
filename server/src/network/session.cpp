// server/src/session.cpp
#include "session.h"
#include <iostream>

Session::Session(tcp::socket socket)
    : socket_(std::move(socket)) {
}

void Session::start() {
    do_read_header();
}

void Session::do_read_header() {
    auto self(shared_from_this());
    
    // Читаем сначала заголовок (4 байта - размер сообщения)
    read_buffer_.resize(4);
    
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_buffer_, 4),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                // Преобразуем 4 байта в uint32_t
                uint32_t body_length = *reinterpret_cast<uint32_t*>(read_buffer_.data());
                do_read_body(body_length);
            } else {
                std::cout << "Error reading header: " << ec.message() << std::endl;
            }
        });
}

void Session::do_read_body(uint32_t body_length) {
    auto self(shared_from_this());
    
    // Читаем тело сообщения
    read_buffer_.resize(body_length);
    
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_buffer_, body_length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                process_message(read_buffer_);
                // Читаем следующее сообщение
                do_read_header();
            } else {
                std::cout << "Error reading body: " << ec.message() << std::endl;
            }
        });
}

void Session::process_message(const std::vector<char>& buffer) {
    try {
        messenger::Envelope envelope;
        if (envelope.ParseFromArray(buffer.data(), buffer.size())) {
            std::cout << "Received message type: " << envelope.type() << std::endl;
            
            // Обрабатываем разные типы сообщений
            if (envelope.type() == messenger::TEXT_MESSAGE) {
                messenger::TextMessage text_msg;
                if (text_msg.ParseFromString(envelope.payload())) {
                    std::cout << "Text message from " << text_msg.sender() 
                              << ": " << text_msg.content() << std::endl;
                    
                    // Эхо-ответ
                    messenger::TextMessage response;
                    response.set_sender("Server");
                    response.set_content("Echo: " + text_msg.content());
                    response.set_timestamp(time(nullptr));
                    
                    messenger::Envelope response_env;
                    response_env.set_type(messenger::TEXT_MESSAGE);
                    response_env.set_version(1);
                    response_env.set_payload(response.SerializeAsString());
                    
                    send_message(response_env);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Error processing message: " << e.what() << std::endl;
    }
}

void Session::send_message(const messenger::Envelope& envelope) {
    auto self(shared_from_this());
    
    // Сериализуем сообщение
    std::string serialized = envelope.SerializeAsString();
    uint32_t message_length = serialized.size();
    
    // Создаем буфер: сначала длина, потом данные
    std::vector<char> buffer(sizeof(message_length) + message_length);
    std::memcpy(buffer.data(), &message_length, sizeof(message_length));
    std::memcpy(buffer.data() + sizeof(message_length), serialized.data(), message_length);
    
    boost::asio::async_write(socket_,
        boost::asio::buffer(buffer),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (ec) {
                std::cout << "Error sending message: " << ec.message() << std::endl;
            }
        });
}