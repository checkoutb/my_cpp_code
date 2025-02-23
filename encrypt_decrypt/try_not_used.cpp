


/*

baidu AI 的，只给了 加密。。后来问了它 加解密 就都给了，但是我发现了： https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption




#include <iostream>
#include <openssl/evp.h>
#include <openssl/aes.h>

int main() {
    // OpenSSL_add_all_algorithm();

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr<<"failed create evp_cipher_ctx"<<std::endl;
        return -1;
    }

    const EVP_CIPHER* cipher_type = EVP_aes_256_cbc();
    if (!cipher_type) {
        std::cerr<<"failed cipher type\n";
        EVP_CIPHER_CTX_free(ctx);
        return -2;
    }

    // if (EVP_EncryptInit_ex(ctx, cipher_type, nullptr, nullptr, nullptr) != 1) {
    //     std::cerr<<"failed init encrypt operation\n";
    //     EVP_CIPHER_CTX_free(ctx);
    //     return -3;
    // }

    unsigned char key[32] = {0};
    unsigned char iv[16] = {0};

    if (EVP_EncryptInit_ex(ctx, cipher_type, nullptr, key, iv) != 1) {
        std::cerr<<"failed set key, iv\n";
        EVP_CIPHER_CTX_free(ctx);
        return -4;
    }

    std::string text = "hi,asdasd";
    std::string cipher;
    int len;
    unsigned char buffer[256];

    if (EVP_EncryptUpdate(ctx, buffer, &len, (const unsigned char*) text.c_str(), text.size()) != 1) {
        std::cerr<<"failed encrypt text\n";
        EVP_CIPHER_CTX_free(ctx);
        return -5;
    }

    cipher.append((char*) buffer, len);

    //

    if (EVP_EncryptFinal_ex(ctx, buffer, &len) != 1) {
        std::cerr<<"failed final\n";
        EVP_CIPHER_CTX_free(ctx);
        return -6;
    }
    cipher.append((char *) buffer, len);

    EVP_CIPHER_CTX_free(ctx);

    std::cout<<"cipher: "<<cipher<<", "<<cipher.size()<<std::endl;
    
    return 0;
}

*/





/*
// https://zhuanlan.zhihu.com/p/667438739
// 原文有问题， 解密的时候 倒数第三行 length 是 密文的长度，不是 明文的长度。
// 可惜了，要 shi 名 。
// 而且 这个也是 openssl3 deprecated 了。 不知道 openssl 3 的 AES 是怎么样的。。

// 64位
#include<iostream>
#include<openssl/md5.h>
#include<openssl/sha.h>
#include<openssl/rsa.h>
#include<openssl/pem.h>
#include<string.h>
#include<openssl/aes.h>
extern "C" {
    //#include "openssl/applink.c"
};

// static const char* kes = "1234567887654321";

// 对称加密AES测试
std::string aes_encrypt(std::string data_str, const char* key="1234567887654321") {
    // 1、准备数据
    const unsigned char* pt = (const unsigned char*)data_str.data();
    // 2 准备秘钥
    //const  char key[] = "1234567887654321";
    // 初始化秘钥
    AES_KEY encKey;
    // 设置加密钥匙
    AES_set_encrypt_key((const unsigned char*)key, 128, &encKey); // 一块为128位，16字节。
    // 加密
    // 计算长度
    int length = 0; // 传入的参数必须是16的整数倍。因为是16个字节为一个数据块，进行加密，
    // 这里取出的是真实的长度
    int len = strlen((char*)pt) + 1;
    if (len % 16 != 0) {
        length = ((len / 16) + 1) * 16;
    }
    else {
        length = len;
    }

    unsigned char ivec[AES_BLOCK_SIZE];
    memset(ivec, 9, sizeof(ivec));
    char* out = new char[length];// 传出的是16的整数倍的密文
    // 记性cbc加密pt，是需要加密的数据， out是加密后的数据，length是存在多少数据块，ivec是初始化向量。
    AES_cbc_encrypt((const unsigned char*)pt, (unsigned char*)out, length, &encKey, ivec, AES_ENCRYPT);

    std::cout<<" ---!! "<<strlen(out)<<std::endl;
    
    std::string res = std::string(out, length);
    delete[] out;
    return res;

}

std::string aes_decrypt(std::string secert, const char* key="1234567887654321") {
    // 首先拿到密文的长度
    int length = secert.size();
    std::cout<<"origgg  "<<length<<std::endl;
    unsigned char* data = new unsigned char[length];
    // 解密准备代码
    unsigned char ivec[AES_BLOCK_SIZE];
    AES_KEY deckey;//准备钥匙 
    memset(ivec, 9, sizeof(ivec));
    // 设置解密钥匙
    AES_set_decrypt_key((const unsigned char*)key, 128, &deckey); // 还是128位
    // 进行解密。
    AES_cbc_encrypt((const unsigned char*)secert.data(), data, length, &deckey, ivec, AES_DECRYPT);
    // 打印还原数据
    std::cout << "还原数据:" << data << ", " << length << std::endl;
    std::string res = std::string((char*)data, length);
    delete[] data;
    return res;
}

int main2()
{
    std:: string secert = aes_encrypt("真的是要加密123吗");
    std::cout <<"密文：" << secert << std::endl;
    std::string res = aes_decrypt(secert);
    std::cout << "明文：" << res << std::endl;
    return 0;
}




// #include <iostream>
// #include <openssl/md5.h>
// #include <openssl/sha.h>
// #include <openssl/rsa.h>
// #include <openssl/pem.h>
// #include <string.h>
// #include <openssl/aes.h>

// extern "C" {
//     //#include "openssl/applink.c"
// }


static const char* key = "123123123";

std::string aes_encrypt2(std::string data_str, const char* key) {
    const unsigned char* pt = (const unsigned char*) data_str.data();
    
    AES_KEY encKey;
    AES_set_encrypt_key((const unsigned char*) key, 128, &encKey);
    int length = 0;
    int len = strlen((char*) pt) + 1;
    if (len % 16 != 0) {
        length = ((len / 16) + 1) * 16;
    } else {
        length = len;
    }
    unsigned char ivec[AES_BLOCK_SIZE];
    memset(ivec, 9, sizeof(ivec));
    char* out = new char[length];
    AES_cbc_encrypt((const unsigned char*) pt, (unsigned char*) out, length, &encKey, ivec, AES_ENCRYPT);
    std::string res = std::string(out, length);
    delete[] out;
    return res;
}

std::string aes_decrypt2(std::string secret, const char* key) {
    int length = secret.size();
    unsigned char* data = new unsigned char[length];
    // unsigned char data[200] = {0};
    unsigned char ivec[AES_BLOCK_SIZE];
    AES_KEY deckey;
    memset(ivec, 9, sizeof(ivec));
    AES_set_decrypt_key((const unsigned char*) key, 128, &deckey);
    AES_cbc_encrypt((const unsigned char*) secret.data(), data, length, &deckey, ivec, AES_DECRYPT);

    std::cout<<"   "<<length<<"\n";
    length = strlen((const char*)data); // !!!
    
    std::cout<<" "<<data<<", "<<length<<std::endl;

    // std::string res;
    // for (int i = 0; i < 200 && data[i] != '\0'; ++i) {
    //     res.push_back(data[i]);
    // }
    
    std::string res = std::string((char *)data, length);
    delete[] data;
    return res;
}


int main() {
    std::string text = "asdsf1we";
    // std::cout<<text<<"--"<<std::endl;
    std::string encrypt = aes_encrypt(text, key);
    std::string decrypt = aes_decrypt2(encrypt, key);
    // std::cout<<text<<"--\n"<<encrypt<<"--\n"<<decrypt<<std::endl;

    std::cout<<decrypt.size()<<", ";
    std::cout<<decrypt<<std::endl;

    encrypt = aes_encrypt2(text, key);
    decrypt = aes_decrypt(encrypt, key);

    std::cout<<decrypt.size()<<" -- "<<decrypt<<std::endl;
    
    return 0;
}

*/


