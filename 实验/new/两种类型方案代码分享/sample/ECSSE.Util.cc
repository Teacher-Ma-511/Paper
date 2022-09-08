/*
 * Created by helen chan  on 06/11/2021
 * Email: kaibaking@gmail.com
 * 
 */

#include "ECSSE.Util.h"
using namespace CryptoPP;

// 系统参数
int max_keyword_length = 20;
int max_nodes_number = 10;

namespace ECSSE
{
    std::string Util::H1(const std::string message)
    {
        byte buf[SHA256::DIGESTSIZE];
        std::string salt = "01";
        SHA256().CalculateDigest(buf, (byte *)((message + salt).c_str()), message.length() + salt.length());
        return std::string((const char *)buf, (size_t)SHA256::DIGESTSIZE);
    }

    std::string Util::H2(const std::string message)
    {
        byte buf[SHA256::DIGESTSIZE];
        std::string salt = "02";
        SHA256().CalculateDigest(buf, (byte *)((message + salt).c_str()), message.length() + salt.length());
        return std::string((const char *)buf, (size_t)SHA256::DIGESTSIZE);
    }
    std::string Util::padding(const std::string str)
    {
        size_t BS = (size_t)AES::BLOCKSIZE;
        size_t pad_len = BS - str.length() % BS;
        return str + std::string(char(pad_len), pad_len);
    }

    std::string Util::remove_padding(const std::string str)
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

    std::string Util::Id_padding(const std::string str){
        //填充id到8byte 
        size_t BS = (size_t)AES::BLOCKSIZE/2;
        size_t pad_len = BS - str.length()%BS;
        return str + std::string(char(pad_len), pad_len);
    }

    std::string Util::remove_Id_padding(const std::string str){
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
    
    std::string Util::Xor(const std::string s1, const std::string s2)
    {
        // std::cout<< "in len = "<< s1.length()<<", s1 = "<<s1<<std::endl;
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
    //AES解密函数
    void Util::descrypt(std::string key, std::string ciphertext,std::string &plaintext){
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

    //AES加密函数
    void Util::encrypt(std::string key, std::string plaintext,std::string &ciphertext){
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

    void Util::print(std::string str){
        for(int i =0;i<str.size();i++){
            int c = (unsigned char)str[i];
            std::cout << c << " ";
        }
        std::cout<<std::endl;
    }


    std::string Util::str2hex(const std::string &input)
    {
        static const char *const lut = "0123456789ABCDEF";
        size_t len = input.length();

        std::string output;
        output.reserve(2 * len);
        for (size_t i = 0; i < len; ++i)
        {
            const unsigned char c = input[i];
            output.push_back(lut[c >> 4]);
            output.push_back(lut[c & 15]);
        }
        return output;
    }

    std::string Util::hex2str(const std::string &input)
    {
        static const char *const lut = "0123456789ABCDEF";
        size_t len = input.length();
        if (len & 1)
            throw std::invalid_argument("odd length");

        std::string output;
        output.reserve(len / 2);
        for (size_t i = 0; i < len; i += 2)
        {
            char a = input[i];
            const char *p = std::lower_bound(lut, lut + 16, a);
            if (*p != a)
                throw std::invalid_argument("not a hex digit");

            char b = input[i + 1];
            const char *q = std::lower_bound(lut, lut + 16, b);
            if (*q != b)
                throw std::invalid_argument("not a hex digit");

            output.push_back(((p - lut) << 4) | (q - lut));
        }
        return output;
    }

    void Util::set_db_common_options(rocksdb::Options &options)
    {
        //options.create_if_missing = true;
        options.create_if_missing = true;
        
        
        rocksdb::CuckooTableOptions cuckoo_options;
        cuckoo_options.identity_as_first_hash = false;
        cuckoo_options.hash_table_ratio = 0.9;  
//        cuckoo_options.use_module_hash = false;
//        cuckoo_options.identity_as_first_hash = true;

        options.table_cache_numshardbits = 4;
        options.max_open_files = -1;
        
        options.table_factory.reset(rocksdb::NewCuckooTableFactory(cuckoo_options));
        
        options.memtable_factory.reset(new rocksdb::VectorRepFactory());
        
        options.compression = rocksdb::kNoCompression;
        options.bottommost_compression = rocksdb::kDisableCompressionOption;

        options.compaction_style = rocksdb::kCompactionStyleLevel;
        options.info_log_level = rocksdb::InfoLogLevel::INFO_LEVEL;


//        options.max_grandparent_overlap_factor = 10;
        
        options.delayed_write_rate = 8388608;
        options.max_background_compactions = 20;

//        options.disableDataSync = true;
        options.allow_mmap_reads = true;
        options.new_table_reader_for_compaction_inputs = true;
        
        options.allow_concurrent_memtable_write = options.memtable_factory->IsInsertConcurrentlySupported();
        
        options.max_bytes_for_level_base = 4294967296; // 4 GB
        options.arena_block_size = 134217728; // 128 MB
        options.level0_file_num_compaction_trigger = 10;
        options.level0_slowdown_writes_trigger = 16;
        options.hard_pending_compaction_bytes_limit = 137438953472; // 128 GB
        options.target_file_size_base=201327616;
        options.write_buffer_size=1073741824; // 1GB
    }

} // namespace ECSSE
