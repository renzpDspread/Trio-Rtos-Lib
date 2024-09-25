# Output file basename

OUTPUT = output
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
INCLUDE_PATH = -I ../src


#CFLAGS += -DA726_MTK_BOARD

#LIBS = -Wl,--start-group $(LIB)/os/LvosApiLib.a  $(LIB)/os/Litelibc.a  $(LIB)/os/LiteMapiLib.a  $(LIB)/os/libm.a -Wl,--end-group
#LIBS = -Wl,--start-group $(LIB)/os/LvosApiLib.a   $(LIB)/os/LiteMapiLib.a  -lm -Wl,--end-group
#LIBS += $(LIB)/farsi_font/FarsiFontlib_1.01.a
LDFLAGS = -L../lib/
LIBS = $(LIB)/os/LvosApiLib.a $(LIB)/os/LiteMapiLib.a 

#LFLAGS  = -Tlink_ram.lds -nostartfiles -march=rv32imc -mabi=ilp32 
#LFLAGS  +=  -fsigned-char -ffunction-sections -fdata-sections  -g3 -Xlinker --gc-sections -Wl,-Map,"risc-v.map"


#project file directory
VPATH += ../src


ASM_OBJS = 

C_OBJS += dsp_tms_process.o md5.o 


# Append OBJ and BIN directories to output filename
OUTPUT := $(OUTPUT)/lib$(LIB_NAME)

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

 

 
 

