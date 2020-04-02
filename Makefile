
CFLAGS = -c -O2 -fpermissive -Dstrlcpy=g_strlcpy -Dstrlcat=g_strlcat -DATCA_HAL_I2C -DATCAPRINTF -DUSE_GLIB -DBUILD_VERSION_WIFI_RTL -fdiagnostics-color=auto
	 
MAIN_DIR = .

TARGET_BIN_DIR = $(MAIN_DIR)/bin
TARGET_OBJ_DIR = $(MAIN_DIR)/obj

BIN_NAME  = tbox

SRC_DIR = $(MAIN_DIR)/src
SDK_DIR = $(MAIN_DIR)/sdk
INCLUDE_PREFIX = -I

LINK_PREFIX = -L

ALL_PATHS = $(MAIN_DIR)
ALL_PATHS += ./inc
ALL_PATHS += ./sdk_inc
ALL_PATHS += ./sdk-includes/common
ALL_PATHS += ./sdk-includes/curl
ALL_PATHS += ./sdk-includes/data
ALL_PATHS += ./sdk-includes/dsi
ALL_PATHS += ./sdk-includes/dsutils
ALL_PATHS += ./sdk-includes/gps
ALL_PATHS += ./sdk-includes/linux/spi
ALL_PATHS += ./sdk-includes/mcm
ALL_PATHS += ./sdk-includes/mqtt
ALL_PATHS += ./sdk-includes/openssl
ALL_PATHS += ./sdk-includes/qmi
ALL_PATHS += ./sdk-includes/qmi-framework
ALL_PATHS += ./sdk-includes/json-c


ALL_PATHS += $(LIB_PATH)/usr/include
ALL_PATHS += $(LIB_PATH)/usr/include/glib-2.0
ALL_PATHS += $(LIB_PATH)/usr/lib/glib-2.0/include
ALL_PATHS += $(LIB_PATH)/usr/include/c++/4.9.2
ALL_PATHS += $(LIB_PATH)/usr/include/c++/4.9.2/arm-oe-linux-gnueabi

ALL_INCLUDES = $(addprefix $(INCLUDE_PREFIX), $(ALL_PATHS))


ALL_LINKS += $(addprefix $(LINK_PREFIX), ./libs)

OBJ_CMD = -o 

LD_CMD = -o

SRC_CPP = $(wildcard ${SRC_DIR}/*.cpp )
SRC_C = $(wildcard ${SDK_DIR}/*.c )


TEST_OBJS = $(patsubst %.cpp, ${TARGET_OBJ_DIR}/%.o, $(notdir $(SRC_CPP))) 
TEST_OBJS += $(patsubst %.c, ${TARGET_OBJ_DIR}/%.o, $(notdir $(SRC_C))) 

STD_LIB= $(LIB_PATH)/usr/lib/libdsi_netctrl.so     \
         $(LIB_PATH)/usr/lib/libdsutils.so         \
         $(LIB_PATH)/usr/lib/libqmiservices.so     \
         $(LIB_PATH)/usr/lib/libqmi_cci.so         \
         $(LIB_PATH)/usr/lib/libqmi_common_so.so   \
         $(LIB_PATH)/usr/lib/libqmi.so             \
		 $(LIB_PATH)/lib/libpthread.so.0	 \
		 $(LIB_PATH)/usr/lib/libqdi.so		\
		 $(LIB_PATH)/usr/lib/librt.so		\
		 $(LIB_PATH)/usr/lib/libssl.so		\
		 $(LIB_PATH)/lib/libcrypto.so		\
		 $(LIB_PATH)/usr/lib/libdl.so		\
         $(LIB_PATH)/usr/lib/libmcm.so          \
         $(LIB_PATH)/usr/lib/libloc_stub.so		\
	   	 $(LIB_PATH)/usr/lib/libsqlite3.so \
	     $(LIB_PATH)/usr/lib/libstdc++.so	\
		 $(LIB_PATH)/usr/lib/libmosquitto.so    \
		 $(LIB_PATH)/usr/lib/libssl.so		\
		 $(LIB_PATH)/usr/lib/libjson-c.so 		\
		 $(LIB_PATH)/usr/lib/libdsi_netctrl.so

BIN_OBJS = $(TEST_OBJS) 

$(TARGET_OBJ_DIR)/%.o:$(SRC_DIR)/%.c
	echo ---------------------------------------------------------
	#echo $(ALL_PATHS)
	#echo $(ALL_INCLUDES)
	echo Build OBJECT $(@) from SOURCE $<
	$(CC) $(CFLAGS) $(ALL_INCLUDES) $(OBJ_CMD) $@ $<
	echo ---------------------------------------------------------

$(TARGET_OBJ_DIR)/%.o:$(SRC_DIR)/%.cpp
	#echo $(ALL_PATHS)
	#echo $(ALL_INCLUDES)
	echo Build OBJECT $(@) from SOURCE $<
	$(CC) $(CFLAGS) $(ALL_INCLUDES) $(OBJ_CMD) $@ $<
	echo ---------------------------------------------------------

$(TARGET_OBJ_DIR)/%.o:$(SDK_DIR)/%.c
	echo ---------------------------------------------------------
	#echo $(ALL_PATHS)
	#echo $(ALL_INCLUDES)
	echo Build OBJECT $(@) from SOURCE $<
	$(CC) $(CFLAGS) $(ALL_INCLUDES) $(OBJ_CMD) $@ $<
	echo ---------------------------------------------------------

$(TARGET_OBJ_DIR)/%.o:$(SDK_DIR)/%.cpp
	#echo $(ALL_PATHS)
	#echo $(ALL_INCLUDES)
	echo Build OBJECT $(@) from SOURCE $<
	$(CC) $(CFLAGS) $(ALL_INCLUDES) $(OBJ_CMD) $@ $<
	echo ---------------------------------------------------------
	

.PHONY: all clean

all:prep bin 

prep:
	@if test ! -d $(TARGET_BIN_DIR); then mkdir $(TARGET_BIN_DIR); fi
	@if test ! -d $(TARGET_OBJ_DIR); then mkdir $(TARGET_OBJ_DIR); fi

bin:$(BIN_OBJS)
	@echo Create bin file $(BIN_NAME)
	@$(CC) -lm -L./libs $(ALL_LINKS) $(BUILD_LDFLAGS) $(STD_LIB) -o $(TARGET_BIN_DIR)/$(BIN_NAME) $^ 
	##-lsdk
	@echo ---------------------------------------------------------
	
clean:
	@rm -fr $(TARGET_OBJ_DIR) $(TARGET_BIN_DIR) 
	
