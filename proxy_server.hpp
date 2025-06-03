#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include "lru_cache.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class ProxySession : public std::enable_shared_from_this<ProxySession> {
public:
    ProxySession(tcp::socket socket, LRUCache<std::string, std::string>& cache);
    void run();

private:
    tcp::socket socket_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;
    LRUCache<std::string, std::string>& cache_;

    void do_read();
    void handle_request();
    void forward_request();
    void send_response();
};

class ProxyServer {
public:
    ProxyServer(net::io_context& ioc, tcp::endpoint endpoint, size_t cacheSize);

private:
    tcp::acceptor acceptor_;
    LRUCache<std::string, std::string> cache_;
    void do_accept();
};

