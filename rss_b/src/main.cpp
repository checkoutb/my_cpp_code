
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <optional>

#include <ctime>
#include <chrono>
// #include <iomanip>

#include <thread>

#include "../include/pugixml.cpp"

#include <mysql-cppconn/mysqlx/xdevapi.h> // /usr/include/mysql-cppconn/mysqlx/

#include <unistd.h>

// #include <cstdint>
// #include <cassert>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>



#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../include/httplib.h"



// `cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/mnt/239G/z_other_git/vcpkg/scripts/buildsystems/vcpkg.cmake`
// `cmake --build build`
// `./build/main`


namespace rss_secret {
    const std::string mysql_pwd = "123456";
    // const std::string mongodb_pwd = "";


}

namespace rss_http { // rss 可以升级一档命名空间

std::optional<std::string> getRSS(std::string server, std::string url_path) {
    std::optional<std::string> optstr;
    httplib::Client cli(server);
    std::cout<<"start to request "<<server<<", "<<url_path<<std::endl;
    if (httplib::Result res = cli.Get(url_path)) {
        if (res->status == httplib::StatusCode::OK_200) {
            optstr.emplace(res->body);
        } else {
            // 这种情况下，错误信息是保存在 error 还是 body？ 可能是body中
            std::cout<<(res->status)<<" ERROR bad http response: "<<httplib::to_string(res.error())<<std::endl;
        }
    } else {
        std::cout<<"ERROR http failed: "<<httplib::to_string(res.error())<<std::endl;
    }
    std::cout<<"end of request rss"<<std::endl;
    return optstr;
}


}

namespace rss_mysql {
    std::string time_point_2_string(std::chrono::system_clock::time_point&);
}

namespace rss_timer {

using namespace std::chrono;

void sleep_to_next_hour() {
    time_point now2 = system_clock::now();
    std::cout<<getpid()<<" now is: "<<rss_mysql::time_point_2_string(now2)<<" ";
    time_point next = std::chrono::round<std::chrono::hours>(now2 + std::chrono::hours(1)) + std::chrono::seconds(5);
    seconds snds = duration_cast<seconds>(next - now2);
    std::cout<<"will sleep seconds: "<<snds.count()<<std::endl;
    std::this_thread::sleep_for(snds);
}


}

namespace rss_mysql {

using namespace std::chrono;

char buffer[20]; // yyyy-MM-dd hh:mm:ss    多线程不安全，多线程时放到方法内部
std::string time_point_2_string(std::chrono::system_clock::time_point& tp) {
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    tm* tm2 = localtime(&tt);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm2);
    return buffer;
}

