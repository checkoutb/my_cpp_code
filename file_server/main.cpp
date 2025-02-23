


#include "httplib.h"


const int port = 3033;

int main(void)
{
    namespace hl = httplib;

    hl::Server svr;
    svr.Get("/hi", [](const hl::Request& req, hl::Response& res) {
        res.set_content("hello world", "text/plain");
    });
    auto ret = svr.set_mount_point("/file", "./file.ignore");    
    svr.listen("localhost", port);
}


