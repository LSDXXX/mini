cmake_minimum_required(VERSION 3.10)
project(mini)

set(CMAKE_CXX_STANDARD 11)

link_libraries(boost_system)

link_libraries(boost_log-mt)

link_libraries(boost_thread-mt)

link_libraries(boost_log_setup-mt)

add_executable(mini  reply.cpp reply.hpp Log.h request_parser.cpp request_parser.hpp
        main.cpp MiniServer.cpp MiniServer.h Buffer.cpp TcpConnection.cpp TcpConnection.h)