
#include <iostream>
// #include <mysql.h>
#include <mysql-cppconn/mysqlx/xdevapi.h> // /usr/include/mysql-cppconn/mysqlx/

#include <string>  // iostream 中估计导入了。

#include <ctime>
#include <chrono>
#include <iomanip>

// #included <mysql_driver.h>
// 老式的连接 ( 就是3306 端口。 这里的代码全是新的连接方式，走 33060) ，走 /usr/include/mysql-cppcon/jdbc
// jdbc文件夹下有  mysql_driver.h, mysql_connection.h, mysql_error.h, cppconn/下面还有一大堆。比如 statement.h, resultset.h
// 代码：
// https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-examples-complete-example-1.html


// connector api
// https://dev.mysql.com/doc/dev/connector-cpp/latest/
// https://dev.mysql.com/doc/dev/connector-cpp/latest/group__devapi__res.html
// https://dev.mysql.com/doc/dev/connector-cpp/latest/classmysqlx_1_1abi2_1_1r0_1_1SqlResult.html


// my_test 需要C++20
// g++ mysql_demo.cpp -l mysqlcppconn8   // 必须有8，不带8 link失败，不知道 这里的8 和 /use/include/mysql-cppcon-8 这个有什么关系？ 但是我头文件不带8
// ./a.out

std::string mysql_pwd = "123456";


void mysql_nosql() {

    //////////////////////
    {
        // collection,  it is nosql
        //
        // https://dev.mysql.com/doc/refman/8.0/en/document-store.html
        // https://dev.mysql.com/doc/x-devapi-userguide/en/
        using namespace mysqlx;

        // c++ code in bottom
        // https://dev.mysql.com/doc/x-devapi-userguide/en/working-with-collections-basic-crud.html

        // ok
        // Session session(33060, "root", mysql_pwd);
        // Schema db = session.getSchema("meta");
        // Collection myColl = db.createCollection("my_coll");

        // myColl.add(R"({"name":"asd", "age":111})").execute();
        // myColl.add(R"({"name":"awe", "age":222})").execute();
        // myColl.add(R"({"name":"axc", "age":333})").execute();

        // DocResult dr = myColl.find("name like :param1 and age < :param2").limit(1).bind("param1", "a%").bind("param2", 223).execute();
        // std::cout<<dr.fetchOne();
        // db.dropCollection("my_coll");
    }

}


// 2种转换， 使用 ctime，  使用 chrono
// https://www.geeksforgeeks.org/date-and-time-parsing-in-cpp/
// // function to parse a date or time string.
// time_t parseDateTime(const char* datetimeString, const char* format)
// {
//     struct tm tmStruct;
//     strptime(datetimeString, format, &tmStruct);
//     return mktime(&tmStruct);
// }
 
// // Function to format a time_t value into a date or time string.
// string DateTime(time_t time, const char* format)
// {
//     char buffer[90];
//     struct tm* timeinfo = localtime(&time);
//     strftime(buffer, sizeof(buffer), format, timeinfo);
//     return buffer;
// }


std::chrono::system_clock::time_point now_time_point() {
    return std::chrono::system_clock::now();
}

std::chrono::system_clock::time_point string_2_time_point(std::string& str) {
    std::tm tm2 = {};
    std::istringstream ss(str);
    ss >> std::get_time(&tm2, "%Y-%m-%d %H:%M:%S");
    return std::chrono::system_clock::from_time_t(mktime(&tm2));
}

std::string time_point_2_string(std::chrono::system_clock::time_point& tp) {
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    tm* tm2 = localtime(&tt);
    char buffer[50];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm2);
    return buffer;
}



