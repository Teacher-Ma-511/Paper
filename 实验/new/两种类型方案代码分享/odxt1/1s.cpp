#include<bits/stdc++.h>
using namespace std;

int main(){
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
		//client.search(key);
		cout<<key.size()<<endl;
        std::cout << "搜索结束test1！"<< std::endl;
		std::cout<<std::endl;
		key.clear();
	}
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
        cout<<key.size()<<endl;
		//client.search(key);
		std::cout << "搜索结束test2！"<< std::endl;
		std::cout<<std::endl;
		key.clear();
	}
    return 0;
}