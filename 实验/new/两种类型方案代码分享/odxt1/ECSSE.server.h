/*
 * Created by helen chan  on 19/02/2022
 * Email: kaibaking@gmail.com
 * 
 */
#ifndef ECSSE_SERVER_H
#define ECSSE_SERVER_H

#include <grpc++/grpc++.h>

#include "ECSSE.grpc.pb.h"

#include "ECSSE.Util.h"

#include "logger.h"
#include "bloom_filter.hpp"
#include <unordered_set>

#define min(x, y) (x < y ? x : y)

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;

using namespace CryptoPP;
Integer g=Integer("12293041548191560959252997681221893073091763148392091587781229526780505319280253304158657014186812803260820624200425866044892534107445664741934978636107726042273789227287786063826002069319009175216162933494359277792631586799194401273339746711856497988529049521334761338869458014310995080149317625448851976250");
Integer p=Integer("35846489658960820960446633115143023883040757131525173446156521584352743991679231122673876526186909574940183912868842246185467405988094236483899550393037943055836758636212712380713704715884242930459327176061234383344181152578596944695560082751184202851538548055157781713238676762830736663499575834426106137609");
Integer q=Integer("91677580900928952829845059692479440057913422538304665006082987855400898556463");
byte iv_s[17] = "0123456789abcdef";

namespace ECSSE
{