// 得 C++ 20, 用到了 std::chrono::year
// https://forums.mysql.com/read.php?167,706530,706530
static inline std::vector<uint64_t> mysqlx_raw_as_u64_vector(const mysqlx::Value& in_value)
{
    std::vector<uint64_t> out;

    const auto bytes = in_value.getRawBytes();
    auto ptr = reinterpret_cast<const std::byte*>(bytes.first);
    auto end = reinterpret_cast<const std::byte*>(bytes.first) + bytes.second;

    while (ptr != end) {
        static constexpr std::byte carry_flag{0b1000'0000};
        static constexpr std::byte value_mask{0b0111'1111};

        uint64_t v = 0;
        uint64_t shift = 0;
        bool is_carry;
        do {
            auto byte = *ptr;
            is_carry = (byte & carry_flag) == carry_flag;
            v |= std::to_integer<uint64_t>(byte & value_mask) << shift;

            ++ptr;
            shift += 7;
        } while (is_carry && ptr != end && shift <= 63);

        out.push_back(v);
    }

    return out;
} 


// 这段代码 有时区问题， 就是 使用了 0时区。 而不是 数据库保存什么就是 什么。
// 数据库 13点， 这里输出的是 21点。  所以 认为数据 是0时区的，  然后输出的时候 转为 +8时区。
// time_point 确实是不带 时区的，就是默认0时区。
// zoned_time 是带时区的。  所以估计  read_time_point 返回的 time_point 中 北京时间13点的话，保存的是 5点。
// 时间这块，有点烂啊， time_t, tm, gmtime(), localtime() ,  + chrono
// 或者应该 不要使用 time_t，  只使用 chrono。 现在是 混在一起了。   time_t  tm 是 ctime 的，c的东西。
// tm 是struct，包含了 年月日，时分秒 作为 属性 ，  所以没有 时区属性
// time_t 是一个 int32， 保存了 1970年到现在的秒数， 也没有时区属性。  没有时区属性，那就可以认为是 当前时区。 不可能认为 utc的
// time_point 是 clock + duration ，  +0时区， 所以 tm time_t 的 当前时区 转换为 +0的 time_point 时， time_point 中保存的时间会 -8小时
    // std::cout<<"hour : "<<(std::chrono::duration_cast<std::chrono::hours>(tp.time_since_epoch()).count()) % 24<<std::endl;

// 这个需要C++20，所以注释掉了。
// std::chrono::system_clock::time_point read_date_time(const mysqlx::Value& value)
// {
//     const auto vector = mysqlx_raw_as_u64_vector(value);
//     if (vector.size() < 3)
//         throw std::out_of_range{"Value is not a valid DATETIME"};

//     // 。。这是什么写法。。yyyy mm dd， / 能自动转换？ 是当作 yyyy/MM/dd 来初始化？ 就是那种 literal value ？
//     auto ymd = std::chrono::year{static_cast<int>(vector.at(0))} / static_cast<int>(vector.at(1)) / static_cast<int>(vector.at(2));
//     auto sys_days = std::chrono::sys_days{ymd};

//     auto out = std::chrono::system_clock::time_point(sys_days);

//     auto it = vector.begin() + 2;
//     auto end = vector.end();

//     if (++it == end)
//     return out;
//     out += std::chrono::hours{*it};

//     if (++it == end)
//     return out;
//     out += std::chrono::minutes{*it};

//     if (++it == end)
//     return out;
//     out += std::chrono::seconds{*it};

//     if (++it == end)
//     return out;
//     out += std::chrono::microseconds{*it};

//     return out;
// } 
///////////////////////

std::chrono::system_clock::time_point read_time_point(const mysqlx::Value& value) {

    const std::vector<uint64_t> vu64 = mysqlx_raw_as_u64_vector(value);
    
    std::tm tm2 = {};
    
    // std::cout<<vu64.size()<<std::endl; // 6
    // std::cout<<static_cast<int>(vu64[0])<<std::endl; // 2024
    // std::cout<<static_cast<int>(vu64[1])<<std::endl; // 6
    // std::cout<<static_cast<int>(vu64[2])<<std::endl; // 17
    // std::cout<<static_cast<int>(vu64[3])<<std::endl; // 13
    // std::cout<<static_cast<int>(vu64[4])<<std::endl; // 14
    // std::cout<<static_cast<int>(vu64[5])<<std::endl; // 3
    // std::cout<<static_cast<int>(vu64[6])<<std::endl; // 0, 越界了。

    // std::cout<<vu64[0]<<std::endl;  // 2024 ... unsigned 64 -> int   ... 不需要 u64 那么大。 但2024怎么保存。 先用吧。

    tm2.tm_year = vu64[0] - 1900;
    tm2.tm_mon = vu64[1] - 1;
    tm2.tm_mday = vu64[2];
    tm2.tm_hour = vu64[3];
    tm2.tm_min = vu64[4];
    tm2.tm_sec = vu64[5];

    std::time_t tt = mktime(&tm2);
    
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(tt);
    
    return tp;
}



void cpp_time_mysql_timestamp() {

    std::chrono::time_point tp = now_time_point();
    std::string s2 = time_point_2_string(tp);
    std::cout<<s2<<std::endl;
    tp = string_2_time_point(s2);
    // std::cout<<tp<<std::endl; // no operator<<

    mysqlx::Session session("localhost", 33060, "root", mysql_pwd);
    std::cout<<" connected to mysql "<<std::endl;
    mysqlx::Schema db = session.getSchema("meta");
    session.sql("use meta;").execute();
    mysqlx::SqlResult result = session.sql("select * from kv").execute();
    mysqlx::Row r;
    // while ((r = result.fetchOne())) {
    while ((r = result.fetchOne())) {
        // for (int i = 0; i < r.colCount(); ++i) {
        //     std::cout<<r[i]<<" ";
        // }
        std::cout<<r.colCount()<<" - "<<r[0].get<int>()<<" ";
        std::cout<<r[1].get<std::string>()<<' ';
        std::cout<<r[3].get<std::string>()<<' ';
        std::cout<<typeid(r[3]).name()<<' ';
        // std::cout<<typeid(r[4]).name()<<' '; // 和上面一样 N6mysqlx4abi22r05ValueE
        // std::cout<<
        // https://dev.mysql.com/doc/dev/connector-cpp/latest/classmysqlx_1_1abi2_1_1r0_1_1Value.html#a1d1cfd8ffb84e947f82999c682b666a7
        std::cout<<r[3].getType()<<' '; // 6 -> string
        std::cout<<r[4].getType()<<' '; // 8 -> raw bytes...
        std::cout<<r[4]<<' '; // <7 raw bytes>
        std::cout<<r[5].getType()<<' '<<r[5]<<' '; // 8 <7 raw bytes>
        // std::cout<<r[5][1]<<' '; // error
        
        std::cout<<r.get(5)<<' ';  // == r[5]

// https://dev.mysql.com/doc/dev/connector-cpp/latest/classmysqlx_1_1abi2_1_1r0_1_1Row.html#aeb0c3821b9b1a259ae61bf8bab0b9221
// https://dev.mysql.com/doc/dev/connector-cpp/latest/classmysqlx_1_1abi2_1_1r0_1_1bytes.html
        mysqlx::bytes bs = r.getBytes(5);
        std::cout<<"\n----\n";
        for (auto a : bs) {
            std::cout<<a<<" ";
        }
        std::cout<<"\n----\n";

        // auto aa = read_date_time(r[5]);
        // std::string s3 = time_point_2_string(aa);
        // std::cout<<s3<<std::endl;

        std::cout<<std::endl;


        std::chrono::system_clock::time_point tp = read_time_point(r[5]);

        // 5,  实际数据库时间是 13点
        std::cout<<"hour : "<<(std::chrono::duration_cast<std::chrono::hours>(tp.time_since_epoch()).count()) % 24<<std::endl;


        std::cout<<time_point_2_string(tp)<<std::endl;
        

        ///////////////////////////////
        // insert

        std::string k = "asd__qwe";
        std::string v = "zzzvvv";
        std::string c = "ccc";

        using namespace std::literals::chrono_literals;
        std::chrono::time_point tp2 = tp + 1h;
        std::string tm = time_point_2_string(tp2);
        auto tp3 = std::chrono::system_clock::now();
        std::string tm22 = time_point_2_string(tp3);
        mysqlx::SqlResult result = session.sql("insert into kv (k,v,c,u,t) value ('" + k + "','" + v + "','" + c + "','" + tm + "','" + tm22 + "');").execute();

        // 找不到 pareparedstatement。。 老版的有。 mysqlx 没有找到。

        // 最多 table.tableInsert() 
        // https://dev.mysql.com/doc/dev/connector-cpp/latest/classmysqlx_1_1abi2_1_1r0_1_1Table.html#a0e4a4ee120437471cf798b55c5a30910

        break;
    }

}

void my_test() {

    cpp_time_mysql_timestamp();



    std::cout<<"  end of my_test"<<std::endl;
}

int main() {

    mysql_nosql();

    my_test();

    // no
    // std::cout<<"mysql client version: " << mysql_get_client_info() << std::endl;

    // ok
    // g++ mysql_demo.cpp -l mysqlcppconn8
    // ./a.out
    //
    // mysqlx::Session session("localhost", 33060, "root", mysql_pwd);
    // std::cout<<" connected to mysql "<<std::endl;
    // mysqlx::Schema db = session.getSchema("meta");
    // session.sql("use meta;").execute();
    // mysqlx::SqlResult result = session.sql("select * from kv").execute();
    // mysqlx::Row r;
    // while ((r = result.fetchOne())) {
    //     for (int i = 0; i < r.colCount(); ++i) {
    //         std::cout<<r[i]<<" ";
    //     }
    //     std::cout<<std::endl;
    // }


    // ok
    // 1. g++ mysql_demo.cpp -l mysqlcppconn8
    // 2. ./a.out
    //
    // {
    //     using namespace mysqlx;
    //     SessionSettings settings(SessionOption::HOST, "localhost", SessionOption::PORT, 33060);
    //     settings.set(SessionOption::USER, "root");
    //     settings.set(SessionOption::PWD, mysql_pwd);
    //     Session sess(settings);
    //     Schema db = sess.getSchema("eee");
    //     std::cout<<"ok"<<std::endl;
    // }


    // ok
    // 直接执行sql
    // g++命令 同上
    // {
    //     using namespace mysqlx;
    //     Session sess("localhost", 33060, "root", mysql_pwd);
    //     // Schema db = sess.getSchema("meta"); // 这个没有 选择 database 的功能。
    //     sess.sql("use meta").execute();

    //     RowResult rs = sess.sql("select * from kv").execute();
    //     for (auto it = rs.begin(); it != rs.end(); ++it) {
    //         std::cout<<(*it).get(0).get<int>()<<" ";
    //         // std::cout<<(it->get(1)).get<std::string>()<<" "; // ‘->’的基操作数具有非指针类型
    //         std::cout<<(*it).get(1).get<std::string>()<<" ";
    //         std::cout<<(*it).get(2).get<std::string>()<<" ";
    //         std::cout<<(*it).get(3).get<std::string>()<<" ";
    //         std::cout<<std::endl;
    //     }
    // }


    // ok
    // mysqlx, 以前都没有听过 这种接口。
    // g++ 命令同上
    // {
    //     mysqlx::Session sess("localhost", 33060, "root", mysql_pwd);
    //     mysqlx::Schema db = sess.getSchema("meta");
    //     mysqlx::Table kv = db.getTable("kv");
    //     mysqlx::RowResult rs = kv.select("id", "k", "v").where("id<10").execute();
    //     for (auto it = rs.begin(); it != rs.end(); ++it) {
    //         std::cout<<(*it).get(0).get<int>()<<" ";
    //         std::cout<<(*it).get(1).get<std::string>()<<" ";
    //         std::cout<<(*it).get(2).get<std::string>()<<" ";
    //         std::cout<<std::endl;
    //     }
    // }
    

    // ok
    // connect pool
    // same g++ command
    // {
    //     // 密码带@
    //     // https://dev.mysql.com/doc/connector-j/en/connector-j-reference-jdbc-url-format.html
    //     // Any reserved characters for URLs (for example, /, :, @, (, ), [, ], &, #, =, ?, and space) that appear in any part of the connection URL must be percent encoded.
    //     // % + ascii的十六进制
    //     //                                   %40  ==  @
    //     mysqlx::Client cli("mysqlx://root:" + mysql_pwd + "@localhost", mysqlx::ClientOption::POOL_MAX_SIZE, 2);
    //     mysqlx::Session sess = cli.getSession();
    //     mysqlx::RowResult rr = sess.getSchema("meta").getTable("kv").select("k", "v").where("id < 10").execute();
    //     for (auto it = rr.begin(); it != rr.end(); ++it) {
    //         std::cout<<(*it).get(0).get<std::string>()<<" ";
    //         std::cout<<(*it).get(1).get<std::string>()<<" ";
    //         std::cout<<std::endl;
    //     }
    //     cli.close();
    // }


    // ok
    // try {
    //     using namespace mysqlx;
    //     Session sess("localhost", 33060, "root", mysql_pwd);
    //     Schema schema = sess.getSchema("meta");
    //     Table kv = schema.getTable("kv");

    //     auto statement = kv.select().where("id=:param1"); // select("k","v")
    //     RowResult rs = statement.bind("param1", 1).execute();
    //     for (auto it = rs.begin(); it != rs.end(); ++it) {
    //         std::cout<<(*it).get(1).get<std::string>()<<" ";
    //         std::cout<<(*it).get(2).get<std::string>()<<" ";
    //         std::cout<<std::endl;
    //     }
    //     rs = statement.bind("param1", "1").execute();
    //     // std::cout<<rs.getColumnCount()<<", "<<rs.getColumnNames()[0]<<std::endl;
    //     std::cout<<rs.getColumnCount()<<std::endl;
    // }
    // catch (const mysqlx::Error &ex) {
    //     std::cout<<ex.what()<<std::endl;
    //     return 1;
    // }
    // catch (std::exception& ex) {
    //     std::cout<<ex.what()<<std::endl;
    //     return 2;
    // }
    // catch (const char* ex) {
    //     std::cout<<ex<<std::endl;
    //     return 3;
    // }


    //////////////////////
    {
        // table.op
        using namespace mysqlx;

        // Session sess("localhost", 33060, "root", mysql_pwd);
        // Schema db = sess.getSchema("meta");
        // Table table = db.getTable("kv");

        // ok
        // insert 声明列名， values 就是值，可以多个
        // table.insert("k","v").values("a_test_k", "a_test_v").values("another_test_k", "another_test_v").execute();

        // ok
        // table.update().set("k", "test_change").set("v", "test_vvvv").where("id=2").execute();

        // no delete, use remove
        // table.delete().where("id=2").execute();
        // ok
        // table.remove().where("id=2").execute();

        
    }

    {
        // transaction
        // using namespace mysqlx;
        // Session session(33060, "root", mysql_pwd);
        // // Schema db = session.getSchema("meta");
        // // Table table = db.getTable("kv");

        // session.sql("use meta").execute();
        // session.startTransaction();
        // session.sql("update kv set v='asdasdasd' where id=3").execute();
        // session.commit(); // 不加这行，上面的update不会生效
    }

    // https://dev.mysql.com/doc/refman/8.0/en/savepoint.html
    
    
    return 0;
}



/*

server: mysql 8.4

client: c++, c++-jdbc, c++-devel 8.4

header: /usr/include/mysql-cppconn/mysqlx/


代码来源：
https://github.com/BarretRen/XDevAPIWithCpp


mysql X devapi doc
https://dev.mysql.com/doc/dev/connector-cpp/latest/



mysql all doc
https://dev.mysql.com/doc/


mysql forum
https://forums.mysql.com/






*/