/*

// 部分函数在 openssl3中已经 deprecated了。

// https://7thzero.com/blog/openssl-c-and-aes-encryption-just-scratching-surface
    
#include <inttypes.h>
#include <string>
#include <algorithm>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <vector>
#include <iostream>

int main() {
    uint8_t Key[32];
    uint8_t IV[AES_BLOCK_SIZE];

    RAND_bytes(Key, sizeof(Key));
    RAND_bytes(IV, sizeof(IV));

    uint8_t IVd[AES_BLOCK_SIZE];
    for (int i = 0; i < AES_BLOCK_SIZE; ++i) {
        IVd[i] = IV[i]; // 必须的
    }

    AES_KEY* AesKey = new AES_KEY();
    AES_set_encrypt_key(Key, 256, AesKey);

    std::string txt("this is a test");
    const int UserDataSize = (const int) txt.length();
    int RequiredPadding = (AES_BLOCK_SIZE - (txt.length() % AES_BLOCK_SIZE));
    std::vector<unsigned char> PaddedTxt(txt.begin(), txt.end());
    for (int i = 0; i < RequiredPadding; ++i) {
        PaddedTxt.push_back(0);
    }

    unsigned char* UserData = &PaddedTxt[0];
    const int UserDataSizePadded = (const int) PaddedTxt.size();

    unsigned char EncryptedData[512] = {0};
    AES_cbc_encrypt(UserData, EncryptedData, UserDataSizePadded, (const AES_KEY*) AesKey, IV, AES_ENCRYPT);

    AES_KEY* AesDecryptKey = new AES_KEY();
    AES_set_decrypt_key(Key, 256, AesDecryptKey);

    unsigned char DecryptedData[512] = {0};
    AES_cbc_encrypt(EncryptedData, DecryptedData, UserDataSizePadded, (const AES_KEY*) AesDecryptKey, IVd, AES_DECRYPT);

    for (int i = 0; i < 10; ++i) {
        std::cout<<PaddedTxt[i]<<", ";
    }
    std::cout<<std::endl;
    for (int i = 0; i < 10; ++i) {
        std::cout<<DecryptedData[i]<<", ";
    }
    std::cout<<std::endl;

    std::string str;
    for (int i = 0; i < 512 && DecryptedData[i] != '\0'; ++i) {
        str.push_back(DecryptedData[i]);
    }
    std::cout<<str<<std::endl;

    return 0;
}
*/


