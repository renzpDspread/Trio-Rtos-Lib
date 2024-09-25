#include "algorithm.h"
#include <QFileDialog>
#include "base.h"

// 转换为base64格式输出
int Base64Encode(unsigned char *plaintext, size_t plainlen, char *ciphertext,size_t *cipherlen)
{
    if (!ciphertext)
    {
        *cipherlen = (plainlen + 2) / 3 * 4;
        return 0;
    }
    int nLen = ((unsigned char *)ciphertext, (const unsigned char *)plaintext,
                               (int)plainlen);
    if (nLen < 0)
    {
        return -2;
    }
    *cipherlen = nLen;
    return 0;
}

QString QStrToBase64(QString str)
{
    QByteArray byteA;
    byteA=str.toUtf8();
    byteA=byteA.toBase64();
    char * cbyteA=byteA.data();

    return QString(cbyteA);
}


QString Base64ToQStr(QString base64Str)
{
    QByteArray byteA;
    std::string stdStr = base64Str.toStdString();
    byteA=QByteArray(stdStr.c_str() );
    byteA=byteA.fromBase64(byteA);

    return  QString::fromUtf8(byteA);
}


algorithm::algorithm()
{

}



bool algorithm::MakeUpRsaKey (QString& strPubKey, QString& strPriKey)
{
     RSA *pRsa = RSA_generate_key(KEY_LENGTH, RSA_3, NULL, NULL);
     if ( !pRsa ){
         return false;
     }
     BIO *pPriBio = BIO_new(BIO_s_mem());
     PEM_write_bio_RSAPrivateKey(pPriBio, pRsa, NULL, NULL, 0, NULL, NULL);
     BIO *pPubBio = BIO_new(BIO_s_mem());
     PEM_write_bio_RSAPublicKey(pPubBio, pRsa);
     // 获取长度
     size_t nPriKeyLen = BIO_pending(pPriBio);
     size_t nPubKeyLen = BIO_pending(pPubBio);
     // 密钥对读取到字符串
     char* pPriKey = new char[nPriKeyLen];
     char* pPubKey = new char[nPubKeyLen];
     BIO_read(pPriBio, pPriKey, nPriKeyLen);
     BIO_read(pPubBio, pPubKey, nPubKeyLen);
     // 存储密钥对
     strPubKey = QByteArray(pPubKey, nPubKeyLen);
     strPriKey = QByteArray(pPriKey, nPriKeyLen);
     // 内存释放
     RSA_free(pRsa);
     BIO_free_all(pPriBio);
     BIO_free_all(pPubBio);
     delete pPriKey;
     delete pPubKey;
     return true;
}

/**
 * @brief rsa_pri_encrypt 私钥加密
 * @param strClearData 明文
 * @param strPriKey 私钥
 * @return 加密后数据(base64格式)
 */
QString algorithm::rsa_pri_encrypt_base64 (const QString& strClearData, const QString& strPriKey)
{
    QByteArray priKeyArry = strPriKey.toUtf8();
    uchar* pPriKey = (uchar*)priKeyArry.data();
    BIO* pKeyBio = BIO_new_mem_buf(pPriKey, strPriKey.length());
    if (pKeyBio == NULL){
        return "";
    }
    RSA* pRsa = RSA_new();
    pRsa = PEM_read_bio_RSAPrivateKey(pKeyBio, &pRsa, NULL, NULL);
    if ( pRsa == NULL ){
         BIO_free_all(pKeyBio);
         return "";
    }
    int nLen = RSA_size(pRsa);
    char* pEncryptBuf = new char[nLen];
    memset(pEncryptBuf, 0, nLen);
    QByteArray clearDataArry = strClearData.toUtf8();
    int nClearDataLen = clearDataArry.length();
    uchar* pClearData = (uchar*)clearDataArry.data();
    int nSize = RSA_private_encrypt(nClearDataLen,
                                    pClearData,
                                    (uchar*)pEncryptBuf,
                                    pRsa,
                                    RSA_PKCS1_PADDING);
    QString strEncryptData = "";
    if ( nSize >= 0 ){
         QByteArray arry(pEncryptBuf, nSize);
         strEncryptData = arry.toBase64();
    }
    // 释放内存
    delete pEncryptBuf;
    BIO_free_all(pKeyBio);
    RSA_free(pRsa);
    return strEncryptData;
}

