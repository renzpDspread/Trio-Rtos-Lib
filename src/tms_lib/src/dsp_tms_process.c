
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <liteos_api.h>
#include "liteMapi.h"
#include <stdbool.h>
#include "dsp_tms.h"

#define APP_UPDATE_ADDR 0x2000000
#define APP_MAX_SIZE (1120 * 1024)

#define OS_UPDATE_ADDR 0x1000000
#define OS_MAX_SIZE (170 * 1024)

#define I_APP_MAX_SIZE (800*1024L)
#define I_EMV_MAX_SIZE (320*1024L)
#define I_APP_FILE ("i_dsp.a")
#define I_EMVADDR_FILE ("i_emv.a")

//#define I_OTA_DEBUG

#ifdef I_OTA_DEBUG
#define OTA_STRLOG DBG_STR
#define OTA_HEXLOG DBG_HEX
#else
#define OTA_STRLOG  
#define OTA_HEXLOG 
#endif

#define BE_PtrToLong(ptr)           \
	( \
		(((u32)(*((u8 *)(ptr) + 0))) << 24) | \
		(((u32)(*((u8 *)(ptr) + 1))) << 16) | \
		(((u32)(*((u8 *)(ptr) + 2))) << 8) | \
		(((u32)(*((u8 *)(ptr) + 3))) << 0) \
	)

#define LONG_HH(val)        ((((u32)(val)) >> 24) & 0xff)
#define LONG_HL(val)        ((((u32)(val)) >> 16) & 0xff)
#define LONG_LH(val)        ((((u32)(val)) >> 8) & 0xff)
#define LONG_LL(val)        (((u32)(val)) & 0xff)

#define DEV_RSA_PUBKEY  ("\x41\x74\xc4\xce\x72\x3f\x9c\x84\xaf\xf0\x56\x34\xf8\x23\x60\xe0\xdc\x19\xe9\x18\x2f\xef\x10\xd6\x0e\x49\x5f\x38\x1b\xea\x0e\x86\x8b\xc9\xde\x9c\xe0\x30\x80\xea\x88\x16\x32\xa2\x8c\x2c\x5e\xec\xcd\x31\xd3\x37\xd1\x47\xb7\x79\xe3\x2c\x43\x61\xc3\x1a\xc9\xf1\xae\x4d\x6e\xf3\xab\x91\x75\xab\x79\x2f\x18\x1f\x62\xac\x32\xfc\x50\x46\xe8\xe3\x40\x44\xd8\xfc\x3a\x22\x4f\xab\x97\x57\x80\xc1\x61\x96\x55\xdf\x71\x2f\x5c\x89\x4e\x16\xbd\x24\x81\x5b\x75\x32\x66\xab\x98\x83\xa8\x3b\x1b\x2a\xf6\x12\x27\x79\xf1\x6b\x82\x12\x86\x06\x8a\xb4\xe4\xb2\xb6\xf2\x07\x61\xfb\x02\xb1\xe7\x6f\x5c\xdd\xd1\x55\xf7\x56\x76\x20\xcc\x33\xfb\x44\x27\x99\x18\x98\x79\xb4\x92\x53\xcd\x57\x44\x02\xdc\x96\x1b\x8e\x4f\x12\xbe\x17\xaa\x5b\x41\xc7\x40\xc3\x6d\x32\x40\x69\x6b\x84\x41\xcf\xbe\x4b\x41\x95\xdb\xb5\x9c\xa2\xa4\xc0\x54\xdb\xd2\x85\xd1\xa1\xd4\x60\xa7\xc8\xc6\xa4\xaa\x46\x8c\xfe\x1e\x07\x7b\x48\x83\x28\x8c\xa3\xe9\x1a\xc1\x89\xeb\x29\xa6\x83\x34\x0d\x11\x96\x69\x06\x1c\x5f\x2d\x77\x15\xad\x76\xa4\x90\x30\x69\x63\xb7\x33\x15\xbf\x49\xe7\x5a")
#define DEV_RSA_PUBKEY_LEN (256)
#define DEV_RSA_EXP ("\x01\x00\x01")
#define DEV_RSA_EXP_LEN (3)

