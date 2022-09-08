/*
 * Created by helen chan  on 19/02/2022
 * Email: kaibaking@gmail.com
 * 
 */
#include "ECSSE.client.h"

#include "logger.h"

using ECSSE::SearchRequestMessage;

int main(int argc, char** argv) {
	std::string db_path = "/home/helen/Desktop/ssesearch/odxt1/test/client.txt";
	grpc::ChannelArguments channel_args;
	channel_args.SetInt("grpc.max_receive_message_length",1024*1024*1024);
	ECSSE::Client client(grpc::CreateCustomChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials(),channel_args),db_path);
	std::vector<std::string> key;
	//std::vector<std::string> keywordsmall  ={"cruelty","industrial","same","in","chenhailong"};
	//std::vector<std::string> keywordmedium  ={"longterm","equation","Forces","began","of"};
//	std::vector<std::string> keywordbig  ={"ceo","Toilet","ponds","prison","since"};
	std::cout<<"fixed w1 =10"<<std::endl;
	std::vector<std::string> kss ={"slightly","lecturer","morning","levels","troops"};
	for(int i =0;i<6;i++){
		std::cout << "开始搜索test1！" << std::endl;
		//key.push_back("Tag");
		if(i==0){
			key.push_back("Tag");
		}else{
			key.push_back("Tag");
			for(int j =0;j<i;j++){
				key.push_back(kss[j]);
			}
		}
		client.search(key);
		std::cout << "搜索结束test1！"<< std::endl;
		std::cout<<std::endl;
		key.clear();
	}
	//std::vector<std::string> kss ={"slightly","lecturer","morning","levels","troops"};
	std::cout<<"fixed w1=10000"<<std::endl;
	for(int i =0;i<6;i++){
		std::cout << "开始搜索test2！" << std::endl;
		if(i==0){
			key.push_back("a");
		}else{
			key.push_back("a");
			for(int j =0;j<i;j++){
				key.push_back(kss[j]);
			}
		}
		client.search(key);
		std::cout << "搜索结束test2！"<< std::endl;
		std::cout<<std::endl;
		key.clear();
	}

	//test 1 fixed w2 =10
	// std::cout<<"fixed w2"<<std::endl;
	// for(int i =0;i<5;i++){
	// 	std::cout << "开始搜索test1！" << std::endl;
	// 	//key.push_back("Tag");
	// 	key.push_back(keywordsmall[i]);
	// 	key.push_back("Tag");
	// 	//key.push_back("Delray");
	// 	//key.push_back(keywordbig[i]);
	// 	client.search(key);
	// 	std::cout << "搜索结束test1！"<< std::endl;
	// 	std::cout<<std::endl;
	// 	key.clear();
	// }
	// //test 2 fixed w1 =10
	// std::cout<<"fixed w1"<<std::endl;
	// for(int i =0;i<5;i++){
	// 	std::cout << "开始搜索test2！" << std::endl;
	// 	key.push_back("Tag");
	// 	//key.push_back("Delray");
	// 	key.push_back(keywordsmall[i]);
	// 	//key.push_back(keywordbig[i]);
	// 	client.search(key);
	// 	std::cout << "搜索结束test2！"<< std::endl;
	// 	std::cout<<std::endl;
	// 	key.clear();
	// }
	return 0;
}

//普通搜索实现
// /.rpc_client  [db_path] [threads_num]  