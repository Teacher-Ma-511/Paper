# DSSE代码实现文档

---
author: 陈海龙、李晓聪
date: 2022-05-31
updated: 2022-06-06
email: `460674682@qq.com`

---



[TOC]

​	本文档由陈海龙(2022届毕业)主要编写，编辑器为markdown，如果有人希望添加新内容，本文档将在实验室内开源，方便传承。本文档的核心目标是详细描述如何快速实现DSSE方案。

## 1.全文总览

​	首先介绍实现方案所使用到的各类工具及其用途。有问题请发邮件给我(邮件在最顶上)或微信联系我。

### 1.1 GRPC

​	GRPC是GOOGLE公司开源的一套微服务框架，是一个**高性能、开源和通用**的 RPC 框架。基于**ProtoBuf(Protocol Buffers)** 序列化协议开发，且支持众多开发语言。本文选用C++版本进行实现，在DSSE方案中主要用来进行通信，即客户端程序可以通过GRPC与服务端进行通信，调用服务端的函数或方法进行计算后返回计算结果，在DSSE方案里表现为服务端搜索到结果文件返回给客户端。底层通信和编码代码由GRPC帮你完成，可以去了解一下原理。

```text
GRPC理论上封装的一般流程：
1、客户端（gRPC Stub）调用 A 方法，发起 RPC 调用。
2、对请求信息使用 Protobuf 进行对象序列化压缩（IDL）。
3、服务端（gRPC Server）接收到请求后，解码请求体，进行业务逻辑处理并返回。
4、对响应结果使用 Protobuf 进行对象序列化压缩（IDL）。
5、客户端接受到服务端响应，解码请求体。回调被调用的 A 方法，唤醒正在等待响应（阻塞）的客户端调用并返回响应结果
```

​	官方文档：`https://www.grpc.io/docs/languages/cpp/quickstart/`

​	前置知识：Protobuf ，最好先学习Protobuf3版本的语法和代码编写。



### 1.2 CryptoPP密码学开源库

​	也可以换成OPENSSL密码库，看个人喜好，我个人使用这个库是因为FASTIO的作者也在使用这个库，我与他有过交流，了解了如何实现和运用。

​	官网：`https://www.cryptopp.com/`

​	官网里有文档接口，可以看对应的密码学函数如何使用，此处不再赘述，注意如果要使用国密SM算法，那就得自己去实现。



### 1.3 RocksDB键值对数据库