typedef struct _dsp_tms_head_stru_
{    
    int tms_emv_addr;
    int tms_emv_len;
    int tms_app_addr;
    int tms_app_len;
    int resv_addr_1;
    int resv_len_1;
    int resv_addr_2;
    int resv_len_2;
    int tms_file_sign_len;
    u8  tms_file_sign_data[512];
}dsp_tms_head_stru;

typedef enum
{
    i_OTA_APP=1,
    i_OTA_EMV,
    i_OTA_OS,
    i_OTA_END,
}i_OTA_TYPE_e;

typedef enum
{
    u_OTA_APP = 1,
    u_OTA_OS,
}u_OTA_TYPE_e;

#define ftms_name ("dsp_tms.b")

int LvoslitEnable(char *key);
int LvoslitWriteFlash( u32, u8 *data, u32 len );

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
    
    return asc_len / 2;
}
u32 dsp_get_emv_addr( void )
{
    u32 addr=0;
    u8 addr_buf[4] = { 0 };
    if (GetEnv( I_EMVADDR_FILE, "e_addr", (u8 *)addr_buf ) == 0)
    {
        addr = BE_PtrToLong(addr_buf);
    }

    if (addr == 0)
    {
        addr = 0x14100000;
    }

    addr += 0x220;

    return addr;
}


bool dsp_set_emv_addr(u32 addr )
{
    u8 addr_buf[4] = { 0 };
    addr_buf[0] = LONG_HH(addr);
    addr_buf[1] = LONG_HL(addr);
    addr_buf[2] = LONG_LH(addr);
    addr_buf[3] = LONG_LL(addr);
    
    if (PutEnv( I_EMVADDR_FILE, "e_addr", (u8 *)addr_buf ) == 0)
    {
        return true;
    }

    return false;
}

int file_md5(char* file_name , u32 offset, char* hash_result)
{
    #include "md5.h"
    int fd, len;
    MD5_CTX md5c;
    unsigned char buffer[8] = {0};

    fd = open( file_name, O_RDWR );
    if(fd < 0){
        OTA_STRLOG("open(%s):%d\r\n", file_name, fd);
        return -1;
    }
    seek( fd, offset, SEEK_SET);

    MD5Init( &md5c ); //³õÊ¼»¯
    while ((len = (u32)read( fd, buffer, 8 )))
    {
        MD5Update( &md5c, buffer, len);
    }
    
    MD5Final( &md5c, hash_result );

    return 0;
}

