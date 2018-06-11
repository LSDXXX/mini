//
// Created by sida liang on 2018/6/7.
//

#ifndef MINI_MINISERVER_H
#define MINI_MINISERVER_H

#include <boost/asio.hpp>
#include <atomic>
#include <functional>
#include <memory>
#include "request.hpp"
#include "request_parser.hpp"
#include "Log.h"
#include <boost/utility/string_view.hpp>

class Buffer;
class TcpConnection;
struct HttpResult {

};
class HttpServer : boost::noncopyable{
public:
    typedef std::shared_ptr<TcpConnection> Connectionptr;
    typedef std::shared_ptr<http::server::request_parser> Parserptr;
    typedef std::shared_ptr<http::server::request> Requestptr;
    typedef std::shared_ptr<Buffer> Bufferptr;
    HttpServer(boost::asio::io_service& s, unsigned short port) :
            acc_(s,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),port)),
            start_(false){
        if(!acc_.is_open()) {
            BOOST_LOG_TRIVIAL(debug) << "[debug]" << "soket isn't open";
        }
    }

    /*
     * void handler(std::shared_ptr<TcpConnection> connptr);
     */
    template <typename Func>
    typename std::remove_reference<Func>::type set_connectcallback(Func&& f) {
        auto temp = on_connectcallback;
        on_connectcallback = std::forward<Func>(f);
        return temp;
    }

    /*
     * void handler(std::shared_ptr<TcpConnection> connptr,
     *              std::shared_ptr<Request> req,
     *              boost::string_view str);
     */
    template <typename Func>
    typename std::remove_reference<Func>::type set_readcallback(Func&& f) {
        auto temp = on_readcompletecallback;
        on_readcompletecallback = std::forward<Func>(f);
        return temp;
    }

    /*
     * void handler(std::shared_ptr<TcpConnection> connptr);
     */
    template <typename Func>
    typename std::remove_reference<Func>::type set_writecallback(Func&& f) {
        auto temp = on_writecompletecallback;
        on_writecompletecallback = std::forward<Func>(f);
        return temp;
    }
    void start();

private:
    void do_accept();
    void http_requestcallback(Parserptr parser,Requestptr req,Connectionptr connptr,Bufferptr buf,size_t s);
    void http_datacallback(
            int nlen,
            Requestptr req,
            Connectionptr connptr,
            Bufferptr buf,
            std::size_t n);
    std::function<void(Connectionptr)> on_connectcallback;
    std::function<void(Connectionptr)> on_writecompletecallback;
    std::function<void(Connectionptr,Requestptr,boost::string_view)> on_readcompletecallback;
    boost::asio::ip::tcp::acceptor acc_;
    std::atomic<bool> start_;
};




#endif //MINI_MINISERVER_H
