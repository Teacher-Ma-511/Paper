/*
 * Created by helen chan  on 19/02/2022
 * Email: kaibaking@gmail.com
 * 
 */
#ifndef ECSSE_CLIENT_H
#define ECSSE_CLIENT_H
#include <bits/stdc++.h>
#include <rocksdb/db.h>
#include <grpc++/grpc++.h>
#include <ECSSE.Util.h>
#include "ECSSE.grpc.pb.h"
#include "logger.h"
#include "bloom_filter.hpp"

using grpc::Channel;
using grpc::ClientAsyncResponseReaderInterface;
using grpc::ClientContext;
using grpc::ClientReaderInterface;
using grpc::ClientWriterInterface;

//用于生成子密钥，主密钥和初始化向量
byte k[17] = "0123456789abcdef";
byte iv_s[17] = "0123456789abcdef";

//SDSSE-CQ-S
byte k_x[17] = "abcdeF0123456789";
byte k_i[17] = "Abcd0123456789ef";
byte k_z[17] = "aB0123456789cdef";


std::string K_I;
std::string K_Z;
std::string K_X;
Integer g=Integer("12293041548191560959252997681221893073091763148392091587781229526780505319280253304158657014186812803260820624200425866044892534107445664741934978636107726042273789227287786063826002069319009175216162933494359277792631586799194401273339746711856497988529049521334761338869458014310995080149317625448851976250");
Integer p=Integer("35846489658960820960446633115143023883040757131525173446156521584352743991679231122673876526186909574940183912868842246185467405988094236483899550393037943055836758636212712380713704715884242930459327176061234383344181152578596944695560082751184202851538548055157781713238676762830736663499575834426106137609");
Integer q=Integer("91677580900928952829845059692479440057913422538304665006082987855400898556463");
// Integer phi=Integer("22851248625660461847777944216946242442211760167752428242462399707021805078124330981722351281764679703678948267679192078661239425576182958362001412009171866472885734296441649628873515231274400107019679159708976846966279012695817756553391096601232245559868964705058145717992093241870799473854914055009139155086319096028264766434917566251370714098114973374963517525236461045980178597718561990434099272052980073309745532498105505187056835169384247714811606853844508405826762352107914780108751431005686823299288076241931461212368503574018111289738690689757345492374907566789554952357721972836468149740999426018305519468920");
// Integer npq=Integer("22851248625660461847777944216946242442211760167752428242462399707021805078124330981722351281764679703678948267679192078661239425576182958362001412009171866472885734296441649628873515231274400107019679159708976846966279012695817756553391096601232245559868964705058145717992093241870799473854914055009139155086624638313037378978337167542909257709103408817185944956592426542411681770203963554353585179218052154877673461331843005539283281321076744367238406991949826885230225949527540846285782266316991306440295829079614985074378221271829506447103552356611364423156080506311488167751934965728300723514145399495754566172059");
// Integer privateD=Integer("15234165750440307898518629477964161628141173445168285494974933138014536718749553987814900854509786469119298845119461385774159617050788638908000941339447910981923822864294433085915676820849600071346452773139317897977519341797211837702260731067488163706579309803372097145328062161247199649236609370006092770057546064018843177623278377500913809398743315583309011683490974030653452398479041326956066181368653382206497021665403670124704556779589498476541071235896338937217841568071943186739167620670457882199525384161287640808245669049345407526492460459838230328249938377859703301571814648557645433160666284012203679645947");


using grpc::Status;
namespace ECSSE
{

