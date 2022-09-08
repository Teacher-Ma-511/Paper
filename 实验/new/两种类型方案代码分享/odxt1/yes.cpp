#include<bits/stdc++.h>
#include <crypto++/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include<cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/base64.h>
#include<cryptopp/files.h>
//#include<ECSSE.Util.h>
#include<bloom_filter.hpp>
#define AES128_KEY_LEN 16
using namespace CryptoPP;
using namespace std;
bool cmp(pair<string,int> &a ,pair<string,int> &b){
    return a.second>b.second;
}
Integer g=Integer("12293041548191560959252997681221893073091763148392091587781229526780505319280253304158657014186812803260820624200425866044892534107445664741934978636107726042273789227287786063826002069319009175216162933494359277792631586799194401273339746711856497988529049521334761338869458014310995080149317625448851976250");
Integer p=Integer("35846489658960820960446633115143023883040757131525173446156521584352743991679231122673876526186909574940183912868842246185467405988094236483899550393037943055836758636212712380713704715884242930459327176061234383344181152578596944695560082751184202851538548055157781713238676762830736663499575834426106137609");
Integer q=Integer("91677580900928952829845059692479440057913422538304665006082987855400898556463");
byte iv_s[17] = "0123456789abcdef";
int log256(Integer n) {
    int res = 0;
    while(n > 0) {
        n/=256;
        res++;
    }
    return res;
}
Integer str2num(std::string str){
    int n = str.size();
    Integer ans((unsigned char)str[n-1]);
    for(int i=n-2;i>=0;i--){
        ans*=256;
        ans+=(unsigned char)str[i];
    }
    return ans;
}
std::string num2str(Integer num){
    long n = log256(num);
    //char result[n];
    string res;
   // cout<<"test1"<<endl;
    for(long i =0;i<n;i++){
        res.push_back ((unsigned char)(num%256));
        num/=256;
    }
    //reverse(res.begin(),res.end());
    return res;
}
void descrypt(std::string key, std::string ciphertext,std::string &plaintext){
    try
    {
        CFB_Mode< AES >::Decryption d;
        d.SetKeyWithIV((byte*) key.c_str(), AES128_KEY_LEN, iv_s, (size_t)AES::BLOCKSIZE); 
        byte tmp_new_st[AES128_KEY_LEN];
        d.ProcessData(tmp_new_st, (byte*) ciphertext.c_str(), ciphertext.length());
        plaintext= std::string((const char*)tmp_new_st, ciphertext.length());
    }
    catch(const CryptoPP::Exception& e)
    {
        std::cerr << "in descrypt()解密出问题 " << e.what()<< std::endl;
        exit(1);
    }
    //return plaintext;
}
void encrypt(std::string key, std::string plaintext,std::string &ciphertext){
    try
    {
        CFB_Mode< AES >::Encryption e;
        e.SetKeyWithIV((byte*) key.c_str(), AES128_KEY_LEN, iv_s, (size_t)AES::BLOCKSIZE); 
        byte tmp_new_st[AES128_KEY_LEN];
        e.ProcessData(tmp_new_st, (byte*) plaintext.c_str(), plaintext.length());
        ciphertext= std::string((const char*)tmp_new_st, plaintext.length());
    }
    catch(const CryptoPP::Exception& e)
    {
        std::cerr << "in descrypt()解密出问题 " << e.what()<< std::endl;
        exit(1);
    }
}
bloom_parameters parameters;
bloom_filter bf;
 void setbf(){
// How many elements roughly do we expect to insert?
    unsigned long long scount=10000000;
    parameters.projected_element_count = scount;
    // Maximum tolerable false positive probability? (0,1)
    parameters.false_positive_probability = 0.0001; // 1 in 10000
    // Simple randomizer (optional)
    parameters.random_seed = 0xA5A5A5A5;

    if (!parameters)
    {
        std::cout << "Error - Invalid set of bloom filter parameters!" << std::endl;
        exit(-1);
    }
    parameters.compute_optimal_parameters();
}
int main(){
    std::cout<<"start reading bf"<<std::endl;
    std::ifstream fin("/home/helen/Desktop/ssesearch/odxt1/2.txt", std::ios::in|std::ios::binary);	
    fin.seekg(0, std::ios::end);
    std::streampos fp = fin.tellg();	
    if (fin.is_open()&&fp) {	
        //然后要把文件指针放到文件头部
        fin.seekg(0);
        boost::archive::binary_iarchive ia(fin);
        ia >> bf;
        fin.close();
        std::cout<<"cong chuncun dakai bf"<<std::endl;
    }else{
        setbf();
        bloom_filter ss(parameters);
        bf=ss;
    }
    std::ofstream fout("/home/helen/Desktop/ssesearch/odxt1/2.txt", std::ios::binary);	
    if (fout.is_open()) {		
        boost::archive::binary_oarchive oa(fout);
        oa << bf;
        fout.close();
        //return true;
    }
    else {
        std::cout << "打开bf失败" << std::endl;
        //return false;
        exit(-1);
    }		
    std::cout<<"chenggong baocun BF!!!"<<std::endl;
    return 0;
}
//72015158