/**
 * @brief rsa_pub_decrypt 公钥解密
 * @param strDecrypt 待解密数据(base64格式)
 * @param strPubKey 公钥
 * @return 明文
 */
QString algorithm::rsa_pub_decrypt_base64(const QString& strDecryptData, const QString& strPubKey)
{
    QByteArray pubKeyArry = strPubKey.toUtf8();
    uchar* pPubKey = (uchar*)pubKeyArry.data();
    BIO* pKeyBio = BIO_new_mem_buf(pPubKey, strPubKey.length());
    if (pKeyBio == NULL){
        qDebug()<<"error 1";
        return "";
    }

    RSA* pRsa = RSA_new();
    if ( strPubKey.contains(BEGIN_RSA_PUBLIC_KEY) ){
        pRsa = PEM_read_bio_RSAPublicKey(pKeyBio, &pRsa, NULL, NULL);
    }else{
        pRsa = PEM_read_bio_RSA_PUBKEY(pKeyBio, &pRsa, NULL, NULL);
    }

    if ( pRsa == NULL ){
        BIO_free_all(pKeyBio);
        qDebug()<<"error 2";
        return "";
    }
    int nLen = RSA_size(pRsa);
    char* pClearBuf = new char[nLen];
    memset(pClearBuf, 0, nLen);
    //解密
    QByteArray decryptDataArry = strDecryptData.toUtf8();
    decryptDataArry = QByteArray::fromBase64(decryptDataArry);
    int nDecryptDataLen = decryptDataArry.length();
    uchar* pDecryptData = (uchar*)decryptDataArry.data();
    int nSize = RSA_public_decrypt(nDecryptDataLen,
                                   pDecryptData,
                                   (uchar*)pClearBuf,
                                   pRsa,
                                   RSA_PKCS1_PADDING);
    QString strClearData = "";
    if ( nSize >= 0 ){
        strClearData = QByteArray(pClearBuf, nSize);
    }

    // 释放内存
    delete pClearBuf;
    BIO_free_all(pKeyBio);
    RSA_free(pRsa);
    return strClearData;
}

/**
 * @brief rsa_pub_encrypt 公钥加密
 * @param strClearData 明文
 * @param strPubKey 私钥
 * @return 加密后数据(base64格式)
 */
QString algorithm::rsa_pub_encrypt_base64 (const QString& strClearData, const QString& strPubKey)
{
    QByteArray pubKeyArry = strPubKey.toUtf8();
    uchar* pPubKey = (uchar*)pubKeyArry.data();
    BIO* pKeyBio = BIO_new_mem_buf(pPubKey, pubKeyArry.length());
    if (pKeyBio == NULL){
        return "";
    }
    RSA* pRsa = RSA_new();
    if ( strPubKey.contains(BEGIN_RSA_PUBLIC_KEY) ){
        pRsa = PEM_read_bio_RSAPublicKey(pKeyBio, &pRsa, NULL, NULL);
    }else{
        pRsa = PEM_read_bio_RSA_PUBKEY(pKeyBio, &pRsa, NULL, NULL);
    }
    if ( pRsa == NULL ){
        BIO_free_all(pKeyBio);
        return "";
    }

    int nLen = RSA_size(pRsa);
    char* pEncryptBuf = new char[nLen];
    memset(pEncryptBuf, 0, nLen);

    QByteArray clearDataArry = strClearData.toUtf8();
    int nClearDataLen = clearDataArry.length();
    uchar* pClearData = (uchar*)clearDataArry.data();
    int nSize = RSA_public_encrypt(nClearDataLen,
                                   pClearData,
                                   (uchar*)pEncryptBuf,
                                   pRsa,
                                   RSA_PKCS1_PADDING);
    QString strEncryptData = "";
    if ( nSize >= 0 ){
        QByteArray arry(pEncryptBuf, nSize);
        strEncryptData = arry.toBase64();
    }
    // 释放内存
    delete pEncryptBuf;
    BIO_free_all(pKeyBio);
    RSA_free(pRsa);
    return strEncryptData;
}

/**
 * @brief rsa_pri_decrypt 私钥解密
 * @param strDecrypt 待解密数据(base64格式)
 * @param strPriKey 私钥
 * @return 明文
 */
