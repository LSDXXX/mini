#include <iostream>
#include <boost/asio.hpp>
#include "request.hpp"
#include "request_parser.hpp"
#include <string>
#include <tuple>
#include "MiniServer.h"
#include <boost/log/trivial.hpp>
#include "Log.h"



using namespace boost::asio;
int main() {
    LOG_INIT("log.test");
    BOOST_LOG_TRIVIAL(debug) << " 1232323";
    io_service service;
    HttpServer server(service,12345);
    server.start();
    service.run();
}