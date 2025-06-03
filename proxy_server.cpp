#include "proxy_server.hpp"
#include <iostream>

ProxySession::ProxySession(tcp::socket socket, LRUCache<std::string, std::string>& cache)
    : socket_(std::move(socket)), cache_(cache) {}

void ProxySession::run() {
    do_read();
}

void ProxySession::do_read() {
    auto self = shared_from_this();
    http::async_read(socket_, buffer_, req_,
        [self](beast::error_code ec, std::size_t) {
            if (!ec)
                self->handle_request();
        });
}

void ProxySession::handle_request() {
    std::string cache_key(req_.target());

    std::string cached;
    if (cache_.get(cache_key, cached)) {
        http::response<http::string_body> res;
        res.result(http::status::ok);
        res.version(req_.version());
        res.body() = cached;
        res.set(http::field::content_type, "text/html");
        res.prepare_payload();
        res_ = std::move(res);
        return send_response();
    }

    forward_request();
}

void ProxySession::forward_request() {
    auto self = shared_from_this();
    auto resolver = std::make_shared<tcp::resolver>(socket_.get_executor());

    std::string host(req_["Host"]);
    std::string port = "80";

    std::cout << "[*] Resolving host: " << host << ":" << port << std::endl;

    resolver->async_resolve(host, port,
        [self, resolver, host](beast::error_code ec, tcp::resolver::results_type results) {
            if (ec) {
                std::cerr << "[!] Resolve error: " << ec.message() << "\n";
                return;
            }
            std::cout << "[*] Host resolved.\n";

            auto stream = std::make_shared<beast::tcp_stream>(self->socket_.get_executor());
            stream->expires_after(std::chrono::seconds(30));
            stream->async_connect(results,
                [self, stream](beast::error_code ec, tcp::resolver::results_type::endpoint_type ep) {
                    if (ec) {
                        std::cerr << "[!] Connect error: " << ec.message() << "\n";
                        return;
                    }
                    std::cout << "[*] Connected to upstream: " << ep << "\n";

                    auto req = std::make_shared<http::request<http::string_body>>(self->req_);
                    std::string full_target = std::string(req->target());

                    // Преобразуем в путь
                    auto pos = full_target.find("://");
                    if (pos != std::string::npos) {
                        full_target = full_target.substr(pos + 3);
                        pos = full_target.find('/');
                        full_target = (pos != std::string::npos) ? full_target.substr(pos) : "/";
                    }
                    req->target(full_target);

                    std::cout << "[*] Forwarding request to target: " << full_target << "\n";

                    http::async_write(*stream, *req,
                        [self, stream, req](beast::error_code ec, std::size_t bytes_transferred) {
                            if (ec) {
                                std::cerr << "[!] Write to upstream failed: " << ec.message() << "\n";
                                return;
                            }
                            std::cout << "[*] Request sent (" << bytes_transferred << " bytes)\n";

                            auto buffer = std::make_shared<beast::flat_buffer>();
                            auto res = std::make_shared<http::response<http::string_body>>();

                            http::async_read(*stream, *buffer, *res,
							[self, buffer, res, stream](beast::error_code ec, std::size_t bytes_read) {
										if (ec) {
											std::cerr << "[!] Read from upstream failed: " << ec.message() << "\n";
											return;
										}
									std::cout << "[*] Response received (" << bytes_read << " bytes)\n";

									self->res_ = *res;
									self->cache_.put(std::string(self->req_.target()), res->body());
									self->send_response();
							});

                        });
                });
        });
}
void ProxySession::send_response() {
    auto self = shared_from_this();
    http::async_write(socket_, res_,
        [self](beast::error_code ec, std::size_t) {
            self->socket_.shutdown(tcp::socket::shutdown_send, ec);
        });
}

ProxyServer::ProxyServer(net::io_context& ioc, tcp::endpoint endpoint, size_t cacheSize)
    : acceptor_(ioc), cache_(cacheSize) {
    beast::error_code ec;
    acceptor_.open(endpoint.protocol(), ec);
    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    acceptor_.bind(endpoint, ec);
    acceptor_.listen(net::socket_base::max_listen_connections, ec);
    do_accept();
}

void ProxyServer::do_accept() {
    acceptor_.async_accept(
        [this](beast::error_code ec, tcp::socket socket) {
            if (!ec)
                std::make_shared<ProxySession>(std::move(socket), cache_)->run();
            do_accept();
        });
}