QString algorithm::rsa_pri_decrypt_base64(const QString& strDecryptData, const QString& strPriKey)
{
    QByteArray priKeyArry = strPriKey.toUtf8();
    uchar* pPriKey = (uchar*)priKeyArry.data();
    BIO* pKeyBio = BIO_new_mem_buf(pPriKey, priKeyArry.length());
    if (pKeyBio == NULL){
        return "";
    }
    RSA* pRsa = RSA_new();
    pRsa = PEM_read_bio_RSAPrivateKey(pKeyBio, &pRsa, NULL, NULL);
    if ( pRsa == NULL ){
        BIO_free_all(pKeyBio);
        return "";
    }
    int nLen = RSA_size(pRsa);
    char* pClearBuf = new char[nLen];
    memset(pClearBuf, 0, nLen);

    //解密
    QByteArray decryptDataArry = strDecryptData.toUtf8();
    decryptDataArry = QByteArray::fromBase64(decryptDataArry);
    int nDecryptDataLen = decryptDataArry.length();
    uchar* pDecryptData = (uchar*)decryptDataArry.data();
    int nSize = RSA_private_decrypt(nDecryptDataLen,
                                    pDecryptData,
                                    (uchar*)pClearBuf,
                                    pRsa,
                                    RSA_PKCS1_PADDING);
    QString strClearData = "";
    if ( nSize >= 0 ){
        strClearData = QByteArray(pClearBuf, nSize);
    }
    // 释放内存
    delete pClearBuf;
    BIO_free_all(pKeyBio);
    RSA_free(pRsa);
    return strClearData;
}

int tst(const QString &filename);
void algorithm::test(const QString &filename)
{
    //tst(filename);
    //return ;
    // 现在进行签名测试，使用私钥签名，公钥验签
    QString priKey = QString("-----BEGIN PRIVATE KEY-----\n")+
    QString("MIICeAIBADANBgkqhkiG9w0BAQEFAASCAmIwggJeAgEAAoGBAJQOMmftEDgwvnM3\n") +
    QString("PKWbvF+xvSGLVaJYySSQtmKczEE2ufYR2LM2qwsZhBUwzW+ssKGVrCsjmmI5G4oF\n") +
    QString("gcVsa3uA221c6oDaERvQYrFNN9KaLNrIdEA1Nh0MTAg5M6AK2wTHnPH5DGrVXtGs\n") +
    QString("k6ZLTuKUjpR8iWMMc+0C5ApG3ssLAgMBAAECgYAEiLbEdf8UKXH7p0plK/LF33SS\n") +
    QString("TWkO8rceNoxPUsvyh1GElqFTQ65TRbHE0FzKObijCilenNWCgos0W9zQhfR4pLkJ\n") +
    QString("jZyv+W4xJ3qAH7drzcd2MgBP1v91gJK6VPjD+J0pNACF1xGLsFBZmWIbFATDU38z\n") +
    QString("3SbTPVS15oPXgDdEwQJBAOF2v+skafKdtKmLgy3Hna+iA+uNNUZ7q8bQyjY4nviV\n") +
    QString("FRL3MBgugIpUYaiiCzhl8BZftoB6pg0DCBOlQxONgiECQQCoG43bGjM0uh7e8cMT\n") +
    QString("45OJDruii7wQnpJuZZH67qOGtnZA/coYDGArqFwmrQz2nQJGwePvNt4kT3Ibighf\n") +
    QString("ev+rAkEAtuXKEkpb2ACvCcqvA3gnJs7bNz45tY+lbYb6Qrnz29u0WMpFLZirlYuW\n") +
    QString("HWI2j+3BsaS7O8ZC5dNLvgQWQcpNgQJBAJwm3CIFuELmD/7ve1FiN408TjayrcKS\n") +
    QString("SVqapnr8aJGds0Kze6HS/RIQlWinnj8FoTOwrtVplfcMhOXn1dc7HXkCQQCRu9UN\n") +
    QString("4mMrQLLg7S88fbjxsy7+cuELxBNrix3GU8F6EAtWWq2e1EaUu0VVtRq7f/IKPdKD\n") +
    QString("uEBxsPfr/EXy6yDw\n")+
    QString("-----END PRIVATE KEY-----");

    QString pbKey = QString("-----BEGIN PUBLIC KEY-----\n")+
    QString("MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCUDjJn7RA4ML5zNzylm7xfsb0h\n") +
    QString("i1WiWMkkkLZinMxBNrn2EdizNqsLGYQVMM1vrLChlawrI5piORuKBYHFbGt7gNtt\n")+
    QString("XOqA2hEb0GKxTTfSmizayHRANTYdDEwIOTOgCtsEx5zx+Qxq1V7RrJOmS07ilI6U\n") +
    QString("fIljDHPtAuQKRt7LCwIDAQAB\n")+
    QString("-----END PUBLIC KEY-----");

    QString aaa = "SZHAILEI88123456&111122334";
    QString ret="";
    QString ret2="";

    RSAEncryptPkcs1Action(aaa, ret, priKey, NULL);
    qDebug("=================================================================");
    RSADecryptPkcs1Action(ret, ret2, NULL, pbKey);
    qDebug("=================================================================");
    RSASignAction(aaa.toUtf8().data(),ret, priKey);
    qDebug("=================================================================");

   bool flag = RSAVerAction(aaa.toUtf8().data(),ret, pbKey);
   if(flag){
       qDebug()<<"@@@@@@@@@@ success !";
   }else{
       qDebug()<<"@@@@@@@@@ failed !";
   }
}

