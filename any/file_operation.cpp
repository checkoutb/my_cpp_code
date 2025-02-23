

//#include <ctime>
#include <string>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <fstream>


/*
.

// -std=c++20  for  std::chrono::file_clock

1. 获得文件夹中文件的最后修改时间
2. append to file
3. read file, get last line


*/



// 获得文件夹中文件的最后修改时间
void dfs_get_last_write_time_in_folder(std::filesystem::path path) {
    // std::string str_path = "";
    // std::filesystem::path path = str_path.empty() ? std::filesystem::current_path();

    std::filesystem::file_time_type ftime = std::filesystem::last_write_time(path);
    std::time_t cftime = std::chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(ftime));



// "/mnt/239G/my_github/my_cpp_code/any" write time : Mon Jan  6 12:49:08 2025
// 2025-01-06 04:49:08.613971700
// 1736138948
// 2025-01-06 04:49:08.613971700
    std::cout<<path<<" write time : "<<std::asctime(std::localtime(&cftime));
    std::cout<<ftime<<'\n'<<cftime<<'\n'<<(std::chrono::file_clock::to_sys(ftime))<<std::endl;
    //std::cout<<std::localtime(&cftime)<<std::endl; // address


    struct tm* ptm = std::localtime(&cftime);


// 2025-01-06T13:15:31+0800
// 2025-01-06T13:15:31+0800
// 2025-01-06T05:15:31GMT

    // hour, min 需要加前置0。。得专门写个方法。
    // tm_mon 从0开始。
    std::cout<<(ptm->tm_year + 1900)<<"-"<<(ptm->tm_mon < 9 ? "0" : "")<<(ptm->tm_mon + 1)<<'-'<<(ptm->tm_mday < 10 ? "0" : "")<<(ptm->tm_mday)<<'T'<<(ptm->tm_hour)<<":"<<(ptm->tm_min)<<":"<<(ptm->tm_sec < 10 ? "0" : "")<<(ptm->tm_sec)<<"+0800"<<std::endl;
    
    char tmstr[100];

    // char timeString[std::size("yyyy-mm-ddThh:mm:ssZ")];   // 长度
    // std::strftime(std::data(timeString), std::size(timeString), // std::data, std::size
    //               "%FT%TZ", std::gmtime(&time));

    // https://en.cppreference.com/w/cpp/iterator/data    std::data
    
    std::strftime(tmstr, sizeof(tmstr), "%FT%T%z", std::localtime(&cftime));  // 这个最好
    std::cout<<tmstr<<std::endl;

    
    std::strftime(tmstr, sizeof(tmstr), "%FT%T%Z", std::gmtime(&cftime));   // "%FT%TZ" 的话是 '2025-01-06T05:03:08Z'， 也没问题，0时区
    std::string sstm = tmstr;
    std::cout<<sstm<<std::endl;
    
    if (std::filesystem::is_directory(path)) {
        for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator(path)) {
            dfs_get_last_write_time_in_folder(entry);
        }
    }
}

namespace {
    std::string app_f_name = "_append_yhb456.ignore";
    
}

void append_to_file() {

    std::filesystem::path appfpath = app_f_name;

    // 目录用 std::filesystem::create_directory(dir_path)
    if (!std::filesystem::exists(appfpath)) {
        std::ofstream fstream(appfpath);
        if (fstream.is_open()) {
            std::cout<<"opened.\n";   // 走这里。
            fstream.close();
        } else {
            std::cout<<"not opened.\n";
        }
    }

    // // 清空文件 + 写入
    // std::ofstream appfstream(appfpath);   // 应该复用 fstream， 但 create 是极小概率，所以无所谓
    // appfstream<<"this is 1\n";
    // appfstream.close();

    
    // 要判断文件 is_open ?

    
    // // append
    // std::ofstream afstream(appfpath, std::ios::app);
    // afstream<<"this is 2\n";
    // afstream.close();


    // append
    std::ofstream afs;
    afs.open(appfpath, std::ios::app); // open 可以接受 char*, std::string, fs::path, fs::path::value_type*
    afs<<"this is 3\n";
    afs.close();

    
}



// https://zhuanlan.zhihu.com/p/8317807057  这里可以跳过非空行。这里的代码没有试。
void read_file_last_line() {

    std::ifstream ifs;

    // ok 读取全部内容
    //ifs.open(app_f_name);
    // if (ifs.is_open()) {
    //     std::string line;
    //     while (std::getline(ifs, line)) {  // 读取空行，不读取行最后的换行符。
    //         std::cout<<std::size(line)<<": "<<line<<std::endl;
    //     }
    //     ifs.close();
    // }


    ifs.open(app_f_name, std::ios::ate);
    std::string line;
    std::string lastLine;
    if (ifs.seekg(-2, std::ios::end).good()) {
        int cnt = 0;
        while (ifs.tellg() > 0 && ifs.peek() != '\n') {  // 这个有bug，如果文件只有一行，那么这个 while 死循环. 后续增加了 tellg
            ifs.seekg(-1, std::ios::cur);
            //std::cout<<ifs.tellg()<<std::endl;  // 文件只有一行，这里永远 -1
            cnt++;
        }

        // https://en.cppreference.com/w/cpp/io/basic_istream/tellg
        // if fail() == true, returns pos_type(-1).
        if (ifs.tellg() == 0) {
            //ifs.seekg(0, std::ios::beg); // 不行
            //ifs.seekg(0); // no
            // ifstream 已经失败了，所以动不了了。
            
            // 改成，while tellg>0, 而不是 tellg>=0， 且这里判断 == 0， 不是 == -1
        } else 
            ifs.seekg(1, std::ios::cur);  // tellg 是-1，这里无法移动指针
        
        
        std::getline(ifs, lastLine);
        std::cout<<"seekg "<<cnt<<", "<<ifs.tellg()<<std::endl;
        
    } else {
        std::cout<<"what happen? \n"; // 文件不存在走这里。
    }
    std::cout<<"last line : "<<lastLine<<std::endl;
    if (ifs.is_open())
        ifs.close();

    
}


int main() {
    std::string str_path = "";
    
    std::filesystem::path path = str_path.empty() ? std::filesystem::current_path() : static_cast<std::filesystem::path>(str_path);

    // dfs_get_last_write_time_in_folder(path);


    // append_to_file();

    read_file_last_line();

    
    return 0;
}


