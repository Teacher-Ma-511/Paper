/*
 * Created by helen chan  on 06/11/2021
 * Email: kaibaking@gmail.com
 * 
 */
#include "ECSSE.db_generator.h"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "参数个数小于2个" << std::endl;
        exit(-1);
    }
    std::string db_path = std::string(argv[1]);
    size_t thread_num = atoi(argv[2]);
    ECSSE::Client client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()), std::string(argv[1]));
    std::cout << "更新开始！" << std::endl;
    ECSSE::File_Get(client,thread_num);
    std::cout << "更新结束！" << std::endl;
    return 0;
}

//取数据进行更新
/*
    ./update [db_path] [thread_num]
*/

