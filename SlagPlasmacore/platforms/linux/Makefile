# Plasmacore Makefile
#
# Copied from platforms/linux/defaults if it does not exist.  Otherwise neither
# this file nor main.cpp are overwritten during a Plasmacore upgrade to allow 
# custom modfications.

LDFLAGS = $(shell sdl-config --libs)

NATIVE_SRC = ../../libraries/native

FLAGS = \
    -O3 \
    -w \
    -D SWAP_RED_AND_BLUE \
    -I $(NATIVE_SRC)/plasmacore/ \
    -I $(NATIVE_SRC)/libpng-1.5.4 \
    -I $(NATIVE_SRC)/zlib-1.2.3 \
    -I $(NATIVE_SRC)/zlib-1.2.3/contrib/minizip/ \
    -I $(NATIVE_SRC)/bard \
    -I $(NATIVE_SRC)/gd-2.0.36RC1 \
    -I .. \
    -laudiere \
    -lSDL \
    -lGL

FILES = \
    main.cpp \
    $(NATIVE_SRC)/plasmacore/plasmacore.cpp \
    $(NATIVE_SRC)/plasmacore/gl_core.cpp \
    $(NATIVE_SRC)/plasmacore/font_system_17.cpp \
    linux_core.cpp \
    linux_audio.cpp \
    build/zlib/*.o \
    build/gd/gd.a

vm: dependencies build/plasmacore-vm

xc: dependencies build/game

dependencies: build/gd/gd.a build/bard/bard_vm.a build/bard/bard_xc.a build/zlib/zlib.a

build/gd/gd.a:
	make -C gd 

build/bard/bard_vm.a:
	make -C bard 

build/bard/bard_xc.a:
	make -C bard 

build/zlib/zlib.a:
	make -C zlib 

build/plasmacore-vm:
	g++ $(FLAGS) \
    -D BARD_VM \
		$(FILES) \
    build/bard/bard_vm.a \
    -o build/plasmacore-vm

build/game: build/game_xc.cpp
	g++ $(FLAGS) \
    -D BARD_XC \
		$(FILES) \
    -I build \
    build/game_xc.cpp \
    build/bard/bard_xc.a \
    -o build/game

build/game_xc.cpp:
	echo "Missing cross-compiled game code.  Type \"./gogo build-xc\" from the root directory of the project."

clean:
	rm -r build

random:
	g++ -DSWAP_RED_AND_BLUE -DLINUX \
    -O3 \
    -I $(NATIVE_SRC)/plasmacore/ \
    -I $(NATIVE_SRC)/libpng-1.5.4 \
    -I $(NATIVE_SRC)/zlib-1.2.3 \
    -I $(NATIVE_SRC)/zlib-1.2.3/contrib/minizip/ \
    -I $(NATIVE_SRC)/bard \
    -I $(NATIVE_SRC)/gd-2.0.36RC1 \
    -I .. \
    -D BARD_VM \
    -w \
    main.cpp \
    $(NATIVE_SRC)/plasmacore/plasmacore.cpp \
    $(NATIVE_SRC)/plasmacore/gl_core.cpp \
    $(NATIVE_SRC)/plasmacore/font_system_17.cpp \
    build/bard/bard_vm.a \
    linux_core.cpp \
    linux_audio.cpp \
    build/zlib/*.o \
    build/gd/gd.a \
    -laudiere \
    -lSDL \
    -lGL \
    -o build/plasmacore-vm