void printRSAKeyDetails(RSA* rsa) {
    const BIGNUM *n, *e, *d;

    // 获取 RSA 的 n, e, d 成员
    RSA_get0_key(rsa, &n, &e, &d);

    if (n && e) {
        qDebug() << "Modulus (n):" << BN_bn2hex(n);
        qDebug() << "Public Exponent (e):" << BN_bn2hex(e);
        if (d) {
            qDebug() << "Private Exponent (d):" << BN_bn2hex(d);
        }
    } else {
        qCritical() << "Failed to retrieve RSA key components.";
    }
}

bool algorithm::RSAEncryptPkcs1Action(QString strClearData, QString &strEncrytped,QString strPriKey,QString strPubKey)
{
    unsigned char szEncHexData[1024] = {0};
    int retLen = 0;

    qDebug()<<"need encrpyt data is "<<strClearData;
    qDebug()<<"strPriKey data is "<<strPriKey;
    qDebug()<<"strPubKey data is "<<strPubKey;


    // 读取 X.509 密钥
    //-----BEGIN PUBLIC KEY-----
    //MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA...
    //-----END PUBLIC KEY-----

    if(strPriKey != NULL){

        QByteArray priKeyArry = strPriKey.toUtf8();
        uchar* pPriKey = (uchar*)priKeyArry.data();
        BIO* pKeyBio = BIO_new_mem_buf(pPriKey, strPriKey.length());
        if (pKeyBio == NULL){
            return false;
        }

        EVP_PKEY* evpPriKey = PEM_read_bio_PrivateKey(pKeyBio, nullptr, nullptr, nullptr);
        if (!evpPriKey) {
            BIO_free_all(pKeyBio);
            return false;
        }

        // 提取 RSA 私钥
        RSA* rsaPriKey = EVP_PKEY_get1_RSA(evpPriKey);
        if (!rsaPriKey) {
            BIO_free_all(pKeyBio);
            EVP_PKEY_free(evpPriKey);
            return false;
        }
        printRSAKeyDetails(rsaPriKey);

        memset(szEncHexData, 0, 1024);

        trace_value("strClearData", strClearData.toUtf8().data(), strClearData.length());

        retLen = RSA_private_encrypt(strClearData.length(), (const unsigned char *)strClearData.toUtf8().data(), szEncHexData, rsaPriKey, RSA_PKCS1_PADDING);

        qDebug()<<"retlen is " << retLen;
        trace_value("szEncHexData is ",(char*)szEncHexData, retLen);
        //int RSA_private_encrypt(int flen, const unsigned char *from, unsigned char *to,
        //                        RSA *rsa, int padding);
    }
    else if(strPubKey != NULL){


        QByteArray pubKeyArry = strPriKey.toUtf8();
        uchar* pPubKey = (uchar*)pubKeyArry.data();
        BIO* pKeyBio = BIO_new_mem_buf(pPubKey, strPriKey.length());
        if (pKeyBio == NULL){
            return false;
        }

        EVP_PKEY* evpPubKey = PEM_read_bio_PUBKEY(pKeyBio, nullptr, nullptr, nullptr);
        if (!evpPubKey) {
            BIO_free_all(pKeyBio);
            return false;
        }

        // 提取 RSA 公钥
        RSA* rsaPubKey = EVP_PKEY_get1_RSA(evpPubKey);
        if (!rsaPubKey) {
            BIO_free_all(pKeyBio);
            EVP_PKEY_free(evpPubKey);
            return false;
        }
        printRSAKeyDetails(rsaPubKey);


        memset(szEncHexData, 0, 1024);

        trace_value("strClearData", strClearData.toUtf8().data(), strClearData.length());

        retLen = RSA_public_encrypt(strClearData.length(), (const unsigned char *)strClearData.toUtf8().data(), szEncHexData, rsaPubKey, RSA_PKCS1_PADDING);

        qDebug()<<"retlen is " << retLen;
        trace_value("szEncHexData is ",(char*)szEncHexData, retLen);


    }
    else {
        return false;
    }


    char szEncHexDataAsc[1024] = {0};
    memset(szEncHexDataAsc, 0, 1024);
    std_bcd_2_asc((char*)szEncHexData, retLen, szEncHexDataAsc);
    strEncrytped = QString::fromUtf8(szEncHexDataAsc);

    return true;


}