    class Client
    {
    public:
        Client(std::shared_ptr<Channel> channel, std::string db_path) : stub_(RPC::NewStub(channel))
        {
            std::cout<<" client starting initializing!!!"<<std::endl;
            K_I=std::string((const char *)k_i, AES128_KEY_LEN);
            K_Z =std::string((const char *)k_z, AES128_KEY_LEN);
            K_X =std::string((const char *)k_x, AES128_KEY_LEN);
            size_t counter = 0;
            uc.clear();
            std::ifstream myfile("/home/helen/Desktop/ssesearch/odxt1/test/client.txt",std::ios::in);
            if(!myfile){
                logger::log(logger::ERROR) << "读取错误"<< std::endl;
                std::cout<< "error1  client db"<<std::endl;
                exit(-1);
            }
            std::string cd,cnt1,keyword;  //stoken fast/bost own ,  cnt1,cnt2 bestie own ,  cnt1 ecdsse own
            while(!myfile.eof() )
            {
                getline(myfile,cd);
                if(cd.empty()){
                    break;
                }
                int j=0;
                for (j= 0; j< cd.size(); j++)
                {   
                    if( (cd[j]>='a' && cd[j]<='z') || (cd[j]>='A' && cd[j]<='Z') ||(cd[j]>='0' && cd[j]<='9')){
                        keyword += cd[j];
                    }else{
                        j++;
                        break;
                    }
                }
                for (; j< cd.size(); j++)
                {   
                    if( (cd[j]>='a' && cd[j]<='z') || (cd[j]>='A' && cd[j]<='Z') ||(cd[j]>='0' && cd[j]<='9')){
                        cnt1+= cd[j];
                    }else{
                        j++;
                        break;
                    }
                }
                counter++;
                //assert(std::stoi(cnt1));
                uc[keyword]=std::stoi(cnt1);
                keyword.clear();
                cnt1.clear();
            }
            myfile.close();
            std::cout << "已存关键字数: "<< counter <<std::endl;

        }
        ~Client()
        {
            size_t keyword_counter = 0;
            std::unordered_map<std::string, size_t>::iterator it;
            std::fstream myfile("/home/helen/Desktop/ssesearch/odxt1/test/client.txt",std::ios::out);
            if(!myfile){
                logger::log(logger::ERROR) << "读取错误"<< std::endl;
                std::cout<< "error2 client db "<<std::endl;
                exit(-1);
            }
            for ( it = uc.begin(); it != uc.end(); ++it) {
                myfile<<it->first<<" "<<std::to_string(it->second)<<std::endl;
                keyword_counter++;
            }
            myfile.close();
            std::cout<< "实际存储关键字数： " << keyword_counter <<std::endl;
        }

        //字典存储与获取
        int store(const std::string key, const std::string value)
        {
            rocksdb::Status s = db->Delete(rocksdb::WriteOptions(), key);
            s = db->Put(rocksdb::WriteOptions(), key, value);
            if (s.ok())
                return 0;
            else
                return -1;
            assert(s.ok());
        }
        std::string get(const std::string key)
        {
            std::string temp;
            rocksdb::Status s = db->Get(rocksdb::ReadOptions(), key, &temp);
            if (s.ok())
                return temp;
            else
                return "";
        }

        //  st/uc get   2022.2.27
        void get_stuc(std::string keyword,size_t &c){
            std::unordered_map<std::string, size_t>::iterator it;		
            it = uc.find(keyword);
            if (it != uc.end()) {
                c = it->second;
            }
            else {
                c=0;
            }
            set_stuc(keyword,c);
        }

        void set_stuc(std::string keyword,size_t c){
            uc[keyword]=c;
        }        


        //获取更新次数/设定更新次数/更新增加一次
        int get_update_time(std::string keyword)
        {
            int update_time;
            std::unordered_map<std::string, size_t>::iterator it;
            it = uc.find(keyword);
            if (it != uc.end())
            {
                update_time = it->second;
            }
            else
            {
                update_time = 0;
                set_update_time(keyword, 0);
            }
            return update_time;
        }

        void set_update_time(std::string keyword, size_t update_time)
        {
            //std::lock_guard<std::mutex> lock(uc_mtx);
            uc[keyword] = update_time;
        }

        void increase_update_time(std::string keyword)
        {
            set_update_time(keyword, get_update_time(keyword) + 1);
        }