    class ECSSEServiceImpl final : public RPC::Service
    {
    private:
        static rocksdb::DB *ss_db;
        int MAX_THREADS; //最大线程数
        rocksdb::Options options;
        bloom_parameters parameters;
        bloom_filter bf;
        std::unordered_map<std::string,std::string> ssdb;
    public:
        ECSSEServiceImpl(const std::string db_path, int concurrent)
        {
            signal(SIGINT, abort); //中断信号，缺省行为终止进程
            MAX_THREADS = concurrent; 

            //bf kaishichushihua 
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

            size_t counter = 0;
            ssdb.clear();
            std::ifstream myfile(db_path,std::ios::in);
            if(!myfile){
                logger::log(logger::ERROR) << "读取错误"<< std::endl;
                std::cout<< "error1 ssdb"<<std::endl;
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
                    if(cd[j]==','){
                        keyword = Util::hex2str(keyword);
                        j++;
                        break;
                    }else{
                        keyword += cd[j];
                    }
                }
                for (; j< cd.size(); j++)
                {   
                     if(cd[j]==','){
                        cnt1= Util::hex2str(cnt1);
                        j++;
                        break;
                    }else{
                        cnt1 += cd[j];
                    }
                }
                counter++;
                //assert(std::stoi(cnt1));
                ssdb[keyword]=cnt1;
                keyword.clear();
                cnt1.clear();
            }
            myfile.close();
            std::cout << "已存ssdb keypair: "<< counter <<std::endl;

        }
        void setbf(){
        // How many elements roughly do we expect to insert?
            unsigned long long scount=200000000;
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
        static void abort(int signum)
        {
            //delete ss_db;
            logger::log(logger::INFO) << "abort: " << signum << std::endl;
            exit(signum);
        }

        static int store(rocksdb::DB *&db, const std::string l, const std::string e)
        {
            rocksdb::Status s;
            rocksdb::WriteOptions write_option = rocksdb::WriteOptions();
            {
                s = db->Put(write_option, l, e);
            }
            if (s.ok())
                return 0;
            else
                return -1;
        }

        static std::string get(rocksdb::DB *&db, const std::string l)
        {
            std::string tmp;
            rocksdb::Status s;
            {
                s = db->Get(rocksdb::ReadOptions(), l, &tmp);
            }
            if (s.ok())
                return tmp;
            else
                return "";
        }
        static void parse(std::string str, std::string &E, std::string &alpha)
        {
            E = str.substr(0, 9);
            alpha = str.substr(9);
        }

        //生成旧状态
        std::string gen_old_st( std::string &new_st, std::string &enc)
        {
            std::string old_st;
            Util::descrypt(new_st, enc, old_st);
            return old_st;
        }

        void search(std::string st,std::string xt, size_t uc,int xnum, std::string &edx,std::string &js,std::string &cntn)
        {   //server查询
            std::string ed;   //操作符，加密id索引
            std::string  temp, e,alpha;           
            std::vector<std::string> stoken,xtoken;
            for(int i =0;i<st.size();i++){
                if(st[i]!=',')
                    temp+=st[i];
                else{
                    stoken.push_back(Util::hex2str(temp));
                    temp.clear();
                }
            }
            logger::log(logger::INFO) << "stoken.size(): "<<stoken.size() << std::endl;
            temp.clear();
            std::vector<std::vector<std::string>> Xt;
            for(int i =0;i<xt.size();i++){
                if(xt[i]!=',')
                    temp+=xt[i];
                else{
                    //xtoken.push_back(Util::base64_decode(temp) );
                    xtoken.push_back(Util::hex2str(temp));
                    if(xtoken.size()%xnum==0){
                        Xt.push_back(xtoken);
                        xtoken.clear();
                    }
                    temp.clear();
                }
            }
            logger::log(logger::INFO) << "Xt.size(): "<<Xt.size() << std::endl;
            logger::log(logger::INFO) << "服务器开始搜索！" << std::endl;
            for (int i = 1; i <= uc; i++)
            {
                int cnt = 1;
                //e = get(ss_db, stoken[i-1]);
                e=ssdb[stoken[i-1]];
               // std::cout<<"stoken[i-1]: "<<stoken[i-1]<<std::endl;
               // std::cout<<"e: "<<e<<std::endl;
                if (e == "")
                {
                    //std::cout<<"st1: "<<1<<std::endl;
                    //logger::log(logger::ERROR) << "这个搜索没货！" << std::endl;
                    break;
                }
                //assert(e != "");
                parse(e, ed, alpha);  //T[U]= E||enc  9+16才科学
                //std::cout<<"ed.size(): "<<ed.size()<<std::endl;
                //std::cout<<"alpha: " <<alpha<<std::endl;
                for(int j =0;j<xnum;j++){
                    //std::cout<<i<<" "<<j+1<<std::endl;
                   // std::cout<<"Xt[i-1][j]: "<<Xt[i-1][j]<<std::endl;
                    Integer xto  = Util::str2num(Xt[i-1][j]);
                    Integer al = Util::str2num(alpha);
                    std::string tq= Util::num2str(a_exp_b_mod_c(xto,al,p));
                   // logger::log(logger::INFO)<<" xtoken calculate xtag: "<<tq<<std::endl;
                    if(bf.contains(tq)){
                        cnt++;
                    }
                }
                //std::cout<<"cnt : "<<cnt<<std::endl;
                //ID[i]= std::make_pair(ed,cnt);
                js+=std::to_string(i)+",";
                edx+=Util::str2hex(ed)+",";
                cntn+=std::to_string(cnt)+",";
            }
            //std::cout<<"js: "<<js.size()<<std::endl;
           // std::cout<<"edx: "<<edx.size()<<std::endl;
           // std::cout<<"cntn: "<<cntn.size()<<std::endl;
        }
            // server RPC
        Status search(ServerContext * context, const SearchRequestMessage *request,
                          SearchReply  *reply)
        {
            std::string st = request->st(); //获得最新状态st
            std::string xt = request->xtoken();
            size_t uc = request->uc();      //最新更新次数uc
            int xnum = request->xnum();
            struct timeval t1, t2;
            // TODO 读取数据库之前要加锁，读取之后要解锁
            logger::log(logger::INFO) << "搜索开始！" << std::endl;
            //std::map<int,std::pair<std::string,int>> ID;
            std::string edx,js,cntn;
            gettimeofday(&t1, NULL);
            search(st, xt,uc,xnum,edx,js,cntn);
            gettimeofday(&t2, NULL);
            logger::log(logger::INFO) << "搜索时间server: " << ((t2.tv_sec - t1.tv_sec) * 1000000.0 + t2.tv_usec - t1.tv_usec) / 1000.0<< " ms" << std::endl;
            // TODO 读取之后需要解锁
            //读取搜索数据
            //SearchReply reply;
            // for(auto it: ID){
            //     reply.set_ind(it.second.first);
            //     reply.set_j(it.first);
            //     reply.set_cnt(it.second.second);
            //     writer->Write(reply);
            // }
            reply->set_ind(edx);
            reply->set_j(js);
            reply->set_cnt(cntn);
            logger::log(logger::INFO) << "搜索结束！" << std::endl;
            return Status::OK;
        }
        // update()实现单次更新操作
        Status update(ServerContext * context, const UpdateRequestMessage *request, ExecuteStatus *response)
        {
            std::string l = request->l();
            std::string e = request->e();
            logger::log(logger::INFO) << "更新1个键值对！" << std::endl;
            int status = store(ss_db, l, e);
            if (status != 0)
            {
                response->set_status(false);
                return Status::CANCELLED;
            }
            response->set_status(true);
            return Status::OK;
        }

        // batch_update()实现批量更新操作
        Status batch_update(ServerContext * context, ServerReader<UpdateRequestMessage> * reader, ExecuteStatus * response)
        {
            std::string l,e,alpha,xtag;
            // TODO 读取数据库之前要加锁，读取之后要解锁
            logger::log(logger::INFO) << "更新批量的键值对！" << std::endl;
            UpdateRequestMessage request;
            while (reader->Read(&request))
            {
                l = request.l();
                e = request.e();
                xtag = request.xtag();
                alpha = request.alpha();
                //store(ss_db, l, e+alpha);
                ssdb[l]=e+alpha;
                bf.insert(xtag);
            }
            //new change 3.29
            //ssdb
            std::cout<<"start writing !! !!"<<std::endl;
            size_t keyword_counter = 0;
            std::unordered_map<std::string, std::string>::iterator it;
            std::fstream myfile("/home/helen/Desktop/ssesearch/odxt1/sdb/server1.txt",std::ios::out);
            if(!myfile){
                logger::log(logger::ERROR) << "读取错误"<< std::endl;
                std::cout<< "error2 ssdb update"<<std::endl;
                exit(-1);
            }
            for ( it = ssdb.begin(); it != ssdb.end(); ++it) {
                myfile<<Util::str2hex(it->first)<<","<<Util::str2hex(it->second)<<","<<std::endl;
                keyword_counter++;
            }
            myfile.close();
            std::cout<< "实际存储ssdb key pair： " << keyword_counter <<std::endl;

            response->set_status(true);
            return Status::OK;
        }

         Status save_BF(ServerContext * context, ServerReader<UpdateBF> * reader, ExecuteStatus * response)
        {
            logger::log(logger::INFO) << "save bf begins!" << std::endl;
            UpdateBF request;
            std::string yes;
            while (reader->Read(&request))
            {
                yes ="1";
            }
            // TODO 读取之后需要解锁
            //baocun bf
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
            response->set_status(true);
            return Status::OK;
        }
    };

}// namespace ECSSE
    //静态成员在主函数外部声明
    rocksdb::DB *ECSSE::ECSSEServiceImpl::ss_db;
    void RunServer(std::string db_path, int concurrent)
    {
        std::string server_address("0.0.0.0:50051"); //可修改服务器地址0.0.0.0
        ECSSE::ECSSEServiceImpl service(db_path,concurrent);
        ServerBuilder builder;
        builder.SetMaxMessageSize(256*1024*1024);
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        std::unique_ptr<Server> server(builder.BuildAndStart());
        ECSSE::logger::log(ECSSE::logger::INFO) << "服务器监听地址：" << server_address << std::endl;
        server->Wait();
    }
#endif // ECSSE_SERVER_H
