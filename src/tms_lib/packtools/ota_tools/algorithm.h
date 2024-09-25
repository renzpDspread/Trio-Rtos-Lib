#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QDebug>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#define BEGIN_RSA_PUBLIC_KEY    "BEGIN RSA PUBLIC KEY"      //
#define BEGIN_PUBLIC_KEY        "BEGIN PUBLIC KEY"          // 公钥头部
#define KEY_LENGTH              1024                        // 密钥长度



class algorithm
{
public:
    algorithm();
    // 生成一对公钥和私钥
    static bool MakeUpRsaKey (QString& publicKey, QString& privateKey);
    /**
    * @brief rsa_pri_encrypt 私钥加密
    * @param strClearData 明文
    * @param strPriKey 私钥
    * @return 加密后数据(base64格式)
    */
    static QString rsa_pri_encrypt_base64 (const QString& strClearData, const QString& strPriKey);
    /**
    * @brief rsa_pub_decrypt 公钥解密
    * @param strDecrypt 待解密数据(base64格式)
    * @param strPubKey 公钥钥
    * @return 明文
    */
    static QString rsa_pub_decrypt_base64 (const QString& strDecryptData, const QString& strPubKey);
    /**
    * @brief rsa_pub_encrypt 公钥加密
    * @param strClearData 明文
    * @param strPubKey 私钥
    * @return 加密后数据(base64格式)
    */
    static QString rsa_pub_encrypt_base64 (const QString& strClearData, const QString& strPubKey);
    /**
    * @brief rsa_pri_decrypt 私钥解密
    * @param strDecrypt 待解密数据(base64格式)
    * @param strPriKey 私钥
    * @return 明文
    */
    static QString rsa_pri_decrypt_base64 (const QString& strDecryptData, const QString& strPriKey);

    // 签名 strData 原始字符串;  strSigned 签名结果 ; strPriKey 私钥
    static bool RSASignAction(QString strData, QString &strSigned,QString &strPriKey);

    // 验签  strData 原始字符串; strSigned 待验签字符串 ;strPubKey 公钥
    static bool RSAVerAction(QString strData, QString &strSigned, QString &strPubKey);

    static bool RSAEncryptPkcs1Action(QString strClearData, QString &strEncrytped,QString strPriKey,QString strPubKey);
    static bool RSADecryptPkcs1Action(QString strEncrytpedData, QString &strClearData,QString strPriKey,QString strPubKey);

    /**< 测试 */
    static void test (const QString &filename);
};

#endif // ALGORITHM_H
