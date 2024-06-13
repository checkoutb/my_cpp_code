
#include <iostream>
// #include <mysql.h>
#include <mysql-cppconn/mysqlx/xdevapi.h> // /usr/include/mysql-cppconn/mysqlx/

#include <string>  // iostream 中估计导入了。




int main() {

    // no
    // std::cout<<"mysql client version: " << mysql_get_client_info() << std::endl;

    // ok
    // g++ mysql_demo.cpp -l mysqlcppconn8
    // ./a.out
    // mysqlx::Session session("localhost", 33060, "root", "8765@ghjk");
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
    // {
    //     using namespace mysqlx;
    //     SessionSettings settings(SessionOption::HOST, "localhost", SessionOption::PORT, 33060);
    //     settings.set(SessionOption::USER, "root");
    //     settings.set(SessionOption::PWD, "8765@ghjk");
    //     Session sess(settings);
    //     Schema db = sess.getSchema("eee");
    //     std::cout<<"ok"<<std::endl;
    // }


    // ok
    // 直接执行sql
    // g++命令 同上
    // {
    //     using namespace mysqlx;
    //     Session sess("localhost", 33060, "root", "8765@ghjk");
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
    //     mysqlx::Session sess("localhost", 33060, "root", "8765@ghjk");
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
    //     mysqlx::Client cli("mysqlx://root:8765%40ghjk@localhost", mysqlx::ClientOption::POOL_MAX_SIZE, 2);
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
    //     Session sess("localhost", 33060, "root", "8765@ghjk");
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

        // Session sess("localhost", 33060, "root", "8765@ghjk");
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


    //////////////////////
    {
        // collection,  it is nosql
        // https://dev.mysql.com/doc/refman/8.0/en/document-store.html
        // https://dev.mysql.com/doc/x-devapi-userguide/en/
        using namespace mysqlx;

        // c++ code in bottom
        // https://dev.mysql.com/doc/x-devapi-userguide/en/working-with-collections-basic-crud.html

        // ok
        // Session session(33060, "root", "8765@ghjk");
        // Schema db = session.getSchema("meta");
        // Collection myColl = db.createCollection("my_coll");

        // myColl.add(R"({"name":"asd", "age":111})").execute();
        // myColl.add(R"({"name":"awe", "age":222})").execute();
        // myColl.add(R"({"name":"axc", "age":333})").execute();

        // DocResult dr = myColl.find("name like :param1 and age < :param2").limit(1).bind("param1", "a%").bind("param2", 223).execute();
        // std::cout<<dr.fetchOne();
        // db.dropCollection("my_coll");
    }


    {
        // transaction
        // using namespace mysqlx;
        // Session session(33060, "root", "8765@ghjk");
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

