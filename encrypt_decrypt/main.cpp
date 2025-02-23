
// https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption

// 复制了 官方的 C 代码。

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <map>

// g++ main.cpp -lcrypto   不需要 -lssl


namespace pwd_rsa {

class EnDecrypt {

public:
    EnDecrypt(std::string key2) {
        key = (unsigned char*) key2.data();
        std::cout<<"cont :"<<key<<", "<<&key2<<std::endl;
    }

    std::string encrypt(std::string ptext) { // plain text

        using EVP_CIPHER_CTX_ptr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>;

        EVP_CIPHER_CTX_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);

        int rc = EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL, key, iv);
        if (rc != 1) {
            std::cerr<<"en failed init\n";
            return "";
        }

        std::string ctext; // cyper text
        ctext.resize(ptext.size() << 1);

       int out_len1 = (int)ctext.size();

        rc = EVP_EncryptUpdate(ctx.get(), (unsigned char*) &ctext[0], &out_len1, (const unsigned char*) &ptext[0], (int)ptext.size());
        if (rc != 1) {
            std::cerr<<"en failed encry update";
            return "";
        }

        int out_len2 = (int) ctext.size() - out_len1;
        rc = EVP_EncryptFinal_ex(ctx.get(), (unsigned char*) &ctext[0] + out_len1, &out_len2);
        if (rc != 1) {
            std::cerr<<"en failed encry final";
            return "";
        }

        // Set cipher text size now that we know it
        ctext.resize(out_len1 + out_len2);
        return ctext;
    }

    std::string decrypt(std::string ctext) {

        using EVP_CIPHER_CTX_ptr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>;

        EVP_CIPHER_CTX_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);

        int rc = EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL, key, iv);
        if (rc != 1) {
            std::cerr<<"de failed init\n";
            return "";
        }

        std::string ptext;
        ptext.resize(ctext.size() << 1);
        int out_len1 = (int) ptext.size();

        if(1 != EVP_DecryptUpdate(ctx.get(), (unsigned char*) &ptext[0], &out_len1, (unsigned char*) ctext.data(), (int) ctext.size())) {
            std::cerr<<"de failed decrypt update\n";
            return "";
        }

        int out_len2 = (int) ptext.size() - out_len1;
        if(1 != EVP_DecryptFinal_ex(ctx.get(), (unsigned char*) &ptext[0] + out_len1, &out_len2)) {
            std::cerr<<"de failed decrypt final\n";
            return "";
        }
        ptext.resize(out_len1 + out_len2);
        return ptext;
    }

private:
    const unsigned char* iv = (unsigned char*) "AFl23F56-8E01!F4";

    // std::string key;
    unsigned char* key;
};

}





int test_endecrypt() {
    using namespace pwd_rsa;
    std::string key = "asd123@@3dasd";
    EnDecrypt ed(key);
    std::string ptext = "qwe#asd^123";
    std::string asd = ed.encrypt(ptext);
    std::string qwe = ed.decrypt(asd);
    std::cout<<qwe<<std::endl;
    return 0;
}

void test_file() {
    std::ofstream ofile("zzz.txt"
    //, std::ofstream::app
    // , std::ios::out | std::ios::binary
    ); // 没有文件 则自动新建
    using namespace pwd_rsa;
    std::string key = "asd123@@3dasd";
    EnDecrypt ed(key);

    std::map<std::string, std::string> map2;
    std::string arr[3] = {"qweqwe##123", "AQdwe@%$@!ads", "zxc----012304"};
    std::string asd;
    for (int i = 0; i < 3; ++i) {
        asd = ed.encrypt(arr[i]);
        
        std::cout<<asd<<", "<<asd.size()<<std::endl;
        ofile<<"miaoshu" + std::to_string(i)<<'\n';
        if (i == 1) 
            asd[5] = '\r';
        ofile<<asd<<"\n";
        // for (int j = 0; j < asd.size(); ++j) {
        //     ofile<<(int) asd[j]<<"#";
        // }
        // ofile<<"\n";

        asd = ed.decrypt(asd);
        std::cout<<asd<<std::endl;
    }
    ofile.close();

    std::ifstream ifile("zzz.txt");
    std::cout<<"           "<<ifile.is_open()<<std::endl;
    std::string vvv;
    for (int i = 0; i < 3; ++i) {
        getline(ifile, asd);
        getline(ifile, vvv);
        map2[asd] = vvv;
        //std::cout<<asd<<", "<<asd.size()<<std::endl;

        //asd = ed.decrypt(asd);
        //std::cout<<"   file : "<<asd<<std::endl;
    }

    for (auto& p : map2) {
        std::cout<<p.first<<" - "<<ed.decrypt(p.second)<<'\n';
    }
    
    std::string awe = "asd13ad\b\t...\a";
    std::cout<<(awe.find('\n') == std::string::npos)<<std::endl;

    awe = "asd\n234qasd\b;";
    std::cout<<(awe.find('\n') == std::string::npos)<<std::endl;

    ifile.close();
    // ofile.close();
}

int main() {

    test_file();

    return 0;
}