        //生成Kw=F(K,w) ,w填充到16byte
        std::string gen_enc_token(const std::string token){
            // 使用padding方式将所有字符串补齐到16的整数倍长度
            std::string token_padding;
            std::string enc_token;
            try
            {
                CFB_Mode< AES >::Encryption e;
                e.SetKeyWithIV(k, AES128_KEY_LEN, iv_s, (size_t)AES::BLOCKSIZE);
                token_padding = Util::padding(token);
                byte tmp_new_st[AES128_KEY_LEN];
                e.ProcessData(tmp_new_st, (byte*) token_padding.c_str(), AES128_KEY_LEN);
                enc_token= std::string((const char*)tmp_new_st, AES128_KEY_LEN);
            }
            catch(const CryptoPP::Exception& e)
            {
                std::cerr << "in gen_enc_token() 在生成kw中出问题 " << e.what()<< std::endl;
                exit(1);
            }
            return enc_token;
        }

        //生成状态st的方法
        std::string gen_st(std::string &keyword, size_t c,std::string &K_w)
        {
            std::string st_c;
            std::string w_str;
            try
            {
                w_str = K_w + std::to_string(c)+keyword;
                st_c = Util::H2(w_str);
                st_c =st_c.substr(0,16); //取前16位字节
            }
            catch (const CryptoPP::Exception &e)
            {
                std::cerr << "in gen_st() 生成状态stc中出问题" << e.what() << std::endl;
                exit(1);
            }
            return st_c;
        }

        void gen_update_token(std::string op, std::string keyword, std::string id, std::string &l, std::string &E)
        {
            std::string cipherid;  //加密id ,8byte
            std::string enc;   // 加密连接密文
            try
            {
                std::string old_st, new_st;  //新旧状态
                std::string K_w, id_padding;  //关键字子密钥和填充id
                size_t c = get_update_time(keyword);
                c++;
                set_update_time(keyword, c);     //缓存更新
                keyword = Util::H1(keyword).substr(0,8);  //保护关键字，如果关键字太长旧取32byte取16byte即可           
                K_w = gen_enc_token(keyword);    //生成子密钥Kw
                old_st = gen_st(keyword, c - 1,K_w); //旧状态16byte
                new_st = gen_st(keyword, c,K_w);     //新状态16byte

                //id_padding =Util::Id_padding(id);  //id填充到8byte
                //Util::encrypt(K_w,id_padding,cipherid);     //response-id   

                Util::encrypt(K_w,id,cipherid); 

                //加密连接密文
                Util::encrypt(new_st, old_st, enc);

                //生成字典键值对
                l = Util::H1(new_st);   //32 byte 的标签
                std::string l2 = l.substr(0, 16);  //异或标签
                E = Util::Xor(cipherid, l2);  //异或保护eid
                E = E+op+enc;   // 8+ 1 + 16= 25byte 的值
            }
            catch (const CryptoPP::Exception &e)
            {
                std::cerr << "in gen_update_token() " << e.what() << std::endl;
                exit(1);
            }
        }

