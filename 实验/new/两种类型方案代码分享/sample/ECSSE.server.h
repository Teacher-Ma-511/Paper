/*
 * Created by helen chan  on 06/11/2021
 * Email: kaibaking@gmail.com
 * 
 */
#ifndef ECSSE_SERVER_H
#define ECSSE_SERVER_H

#include <grpc++/grpc++.h>

#include "ECSSE.grpc.pb.h"

#include "ECSSE.Util.h"

#include "logger.h"

#include <unordered_set>

#define min(x, y) (x < y ? x : y)

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;

using namespace CryptoPP;

byte iv_s[17] = "0123456789abcdef";

namespace ECSSE
{

    class ECSSEServiceImpl final : public RPC::Service
    {
    private:
        static rocksdb::DB *ss_db;
        int MAX_THREADS; //最大线程数
        rocksdb::Options options;

    public:
        ECSSEServiceImpl(const std::string db_path, int concurrent)
        {
            signal(SIGINT, abort); //中断信号，缺省行为终止进程
            options.create_if_missing = true;
            Util::set_db_common_options(options); //数据库默认配置
            rocksdb::Status s1 = rocksdb::DB::Open(options, db_path, &ss_db);
            MAX_THREADS = concurrent; 
        }

        static void abort(int signum)
        {
            delete ss_db;
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
        static void parse(std::string str, std::string &E, std::string &enc,std::string&op)
        {
            E = str.substr(0, 8);
            op = str.substr(8, 1);
            enc = str.substr(9,AES128_KEY_LEN);
        }

        //生成旧状态
        std::string gen_old_st( std::string &new_st, std::string &enc)
        {
            std::string old_st;
            Util::descrypt(new_st, enc, old_st);
            return old_st;
        }

        void search(std::string st, size_t uc, std::unordered_set<std::string> &ID)
        {   //server查询
            std::string op,ed;   //操作符，加密id索引
            std::string  l,l2, e, enc, E;
            std::unordered_set<std::string> result_set; //结果集ER
            std::unordered_set<std::string> delete_set; //删除表DT                                 //状态寄托16字节
            logger::log(logger::INFO) << "服务器开始搜索！" << std::endl;
            for (size_t i = 1; i <= uc; i++)
            {
                l = Util::H1(st);
                //l2 = l.substr(0, 16);
                e = get(ss_db, l);
                // if (e == "")
                // {
                //     logger::log(logger::ERROR) << "这个搜索没货！" << std::endl;
                //     break;
                // }
                //assert(e != "");
                parse(e, ed, enc,op);  //T[U]= E||enc  9+16才科学
                //ed = Util::Xor(E,l2); //E异或H2(st) =(op||ed)
                if (op == "0") //0代表删除
                {
                    delete_set.insert(ed);
                }
                else
                {
                    std::unordered_set<std::string>::iterator it = delete_set.find(ed);
                    if (it != delete_set.end())
                    {
                        delete_set.erase(ed);
                    }
                    result_set.insert(ed);
                }
                //st = gen_old_st(st, enc);
                Util::descrypt(st,enc,st);
            }
            ID = result_set;
        }
            // server RPC
        Status search(ServerContext * context, const SearchRequestMessage *request,
                          ServerWriter<SearchReply> *writer)
        {
            std::string st = request->st(); //获得最新状态st
            size_t uc = request->uc();      //最新更新次数uc
            struct timeval t1, t2;
            // TODO 读取数据库之前要加锁，读取之后要解锁
            std::unordered_set<std::string> ID;
            logger::log(logger::INFO) << "搜索开始！" << std::endl;

            gettimeofday(&t1, NULL);
            //for(int i=0;i<1000;i++){
                search(st, uc, ID);
            //}
            //search(st, uc, ID);
            gettimeofday(&t2, NULL);
            logger::log(logger::INFO) << "查询结果集个数:" << ID.size() << " ,搜索时间: " << ((t2.tv_sec - t1.tv_sec) * 1000000.0 + t2.tv_usec - t1.tv_usec) / 1000.0<< " ms" << std::endl;
            // TODO 读取之后需要解锁
            //读取搜索数据
            SearchReply reply;
            for (auto it:ID)
            {
                reply.set_ind(it);
                writer->Write(reply);
            }
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
            std::string l;
            std::string e;
            // TODO 读取数据库之前要加锁，读取之后要解锁
            logger::log(logger::INFO) << "更新批量的键值对！" << std::endl;
            UpdateRequestMessage request;
            while (reader->Read(&request))
            {
                l = request.l();
                e = request.e();
                store(ss_db, l, e);
            }
            // TODO 读取之后需要解锁
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
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        std::unique_ptr<Server> server(builder.BuildAndStart());
        ECSSE::logger::log(ECSSE::logger::INFO) << "服务器监听地址：" << server_address << std::endl;
        server->Wait();
    }
#endif // ECSSE_SERVER_H
