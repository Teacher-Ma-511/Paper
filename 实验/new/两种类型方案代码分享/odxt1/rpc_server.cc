/*
 * Created by helen chan  on 19/02/2022
 * Email: kaibaking@gmail.com
 * 
 */
#include "ECSSE.server.h"

int main(int argc, char *argv[])
{
    // if (argc < 3)
    // {
    //     std::cerr << "参数不足2个！" << std::endl;
    //     exit(-1);
    // }
    // RunServer(std::string(argv[1]), atoi(argv[2]));
    std::string db_path="/home/helen/Desktop/ssesearch/odxt1/sdb/server1.txt";
    RunServer(db_path,4);
}

//.rpc_server [sdb_path] [threadsnum]
// ./rpc_server