# Download ImageIO from https://github.com/AbePralle/ImageIO
IMAGE_IO_FOLDER = ../../../ImageIO

CC_FLAGS = -Wall
CC_FLAGS += -I ../GGBitmap
CC_FLAGS += -I $(IMAGE_IO_FOLDER)/Source/ImageIO
CC_FLAGS += -I $(IMAGE_IO_FOLDER)/Source/jpeg-9a
CC_FLAGS += -I $(IMAGE_IO_FOLDER)/Source/lpng1619

# Exposes GG_allocated_byte_count - not necessary in production
CC_FLAGS += -D GG_TRACK_ALLOCATIONS

all: $(IMAGE_IO_FOLDER) $(IMAGE_IO_FOLDER)/Build/libimageio.a GGDemo
	./GGDemo

clean:
	rm -f GGDemo
	rm -rf Output

$(IMAGE_IO_FOLDER):
	@echo "======================================================================"
	@echo "The ImageIO library is not available - download it from"
	@echo "https://github.com/AbePralle/ImageIO and adjust the IMAGE_IO_FOLDER"
	@echo "variable in the Makefile (note: only required for this demo)."
	@echo "======================================================================"

$(IMAGE_IO_FOLDER)/Build/libimageio.a:
	make -C $(IMAGE_IO_FOLDER)

GGDemo: Output GGDemo.cpp ../GGBitmap/GGBitmap.cpp ../GGBitmap/GGBitmap.h
	g++ $(CC_FLAGS) -L $(IMAGE_IO_FOLDER)/Build GGDemo.cpp ../GGBitmap/GGBitmap.cpp -limageio -lz -o GGDemo

Output:
	mkdir -p Output