bool algorithm::RSADecryptPkcs1Action(QString strEncrytpedData, QString &strClearData,QString strPriKey,QString strPubKey)
{
    int retLen = 0;
    unsigned char szDecHexData[1024] = {0};
    unsigned char szEncHexData[1024] = {0};

    qDebug()<<"need Decrypt data is "<<strEncrytpedData;
    qDebug()<<"strPriKey data is "<<strPriKey;
    qDebug()<<"strPubKey data is "<<strPubKey;

    memset(szEncHexData, 0, 1024);
    std_asc_2_bcd((char*)strEncrytpedData.toUtf8().data(), strEncrytpedData.length(), (char*)szEncHexData);
    retLen = strEncrytpedData.length()/2;

    // 读取 X.509 密钥
    //-----BEGIN PUBLIC KEY-----
    //MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA...
    //-----END PUBLIC KEY-----
    if(strPriKey != NULL){

        QByteArray priKeyArry = strPriKey.toUtf8();
        uchar* pPriKey = (uchar*)priKeyArry.data();
        BIO* pKeyBio = BIO_new_mem_buf(pPriKey, strPriKey.length());
        if (pKeyBio == NULL){
            return false;
        }

        EVP_PKEY* evpPriKey = PEM_read_bio_PrivateKey(pKeyBio, nullptr, nullptr, nullptr);
        if (!evpPriKey) {
            BIO_free_all(pKeyBio);
            return false;
        }

        // 提取 RSA 公钥
        RSA* rsaPriKey = EVP_PKEY_get1_RSA(evpPriKey);
        if (!rsaPriKey) {
            BIO_free_all(pKeyBio);
            EVP_PKEY_free(evpPriKey);
            return false;
        }
        printRSAKeyDetails(rsaPriKey);

        memset(szDecHexData, 0, 1024);

        trace_value("strEncrytpedData", (char*)szEncHexData, retLen);

        retLen = RSA_private_decrypt(retLen, (const unsigned char *)szEncHexData, szDecHexData, rsaPriKey, RSA_PKCS1_PADDING);

        qDebug()<<"retlen is " << retLen;
        trace_value("szEncHexData is ",(char*)szDecHexData, retLen);
        //int RSA_private_encrypt(int flen, const unsigned char *from, unsigned char *to,
        //                        RSA *rsa, int padding);
    }
    else if(strPubKey != NULL){

        QByteArray pubKeyArry = strPubKey.toUtf8();
        uchar* pPubKey = (uchar*)pubKeyArry.data();
        BIO* pKeyBio = BIO_new_mem_buf(pPubKey, strPubKey.length());
        if (pKeyBio == NULL){
            return false;
        }

        EVP_PKEY* evpPubKey = PEM_read_bio_PUBKEY(pKeyBio, nullptr, nullptr, nullptr);
        if (!evpPubKey) {
            BIO_free_all(pKeyBio);
            return false;
        }

        // 提取 RSA 公钥
        RSA* rsaPubKey = EVP_PKEY_get1_RSA(evpPubKey);
        if (!rsaPubKey) {
            BIO_free_all(pKeyBio);
            EVP_PKEY_free(evpPubKey);
            return false;
        }
        printRSAKeyDetails(rsaPubKey);
        memset(szDecHexData, 0, 1024);

        trace_value("strEncrytpedData", (char*)szEncHexData, retLen);

        retLen = RSA_public_decrypt(retLen, (const unsigned char *)szEncHexData, szDecHexData, rsaPubKey, RSA_PKCS1_PADDING);

        qDebug()<<"retlen is " << retLen;
        trace_value("szEncHexData is ",(char*)szDecHexData, retLen);
        //int RSA_private_encrypt(int flen, const unsigned char *from, unsigned char *to,
        //                        RSA *rsa, int padding);

    }
    else {
        return false;
    }

    char szDecHexDataAsc[1024] = {0};
    memset(szDecHexDataAsc, 0, 1024);
    std_bcd_2_asc((char*)szDecHexData, retLen, szDecHexDataAsc);
    strClearData = QString::fromUtf8(szDecHexDataAsc);

    return true;


}




