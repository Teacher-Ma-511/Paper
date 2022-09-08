/*
 * Created by helen chan  on 19/02/2022
 * Email: kaibaking@gmail.com
 * 
 */
#include "ECSSE.db_generator.h"

int main(int argc, char **argv)
{
    // if (argc < 3)
    // {
    //     std::cerr << "参数个数小于2个" << std::endl;
    //     exit(-1);
    // }
    std::string db_path="/home/helen/Desktop/ssesearch/odxt1/test/client.txt";
     grpc::ChannelArguments channel_args;
	channel_args.SetInt("grpc.max_receive_message_length",1024*1024*1024);
	ECSSE::Client client(grpc::CreateCustomChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials(),channel_args),db_path);
    std::cout << "更新开始！" << std::endl;
    ECSSE::File_Get(client);
    std::cout << "更新结束！" << std::endl;
    std::cout << "BF save 开始！" << std::endl;
    ECSSE::Save_BF(&client);
    std::cout << "BF save 结束！" << std::endl;

    return 0;
}

//取数据进行更新
/*
    ./update [db_path] [thread_num]
*/

