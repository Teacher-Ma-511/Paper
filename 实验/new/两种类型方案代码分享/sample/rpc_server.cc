/*
 * Created by helen chan  on 06/11/2021
 * Email: kaibaking@gmail.com
 * 
 */
#include "ECSSE.server.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "参数不足2个！" << std::endl;
        exit(-1);
    }
    RunServer(std::string(argv[1]), atoi(argv[2]));
}

//.rpc_server [sdb_path] [threadsnum]