bool algorithm::RSASignAction(QString strData, QString &strSigned,QString &strPriKey)
{
    qDebug()<<"need signed data is "<<strData;
    QByteArray clearDataArry = strData.toUtf8();
    int len = clearDataArry.length();
    QByteArray priKeyArry = strPriKey.toUtf8();
    uchar* pPriKey = (uchar*)priKeyArry.data();
    BIO* pKeyBio = BIO_new_mem_buf(pPriKey, strPriKey.length());
    if (pKeyBio == NULL){
        return false;
    }

    // 读取 X.509 私钥
    //-----BEGIN PUBLIC KEY-----
    //MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA...
    //-----END PUBLIC KEY-----

    EVP_PKEY* evpPriKey = PEM_read_bio_PrivateKey(pKeyBio, nullptr, nullptr, nullptr);
    if (!evpPriKey) {
        BIO_free_all(pKeyBio);
        return false;
    }

    // 提取 RSA 公钥
    RSA* rsaPriKey = EVP_PKEY_get1_RSA(evpPriKey);
    if (!rsaPriKey) {
        BIO_free_all(pKeyBio);
        EVP_PKEY_free(evpPriKey);
        return false;
    }
    //printRSAKeyDetails(rsaPriKey);
    /*********************PKCS#1 格式的 RSA 公钥**********************/
    //-----BEGIN RSA PUBLIC KEY-----
    //MIIBCgKCAQEA...
    //-----END RSA PUBLIC KEY-----

//    RSA* prsa = NULL;
//    prsa = PEM_read_bio_RSAPrivateKey(pKeyBio, &prsa, NULL, NULL);
//    if ( prsa == NULL ){
//        BIO_free_all(pKeyBio);
//        qDebug()<<"PEM_read_bio_RSAPrivateKey error";
//        return false;
//    }
//    printRSAKeyDetails(prsa);

    unsigned char szSHAResult[1024] = {0};
    SHA256((const unsigned char*)clearDataArry.data(),len,szSHAResult);
    trace_value((char*)"SHA-256 result ", (char*)szSHAResult, SHA256_DIGEST_LENGTH);

    int nlen = 0;
    unsigned int nlenRet = 0;
    char szSignDataHex[1024] = {0};
    memset(szSignDataHex, 0, 1024);

    nlen  = RSA_sign(NID_sha256,szSHAResult,SHA256_DIGEST_LENGTH,(unsigned char*)szSignDataHex,&nlenRet,rsaPriKey);
    if(nlen!=1){
        //RSA_free(prsa);
        BIO_free_all(pKeyBio);
        EVP_PKEY_free(evpPriKey);
        return false;
    }
    char szSignDataAsc[1024] = {0};
    memset(szSignDataAsc, 0, 1024);
    std_bcd_2_asc(szSignDataHex, nlenRet,szSignDataAsc);
    strSigned = QString::fromUtf8(szSignDataAsc);
    qDebug()<<"Sign Data is: " << strSigned;

    BIO_free_all(pKeyBio);
    //RSA_free(prsa);

    return true;
}

