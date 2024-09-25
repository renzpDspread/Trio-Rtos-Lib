#include "base.h"
#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QCryptographicHash>

void* std_malloc(int len)
{
    void* new_malloc = malloc(len);
    if(new_malloc){
        memset(new_malloc, 0, len);
        return new_malloc;
    }

    return NULL;

}

void std_free(void* ptr)
{
    free(ptr);
}



char std_s_asc_bcd(char c, int t){
    if(t==0){
        if(c>='0'&&c<='9'){
            c = c-'0';
        }
        else if(c>='a'&&c<='f'){
            c = c-'a'+10;
        }
        else if(c>='A'&&c<='F'){
            c = c-'A'+10;
        }
    }
    else {
        if(c>=0 && c<=9){
            c = c+'0';
        }
        else if(c>=0x0A &&c<=0x0F){
            c = c-0x0A+'A';
        }
    }

    return c;
}

int std_bcd_2_asc(char* bcd, int bcd_len, char* asc){

    for(int i = 0; i<bcd_len; i++){
        *(asc+i*2) = std_s_asc_bcd((*(bcd+i)&0xF0)>>4, 1);
        *(asc+i*2+1) = std_s_asc_bcd((*(bcd+i)&0x0F), 1);
    }

    return bcd_len*2;
}

int std_asc_2_bcd(char* asc, int asc_len, char* bcd){

    //qDebug()<<"asc data is" << QString(QLatin1String(asc));

    if(asc_len%2)
        return 0;

    for(int i=0;i<asc_len;){
        char asc1 = *(asc+i);
        char asc2 = *(asc+i+1);
        *(bcd+i/2)=(std_s_asc_bcd(asc1,0)<<4)|std_s_asc_bcd(asc2,0);
        i += 2;

    }

    //char asc_buf[20] = {0};
    //std_bcd_2_asc(bcd, asc_len/2, asc_buf);
    //qDebug()<<"bcd data is" << QString(QLatin1String(asc_buf));

    return asc_len/2;
}

void std_char_opposite_endian(char* data, int len){

    char* rev_char = (char*)std_malloc(len+1);
    if(!rev_char){
        return;
    }

    for(int i =0;i<len;i++){
        *(rev_char+i) = *(data+len-i-1);
    }
    memcpy(data, rev_char, len);

}

void trace_value(char* title, char* hex, int len)
{

 char* pszAscShaResult = (char*)std_malloc(len*2+1);
 if(pszAscShaResult == NULL){
     return;
 }

 std_bcd_2_asc((char*)hex,len, pszAscShaResult);
 qDebug()<< title << len << QString(QLatin1String(pszAscShaResult));
 std_free(pszAscShaResult);

}


QString fileMd5(const QString &path, qint64 offset)
{
    QFile sourceFile(path);
    qint64 fileSize = sourceFile.size();
    const qint64 bufferSize = 10240;

    if (sourceFile.open(QIODevice::ReadOnly)) {
        char buffer[bufferSize];
        int bytesRead;
        int readSize = qMin(fileSize, bufferSize);

        QCryptographicHash hash(QCryptographicHash::Md5);
        sourceFile.seek(offset);

        while (readSize > 0 && (bytesRead = sourceFile.read(buffer, readSize)) > 0) {
            fileSize -= bytesRead;
            hash.addData(buffer, bytesRead);
            readSize = qMin(fileSize, bufferSize);
        }

        sourceFile.close();
        return QString(hash.result().toHex());
    }

    return QString();
}

