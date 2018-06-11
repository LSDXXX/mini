#include <iostream>
#include <boost/asio.hpp>
#include "request.hpp"
#include "request_parser.hpp"
#include <string>
#include <tuple>
#include "MiniServer.h"
#include <boost/log/trivial.hpp>
#include "Log.h"
#include "request.hpp"
#include "reply.hpp"
#include "TcpConnection.h"


void test_readcallback(std::shared_ptr<TcpConnection> connptr,
                        std::shared_ptr<http::server::request> req,
                        boost::string_view str) {
    auto rep = std::make_shared<http::server::reply>
            (http::server::reply::stock_reply(
            http::server::reply::status_type::hello
            ));
    connptr->async_write([rep](std::shared_ptr<TcpConnection>){},
    rep->to_buffers());
}


using namespace boost::asio;
int main() {
    LOG_INIT("log.test");
    io_service service;
    HttpServer server(service,12345);
    server.set_readcallback(std::function<void(std::shared_ptr<TcpConnection>,
                                               std::shared_ptr<http::server::request>,
                                               boost::string_view)>
                                    (test_readcallback));
    server.start();
    service.run();
}