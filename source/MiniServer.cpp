//
// Created by sida liang on 2018/6/7.
//

#include "MiniServer.h"
#include "TcpConnection.h"
#include <memory>
#include "Log.h"
#include "reply.hpp"
void HttpServer::start() {

    if(!start_) {
        do_accept();
        start_ = true;
    }
    //acc_.async_accept(conn->get_socket(),);
//    acc_.async_accept()
}

void HttpServer::do_accept() {
    auto new_conn = std::make_shared<TcpConnection>
            (acc_.get_io_service());
    acc_.async_accept(*(new_conn->get_socket()),
    [new_conn,this](const boost::system::error_code& e) {
        //new_conn->async_read()
        BOOST_LOG_TRIVIAL(info) << "[info]" <<"new connector : "
                                   << new_conn->get_socket()->remote_endpoint().address().to_string();
        if(!e) {
            auto parser = std::make_shared<http::server::request_parser>();
            auto req = std::make_shared<http::server::request>();
            new_conn->async_read(std::bind(&HttpServer::http_requestcallback,this,
                                           parser,req,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
            if(on_connectcallback)
                on_connectcallback(new_conn);
        }
        do_accept();
    });
}

void HttpServer::http_requestcallback(HttpServer::Parserptr parser,
                                   HttpServer::Requestptr req,
                                   HttpServer::Connectionptr connptr,
                                   HttpServer::Bufferptr buf, size_t s) {

    http::server::request_parser::result_type state;
    char* cur = nullptr;
    auto beg_read = buf->get_data() + buf->get_read_pos();
    auto end_read = beg_read + buf->readable();
    std::tie(state,cur) = parser->parse(*req,beg_read,end_read) ;
    int nparsered = (int)(cur - beg_read);
    if(state == http::server::request_parser::result_type::good) {
        BOOST_LOG_TRIVIAL(info) << "[info]" << "Http request state : Good by " <<
                                    connptr->get_socket()->remote_endpoint().address();
        buf->commit(nparsered);
        auto& headers = req->headers;
        auto it = std::find_if(headers.begin(),headers.end(),[](const http::server::header& h)
        {
            return h.name == "Content-Length";
        });
        if(it == headers.end()) {
            if(on_readcompletecallback)
                on_readcompletecallback(connptr,req,boost::string_view());
        } else {
            int len = std::stoi(it->value);
            http_datacallback(len,req,connptr,buf,s);
        }
    } else if(state == http::server::request_parser::result_type::bad) {
        /*
         * to do handle bad
         */
        auto rep = http::server::reply::stock_reply(http::server::reply::status_type::bad_request);
        connptr->async_write([](Connectionptr){},rep.to_buffers());
        connptr->set_context(std::move(rep));
        BOOST_LOG_TRIVIAL(warning) << "[warnning]" << "Http request state : Bad by " <<
                                connptr->get_socket()->remote_endpoint().address();
    } else {
       buf->commit(nparsered);
       connptr->async_read(std::bind(&HttpServer::http_requestcallback,this,
       parser,req,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    }
}


void HttpServer::http_datacallback(int nlen,HttpServer::Requestptr req, HttpServer::Connectionptr connptr,
                                   HttpServer::Bufferptr buf,std::size_t n) {

    if (buf->readable() >= nlen) {
        BOOST_LOG_TRIVIAL(debug) << "[debug]" << "http data ：" << buf->get_all();
        if (on_readcompletecallback) {
            boost::string_view temp(buf->get_data() + buf->get_read_pos(),nlen);
            on_readcompletecallback(connptr,req,temp);
        }
    } else {
        connptr->async_read(std::bind(&HttpServer::http_datacallback, this,
                                      nlen, req, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );
    }
}
