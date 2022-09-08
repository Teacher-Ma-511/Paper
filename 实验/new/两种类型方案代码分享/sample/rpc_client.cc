/*
 * Created by helen chan  on 06/11/2021
 * Email: kaibaking@gmail.com
 * 
 */
#include "ECSSE.client.h"

#include "logger.h"

using ECSSE::SearchRequestMessage;

int main(int argc, char** argv) {
	ECSSE::Client client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()), std::string(argv[1]));
	
	if (argc < 3) {
		std::cerr<<"参数不足2个"<<std::endl;	
		exit(-1);
	}
	int threads_num = atoi(argv[2]);
	std::map<int,std::string> qq;

	//small.txt
	qq[1]="MVP";
	qq[2]="dark";
	qq[3]="there";
	qq[4]="in";
	qq[5] = "chenhailong";

	//medium.txt
	// qq[1]="Beating";
	// qq[2]="yacht";
	// qq[3]="Forces";
	// qq[4]="while";
	// qq[5] = "of";

	//big.txt
	// qq[1]="dique";
	// qq[2]="Mirai";
	// qq[3]="losers";
	// qq[4]="audience";
	// qq[5] = "State";


	std::string keyword = "dique";
	std::cout << "开始搜索！" << std::endl;
	for(int i =0;i<5;i++){
		keyword = qq[i + 1];
		client.search(keyword);
	}
	//client.search(keyword);
	std::cout << "搜索结束！"<< std::endl;
	return 0;
}

//普通搜索实现
// /.rpc_client  [db_path] [threads_num]  