// https://forums.mysql.com/read.php?167,706530,706530
static inline std::vector<uint64_t> mysqlx_raw_as_u64_vector(const mysqlx::Value& in_value) {
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

std::tm tm2 = {};
std::chrono::system_clock::time_point mysqlx_column_2_time_point(const mysqlx::Value& value) {
    const std::vector<uint64_t> vu64 = mysqlx_raw_as_u64_vector(value);
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

const int MAX_GAP = 7 * 24;
class RssSource {

public:

    // rsid, rsname, rspath, rsstatus, rslast, rsnext, rsgap
    RssSource(int id, std::string name, std::string path, int status, system_clock::time_point last_execute, system_clock::time_point next_execute, int gap)
        : id(id), name(name), path(path), status(status), last_execute(last_execute), next_execute(next_execute), gap(gap) {
        
    }

    std::string get_name() {
        return name;
    }

    std::string get_path() {
        return path;
    }

    int get_id() {
        return id;
    }

    // 本次是否成功，是否有新数据
    void update_2_db(bool success, bool new_data) {
        // deal status, gap
        if (success) {
            status = new_data ? std::max(0, status) + 1 : 0;
        } else {
            status = status >= 0 ? -1 : (status - 1);
        }
        if (status < -3) {
            gap = std::min(gap << 1, MAX_GAP);
            status = 0;
        } else if (status > 3) {
            gap = std::max(1, gap >> 1);
            status = 0;
        }

        // last_execute = now, next_execute = (now + gap) remove mm,ss
        last_execute = system_clock::now();

        next_execute = last_execute + std::chrono::hours(gap);
        next_execute = std::chrono::round<std::chrono::hours>(next_execute); // C++17

        // update to db
        std::string last_exe_s = time_point_2_string(last_execute);
        std::string next_exe_s = time_point_2_string(next_execute);

        mysqlx::Session session("localhost", 33060, "root", rss_secret::mysql_pwd);
        // std::format, c++20...
        std::string update_sql = "update rss.rss_source";
        update_sql += " set status=" + std::to_string(status);
        update_sql += ",gap=" + std::to_string(gap);
        update_sql += ",last_execute='" + last_exe_s + "',next_execute='" + next_exe_s + "'";
        update_sql += " where id=" + std::to_string(id);

        // 异常的处理
        mysqlx::SqlResult result = session.sql(update_sql).execute();

    }

private:
    int id;
    std::string name;
    std::string path;
    int status;
    system_clock::time_point last_execute; // 根据status判断上次是否成功
    system_clock::time_point next_execute;
    int gap;  // hour
};


std::vector<RssSource> getRssSources() {
    system_clock::time_point tp = system_clock::now();
    std::string tp_s = time_point_2_string(tp);

    mysqlx::Session session("localhost", 33060, "root", rss_secret::mysql_pwd);
    std::string query_sql = "select * from rss.rss_source where active=1 and next_execute<='" + tp_s + "'";

    std::cout<<query_sql<<std::endl;

    mysqlx::SqlResult result = session.sql(query_sql).execute();

    mysqlx::Row r;
    std::vector<RssSource> vrs; // result.count() return row_count_t
    int rsid, rsstatus, rsgap;
    std::string rsname, rspath;
    system_clock::time_point rslast, rsnext;
    while ((r = result.fetchOne())) {
        rsid = r[0].get<int>();
        rsname = r[1].get<std::string>();
        rspath = r[2].get<std::string>();
        rsstatus = r[3].get<int>();
        rslast = mysqlx_column_2_time_point(r[4]);
        rsnext = mysqlx_column_2_time_point(r[5]);
        rsgap = r[6].get<int>();

        vrs.emplace_back(RssSource(rsid, rsname, rspath, rsstatus, rslast, rsnext, rsgap));
    }
    return vrs;
}


// std::optional<std::string> get_latest_link(int sourceid) {
//     mysqlx::Session session("localhost", 33060, "root", rss_secret::mysql_pwd);
//     std::string query_sql = "select link from rss.rss_item where sourceid=" + std::to_string(sourceid) + " order by id desc limit 1";
//     mysqlx::SqlResult result = session.sql(query_sql).execute();
//     mysqlx::Row r;
//     std::optional<std::string> optstr;
//     if (r = result.fetchOne()) {
//         optstr.emplace(r[0].get<std::string>());
//     }
//     return optstr;
// }

// 本来用这个来去重，但是只需要最新的就可以用来去重了，因为RSS的数据(应该?)是有序的。
// 不行，有些RSS是10条xx + 20条yy，所以无法用最新的来比。
std::set<std::string> get_latest_links(int sourceid, int count) {
    mysqlx::Session session("localhost", 33060, "root", rss_secret::mysql_pwd);
    std::string query_sql = "select link from rss.rss_item where sourceid=" + std::to_string(sourceid) + " order by id desc limit " + std::to_string(count);
    // std::cout<<query_sql<<std::endl;
    mysqlx::SqlResult result = session.sql(query_sql).execute();
    std::set<std::string> set2;
    mysqlx::Row r;
    while ((r = result.fetchOne())) {
        set2.insert(r[0].get<std::string>());
    }
    return set2;
}


class RssItem {
public:

    RssItem(int sourceid, std::string title, std::string link, std::string pubdate): id(id), sourceid(sourceid), title(title), link(link), pubdate(pubdate) {

    }

    int insert_2_db() {
        mysqlx::Session session("localhost", 33060, "root", rss_secret::mysql_pwd);
        std::string insert_sql = "insert into rss.rss_item (sourceid,title,link,pubdate) values (" + std::to_string(sourceid) + ",'" + title + "','" + link + "','" + pubdate + "');";

        std::cout<<insert_sql<<std::endl;

        mysqlx::SqlResult result = session.sql(insert_sql).execute();

        // insert 后 select，并发时会有问题。 需要事务。
        std::string query_sql = "select max(id) from rss.rss_item where link='" + link + "'";

        result = session.sql(query_sql).execute();
        mysqlx::Row r;
        int dbid = -1;
        while ((r = result.fetchOne())) {
            dbid = r[0].get<int>();
        }
        id = dbid;
        return dbid;
    }

    // int get_id_from_db() {
        
    // }

    int get_id() {
        return id;
    }

private:
    int id;
    int sourceid; // RssSource id
    std::string title;
    std::string link;
    std::string pubdate; // pubDate published
    system_clock::time_point createtime;
};

}

namespace rss_mongodb {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    
class RssMongo {

public:
    RssMongo(int itemid, std::string title, std::string link, std::string desc): itemid(itemid), title(title), link(link), description(desc) {

    }

    void insert_2_mongo() {
        // mongocxx::instance instance{}; // This should be done only once.
        mongocxx::client client{mongocxx::uri{}};

        auto db = client["rss"];
        auto collection = db["rssitem"];
        auto insert_one_result = collection.insert_one(make_document(kvp("itemid", itemid), kvp("title", title), kvp("link", link), kvp("description", description)));
        assert(insert_one_result);
        // auto doc_id = insert_one_result->inserted_id();
    }

private:
    int itemid;
    std::string title;
    std::string link;
    std::string description;
};


}

namespace rss_xml {

class RssEntry {
public:

    RssEntry(std::string title, std::string link, std::string desc, std::string pubdate): title{title}, link{link}, description{desc}, pubdate(pubdate) {

    }

    std::string get_title() {
        if (title.empty()) {
            return "no title";
        }
        return this->title;
    }
    
    std::string get_link() {
        return this->link;
    }

    std::string get_description() {
        return description;
    }

    std::string get_pubdate() {
        return pubdate;
    }

private:
    std::string title;
    std::string link;
    std::string description;
    std::string pubdate;
};

std::vector<RssEntry> convert(std::string& xmlstr) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(xmlstr.c_str());
    std::vector<RssEntry> vre;
    if (!result) {
        std::cout<<"Error "<<result.description()<<std::endl;
        return vre;
    }

    std::stringstream ss;

    if (doc.child("feed")) {
        for (pugi::xml_node node = doc.child("feed").child("entry"); node; node = node.next_sibling("entry")) {
            std::cout<<node.child("link").attribute("href").value()<<std::endl;

            node.child("content").print(ss, "");
            vre.emplace_back(RssEntry(node.child("title").child_value(), node.child("link").attribute("href").value(), ss.str(), node.child("published").child_value()));

            ss.str("");
        }
    } else {
        // value, text, child_value ???
        for (pugi::xml_node node = doc.child("rss").child("channel").child("item"); node; node = node.next_sibling("item")) {
            // std::cout<<node.child("title").child_value()<<std::endl;
            std::cout<<node.child("link").child_value()<<std::endl;
            // std::cout<<std::string(node.child("description").child_value()).substr(0, 50)<<std::endl;
            vre.emplace_back(RssEntry(node.child("title").child_value(), node.child("link").child_value(), node.child("description").child_value(), node.child("pubDate").child_value()));
        }
    }

    return vre;
}

}

