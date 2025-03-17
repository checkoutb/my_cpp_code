

#include <iostream>
#include <fstream>
#include <curl/curl.h>



/*

fedora: sudo dnf install libcurl-devel

g++ down_pic.cpp -lcurl


CURLcode curl_easy_setopt(CURL *handle, CURLoption option, parameter);
- CURLOPT_URL， 要访问的URL
- CURLOPT_WRITEFUNCTION， 回调函数，处理响应数据
- CURLOPT_SSL_VERIFYPEER， 启用或禁用 SSL 证书
- CURLOPT_SSL_VERIFYHOST
- CURLOPT_POST， 发送POST请求
- CURLOPT_OISTFIELDS
- CURLOPT_TIMEOUT， 超时时间
- CURLOPT_FOLLOWLOCATION， 跟随重定向
- CURLOPT_HTTPHEADERS， 自定义http头
- CURLOPT_WRITEDATA， 保存响应到文件，使用前面的 回调函数，可以将响应内容保存到文件中


@More  https://curl.se/libcurl/c/

---

浏览器F12，network，刷新页面，查看 request header。



*/



size_t writeCallback(void* content, size_t size, size_t nmemb, void* userp) {
    std::ofstream* ofs = static_cast<std::ofstream*>(userp);
    size_t totalsize = size * nmemb;
    ofs->write(static_cast<char*>(content), totalsize);
    return totalsize;
}

// ok
int main_down_pic() {
    const char* url = "https://pics0.baidu.com/feed/0824ab18972bd4072505ede2fdcfde5c0eb30945.jpeg";
    const char* opath = "./ddd.jpeg.ignore";

    CURL* curl;
    CURLcode res;

    std::ofstream ofs(opath, std::ios::binary);

    if (!ofs.is_open()) {
        std::cout<<"failed to open file for write"<<std::endl;
        return -1;
    }

    curl = curl_easy_init();

    if (curl) {
        // 设置下载地址
        curl_easy_setopt(curl, CURLOPT_URL, url);
        // 回调函数，用于处理 响应数据
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        // 设置回调函数的用户数据，即回调流
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ofs);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cout<<"curl perform fail"<<std::endl;
        }
        curl_easy_cleanup(curl);
    }
    ofs.close();
    return 0;
}


size_t writeCB(void* content, size_t size, size_t nmemb, void* userp) {
    std::ofstream* ofs = static_cast<std::ofstream*>(userp);
    size_t totalsize = size * nmemb;
    ofs->write(static_cast<char*>(content), totalsize);
    return totalsize;
}

// 
int main_down_webpage() {

    char const * url = "https://baijiahao.baidu.com/s?id=1.........5&wfr=spider&for=pc";  // 。。只会发压缩包，但是我本地试了几种压缩格式，都无法解压。
    //char const * url = "https://doc.rust-lang.org/std/index.html#keywords"; //ok
    //char const * url = "https://www.baidu.com/"; // ok
    
    
    char const * opath = "./ttxt.ignore";
    CURL* curl;
    CURLcode res;

    std::ofstream ofs(opath, std::ios::binary);
    if (!ofs.is_open()) {
        std::cerr<<"failed to open file"<<std::endl;
        return -1;
    }

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        //curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0");
        //curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/png,image/svg+xml,*/*;q=0.8");
        // headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0");
        headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:131.0) Gecko/20100101 Firefox/131.0");
        headers = curl_slist_append(headers, "Accept-Language: en-US,en;q=0.5");
        headers = curl_slist_append(headers, "Accept-Encoding: gzip, deflate, br, zstd");
        headers = curl_slist_append(headers, "Connection: keep-alive");
        //headers = curl_slist_append(headers, "Cookie: ismob=0");
        headers = curl_slist_append(headers, "Cookie: BIDUPSI...............................oQ11g:C");
        
        headers = curl_slist_append(headers, "Host: baijiahao.baidu.com");
        
        
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCB);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ofs);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cout<<"curl failed"<<std::endl;
        }
        curl_easy_cleanup(curl);
    }
    ofs.close();
    return 0;
    
}


int main(int argc, char** argv) {

    //main_down_pic();

    //main_down_webpage();
}