        // 客户端RPC通信部分
        std::string search(const std::vector<std::string> KeywordList)
        {
            std::string st,xt,K_w,st_temp,xt_temp;
            std::string wx,z;
            size_t c;
            timeval t1,t2;
           //get_update_time()
            std::string w1 = KeywordList[0];
            //c= get_update_time(w1);
            get_stuc(KeywordList[0],c);
            gettimeofday(&t1, NULL);
            w1 = Util::H1(w1).substr(0, 8);
            K_w = gen_enc_token(w1);
            for(int i =1;i<=c;i++){
                //Util::encrypt(K_w,w1+std::to_string(i)+"0",st_temp);
                st_temp = Util::H2(K_w+w1+std::to_string(i)+"0").substr(0,16);
                st+=Util::str2hex(st_temp)+",";
                st_temp.clear();
                for(int j =1;j<KeywordList.size();j++){
                    Util::encrypt(K_Z,w1+std::to_string(i),z);   //Fp(Kz,w||c)
                    Integer zn = Util::str2num(z);
                    Util::encrypt(K_X,Util::H1(KeywordList[j]).substr(0,8),wx);  // wx = Fp(Kx,wj)
                    Integer wxn = Util::str2num(wx);
                    xt_temp= Util::num2str( a_exp_b_mod_c(g%p,a_times_b_mod_c(wxn,zn,q),p));
                   //std::cout<<i<<" "<<j<<std::endl;
                   //std::cout<<"xt_temp: "<<xt_temp<<std::endl;
                    xt+= Util::str2hex(xt_temp)+",";
                    xt_temp.clear();
                }
            }
            int counter= search(st,c,xt,w1,K_w,KeywordList.size());
            gettimeofday(&t2, NULL);
            logger::log(logger::INFO) <<"Search time: " << ((t2.tv_sec - t1.tv_sec) * 1000000.0 + t2.tv_usec - t1.tv_usec) /1000.0<< " ms" << std::endl;  //counter
            return "OK";
        }

        int search(const std::string st, const size_t c,const std::string xt,std::string w1,std::string K_w,int n)
        {
            SearchRequestMessage request;
            if (c == 0)
                request.set_st(""); // TODO attentaion here !!!
            else{
                request.set_st(st);
                request.set_xtoken(xt);
            }
            request.set_uc(c);
            request.set_xnum(n-1);
            //std::cout<<"xnum: "<< n-1<<std::endl;
            // Context for the client. It could be used to convey extra information to the server and/or tweak certain RPC behaviors.
            ClientContext context;
            context.set_compression_algorithm(GRPC_COMPRESS_DEFLATE);
            SearchReply reply;
            Status status = stub_->search(&context, request, &reply);
            //std::unique_ptr<ClientReaderInterface<SearchReply>> reader = stub_->search(&context, request);
            std::string valxor;
            std::string temp,op,rans,rj,rcnt,id,ed;
            std::vector<std::string> result;
            timeval t1,t2;
            gettimeofday(&t1,NULL);
            rans = reply.ind();
            rj = reply.j();
            rcnt = reply.cnt();
            std::vector<std::string> rind;
            std::vector<int> rjj;
            std::vector<int> rcntt;
            temp.clear();
            for(int i =0;i<rans.size();i++){
                if(rans[i]==','){
                    rind.push_back(Util::hex2str(temp));
                    temp.clear();
                }else{
                    temp+=rans[i];
                }
            }
            for(int i =0;i<rj.size();i++){
                if(rj[i]==','){
                    rjj.push_back(std::stoi(temp));
                    temp.clear();
                }else{
                    temp+=rj[i];
                }
            }
            for(int i =0;i<rcnt.size();i++){
                if(rcnt[i]==','){
                    rcntt.push_back(std::stoi(temp));
                    temp.clear();
                }else{
                    temp+=rcnt[i];
                }
            }
            std::cout<<rans.size()<<std::endl;
            std::cout<<rjj.size()<<std::endl;
            std::cout<<rcnt.size()<<std::endl;
            for(int i =0;i<rind.size();i++){
                int j = rjj[i];
                int cnt =rcntt[i];
                valxor = Util::H1(K_w+w1+std::to_string(j)+"1");
                id = Util::Xor(rind[i],valxor);
                op = id.substr(8,1);
                id =id.substr(0,8);
                // std::cout<<"op: "<<std::endl; 
                // std::cout<<"id: "<<std::endl;
                if(cnt==n && op=="1"){
                    result.push_back(id);
                    // logger::log(logger::INFO) << "conjunctive id: "<< id << std::endl;
                }
            }
            //while (reader->Read(&reply))
            //{
                // std::string id,ed;
                // ans = reply.ind();
                // j = reply.j();
                // cnt = reply.cnt();
                // //Util::encrypt(K_w,w1+std::to_string(j)+"1",valxor); 
                // valxor = Util::H1(K_w+w1+std::to_string(j)+"1");
                // id = Util::Xor(ans,valxor);
                // op = id.substr(8,1);
                // id =id.substr(0,8);
                // // if(op=="1"){
                // //     std::cout<<"op =1"<<std::endl;
                // //    // std::cout<<"id: "<<id<<std::endl;
                // // }
                // if(cnt==n && op=="1"){
                //     result.push_back(id);
                //     // logger::log(logger::INFO) << "conjunctive id: "<< id << std::endl;
                // }
            //}
            gettimeofday(&t2, NULL);
            logger::log(logger::INFO) <<"Search time client : " << ((t2.tv_sec - t1.tv_sec) * 1000000.0 + t2.tv_usec - t1.tv_usec) / 1000.0 << " ms" << std::endl;
            logger::log(logger::INFO) << " search result number size : "<< result.size() << std::endl;
            return result.size();
        }

