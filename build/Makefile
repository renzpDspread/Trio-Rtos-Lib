# Output file basename

BIN = output
OBJ = obj
SRC =../source
INC =../include
LIB =../libs
CROSS = riscv-none-embed-
LIB_NAME = $(NAME)$(VER).a

# Compilation tools
CC = $(CROSS)gcc 
LD = $(CROSS)gcc
AR = $(CROSS)ar

SIZE = $(CROSS)size
STRIP = $(CROSS)strip
OBJCOPY = $(CROSS)objcopy

CFLAGS = -D__riscv_float_abi_soft -march=rv32imc -mabi=ilp32 -std=gnu99 -fno-strict-aliasing -fsigned-char
CFLAGS += -ffunction-sections -fdata-sections -fstack-usage
#CFLAGS += -fstack-usage
#CFLAGS += -Os -g3 -c -I $(SRC)/system/inc -I $(SRC)  -I $(INC)
CFLAGS += -Os -g3 -c -I $(SRC) -I $(INC)
CFLAGS += -DLIB_VER='"$(VER)"'
INCLUDE_PATH = -I ../src/cJSON-1.7.15
INCLUDE_PATH += -I ../src/mbedtls-3.0.0/include
INCLUDE_PATH += -I ../src/mbedtls-3.0.0/library
INCLUDE_PATH += -I ../src/internal-libarys
INCLUDE_PATH += -I ../src/internal-libarys/chttp 
INCLUDE_PATH += -I ../src/internal-libarys/dymem 
#INCLUDE_PATH += -I ../third-party/internal-libarys/tms-dspread
#INCLUDE_PATH += -I ../third-party/mbedtls-3.0.0/include/mbedtls
INCLUDE_PATH += -I ../src/internal-libarys/mbedtls-socket
#INCLUDE_PATH += -I ../third-party/internal-libarys/net/drv_socket
INCLUDE_PATH += -I ../src/internal-libarys/net/net-container
INCLUDE_PATH += -I ../src/internal-libarys/net/net_socket
INCLUDE_PATH += -I ../src/internal-libarys/net/net_tls
INCLUDE_PATH += -I ../src/internal-libarys/platform
INCLUDE_PATH += -I ../src
INCLUDE_PATH += -I ../src/internal-libarys


#CFLAGS += -DA726_MTK_BOARD

#LIBS = -Wl,--start-group $(LIB)/os/LvosApiLib.a  $(LIB)/os/Litelibc.a  $(LIB)/os/LiteMapiLib.a  $(LIB)/os/libm.a -Wl,--end-group
#LIBS = -Wl,--start-group $(LIB)/os/LvosApiLib.a   $(LIB)/os/LiteMapiLib.a  -lm -Wl,--end-group
#LIBS += $(LIB)/farsi_font/FarsiFontlib_1.01.a
LDFLAGS = -L../lib/
LIBS = $(LIB)/os/LvosApiLib.a $(LIB)/os/LiteMapiLib.a 

#LFLAGS  = -Tlink_ram.lds -nostartfiles -march=rv32imc -mabi=ilp32 
#LFLAGS  +=  -fsigned-char -ffunction-sections -fdata-sections  -g3 -Xlinker --gc-sections -Wl,-Map,"risc-v.map"


#project file directory
#VPATH += $(wildcard $(SRC)) $(wildcard $(SRC)/*/*)
VPATH += ../src/cJSON-1.7.15 ../src/mbedtls-3.0.0/include ../src/mbedtls-3.0.0/library
#VPATH += ../third-party/internal-libarys/chttp  ../third-party/internal-libarys/dymem ../third-party/internal-libarys/tms-dspread
VPATH += ../src/internal-libarys
VPATH += ../src/internal-libarys/chttp  ../src/internal-libarys/dymem
VPATH += ../src/internal-libarys/mbedtls-socket
VPATH += ../src/internal-libarys/net/net-container
VPATH += ../src/internal-libarys/net/net_socket
VPATH += ../src/internal-libarys/net/net_tls
VPATH += ../src/internal-libarys/platform
VPATH += ../src
VPATH += ../src/internal-libarys


ASM_OBJS = 

C_OBJS += dsp_lib_based_api.o dsp_lib_alg_api.o 
C_OBJS += cJSON.o cJSON_Utils.o
#C_OBJS += chttp.o dymem.o tms_dspread.o tms_dspread_platform.o lring.o
C_OBJS += chttp.o dymem.o
C_OBJS += aes.o aesni.o aria.o asn1parse.o asn1write.o base64.o bignum.o camellia.o ccm.o chacha20.o chachapoly.o cipher.o 
C_OBJS += cipher_wrap.o cmac.o ctr_drbg.o debug.o des.o dhm.o ecdh.o ecdsa.o ecjpake.o ecp.o ecp_curves.o entropy.o entropy_poll.o gcm.o hkdf.o hmac_drbg.o md.o md5.o 
C_OBJS += memory_buffer_alloc.o mps_reader.o mps_trace.o net_sockets.o nist_kw.o oid.o padlock.o pem.o pk.o pkcs12.o pkcs5.o pkparse.o pkwrite.o pk_wrap.o platform.o 
C_OBJS += platform_util.o poly1305.o psa_crypto.o psa_crypto_aead.o psa_crypto_cipher.o psa_crypto_client.o psa_crypto_driver_wrappers.o psa_crypto_ecp.o psa_crypto_hash.o 
C_OBJS += psa_crypto_mac.o psa_crypto_rsa.o psa_crypto_se.o psa_crypto_slot_management.o psa_crypto_storage.o psa_its_file.o ripemd160.o rsa.o rsa_alt_helpers.o sha1.o sha256.o 
C_OBJS += sha512.o ssl_cache.o ssl_ciphersuites.o ssl_cli.o ssl_cookie.o ssl_msg.o ssl_srv.o ssl_ticket.o ssl_tls.o ssl_tls13_keys.o threading.o timing.o 
C_OBJS += version.o x509.o x509write_crt.o x509write_csr.o x509_create.o x509_crl.o x509_crt.o x509_csr.o 
C_OBJS += dspapi_tls.o net_container.o dspapi_socket.o net_container_platform.o mbedtls_socket.o mbedtls_socket_platform.o 


# Append OBJ and BIN directories to output filename
OUTPUT := lib$(LIB_NAME)

C_OBJS_OUTPUT = $(addprefix $(OBJ)/, $(C_OBJS))
ASM_OBJS_OUTPUT = $(addprefix $(OBJ)/, $(ASM_OBJS))

all: $(C_OBJS_OUTPUT) $(ASM_OBJS_OUTPUT) 
	echo start AR....
	$(AR) r $(OUTPUT) $^

$(C_OBJS_OUTPUT): $(OBJ)/%.o: %.c  

	echo CC $<
#	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE_PATH)  $(LIBS) -c -o $@ $<
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE_PATH)  -c -o $@ $<
#	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATH) $(LIBS) 

$(ASM_OBJS_OUTPUT): $(OBJ)/%.o: %.S 
	echo CC $<
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -c -o $@ $<

clean:
	-rm -f $(OBJ)/*.o $(OBJ)/*.su *.a

 

 
 

