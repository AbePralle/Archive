ZLIB_SRC = ../../../libraries/native/zlib-1.2.3
ZLIB_FLAGS = -O3 -c -w -I $(ZLIB_SRC) -I $(ZLIB_SRC)/contrib/minizip
BUILD_DIR = ../build/zlib

ZLIB_OFILES = $(BUILD_DIR)/adler32.o \
							$(BUILD_DIR)/crc32.o \
							$(BUILD_DIR)/inffast.o \
							$(BUILD_DIR)/inflate.o \
							$(BUILD_DIR)/inftrees.o \
							$(BUILD_DIR)/ioapi.o \
							$(BUILD_DIR)/uncompr.o \
							$(BUILD_DIR)/unzip.o \
							$(BUILD_DIR)/zutil.o \
							$(BUILD_DIR)/deflate.o \
							$(BUILD_DIR)/trees.o \
							$(BUILD_DIR)/compress.o \
							$(BUILD_DIR)/zip.o

zlib: $(BUILD_DIR) $(BUILD_DIR)/zlib.a
	
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/zlib.a: $(ZLIB_OFILES)
	ar rcs $(BUILD_DIR)/zlib.a $(ZLIB_OFILES)

$(BUILD_DIR)/compress.o: $(ZLIB_SRC)/compress.c
	gcc $(ZLIB_FLAGS) $(ZLIB_SRC)/compress.c -o $(BUILD_DIR)/compress.o

$(BUILD_DIR)/trees.o: $(ZLIB_SRC)/trees.c
	gcc $(ZLIB_FLAGS) $(ZLIB_SRC)/trees.c -o $(BUILD_DIR)/trees.o

$(BUILD_DIR)/adler32.o: $(ZLIB_SRC)/adler32.c
	gcc $(ZLIB_FLAGS) $(ZLIB_SRC)/adler32.c -o $(BUILD_DIR)/adler32.o

$(BUILD_DIR)/crc32.o: $(ZLIB_SRC)/crc32.c
	gcc $(ZLIB_FLAGS) $(ZLIB_SRC)/crc32.c -o $(BUILD_DIR)/crc32.o

$(BUILD_DIR)/inffast.o: $(ZLIB_SRC)/inffast.c
	gcc $(ZLIB_FLAGS) $(ZLIB_SRC)/inffast.c -o $(BUILD_DIR)/inffast.o

$(BUILD_DIR)/inflate.o: $(ZLIB_SRC)/inflate.c
	gcc $(ZLIB_FLAGS) $(ZLIB_SRC)/inflate.c -o $(BUILD_DIR)/inflate.o

$(BUILD_DIR)/inftrees.o: $(ZLIB_SRC)/inftrees.c
	gcc $(ZLIB_FLAGS) $(ZLIB_SRC)/inftrees.c -o $(BUILD_DIR)/inftrees.o

$(BUILD_DIR)/ioapi.o: $(ZLIB_SRC)/contrib/minizip/ioapi.c
	gcc $(ZLIB_FLAGS) $(ZLIB_SRC)/contrib/minizip/ioapi.c -o $(BUILD_DIR)/ioapi.o

$(BUILD_DIR)/uncompr.o: $(ZLIB_SRC)/uncompr.c
	gcc $(ZLIB_FLAGS) $(ZLIB_SRC)/uncompr.c -o $(BUILD_DIR)/uncompr.o

$(BUILD_DIR)/unzip.o: $(ZLIB_SRC)/contrib/minizip/unzip.c
	gcc $(ZLIB_FLAGS) $(ZLIB_SRC)/contrib/minizip/unzip.c -o $(BUILD_DIR)/unzip.o

$(BUILD_DIR)/zutil.o: $(ZLIB_SRC)/zutil.c
	gcc $(ZLIB_FLAGS) $(ZLIB_SRC)/zutil.c -o $(BUILD_DIR)/zutil.o

$(BUILD_DIR)/deflate.o: $(ZLIB_SRC)/deflate.c
	gcc $(ZLIB_FLAGS) $(ZLIB_SRC)/deflate.c -o $(BUILD_DIR)/deflate.o

$(BUILD_DIR)/zip.o: $(ZLIB_SRC)/contrib/minizip/zip.c
	gcc $(ZLIB_FLAGS) $(ZLIB_SRC)/contrib/minizip/zip.c -o $(BUILD_DIR)/zip.o

