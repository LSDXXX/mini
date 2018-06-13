//
// Created by sida liang on 2018/6/12.
//

#ifndef MINI_MYSQL_H
#define MINI_MYSQL_H
#include <mysql/mysql.h>
#include <boost/noncopyable.hpp>
#include <stdexcept>
#include <string>
#include <memory>
#include <map>
#include <boost/utility.hpp>
#include <vector>
//#include "Log.h"

enum FT
{
    DB_INT,
    DB_STR
};

class SqlRes : boost::noncopyable{
public:

    typedef std::map<std::string,
            boost::string_view> RowRes;
    SqlRes(MYSQL_RES* re) : res_(re),
                            is_prepare(false){}

    void prepare();

    size_t get_num_row() const{
        return data_.size();
    }

    const RowRes&
            operator[](std::size_t s) const{
        return data_[s];
    }

    const RowRes* get_cur_row()  {
        if(it_ != data_.end())
            return &(*it_++)
        return nullptr;
    }


    ~SqlRes();
private:
    MYSQL_RES* res_;
    bool is_prepare;
    std::vector<RowRes> data_;
    std::vector<RowRes>::iterator it_;
};



class MySql : boost::noncopyable{
public:


    typedef std::pair<FT,std::string> ValueMap;
    typedef std::shared_ptr<SqlRes> Resultptr;
    MySql() {
        mysql_thread_init();
        mysql_init(&sql_);
    }

    static void init() {
        if(mysql_library_init(0,NULL,NULL) != 0) {
            throw std::runtime_error("mysql init failed");
        }
    }

    static void close() {
        mysql_library_end();
    }

    bool connect(const std::string& ip,
                 const std::string& usr,
                 const std::string& pwd,
                 const std::string& db,
                 unsigned int port) {
        return (mysql_real_connect(&sql_,ip.data(),usr.data(),pwd.data(),
        db.data(),port,NULL,0) != nullptr);
    }

    size_t query(const std::string& ssql) {
        if(ssql == last_sql)
            return true;
        last_sql = ssql;
        return mysql_real_query(&sql_,ssql.data(),ssql.size());
    }

    Resultptr get_result();

    static std::string build_select_sql(const std::string& tabnames,
                                 const std::string& cols,
                                 const std::string& conditions);

    static std::string build_insert_sql(const std::string& tabnames,
                                const std::map<std::string,ValueMap>& info);

    static std::string build_updata_sql(const std::string& tabnames,
                                const std::map<std::string,ValueMap>& info,
                                const std::string& conditions);

    std::string last_error_message() {
        const char* e = mysql_error(&sql_);
        return std::string(e);
    }

    ~MySql() {
        mysql_close(&sql_);
        mysql_thread_end();
    }
private:
    MYSQL sql_;
    std::string last_sql;
};


#endif //MINI_MYSQL_H