​	使用这个键值对数据库的原因同上，其实可以使用别的键值对数据库，在其他文献里有人用redis、riak(https://docs.riak.com/)。本文选用版本为5.7.3，使用这个版本是因为fastio的开源代码使用了这个版本。

​	官网:`http://rocksdb.org/`

​	github:`https://github.com/facebook/rocksdb`



### 1.4 Boost库

​	boost库主要用来实现序列化与反序列化(bloom filter)，因为实现一个布隆过滤器的时候，有时候关闭了服务端程序，需要将bf的状态保存到硬盘，再次打开服务端程序时需要将其重新从硬盘中读取到内存，这个过程需要序列化和反序列化。如果你使用别的方法实现了对一个对象的序列化与反序列化，也可以不用安装该库。

​	官网：`https://www.boost.org/`



### 1.5 OPENSSL开源库

​	本人有想过放弃cryptopp密码库，迁移到openssl密码库里去，想了想工作量有点大，还是交给师弟师妹们去做吧，有喜欢折腾的可以自行迁移到OPENSSL开源库，同理如果需要其他更精确的大数库可以迁移到gmp库、ntl库等。



### 1.6 未来工作

- 可以考虑将搭建的环境封装成docker或虚拟机保存下来，一代一代传承，也可以尝试将每个安装的库升级到最新版，防止版本过旧，不好找安装包。
- 本文档使用markdown编写，未来继续做这个方向的人可以继续添加新内容，把自己的知识和工作留给后人，也是一件比较好的传承工作，需要各位贡献自己的一份力量。建议自行学习markdown语法，里面的公式语法可以在使用Latex写论文时使用到，请务必学习一下。参考网站：`https://www.zybuluo.com/codeep/note/163962#12%E5%A6%82%E4%BD%95%E8%BE%93%E5%85%A5%E5%85%B6%E5%AE%83%E7%89%B9%E6%AE%8A%E5%AD%97%E7%AC%A6`
- SSE方向的能力需求：latex文档编写能力、掌握C++\Java\Python等任意一门主流编程语言、算法与工程能力、熟悉常见数据结构(可能在未来的方案中使用不同的数据结构)、Linux基础(常见linux基础命令、操作系统基础)、密码学基础(语义安全定义及安全性证明和常见密码学算法原理)



## 2.环境搭建

​	本文实现的DSSE方案均在Unix类环境下运行，请务必保证您的系统为Linux或Mac os，一定要在Windows下执行，可以使用WSL。本文以Ubuntu 20.04LTS为例进行环境配置。建议使用VMware虚拟机进行配置，万一出错可以用快照恢复到上一个步骤，节省时间。

​	本文默认已安装VMware Workstation + Ubuntu20.04LTS 虚拟机或Windows下的WSL，如果没有安装就自行谷歌，建议Ubuntu使用全英文配置(English)，防止因为需要安装输入法而更加麻烦，如果喜欢折腾当我没说。虚拟机内存可以自行设置，越大越好。

### 2.1 Root用户权限

​	换源前必做的一个操作：给root权限用户添加密码，这将为之后的sudo命令提供权限

```
在安装Ubuntu系统的时候，就会有一个操作是设置普通用户，这个用户的账户名和密码由你来设置。
安装成功后进入系统，使用以下命令进行配置：
sudo passwd  
该命令可以赋予root权限用户一个密码，密码由你自己设定
但是应该要输入3次，第一次是你安装时设置的用户的密码，第二第三次是root用户的密码
```

​	![](C:\Users\helen\Desktop\DSSE代码实现\1.jpg)

​	如果是VMware虚拟机安装的系统，需要安装vmware tools，这个工具可以传输虚拟机和宿主机的文件，还可以调整屏幕分辨率，避免出现下图里的情况。

![](C:\Users\helen\Desktop\DSSE代码实现\2.jpg)

```
安装流程：
1.点击虚拟机(M)->安装VMWARE TOOLS(或重新安装 VMWARE TOOLS)，如果不能点就重启一下
2.系统弹出一个VMWARE TOOLS的文件夹，点击文件夹(home->Desktop)，将tar.gz文件复制到Desktop文件夹下
3.在desktop文件夹下打开终端(terminal)，按顺序输入命令
tar -xvzf VMwareTools-10.3.10-13959562.tar.gz 
cd vmware-tools-distrib/
sudo ./vmware-install.pl

有no/yes选项的选yes
路径的就按Enter键选默认

安装成功后重启
```

![](C:\Users\helen\Desktop\DSSE代码实现\3.jpg)



### 2.2 换源

​	第一件事就是换源。为什么要换源呢？因为Ubuntu的APT安装源默认是国外的服务器，国内连接速度较慢，最好切换为国内的软件源，这样安装其他软件速度会快上不少。

​	国内的软件源挺多的：有清华镜像源、中科大镜像源、阿里镜像源等

​	推荐使用中科大镜像源镜像源

​    镜像源网站：`https://mirrors.ustc.edu.cn/help/ubuntu.html`

```
选择Ubuntu 20.04LTS
具体步骤：  
输入命令：
sudo gedit /etc/apt/sources.list
替换里面的东西为下面的，#注释的东西可以不用写

# 默认注释了源码仓库，如有需要可自行取消注释
deb https://mirrors.ustc.edu.cn/ubuntu/ focal main restricted universe multiverse
# deb-src https://mirrors.ustc.edu.cn/ubuntu/ focal main restricted universe multiverse

deb https://mirrors.ustc.edu.cn/ubuntu/ focal-security main restricted universe multiverse
# deb-src https://mirrors.ustc.edu.cn/ubuntu/ focal-security main restricted universe multiverse

deb https://mirrors.ustc.edu.cn/ubuntu/ focal-updates main restricted universe multiverse
# deb-src https://mirrors.ustc.edu.cn/ubuntu/ focal-updates main restricted universe multiverse

deb https://mirrors.ustc.edu.cn/ubuntu/ focal-backports main restricted universe multiverse
# deb-src https://mirrors.ustc.edu.cn/ubuntu/ focal-backports main restricted universe multiverse

# 预发布软件源，不建议启用
# deb https://mirrors.ustc.edu.cn/ubuntu/ focal-proposed main restricted universe multiverse
# deb-src https://mirrors.ustc.edu.cn/ubuntu/ focal-proposed main restricted universe multiverse

保存文件，然后运行下面的命令：
sudo apt-get update
sudo apt-get upgrade

然后运行结束就换源成功
```

![](C:\Users\helen\Desktop\DSSE代码实现\4.jpg)



### 2.3 安装基础工具

​	常用的C++库、系统配置等所需配置安装。

```
直接运行下面的命令（来源于bost开源文档，基本上满足需求）
sudo apt-get install build-essential autoconf libtool yasm openssl scons git net-tools
```



### 2.4 安装CMAKE

​	选用cmake工具来辅助编译代码。

​	官网：`https://cmake.org/`

```
下载地址：https://cmake.org/files/v3.17/ 
选择cmake-3.17.3.tar.gz
通过vmware tools直接拖入到虚拟机的磁盘文件夹里，我放在/home/kali/Desktop中
安装流程：
解压文件：
tar -xvzf cmake-3.17.3.tar.gz
cd cmake-3.17.3/
./bootstrap && make && sudo make install

可能出现这个错误：
-- Could NOT find OpenSSL, try to set the path to OpenSSL root folder in the system variable OPENSSL_ROOT_DIR (missing: OPENSSL_CRYPTO_LIBRARY OPENSSL_INCLUDE_DIR) 
CMake Error at Utilities/cmcurl/CMakeLists.txt:454 (message):
  Could not find OpenSSL.  Install an OpenSSL development package or
  configure CMake with -DCMAKE_USE_OPENSSL=OFF to build without OpenSSL.

解决方法：
sudo apt install libssl-dev  
//问题的原因是找不到openssl，所以需要下一个openssl库帮助我们找到这个库

重新执行上面那个命令：
./bootstrap && make && sudo make install

验证一下是否安装成功：
cmake --version
```

![](C:\Users\helen\Desktop\DSSE代码实现\5.jpg)



### 2.5 Vscode IDE

​	我选择编写代码的环境：VSCODE，开源编辑器，插件多，可满足各种需求。

​	官网下载：`https://code.visualstudio.com/`

```
安装流程：
下载deb包，通过vmware tools直接拖入到虚拟机的磁盘文件夹里，我放在/home/kali/Desktop中
用以下命令进行安装
sudo dpkg -i code_1.67.2-1652812855_amd64.deb

选一个终端,使用以下命令打开vscode
code .
```

![](C:\Users\helen\Desktop\DSSE代码实现\7.jpg)



### 2.6 安装RocksDB

​	也可以换成别的键值对数据库，想用别的自己学吧。

```
下载地址：
https://github.com/facebook/rocksdb/releases/tag/rocksdb-5.7.3 
我下的是tar.gz格式的压缩包
通过vmware tools直接拖入到虚拟机的磁盘文件夹里，我放在/home/kali/Desktop中
安装流程：
1.解压tar.gz压缩包
tar -xvzf rocksdb-rocksdb-5.7.3.tar.gz 
2.安装依赖库
sudo apt-get install libgflags-dev libsnappy-dev zlib1g-dev libbz2-dev liblz4-dev libzstd-dev
3.进入文件夹
cd rocksdb-rocksdb-5.7.3/

4.开始安装流程：
make shared_lib -j4
sudo make install

```



```
验证安装是否成功代码：
#include <cstdio>
#include <string> 
#include "rocksdb/db.h" 
#include "rocksdb/slice.h" 
#include "rocksdb/options.h" 
using namespace std; 
using namespace rocksdb;  
const std::string PATH = "/tmp/rocksdb_tmp";  
int main(){     
    DB* db;    
    Options options;     
    options.create_if_missing = true;     
    Status status = DB::Open(options, PATH, &db);     
    assert(status.ok());     
    Slice key("foo");     
    Slice value("bar");     
    std::string get_value;     
    status = db->Put(WriteOptions(), key, value);     
    if(status.ok()){         
        status = db->Get(ReadOptions(), key, &get_value);         
        if(status.ok()){             
            printf("get %s success!!\n", get_value.c_str());         
        }else{            
            printf("get failed\n");         
        }     
    }else{         
        printf("put failed\n");   
    }     
    delete db;
    return 0;
}

编译命令：
g++ -std=c++11 -o rocksdbtest test.cpp -lrocksdb -lpthread -ldl

运行
./rocksdbtest

运行报错：error while loading shared libraries: librocksdb.so.5.7: cannot open shared object file: No such file or directory
解决方案：
sudo ldconfig
```

![](C:\Users\helen\Desktop\DSSE代码实现\6.jpg)



### 2.7 安装CryptoPP密码学库

```
安装流程：
sudo apt-get install libcrypto++-dev libcrypto++-doc libcrypto++-utils
apt-cache pkgnames | grep -i crypto++   //查看版本和安装成功无？

```

![](C:\Users\helen\Desktop\DSSE代码实现\9.jpg)



```
验证代码：
#include <bits/stdc++.h>
#include <crypto++/integer.h>
using namespace std;
using namespace CryptoPP;

int main()
{
    Integer i("11212321321312");
    i -= 2;
    cout << i << endl;
    return 0;
}

编译命令：
g++ main.cpp -o main -lcryptopp 

运行：
./main
```

![](C:\Users\helen\Desktop\DSSE代码实现\8.jpg)



### 2.8 安装GRPC

​	前置条件：安装CMAKE，版本cmake-3.17.3以上，因为github有墙，请务必提前弄好自己的linux终端代理科学上网，否则下载不下来grpc的所有文件。

```
教程：https://chromium.googlesource.com/external/github.com/grpc/grpc/+/chromium-deps/2016-07-19/examples/cpp/helloworld/README.md

ipaddress.com  查询解析DNS
修改
sudo gedit /etc/hosts 
140.82.112.4 github.com
#建议以下步骤开vpn代理下载比较快

git clone https://github.com/grpc/grpc.git
cd grpc
git submodule update --init   
make -j8
sudo apt-get install build-essential autoconf libtool pkg-config clang libc++-dev

cd cmake
cmake ..
make
sudo make install 


可以用grpc里的examples测试一下是否安装成功
cd examples/cpp/helloworld
mkdir  -p  cmake/build
pushd cmake/build
cmake –DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..
make –j4

//下面的命令说明protobuf是否正确安装
protoc --version 
```



### 2.9 安装BOOST

```
https://sourceforge.net/projects/boost/ 去官网上下载：
我选择下载 boost_1.57.0.tar.gz，通过vmware tools直接拖入到虚拟机的磁盘文件夹里，我放在/home/kali/Desktop中
解压文件
tar -xvzf boost_1_57_0.tar.gz 
cd boost_1_57_0
./bootstrap.sh --with-libraries=all
./b2
sudo ./b2 install
```

![](C:\Users\helen\Desktop\DSSE代码实现\10.jpg)



## 3.数据集处理

### 3.1 WikiPedia数据集处理

```
1.wikipedia extrator 安装 (维基百科数据集处理工具)
方法1：
git clone https://github.com/attardi/wikiextractor wikiextractor
cd wikiextractor
python setup.py install

方法2：(推荐该方法)
pip install wikiextractor
建议python版本为python3.7以下，3.3以上，不然可能报错

2.https://dumps.wikimedia.org/enwiki/20220501/
下载article.xml.bz2 格式的压缩包，可以选小的下（如图所示）。
我以其中一个小的数据集为例，比如
enwiki-20220501-pages-articles27.xml-p69975910p70682240.bz2

将数据集下载后放到自己指定的文件夹内，执行下面命令
mkdir extra
wikiextractor -b 1024M -o ./extra enwiki-20220501-pages-articles27.xml-p69975910p70682240.bz2

进入extra/AA 里寻找 wiki_00等提取文件即可(此时文档是XML格式的)
```

![](C:\Users\helen\Desktop\DSSE代码实现\11.jpg)



```python
#如果提取的wiki文档有多个，可以使用上面的这些注释代码去批处理
#y = open('tji.txt','r')
#num=[]
#filename =[]
#outdir =[]
# for i in range(32):
#     s = y.readline()
#     g=len(s)
#     s=s[:g-1]
#     nums=s[12:19]
#     strs=s[0:11]
#     odir ='new_'+str(i)+'.txt'
#     num.append(int(nums) )
#     filename.append(strs)
#     outdir.append(odir)
#     #print(s)
# print(num)
# print(filename)
# print(outdir)
#y.close()

#以下是案例代码，即以上面的enwiki-20220501-pages-articles27.xml-p69975910p70682240.bz2提取的xml数据集为例进行处理。
num=[1267449]   #num数组代表每个文档里的行数，如图所示的左下角红框。
filename =['wiki_00']
outdir =['ans_00']
for r in range(1):
    fx = filename[r]
    nx = num[r]
    ox = outdir[r]
    f = open(fx, "r")
    q = open(ox, 'w')
    i = 0
    id = ''
    k = []
    temp = ''
    while(i < nx):
        s = f.readline()
        if(len(s)==0):
            i+=1
            continue
        if(s[0] == '\n'):
            i = i+1
            continue
        elif(s[0] == '<'):
            if s[1] == 'd':
                j = 9

                while(True):
                    if s[j] == '\"':
                        break
                    id = id+s[j]
                    j += 1
            elif s[1] == '/':
                q.write(id+' ')
                for l in k:
                    ss = str(l).replace(
                        "[", " ").replace("]", " ").replace(",", "")
                    q.write(ss+" ")
                q.write('\n')
                k = []
                id = ''
        else:
            for g in range(len(s)):
                if s[g].isalpha() or s[g].isalnum():
                    temp += s[g]
                else:
                    k.append(temp)
                    temp = ''
        # print(s)
        # print(s)
        # break
        i = i+1
    q.close()
    f.close()
print("address all new_txt to final!!!!\n")
```

![](C:\Users\helen\Desktop\DSSE代码实现\13.jpg)

处理后结果图：

![](C:\Users\helen\Desktop\DSSE代码实现\14.jpg)



### 3.2 Enron数据集处理

```
下载地址：https://www.cs.cmu.edu/~./enron/
http://www.enron-mail.com/
压缩包：1.7G左右

//解析该邮件集合的论文：
https://web.eecs.umich.edu/~mihalcea/papers/lahiri.jnle16.pdf

处理办法（这里我没有实现，可以自由发挥）：
1.可以尝试使用RAKE算法进行邮件关键字提取(rapid automatic keyword extraction)，这里不给出方法。
2.也可以尝试自己写脚本批处理所有的文件关键字：
下面是2020年的Secure Dynamic Searchable Symmetric Encryption With Constant Client Storage Cost 文章里提到的对enron数据集的处理，可以参考一下，没有给具体方法。
We exploited RAKE, a python implementation
of the rapid automatic keyword extraction, to extract keywords
from the Enron email dataset. After discarding some files in
BASE64 encoding, we got 517,080 plain-text files. From those
files, we extracted 390,423 keywords, and the total number
of keyword/document pairs was 22,900,317. To analyze the
impact of database size, we also derived three databases
from the Enron email dataset.
```

![](C:\Users\helen\Desktop\DSSE代码实现\15.jpg)



### 3.3 自行生成键值对数据集

​	提供一个思路，具体怎么实现可以自行决定，以下是python脚本。

```python
#coding=UTF-8
import random
import string
# print(string.digits)
# len = 80000
seed = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
str1 = []
len1 = [8000, 40000, 80000]
f = open('nmd.txt', 'w')

#下面的代码将生成倒排表
for i in range(3):  # 可以自行设定要多少个倒排表
    f.write("w" + str(i + 1))  #第一个单词写入关键字wi
    for j in range(len1[i]):  #在这一行关键字后面加入随机的文档标识符，数量由len1[i]决定
        str1.append(random.choice(seed))
        if((j+1) % 8 == 0):
            StringS = ''.join(str1)
            f.write(" "+StringS)
            str1.clear()
           # print(StringS)
    f.write('\n')
    str1.clear()
f.close()
```



## 4.具体方案实现

### 4.1 密码学工具

​	均在ECSSE.Util.cc 和 ECSSE.Util.h里给出，直接调用接口即可。

接口详情：

- 3种SHA256哈希函数(H1-H3)： string H1(string message) ; //它们的区别是加入的盐值不同
- 填充函数padding： string padding(string str) ;  //自行查看细节，可以自己调节填充大小
- 填充函数Id_padding和remove_Id_padding：我写的专门用来将不满足8字节大小的ID进行填充\移除的函数
- 异或函数Xor： string Xor(string s1, string s2)； //注意s1的长度不能比s2大，否则报错。
- 字符串转16进制字符函数str2hex： string str2hex(string input); //可将字符串转换为16进制数类型的字符串
- 16进制字符转字符串函数hex2str： string hex2str(string input); //可将16进制数类型的字符串转换成字符串
- 加密函数/伪随机函数encrypt(string key , string  plaintext,string ciphertext); //参数为密钥、明文和密文
- 解密函数descrypt(string key , string  ciphertext,string plaintext); //参数为密钥、密文和明文
- Rocksdb设置接口：set_db_common_options(rocksdb::Options & options); //里面包含了一些rocksdb的设置，如果有需要可以自己查文档进行调节。
- 字符串转数字(大整数) \ 数字(大整数)转字符串 函数： string num2str(Integer num); //输入大整数类型num，输出字符串string。Integer str2num(string str);  //相反操作
- log256函数： int log256(Integer n); //是字符串转数字等函数的必须操作，不能删除，可以自己理解一下。 



计时方法：

```c++
struct timeval t1, t2;
gettimeofday(&t1, NULL);
//执行的函数
gettimeofday(&t2, NULL);
cout << "总更新时间： " << ((t2.tv_sec - t1.tv_sec) * 1000000.0 + t2.tv_usec - t1.tv_usec) / 1000.0 << " ms" << std::endl;
```



### 4.2 方案整体架构

​	本人所实现的代码编译后包括3个可执行程序，它们分别为：

- rpc_server ：服务器端主程序，它表现为一直监听端口的消息，等待客户端连接和处理搜索。
- rpc_client：客户端搜索程序，它表现为连接服务端并实现查询算法的功能。
- update：客户端更新程序，它表现为连接服务端并实现更新算法的功能。

如何编译：配置完2.1~2.9

```
make -j8 // 可以直接使用makefile进行编译，-j8是参数，表示使用多个线程同时编译，能更快编译

如果编译后要修改代码，先执行以下命令清除编译文件：
make clean 
make exe 

上述命令可以在学会makefile的编写后自行对makefile进行编辑修改。

如果makefile 报错，可能是因为多线程并发(-j8参数)，需要再执行一次，直到出现可执行文件为止。

还有：
每一个文档里我都有写一个d.sh类似的shell脚本，这些脚本的用途为删除之前执行更新、查询算法生成的文件，建议编译后先执行一遍该脚本。
注意修改你们的脚本内路径为满足自己环境里的文件路径
(比如我的是/home/helen/Desktop/ssesearch/EFOXT1/test),到你的环境里就可能是其他用户名，比如/home/xiaocong/Desktop/ssesearch/EFOXT1/test，请务必注意你的路径位置是否正确。
如果脚本没有权限，可以使用下面的命令赋予其执行权限
sudo chmod +x ./d.sh

.grpc.pb.cc、.grpc.pb.h、.pb.cc 、 .pb.h等文件是makefile执行命令自行生成，这是grpc帮我们编译生成的，不需要管代码编辑器中的标红信息。

```

![](C:\Users\helen\Desktop\DSSE代码实现\18.jpg)



​	整体架构：

​	1.ECSSE.client.h

​	该函数主要负责实现各种客户端需要执行的算法，并使用grpc将搜索令牌或更新数据发送给服务端。

​	2.ECSSE.server.h

​	该函数主要负责实现各自服务端需要执行的算法，并使用grpc将搜索结果发送给客户端。

​	3.ECSSE.db_generator.h

​	该函数主要实现客户端本地明文数据集的预处理和更新算法的调用。

​	4.bloomfilter.hpp

​	布隆过滤器的一种C++实现，可以自行查看代码进行接口调用。

​	5.ECSSE.Util.h

​	所有你可能需要的密码学工具，自己查看Util.cc里的具体实现，也许可以自己修改一下。

​	6.logger.h

​	本人用来debug的打印函数，可以用也可以不用，使用方法在代码里的，自行查阅。

​	7.ECSSE.proto

​	该文件主要是grpc的protobuf参数设置，建议先自行学习protobuf3的语法再回来就明白并且可以修改了。

​	8.Makefile

​	makefile编译文件，建议先学习一下makefile和cmake等编译工具的用法，方便调试debug。

![](C:\Users\helen\Desktop\DSSE代码实现\19.jpg)

如何运行可执行程序：

```
编译完成后，开启两个终端(LAN环境)，如果是WAN环境，就在云服务器上运行rpc_server程序

1.第一个终端运行服务器程序：
./rpc_server ./sdb 4

2.第二个终端运行客户端数据更新程序：
./update ./test 4

3.第二个终端运行客户端数据查询程序(在更新完成后运行)：
./rpc_client ./test 4

第二个参数是存储文件夹的位置，这里server端是读取/存储到sdb文件夹内，client存储\读取到test文件夹内。
参数4代表一个线程数，但是其实没有实现多线程，所以这个参数也可以在代码里删除，不过需要自己去清除。

以rpc_server.cc为例：
该函数接收2个终端参数，不够2个参数就会报错。也可以设定为只需要1个文件夹路径参数，即argc<2才报错。
具体的修改可以自己想。

如果运行完更新程序后，运行搜索程序发现搜索时间过长，可以ctrl + c 杀掉server和client进程，重新运行server进程，等待server将数据读入内存后再执行client搜索程序，这样就没有磁盘IO的影响。
```

![](C:\Users\helen\Desktop\DSSE代码实现\20.jpg)



### 4.3 给出代码的其他方案(可以自行补全)

- OSSE和LLSE：`https://github.com/jgharehchamani/OS-SSE`

论文：eprint上的2022-648，叫做Dynamic Searchable Encryption with Optimal Search in the Presence of Deletions

- SDA、SDD和QOS：`https://github.com/jgharehchamani/Small-Client-SSE`

论文：NDSS2020，Dynamic Searchable Encryption with Small Client Storage

- Mitra、Orion、Horus、Fides和Diana_Del：`https://github.com/jgharehchamani/SSE`

论文：New Constructions for Forward and Backward Private Symmetric Searchable Encryption

- sophos：`https://github.com/OpenSSE/opensse-schemes`

论文：Bost，CCS16和CCS17，forward and backward private Searchable Symmetric Encryption

Σoφoς – Forward Secure Searchable Encryption 和Forward and Backward Private Searchable Encryption from Constrained Cryptographic Primitives

- FASTIO：`https://github.com/BintaSong/DistSSE/tree/2.0`

论文：Binta Song ，Forward Private Searchable Symmetric Encryption with Optimized I/O Efficiency

- NSSEN：本代码是本人通过邮件向作者咨询获取，需要的话本人可以通过邮件发送。

论文：shi-feng sun，Non-Interactive Multi-Client Searchable Encryption: Realization and Implementation

- PHXT、FHXT：该方案是由2020届毕业生古宜平和李洪飞共同编写，其中代码部分由李洪飞编写，他已将代码给予了本人，但是本人也没有跑过，但是感觉修改一下就能跑，可以阅读一下，加深对SSE代码实现的理解，有需要的可以联系本人。

论文： Ma chang she 、Gu yiping、Li hongfei等，Practical Searchable Symmetric Encryption Supporting Conjunctive Queries Without Keyword Pair Result Pattern Leakage

- KafeDB：`https://github.com/zheguang/encrypted-spark`

论文：关系型数据库加密系统设计：KafeDB: a Structurally-Encrypted Relational Database Management System

​	看论文的时候，看该论文的experimental evaluation的章节有没有介绍方案环境，看看是否有开源实现，可以找一下github、gitlab等关键字，这样可以慢慢积累不同的方案代码实现。如果没有给代码的话，可以发邮件给作者去询问能否获取源码。



### 4.4 使用C++代码连接到MYSQL

（后期可以尝试将SSE连接到MYSQL，本人没实现，只是给个C++代码连接方法）

```
1.安装mysql
sudo apt install mysql-server

//验证mysql是否启动
sudo systemctl status mysql  

//直接进入mysql
sudo mysql

//设置mysql的root初始密码
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '12345678';

//MySQL安装随附一个名为的脚本mysql_secure_installation，可让您轻松提高数据库服务器的安全性。
调用不带参数的脚本：
sudo mysql_secure_installation

根据情况自主选择（以下仅作参考）：
是否建立密码验证插件（用以验证密码强度）：n
首次运行则会要求输入并确认root密码，设置过第2步的root初始密码则会提示是否修改密码。
如果遇到以下报错，请先执行上面第2步的设置root初始密码：
Failed! Error: SET PASSWORD has no significance for user ‘root’@’localhost’ as the authentication method used doesn’t store authentication data in the MySQL server. Please consider using ALTER USER instead if you want to change authentication parameters.
是否删除匿名用户：y
是否禁止root远程登陆：n
是否删除test数据库：y
刷新权限：y


//登录MySQL，创建新用户及修改权限
mysql -u root -p

//给root账号开放所有权限
GRANT ALL PRIVILEGES ON *.* TO root@'localhost';

//@后面'localhost'就是仅限本地访问，配置成'%'就是所有主机都可连接
CREATE USER new_user@'%' IDENTIFIED BY 'your_password';
GRANT ALL PRIVILEGES ON *.* TO new_user@'%' ;
//修改密码
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'your_password';

//MySQL服务的启动/停止/重启
# 启动MySQL服务
service mysql start
# 停止MySQL服务
service mysql stop
# 重启MySQL服务
service mysql restart



2.c++调用MYSQL API
安装依赖 sudo apt install mysql-server mysql-client libmysqlclient-dev
然后用下面的验证代码进行测试是否能顺利用API函数连接到MYSQL

```

![](C:\Users\helen\Desktop\DSSE代码实现\16.jpg)



验证代码：

```c++
#include <iostream>  
#include<bits/stdc++.h>
#include<stdio.h>
#include <mysql.h>    //引入mysql头文件(一种方式是在vc++目录里面设置，一种是文件夹拷到工程目录，然后这样包含)  
 
MYSQL mysql; //mysql连接
MYSQL_FIELD *fd;  //字段列数组
char field[32][32];  //存字段名二维数组
MYSQL_RES *res; //这个结构代表返回行的一个查询结果集
MYSQL_ROW column; //一个行数据的类型安全(type-safe)的表示，表示数据行的列
char query[150]; //查询语句
 
bool ConnectDatabase();    	//函数声明  
void FreeConnect();
bool QueryDatabase1();  	//查询1  
bool QueryDatabase2();  	//查询2  
bool InsertData();		//增
bool ModifyData();		//改
bool DeleteData();		//删
int main(int argc, char **argv)
{
	ConnectDatabase();
	QueryDatabase1();
	//InsertData();
	//QueryDatabase2();
	//ModifyData();
	//QueryDatabase2();
	//DeleteData();
	//QueryDatabase2();
	FreeConnect();
	return 0;
}
//连接数据库  
bool ConnectDatabase()
{
	//初始化mysql  
	mysql_init(&mysql);  //连接mysql，数据库  
	const char host[] = "localhost";
	const char user[] = "root";
	const char psw[] = "12345678";
	const char table[] = "mysql";//需要链接的数据库
	const int port = 3306;
	//返回false则连接失败，返回true则连接成功  
	if (!(mysql_real_connect(&mysql, host, user, psw, table, port, NULL, 0)) )
		//中间分别是主机，用户名，密码，数据库名，端口号（可以写默认0或者3306等），可以先写成参数再传进去  
	{
		printf("Error connecting to database:%s\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Connected...\n");
		return true;
	}
}
//释放资源  
void FreeConnect()
{
	mysql_free_result(res);  //释放一个结果集合使用的内存。
	mysql_close(&mysql);	 //关闭一个服务器连接。
}
 
/***************************数据库操作***********************************/
//其实所有的数据库操作都是先写个sql语句，然后用mysql_query(&mysql,query)来完成，包括创建数据库或表，增删改查  
//查询数据  
bool QueryDatabase1()
{
	strcpy(query, "select * from user"); //执行查询语句，这里是查询所有，user是表名，不用加引号，用strcpy也可以  
	mysql_query(&mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
	//返回0 查询成功，返回1查询失败  
	if (mysql_query(&mysql, query)){        // 执行指定为一个空结尾的字符串的SQL查询。	
		printf("Query failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else{
		printf("query success\n");
	}
	//获取结果集  
	if (!(res = mysql_store_result(&mysql)))    //获得sql语句结束后返回的结果集  
	{
		printf("Couldn't get result from %s\n", mysql_error(&mysql));
		return false;
	}
 
	//打印数据行数  
	printf("number of dataline returned: %d\n", mysql_affected_rows(&mysql));
 
	//获取字段的信息  
	char *str_field[32];  //定义一个字符串数组存储字段信息
	for (int i = 0; i < 4; i++)   //在已知字段数量的情况下获取字段名 
	{
		str_field[i] = mysql_fetch_field(res)->name;	//返回一个所有字段结构的数组。
	}
	for (int i = 0; i < 4; i++)   //打印字段  
		printf("%10s\t", str_field[i]);
	printf("\n");
	//打印获取的数据  
	while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行  
	{
		printf("%10s\t%10s\t%10s\t%10s\n", column[0], column[1], column[2], column[3]);  //column是列数组  
	}
	return true;
}
bool QueryDatabase2()
{
	mysql_query(&mysql, "set names gbk");
	//返回0 查询成功，返回1查询失败  
	if (mysql_query(&mysql, "select * from user"))        //执行SQL语句  
	{
		printf("Query failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("query success\n");
	}
	res = mysql_store_result(&mysql);
	//打印数据行数  
	printf("number of dataline returned: %d\n", mysql_affected_rows(&mysql));
	for (int i = 0; fd = mysql_fetch_field(res); i++)  //获取字段名  
		strcpy(field[i], fd->name);
	int j = mysql_num_fields(res);  // 获取列数  
	for (int i = 0; i<j; i++)  //打印字段  
		printf("%10s\t", field[i]);
	printf("\n");
	while (column = mysql_fetch_row(res))
	{
		for (int i = 0; i<j; i++)
			printf("%10s\t", column[i]);
		printf("\n");
	}
	return true;
}
 
//插入数据  
bool InsertData()
{
	//可以想办法实现手动在控制台手动输入指令  
	strcpy(query, "insert into user values (NULL, 'Lilei', 'wyt2588zs','lilei23@sina.cn');");  
	
	if (mysql_query(&mysql, query))        //执行SQL语句  
	{
		printf("Query failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Insert success\n");
		return true;
	}
}
 
//修改数据  
bool ModifyData()
{
	strcpy(query, "update user set email='lilei325@163.com' where name='Lilei'");
	if (mysql_query(&mysql, query))        //执行SQL语句  
	{
		printf("Query failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Insert success\n");
		return true;
	}
}
//删除数据  
bool DeleteData()
{
	/*sprintf(query, "delete from user where id=6");*/
	char query[100];
	printf("please input the sql:\n");
	//gets(query);  //这里手动输入sql语句  ,这句会在c++编译器报错，因为已经弃用
	if (mysql_query(&mysql, query))        //执行SQL语句  
	{
		printf("Query failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Insert success\n");
		return true;
	}
}

g++ mysql.cpp -o mysqls -I /usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient

```

![](C:\Users\helen\Desktop\DSSE代码实现\17.jpg)



### 4.5 OU不经意更新树

​	本数据结构由李晓聪(2022届毕业)设计并给出实现。下面是他对该结构的描述：

​	基于Path-ORAM的关键词状态更新树文档：

​	首先，这不是一个Path-ORAM实现，如果要去找标准Path-ORAM实现，github有很多。关键词状态更新树（下文称为本结构）与Path-ORAM的区别在于：Path-ORAM可以保护访问模式和更新模式，本结构只能保护访问模式，因为本结构的主要用途在于：不将关键词状态存放在本地（有减少本地客户端存储，拓展到多用户环境的收益），因此，本结构无需Path-ORAM中的shuffle步骤，使得更新和读取时的搜索复杂度为logN级别（Path-ORAM为log2N级别）。

![](C:\Users\helen\Desktop\DSSE代码实现\21.png)



- Block：类似于Path-ORAM中的数据块

- Bucket：类似于Path-ORAM中的桶，一个Bucket中有Bucket_size个Block。

- Job：测试类，直接运行的话，是测试更新10000000次时，本结构的本地Stash空间的平均值和最大值。

- ORAMInterface：接口，定义了Path-ORAM原文中涉及的函数，以及本结构中需要的函数

- ORAMWithReadPathEviction：本结构的具体实现。

- RandomForORAMHW：用于本结构中涉及随机选取路径的一些实现

- UntrustedStorageInterface：定义不受信任服务器的功能，比如读取一个桶（ReadBucket）和更新一个块（WriteBucket）

- ServerStorageForHW：不受信任服务器的具体实现。

  具体代码可以单独发邮件咨询本人(本文档标题处的邮件)，或咨询在校的师兄师姐。



## 5.SSE方案现实应用

### 5.1 Pixek

​	Pixek: An App That Encrypts Your Photos From Camera to Cloud. https://www.wired.com/story/pixek-app-encrypts-photos-from-camera-to-cloud/. （使用了Structured encryption)

​	貌似还在google play testing阶段，没有上google play，是Seny Kamara和产业合作的一个应用。