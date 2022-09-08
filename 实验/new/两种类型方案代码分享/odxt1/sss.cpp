#include<bits/stdc++.h>
#include <crypto++/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include<cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/base64.h>
#include <boost/archive/binary_iarchive.hpp> //二进制序列化
#include <boost/archive/binary_oarchive.hpp> //二进制序列化
#include <boost/serialization/vector.hpp> //序列化STL容器要导入
#include<boost/serialization/map.hpp> //序列化STL容器要导入
#include <boost/serialization/string.hpp>
#include<bloom_filter.hpp>
using namespace std;
using namespace CryptoPP;

std::string num2str(Integer num){
    long n = (num/256).ConvertToLong();
    char result[n];
    for(int i =0;i<n;i++){
        result[i]= char(num%256);
        num/=256;
    }
    return (std::string)result;
}
Integer str2num(std::string str){
    int n = str.size();
    Integer ans((char)str[n-1]);
    for(int i=n-2;i>=0;i--){
        ans*=256;
        ans+=(char)str[i];
    }
    return ans;
}
// class A{
//     public:
//         int a;
//     A(){
//         a=1;
//     }
//     A(int c){
//         a=c;
//     }
//     void print(){
//         cout<<a<<endl;
//     }
//     template<class Archive>
//     void serialize(Archive & ar, const unsigned int version)
//     {
//         ar & a;
//         //ar & minutes;
//         //ar & seconds;
//     }
// };
int main(){
    // string s ="12a";
    // Integer c = str2num(s);
    // cout<<c<<endl;
    // string ans =num2str(c);
    // cout<<ans<<endl;
    // A a(5);
    bloom_parameters   parameters;
    parameters.projected_element_count = 100000;
        // Maximum tolerable false positive probability? (0,1)
    parameters.false_positive_probability = 0.0001; // 1 in 10000
    // Simple randomizer (optional)
    parameters.random_seed = 0xA5A5A5A5;

    if (!parameters)
    {
        std::cout << "Error - Invalid set of bloom filter parameters!" << std::endl;
        return 1;
    }
    parameters.compute_optimal_parameters();
    bloom_filter s(parameters);
    s.insert("nmd");
    s.insert("weqweqq");
    // serialize 
    ofstream fout("/home/helen/Desktop/conjunctive/2.txt", ios::binary);	
	if (fout.is_open()) {		
		boost::archive::binary_oarchive oa(fout);
		oa << s;
		fout.close();
		//return true;
	}
	else {
		cout << "打开失败" << endl;
		//return false;
        exit(-1);
	}		

    bloom_filter q;
    //unserialize
    //首先判断文件是否为空
	ifstream fin("/home/helen/Desktop/conjunctive/2.txt", ios::in|ios::binary);	
	fin.seekg(0, ios::end);
	streampos fp = fin.tellg();	
	if (fin.is_open()&&fp) {	
		//然后要把文件指针放到文件头部
		fin.seekg(0);
		boost::archive::binary_iarchive ia(fin);
		ia >> q;
		fin.close();
	}	
    if(q.contains("nmd"))			
        cout<<"1123123"<<endl;
    return 0;
}