int rsa_ver_sign_pkcs1( char *rsa_key, int rsa_key_len, char *exp, int exp_len, char *in, char *hash_data, int hash_data_len)
{
    int ret = -1, i =0, retLen = 0;

    u8 *rsa_dec_data_ou = malloc( rsa_key_len );
    ret = RSARecover( rsa_key, rsa_key_len, exp, exp_len, in, rsa_dec_data_ou );
    if (ret != 0)
    {
        free( rsa_dec_data_ou );
        return -1;
    }

    //OTA_HEXLOG( "rsa_dec_data_ou ", rsa_dec_data_ou, rsa_key_len );

    for (i = 2;i < DEV_RSA_PUBKEY_LEN;i++)
    {
        if (*(rsa_dec_data_ou + i) == 0x00)
        {
            i++;
            break;
        }
    }
    u8 *hash_result_data = rsa_dec_data_ou + i;
    u8 *SHA_256_TAG = "\x30\x31\x30\x0D\x06\x09\x60\x86\x48\x01\x65\x03\x04\x02\x01\x05\x00\x04\x20";
    u32 SHA_256_TAG_len = 19;
    
    if (strstr( hash_result_data, SHA_256_TAG ))
    {
        hash_result_data = hash_result_data + SHA_256_TAG_len;
        retLen = 32;
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    if (ret != -1)
    {
        if (memcmp( hash_data, hash_result_data, retLen ) || hash_data_len != retLen)
        {
            return -1;
        }
    }

    free( rsa_dec_data_ou );
    return ret;
}

int i_verify_tms_file(u8 *file_name, u_OTA_TYPE_e ota_type)
{
    int fd, save_size = 0;
    int ret = 0 , i = 0;
	u8 *name = file_name;
    dsp_tms_head_stru file_head;

    OTA_STRLOG( "i_verify_tms_file \r\n" );

    if (name == NULL || filesize( name ) == 0)
    {
        OTA_STRLOG( "ERR\r\n" );
        return -1;
    }
    
    fd = open( name, O_RDWR );
    if(fd < 0){
        OTA_STRLOG("open(%s):%d\r\n", name, fd);
        return -1;
    }
    
    ret = read( fd, (u8*)&file_head, sizeof( file_head ) );
	if(ret != sizeof( file_head ))
	{
		return -1;
    }
    
    if (!file_head.tms_app_len && !file_head.tms_emv_len && ota_type == u_OTA_APP)
    {
        return 0xFF;//no need to update
    }

    if (file_head.tms_file_sign_len != 256)
    {
        OTA_STRLOG("tms_file_sign_len :%d\r\n", file_head.tms_file_sign_len);
        return -1;
    }
    
    u8 hash_result[256] = { 0 };
    memset( hash_result, 0, sizeof( hash_result ) );
    file_md5( file_name, sizeof( file_head ), hash_result );
    std_bcd_2_asc( hash_result, 16, hash_result + 16 );
    strcpy( hash_result + 16 + 32, "tms_ota" );
    SHA( SHA_TYPE_256, hash_result + 16, 39, hash_result + 128 );

    u8 *rsa_clear_key = malloc( DEV_RSA_PUBKEY_LEN );
    
    for (i = 0;i < DEV_RSA_PUBKEY_LEN / 16;i++)
    {
        AES( DEV_RSA_PUBKEY + 16 * i, rsa_clear_key + 16 * i, "\x23\x45\xaf\xcc\x78\x09\x1a\x2b\x3c\x4d\x5e\x6f\x70\x80\x90\xa1\xb2\xc3\xd4\xe5\xf6\x07\x18\x29\x3a\x4b\x5c\x6d\x8e\x9f\xa0\xb1", 32, 1 );        
    }

    u8 *rsa_hash = malloc( 128 );
    ret = rsa_ver_sign_pkcs1(rsa_clear_key, DEV_RSA_PUBKEY_LEN, DEV_RSA_EXP, DEV_RSA_EXP_LEN, file_head.tms_file_sign_data, hash_result + 128, 32 );
    if (ret == -1)
    {
        OTA_STRLOG( "rsa_ver_sign_pkcs1 error");
        free( rsa_clear_key );
        free( rsa_hash );
        return -1;
    }    

    free( rsa_clear_key );
    free( rsa_hash );

    return 0;
}
int i_tms_write_file( u8 *file_name, i_OTA_TYPE_e ota_type )
{
    int fd, fd_2, save_size = 0, remain_size = 0, has_read_len = 0;
    int iRet, offset = 0;
	u8 *name = file_name, *pReadBuf=NULL;
    int readMaxLen = 1024 * 16, readLen = 0;
    dsp_tms_head_stru file_head;
    u32 ota_addr = 0, i = 0;

    OTA_STRLOG( "i_tms_write_file \r\n" );

    if (name == NULL || filesize( name ) == 0)
    {
        OTA_STRLOG( "ERR\r\n" );
        return -1;
    }
    
    fd = open( name, O_RDWR );
    if(fd < 0){
        OTA_STRLOG("open(%s):%d\r\n", name, fd);
        return -1;
    }
    
    iRet = read( fd, (u8*)&file_head, sizeof( file_head ) );
	if(iRet != sizeof( file_head ))
	{
		return -1;
    }

    switch (ota_type)
    {
        case i_OTA_APP:
            file_head.tms_app_len = file_head.tms_app_len > I_APP_MAX_SIZE ? I_APP_MAX_SIZE : file_head.tms_app_len;
            save_size = file_head.tms_app_len;
            offset = file_head.tms_emv_len;
            break;
        case i_OTA_EMV:
            offset = 0;
            save_size = file_head.tms_emv_len;
            ota_addr =APP_UPDATE_ADDR+file_head.tms_emv_addr;
            //ota_addr = file_head.tms_emv_addr + 0x2000220 + 0x14000220;
            OTA_STRLOG("emv file size :%08x\r\n", save_size);
            break;
        default:
            return -1;
    }
    
    remain_size = save_size;
    if (!remain_size)
    {
        return 0xFF;//no need to update
    }

    if (file_head.tms_file_sign_len != 256)
    {
        OTA_STRLOG("tms_file_sign_len :%d\r\n", file_head.tms_file_sign_len);
        return -1;
    }    
    
    OTA_STRLOG( "LvoslitEnable iRet:%d\r\n", LvoslitEnable( "lmtascrdenbal" ) );

    if (ota_type == i_OTA_APP)
    {
        remove( I_APP_FILE );
        fd_2 = open( I_APP_FILE, O_CREATE );
        if (fd_2 < 0)
        {
            OTA_STRLOG( "open(%s):%d\r\n", I_APP_FILE, fd_2 );
            return -1;
        }
    }

	if((pReadBuf = malloc(readMaxLen)) == NULL)
	{
		 OTA_STRLOG("cannot apply for memory\r\n");
		 return -1;
	}

    while (1)
	{   
		readLen = (remain_size>readMaxLen?readMaxLen:remain_size);

        iRet = seek( fd, sizeof( file_head ) + offset + has_read_len, SEEK_SET );
		if(iRet < 0) {
			iRet = -1;
			break;
		}		

		iRet = read(fd, (u8*)pReadBuf, readLen);
		
		if(iRet != readLen){
			OTA_STRLOG("read file iRet:%d\r\n", iRet);
			iRet = -1;
			break;
		}		

        iRet = -1;
        switch (ota_type)
        {
            case i_OTA_APP:
                seek( fd_2, has_read_len, SEEK_SET );
                iRet = write( fd_2, pReadBuf, readLen );
                if (iRet == readLen)
                {
                    iRet = 0;
                }
                break;
            case i_OTA_EMV:
                OTA_STRLOG("LvoslitWriteFlash ota_addr = %08x\r\n", ota_addr + has_read_len);
                iRet = LvoslitWriteFlash( ota_addr + has_read_len, pReadBuf, readLen );
                break;
            default:
                break;
        }
        OTA_STRLOG("WriteFlash iRet:%d %d\r\n", iRet, readLen);
        if (iRet != 0)
        {			
			iRet = -1;
			break;
		}

        has_read_len += readLen;
        if (has_read_len == save_size)
        {
			iRet = 0;
			break;
		}

		remain_size -= readLen;
	}

    close( fd );
    if (ota_type == i_OTA_APP)
    {
        close( fd_2 );
    }
    else if (ota_type == i_OTA_EMV)
    {
        dsp_set_emv_addr(file_head.tms_emv_addr+0x14038000);
    }

	free(pReadBuf);

    return iRet;
}


int dsp_tms_file_to_app(u8*  file_name, u_OTA_TYPE_e ota_type)
{
    long file_size = filesize( file_name );
    dsp_tms_head_stru file_head;
    int fid;
    int ret = -1;

    OTA_STRLOG( "dsp_tms_file_to_app \r\n" );
    if (u_OTA_APP == ota_type)
    {
        ret = i_verify_tms_file( file_name, u_OTA_APP );
        if (ret != 0 && ret != 0xFF)
        {
            return ret;
        }

        ret = i_tms_write_file( file_name, i_OTA_EMV );
        if (ret == 0xFF || ret == 0)
        {
            i_tms_write_file( file_name, i_OTA_APP );
            FileToApp( I_APP_FILE );
        }
        else {
            Reboot();
        }
        

    }
    
}
