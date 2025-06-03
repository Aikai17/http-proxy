#include "proxy_server.hpp"
#include <iostream>
int main() {
    try {
        net::io_context ioc{1};
        ProxyServer server(ioc, tcp::endpoint(tcp::v4(), 8080), 100);
        ioc.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
}

