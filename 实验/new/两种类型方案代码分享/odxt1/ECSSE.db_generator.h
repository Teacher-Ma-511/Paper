/*
 * Created by helen chan  on 19/02/2022
 * Email: kaibaking@gmail.com
 * 
 */
#include "ECSSE.client.h"
#include "ECSSE.Util.h"
#include "logger.h"
#include <cmath>
#include <chrono> // 是一个time library ，源于boost库，已经是c++标准了
#include<unordered_map>
namespace ECSSE
{
    std::unordered_map<std::string, std::vector<std::string>> Kid;
    long long sum = 0;
    void File_Upload(Client &client)
    {
        std::ifstream myfile("/home/helen/Desktop/sse/dataset/small.txt",std::ios::in);
        // std::ifstream myfile("/home/helen/Desktop/sse/dataset/medium.txt",std::ios::in);
        // std::ifstream myfile("/home/helen/Desktop/sse/dataset/big.txt",std::ios::in);
        //std::ifstream myfile("/home/helen/Desktop/sse/odxt/1.txt",std::ios::in);
      // std::ifstream myfile("/home/helen/Desktop/sse/dataset/update.txt",std::ios::in);
        if(!myfile){
            logger::log(logger::ERROR) << "读取错误"<< std::endl;
            exit(-1);
        }
        //100001 、320000、3116431
        // std::string cd,id,keyword;
        // for (int i = 0; i < 3116431; i++)
        // {
        //     getline(myfile,cd);
        //     id = cd.substr(0, 8);
        //     for (int j= 8; j< cd.size(); j++)
        //     {   
        //         if( (cd[j]>='a' && cd[j]<='z') || (cd[j]>='A' && cd[j]<='Z') ||(cd[j]>='0' && cd[j]<='9')){
        //             keyword += cd[j];
        //         }else{
        //             if(keyword==""){
        //                 continue;
        //             }
        //             Kid[keyword].push_back(id);
        //             //Kid[id].push_back(id);
        //             sum += 1;
        //             keyword = "";
        //         }
        //     }
        // }

        //1.txt 8 files
        std::string cd,id,keyword;
        for (int i = 0; i < 100001; i++)
        {
            getline(myfile,cd);
            id = cd.substr(0, 8);
            for (int j= 8; j< cd.size(); j++)
            {   
                if( (cd[j]>='a' && cd[j]<='z') || (cd[j]>='A' && cd[j]<='Z') ||(cd[j]>='0' && cd[j]<='9')){
                    keyword += cd[j];
                }else{
                    if(keyword==""){
                        continue;
                    }
                    Kid[id].push_back(keyword);
                    sum += 1;
                    keyword = "";
                }
            }
            if(keyword!=""){
                Kid[id].push_back(keyword);
                sum+=1;
                keyword="";
            }
        }
        //std::cout<<"jiazai wancheng"<<std::endl;
        myfile.close();
    }

    void Subjob(Client *client){
        //gRPC  update操作
        UpdateRequestMessage request;
        ClientContext context;
        ExecuteStatus exec_status;
        grpc::ChannelArguments channel_args;
	    channel_args.SetInt("grpc.max_receive_message_length",1024*1024*1024);
        std::unique_ptr<RPC::Stub> stub_(RPC::NewStub(grpc::CreateCustomChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials(),channel_args)));
        std::unique_ptr<ClientWriterInterface<UpdateRequestMessage>> writer(stub_->batch_update(&context, &exec_status));
        timeval t1,t2;
        for (auto it : Kid)
        {
            //gettimeofday(&t1,NULL);
            for(int i =0;i<it.second.size();i++){
                writer->Write(client->gen_update_request("1", it.second[i], it.first));
                //writer->Write(client->gen_update_request("1", it.first, it.second[i]));
            }
            //gettimeofday(&t2,NULL);
            //logger::log(logger::INFO) << it.second.size()<<" 更新时间： " << ((t2.tv_sec - t1.tv_sec) * 1000000.0 + t2.tv_usec - t1.tv_usec) / 1000.0 << " ms" << std::endl;

        }
        // 向服务器说明更新完事了
        writer->WritesDone();
        Status status = writer->Finish();
        //std::cout<<"subjob wenti"<<std::endl;
        assert(status.ok());
    }

    //文件数据导入
    void File_Get(Client &client){
        File_Upload(client);
        struct timeval t1, t2;
        gettimeofday(&t1, NULL);
        Subjob(&client);
        gettimeofday(&t2, NULL);
        logger::log(logger::INFO) << "总更新时间： " << ((t2.tv_sec - t1.tv_sec) * 1000000.0 + t2.tv_usec - t1.tv_usec) / 1000.0 << " ms" << std::endl;
        logger::log(logger::INFO)<< "更新关键字个数："<<Kid.size()<<std::endl;
        logger::log(logger::INFO)<< "更新键值对总数："<<sum<<std::endl;
    }
    void Save_BF(Client *client){
        UpdateBF request;
        ClientContext context;
        ExecuteStatus exec_status;
        //std::unique_ptr<RPC::Stub> stub_(RPC::NewStub(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials())));
        grpc::ChannelArguments channel_args;
	    channel_args.SetInt("grpc.max_receive_message_length",1024*1024*1024);
        std::unique_ptr<RPC::Stub> stub_(RPC::NewStub(grpc::CreateCustomChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials(),channel_args)));        
        std::unique_ptr<ClientWriterInterface<UpdateBF>> writer(stub_->save_BF(&context, &exec_status));
        writer->Write(client->save_bf());
        writer->WritesDone();
        Status status = writer->Finish();
        assert(status.ok());
    }

    // void test_BF(Client &client){
    //     Save_BF(&client);
    // }

} //namespace ECSSE
