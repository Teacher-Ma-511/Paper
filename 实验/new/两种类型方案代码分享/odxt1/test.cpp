#include<bits/stdc++.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include<cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/base64.h>
#include<cryptopp/files.h>
#include<cryptopp/aes.h>
using namespace std;
using namespace CryptoPP;
#define AES128_KEY_LEN 16
byte k[17] = "0123456789abcdef";
byte iv_s[17] = "0123456789abcdef";
std::string padding(const std::string str)
{
    size_t BS = (size_t)AES::BLOCKSIZE;
    size_t pad_len = BS - str.length() % BS;
    return str + std::string(char(pad_len), pad_len);
}

std::string remove_padding(const std::string str)
{
    int len = str.length();
    int pad_len = int(str[len - 1]);
    bool flag = false;
    for (int i = len - pad_len; i < len; i++)
    {
        if (int(str[i]) != pad_len)
        {
            flag = true;
            break;
        }
    }
    if (flag)
        std::cout << "wrong padding" << std::endl;
    else
        return std::string(str, 0, len - pad_len);
}
std::string gen_enc_token(const std::string token){
    // 使用padding方式将所有字符串补齐到16的整数倍长度
    std::string token_padding;
    std::string enc_token;
    try {
        CFB_Mode< AES >::Encryption e;
        //主密钥和初始化向量固定，对关键字进行padding使得F(K,w)->K_w
        e.SetKeyWithIV(k, AES128_KEY_LEN, iv_s, (size_t)AES::BLOCKSIZE); 
        token_padding = padding(token);
        // //byte cipher_text[token_padding.length()];
        // byte cipher_text[AES128_KEY_LEN];
        // e.ProcessData(cipher_text, (byte *)token_padding.c_str(), AES128_KEY_LEN);
        // enc_token = std::string((const char*) cipher_text, AES128_KEY_LEN);

        StringSource s(token_padding, true, 
            new StreamTransformationFilter(e,
                new StringSink(enc_token)
            ) // StreamTransformationFilter
        ); // StringSource
        //测试代码
        // std::cout << "K_w jiami text: ";
        // HexEncoder encoder(new FileSink(std::cout));
        // encoder.Put((const byte*)&enc_token[0], enc_token.size());
        // encoder.MessageEnd();
        // std::cout << std::endl;
        // cout << "enc : " << enc_token << endl;

        // std::string recover;
        // CFB_Mode<AES>::Decryption d;
        // d.SetKeyWithIV(k, AES128_KEY_LEN, iv_s, (size_t)AES::BLOCKSIZE);
        // // byte tmp_new_st[AES128_KEY_LEN+1];
        // // d.ProcessData(tmp_new_st, (byte *)enc_token.c_str(),  (size_t)AES::BLOCKSIZE);
        //  StringSource ds(enc_token ,true, 
        //     new StreamTransformationFilter(d,
        //         new StringSink(recover)
        //     ) // StreamTransformationFilter
        // ); // StringSource
        // cout<<recover<<endl;
    }
    catch(const CryptoPP::Exception& e)
    {
        std::cerr << "in gen_enc_token() 在生成kw中出问题 " << e.what()<< std::endl;
        exit(1);
    }
    //cout << enc_token.size() << endl;
    return enc_token;
}
string H1(const std::string message)
{
    byte buf[SHA256::DIGESTSIZE];
    string salt = "01";
    SHA256().CalculateDigest(buf, (byte *)((message + salt).c_str()), message.length() + salt.length());
    return string((const char *)buf, (size_t)SHA256::DIGESTSIZE);
}
std::string H2(const std::string message)
{
    byte buf[SHA256::DIGESTSIZE];
    std::string salt = "02";
    SHA256().CalculateDigest(buf, (byte *)((message + salt).c_str()), message.length() + salt.length());
    return std::string((const char *)buf, (size_t)SHA256::DIGESTSIZE);
}
std::string gen_st(std::string &keyword, size_t c,std::string &K_w)
{
    //生成状态st_c 
    std::string st_c;
    std::string w_str;
    try
    {
        w_str = K_w + std::to_string(c);
        st_c = H2(w_str);
        //cout << st_c.size() << endl;
    }
    catch (const CryptoPP::Exception &e)
    {
        std::cerr << "in gen_st() 生成状态stc中出问题" << e.what() << std::endl;
        exit(1);
    }
    return st_c.substr(0,AES128_KEY_LEN);
}
std::string Xor(const std::string s1, const std::string s2)
{
    std::string result = s1;
    if (s1.length() > s2.length())
    {
        //ERROR
        std::cout << "not sufficient size: " << s1.length() << ", " << s2.length() << std::endl;
        return "";
    }

    for (int i = 0; i < result.length(); i++)
    {
        result[i] ^= s2[i];
    }
    return result;
}
int main(){
    string keyword ="312";
    string id1 = "1";
    string id2 = "2";
    string K_w = gen_enc_token(keyword);
    string st0 = gen_st(keyword, 0,K_w);
    string st1 = gen_st(keyword, 1,K_w);
    string st2 = gen_st(keyword, 2,K_w);
    string st3 = gen_st(keyword,3,K_w);
    string st4 = gen_st(keyword,4,K_w);
    cout << "KW: " << K_w <<" "<<K_w.size()<<endl;
    cout<<"st0: "<<st0<<" "<<st0.size()<<endl;
    cout<<"st1: "<<st1<<" "<<st1.size()<<endl;
    cout<<"st2: "<<st2<<" "<<st2.size()<<endl;
    cout<<"st3: "<<st3<<" "<<st3.size()<<endl;
    cout<<"st4: "<<st4<<" "<<st4.size()<<endl;
    //第一次更新,st1对应一个新状态，更新了第一个文件id1
    string cipherid1,enc1,op1="1",l1,E1;
    CFB_Mode<AES>::Encryption e;
    e.SetKeyWithIV((byte *)K_w.c_str(), AES128_KEY_LEN, iv_s, (size_t)AES::BLOCKSIZE);
    StringSource s1(id1, true, 
        new StreamTransformationFilter(e,
            new StringSink(cipherid1)
        ) // StreamTransformationFilter
    ); // StringSource
    CFB_Mode<AES>::Encryption e1;
    e1.SetKeyWithIV((byte *)st1.c_str(), AES128_KEY_LEN, iv_s, (size_t)AES::BLOCKSIZE);
    StringSource s3(st0, true, 
        new StreamTransformationFilter(e1,
            new StringSink(enc1)
        ) // StreamTransformationFilter
    ); // StringSource
    
    //生成字典键值对
    l1 = H1(st1);
    E1 = Xor(op1 + cipherid1, H2(st1));
    E1 = E1 + enc1;

    string cipherid2,enc2,op2="1",l2,E2;
    //第2次更新,st2对应一个新状态，更新了第2个文件id2
    CFB_Mode<AES>::Encryption e2;
    e2.SetKeyWithIV((byte *)K_w.c_str(), AES128_KEY_LEN, iv_s, (size_t)AES::BLOCKSIZE);
    StringSource s2(id2, true, 
        new StreamTransformationFilter(e2,
            new StringSink(cipherid2)
        ) // StreamTransformationFilter
    ); // StringSource
    CFB_Mode<AES>::Encryption e3;
    e3.SetKeyWithIV((byte *)st2.c_str(), AES128_KEY_LEN, iv_s, (size_t)AES::BLOCKSIZE);
    StringSource ss2(st1, true, 
        new StreamTransformationFilter(e3,
            new StringSink(enc2)
        ) // StreamTransformationFilter
    ); // StringSource
    
    //生成字典键值对
    l2 = H1(st2);
    E2 = Xor(op2 + cipherid2, H2(st2));
    E2 = E2 + enc2;

    int c =2;
    string dl, dE,ddE,ddenc,stx;
    stx = st2;
    set<string> ID;
    for (int i = 0; i < c; i++)
    {
        cout << "new state：" << stx << endl;
        dl = H1(stx);
        dE=E2;
        ddE = dE.substr(0,2);
        ddenc= dE.substr(2,AES128_KEY_LEN);
        ddE =Xor(ddE,H2(stx));
        CFB_Mode<AES>::Decryption d;
        d.SetKeyWithIV((byte *)stx.c_str(), AES128_KEY_LEN, iv_s, (size_t)AES::BLOCKSIZE);
        StringSource ss1(ddenc, true, 
            new StreamTransformationFilter(d,
                new StringSink(stx)
            ) // StreamTransformationFilter
        ); // StringSource
        cout << "old state: " << stx << endl;
        std::string asd;
        CFB_Mode<AES>::Decryption d1;
        d1.SetKeyWithIV((byte *)K_w.c_str(), AES128_KEY_LEN, iv_s, (size_t)AES::BLOCKSIZE);
        StringSource ss21(ddE, true, 
            new StreamTransformationFilter(d1,
                new StringSink(asd)
            ) // StreamTransformationFilter
        ); // StringSource
        ID.insert(asd);
    }
    set<string>::iterator it=ID.begin();
    cout << ID.size() << endl;
    for (; it != ID.end(); it++)
    {
        cout << "ID:" << *it << endl;
    }
    return 0;
}