        UpdateBF save_bf(){
            UpdateBF msg;
            msg.set_yes("1");
            return msg;
        }

        UpdateRequestMessage gen_update_request(std::string op, std::string keyword, std::string id)
        {
            try
            {
                //conjunctive scheme
                UpdateRequestMessage msg;
                std::string addr, l, E, cipherid, K_w,enc;
                std::string xind,z,y,xtag,alpha,wx,val,valxor;
                // size_t c = get_update_time(keyword);
                // c++;
                // set_update_time(keyword, c);
                std::string ts = keyword;
                size_t c;
                get_stuc(keyword,c);
                c++;
                set_stuc(keyword,c);
                keyword = Util::H1(keyword).substr(0, 8);
                K_w = gen_enc_token(keyword);
               // Util::encrypt(K_w,keyword+std::to_string(c)+"0",addr);  //addr = F(K_w , w+counter+0)
                //Util::encrypt(K_w,keyword+std::to_string(c)+"1",valxor);    // valxor = F(K_w,w+counter+1)
                addr = Util::H2(K_w+keyword+std::to_string(c)+"0").substr(0,16);
                valxor = Util::H1(K_w+keyword+std::to_string(c)+"1");

                val=Util::Xor(id+op,valxor);   //val = Xor(id+op,valxor)

                Util::encrypt(K_I,id+op,xind);   //xind= F(k_i,id || op) 9 byte
                Util::encrypt(K_Z,keyword+std::to_string(c),z);   //Fp(Kz,w||c)
                Integer zn = Util::str2num(z).InverseMod(q);
                Integer ynum = Util::str2num(xind); 
                Integer yzn = a_times_b_mod_c(zn,ynum,q);
                alpha = Util::num2str(yzn);
                Util::encrypt(K_X,keyword,wx);  // wx = Fp(Kx,w)
                Integer wxn = Util::str2num(wx);
                xtag = Util::num2str( a_exp_b_mod_c(g%p,a_times_b_mod_c(wxn,ynum,q),p));
                l=addr;
                E=val;
                msg.set_l(l);
                msg.set_e(E);
                msg.set_xtag(xtag);
                msg.set_alpha(alpha);
                return msg;
            }
            catch (const CryptoPP::Exception &e)
            {
                std::cerr << "in gen_update_request() " << e.what() << std::endl;
                exit(1);
            }
        }
        Status batch_update(std::vector<UpdateRequestMessage> update_list) {
            UpdateRequestMessage request;

            ClientContext context;

            ExecuteStatus exec_status;

            std::unique_ptr<ClientWriterInterface<UpdateRequestMessage>> writer(stub_->batch_update(&context, &exec_status));
            int i = 0;		
            while(i < update_list.size()){
                writer->Write(update_list[i]);
            }
            writer->WritesDone();
            Status status = writer->Finish();

            return status;
        }

    private:
        rocksdb::DB *db;
        std::unordered_map<std::string, size_t> uc; //client存的状态！
        //std::mutex uc_mtx;
        std::unique_ptr<RPC::Stub> stub_;
    };
}
#endif