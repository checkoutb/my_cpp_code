


#include <iostream>
#include <filesystem>
#include <format>
#include <thread>
#include <chrono>
#include <string>


/*

可以压缩
- mp4
- jpg


1. main 中指定 v_path的值
2. 处理 mp4
3. 压缩后文件名为 原文件名 减去 后缀 + _COMPRESS + 原后缀
4. 删除 原文件

5. 使用 ffmpeg 压缩图片


*/
namespace {
    std::string v_path;

    // constexpr std::string ffmpeg_command = "ffmpeg -i '{}' -threads 4 -preset faster '{}'";

    void compress_by_ffmpeg(std::string& in_path, std::string& out_path) {
        std::string cmd = std::format("ffmpeg -i '{}' -threads 4 -preset faster '{}'", in_path, out_path);

        FILE* fp;
        const char* command = cmd.c_str();
        fp = popen(command, "r");
        if (fp == NULL) {
            std::cout<<"failed ffmpeg\n";
            exit(1);
        }

        pclose(fp);
    }

    // bool has_compressed(std::string& path) {
        
    // }

    // void compress_ok(std::string& path) {
        
    // }

    // return: invoke ffmpeg?
    bool process_mp4(
                     // std::filesystem::directory_entry& entry,
                     std::string& in_path) {
        // if ((entry.file_size() >> 20) < 2) {  // not process the file that < 2mb
        //     return false;
        // }

        if (in_path.ends_with("_COMPRESS.mp4")) { // has compressed
            return false;
        }

        std::string out_path = in_path.substr(0, in_path.size() - 4) + "_COMPRESS.mp4";
        compress_by_ffmpeg(in_path, out_path);
        std::this_thread::sleep_for(std::chrono::seconds(10));
        
        return true;
    }
    
    bool process_jpg(std::string& in_path) {
        if (in_path.ends_with("_COMPRESS.jpg"))
            return false;

        std::string out_path = in_path.substr(0, in_path.size() - 4) + "_COMPRESS.jpg";
        compress_by_ffmpeg(in_path, out_path);
        
        return true;
    }
    
    void scan_file_compress(std::filesystem::path fpath) {

        // std::cout<<" -- "<<fpath<<std::endl;
        for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator(fpath)) {
            // std::cout<<" == "<<entry.path()<<std::endl;
            if (entry.is_directory()) {
                scan_file_compress(entry.path());
                continue;
            }

            std::string in_path = entry.path();
            bool ok = false;
            if (in_path.ends_with(".jpg")) {
                ok = process_jpg(in_path);
            } else if (in_path.ends_with(".mp4")) {
                ok = process_mp4(in_path);
            }

            if (!ok) {
                continue;
            }
            
            ///////////// delete if success ////////////////
            std::cout<<"processed file: "<<in_path<<std::endl;
            std::filesystem::remove(in_path);
        }
    }
    
}

int main(int argc, char* argv[]) {

    v_path = "/mnt/239G/z_my_backup/phone_camera_2025-2-19/Camera/";

    // ffmpeg_command = "ffmpeg -i '{}' -threads 4 -preset faster '{}'";

    std::cout<<"start ...\n";

    scan_file_compress(v_path);

    std::cout<<"end...\n";

    return 0;
}