bool algorithm::RSAVerAction(QString strData, QString &strSigned, QString &strPubKey)
{
    qDebug()<<"need verify data is "<<strData;
    qDebug()<<"signed data is "<<strSigned;

    char szSignedHexData[1024] = {0};
    memset(szSignedHexData, 0, 1024);

    std_asc_2_bcd(strSigned.toUtf8().data(), strSigned.length(), szSignedHexData);

    QByteArray clearDataArry = strData.toUtf8();
    int len = clearDataArry.length();
    QByteArray pubKeyArry = strPubKey.toUtf8();
    uchar* pPubKey = (uchar*)pubKeyArry.data();


    BIO* pKeyBio = BIO_new_mem_buf(pPubKey, pubKeyArry.length());
    if (pKeyBio == NULL){
        qDebug()<<"112231";
        return false;
    }

    // 读取 X.509 公钥
    EVP_PKEY* evpPublicKey = PEM_read_bio_PUBKEY(pKeyBio, nullptr, nullptr, nullptr);
    if (!evpPublicKey) {
        BIO_free_all(pKeyBio);
        qDebug()<<"112232";
        return false;
    }

    // 提取 RSA 公钥
    RSA* rsaPublicKey = EVP_PKEY_get1_RSA(evpPublicKey);
    if (!rsaPublicKey) {
        BIO_free_all(pKeyBio);
        EVP_PKEY_free(evpPublicKey);
        qDebug()<<"112233";
        return false;
    }

    printRSAKeyDetails(rsaPublicKey);


    unsigned char szTmp[1024] = {0};
    SHA256((const unsigned char*)clearDataArry.data(),len,szTmp);

    //RSA_set0_key
    int nlen = RSA_verify(NID_sha256, szTmp, SHA256_DIGEST_LENGTH, (const unsigned char *)szSignedHexData, RSA_bits(rsaPublicKey)/8, rsaPublicKey);
    if(nlen!=1){
        RSA_free(rsaPublicKey);
        BIO_free_all(pKeyBio);
        EVP_PKEY_free(evpPublicKey);
        qDebug()<<"112234";
        return false;
    }

    RSA_free(rsaPublicKey);
    BIO_free_all(pKeyBio);
    EVP_PKEY_free(evpPublicKey);

    qDebug()<<"verify success ";
    return true;
}

#if 0
/*********************************************/


RSA* loadRSAPublicKey(const QByteArray &pemData) {
    // 创建 BIO 对象
    BIO* bio = BIO_new_mem_buf(pemData.constData(), pemData.size());
    if (!bio) {
        qCritical() << "Failed to create BIO";
        return nullptr;
    }

    // 读取公钥
    RSA* rsaPublicKey = PEM_read_bio_RSAPublicKey(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);  // 释放 BIO 资源

    if (!rsaPublicKey) {
        qCritical() << "Failed to load RSA public key";
        ERR_print_errors_fp(stderr);  // 打印 OpenSSL 错误信息
    }

    return rsaPublicKey;
}


RSA* readRSAPublicKeyFromPEM(const QString& pemKey) {
    // 将 QString 转换为 std::string
    std::string pemKeyStr = pemKey.toStdString();

    // 创建一个 OpenSSL BIO 对象，用于处理内存中的字符串
    BIO* bio = BIO_new_mem_buf(pemKeyStr.data(), -1);  // -1 表示读取到字符串结束
    if (!bio) {
        qDebug() << "Failed to create BIO";
        return nullptr;
    }

    // 读取 X.509 公钥
    EVP_PKEY* evpPublicKey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    if (!evpPublicKey) {
        qDebug() << "Failed to read EVP public key";
        BIO_free(bio);
        return nullptr;
    }

    // 提取 RSA 公钥
    RSA* rsaPublicKey = EVP_PKEY_get1_RSA(evpPublicKey);
    if (!rsaPublicKey) {
        qDebug() << "Failed to extract RSA public key";
    }

    // 释放资源
    EVP_PKEY_free(evpPublicKey);
    BIO_free(bio);

    printRSAKeyDetails(rsaPublicKey);
    return rsaPublicKey;
}

int tst(const QString &filename) {
    // 假设 public_key.pem 文件包含 PEM 格式的公钥
    //QFile file(":/public_key.pem");
    qCritical() << "start read pem";
    QFile file;
    file.setFileName(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Failed to open public key file";
        return -1;
    }

    QByteArray pemData = file.readAll();
    file.close();

    qCritical() << "read pem \r\n" << pemData.data();
    qCritical() << "check pem";

    // 通过 PEM_read_bio_RSAPublicKey 读取公钥
    RSA* rsaPublicKey = readRSAPublicKeyFromPEM(pemData);

//    if (rsaPublicKey) {
//        qDebug() << "RSA public key loaded successfully";
//        // 在这里可以使用 rsaPublicKey 进行加密或验证操作...

//        RSA_free(rsaPublicKey);  // 使用完后释放资源
//    } else {
//        qCritical() << "Failed to load RSA public key";
//    }

    return 0;
}
#endif