int main() {

    // std::this_thread::sleep_for(std::chrono::seconds(5));

    std::ofstream ofs("/var/log/rss/log.txt"); // 创建路径并修改权限
    std::cout.rdbuf(ofs.rdbuf());

    while (true) {

        std::vector<rss_mysql::RssSource> vrs = rss_mysql::getRssSources();
        std::cout<<"source size: "<<vrs.size()<<std::endl;
        
        for (int i = 0; i < vrs.size(); ++i) {
            rss_mysql::RssSource rss = vrs[i];
            std::cout<<"start to process "<<rss.get_id()<<std::endl;

            std::optional<std::string> rssXml = rss_http::getRSS(rss.get_name(), rss.get_path());
            if (!rssXml) {
                std::cout<<"get rss failed\n";
                rss.update_2_db(false, false);
                continue;
            }

            std::vector<rss_xml::RssEntry> vre = rss_xml::convert(rssXml.value());

            // 判重。 pubdate，但是没有保存，而且各个源，pubdate格式不同。
            // 查看最近的 vre.size 个。
            bool hasNew = false;
            std::set<std::string> latest = rss_mysql::get_latest_links(rss.get_id(), vre.size());
            // std::optional<std::string> latest = rss_mysql::get_latest_link(rss.get_id());

            int j = vre.size() - 1;
            // if (latest) { // 用最新的link，有个限制：保存的时候需要反向。
            //     for (int k = 0; k < vre.size(); ++k) {
            //         if (latest.value() == vre[k].get_link()) {
            //             j = k - 1;
            //             break;
            //         }
            //     }
            // }

            std::cout<<"vre size "<<vre.size()<<", j: "<<j<<std::endl;
            // for (int j = 0; j < vre.size(); ++j) {
            for (; j >= 0; --j) {
                if (latest.find(vre[j].get_link()) != latest.end()) {
                    continue;
                }
                std::cout<<"start to deal "<<j<<std::endl;
                // 新的文章
                rss_mysql::RssItem item = rss_mysql::RssItem(rss.get_id(), vre[j].get_title(), vre[j].get_link(), vre[j].get_pubdate());
                try {
                    int itemid = item.insert_2_db(); // begin transaction, insert mysql, insert mongodb, commit
                    rss_mongodb::RssMongo rssmongo(itemid, vre[j].get_title(), vre[j].get_link(), vre[j].get_description());
                    rssmongo.insert_2_mongo();
                    hasNew = true;
                } catch (const mysqlx::Error& err) {
                    std::cout<<"ERROR insert into db. "<<err<<std::endl;
                }
            }
            
            rss.update_2_db(true, hasNew);
        }
        rss_timer::sleep_to_next_hour();
    }
}




