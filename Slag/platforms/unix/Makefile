LIBDIR    = ../../libraries/native
BINDIR    = ../../bin/unix
BUILDDIR  = ../../build
SLAG_LIBS = ../../libraries/slag
SLAGC_ETC = $(SLAG_LIBS)/slagc/slagc.etc
SRCDIR = ../../src

VM_DEFINES    = -DSLAG_VM
SLAGC_DEFINES = -DSLAG_XC
FLAGS         = -O3 -w -fno-strict-aliasing -I $(LIBDIR)/slag -lnsl -lpthread -D_REENTRANT -D UNIX -fdiagnostics-show-option 

CC       = g++ 

SLAG_H    = $(LIBDIR)/slag/slag.h $(LIBDIR)/slag/slag_runtime.h $(LIBDIR)/slag/slag_mm.h $(LIBDIR)/slag/slag_stdlib.h $(SLAGC_ETC)
SLAG_CPP  = $(LIBDIR)/slag/slag_runtime.cpp $(LIBDIR)/slag/slag_mm.cpp $(LIBDIR)/slag/slag_stdlib.cpp
VM_H      = $(SLAG_H) $(LIBDIR)/slag/slag_vm.h
VM_CPP    = $(SLAG_CPP) $(LIBDIR)/slag/slag_vm.cpp
GOGO_FLAGS = $(FLAGS) \
             -Wno-write-strings \
             -D HAVE_LIBPNG \
             -D HAVE_LIBJPEG \
             -I $(LIBDIR)/lpng141 \
             -I $(LIBDIR)/jpeg-8a \
             -I $(LIBDIR)/gd-2.0.36RC1 \
             -I $(LIBDIR)/slag \
             -I $(LIBDIR)/zlib-1.2.3 \
             -I $(LIBDIR)/zlib-1.2.3/contrib/minizip 

ZLIB_C  = -x c $(LIBDIR)/zlib-1.2.3/adler32.c \
          -x c $(LIBDIR)/zlib-1.2.3/compress.c \
          -x c $(LIBDIR)/zlib-1.2.3/crc32.c \
          -x c $(LIBDIR)/zlib-1.2.3/deflate.c \
          -x c $(LIBDIR)/zlib-1.2.3/inffast.c \
          -x c $(LIBDIR)/zlib-1.2.3/inflate.c \
          -x c $(LIBDIR)/zlib-1.2.3/inftrees.c \
          -x c $(LIBDIR)/zlib-1.2.3/trees.c \
          -x c $(LIBDIR)/zlib-1.2.3/uncompr.c \
          -x c $(LIBDIR)/zlib-1.2.3/zutil.c \
          -x c $(LIBDIR)/zlib-1.2.3/contrib/minizip/ioapi.c \
          -x c $(LIBDIR)/zlib-1.2.3/contrib/minizip/unzip.c \
          -x c $(LIBDIR)/zlib-1.2.3/contrib/minizip/zip.c
SLAGC_H   = $(SLAG_H) $(LIBDIR)/slag/slag_xc.h $(BUILDDIR)/slagc_xc.h
SLAGC_CPP = $(SLAG_CPP) $(LIBDIR)/slag/slag_xc.cpp $(BUILDDIR)/slagc_xc.cpp


GOGO_CPP  = $(VM_CPP) \
            $(SRCDIR)/gogo.cpp

GOGO_C    = $(ZLIB_C) \
            -x c $(LIBDIR)/gd-2.0.36RC1/gd.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gd_gd.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gd_gd2.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gd_gif_in.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gd_gif_out.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gd_io.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gd_io_dp.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gd_io_file.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gd_io_ss.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gd_jpeg.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gd_png.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gd_security.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gd_ss.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gd_topal.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gd_wbmp.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gdcache.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gdfontg.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gdfontl.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gdfontmb.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gdfonts.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gdfontt.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gdft.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gdhelpers.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gdkanji.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/gdtables.c \
            -x c $(LIBDIR)/gd-2.0.36RC1/wbmp.c \
	    -x c $(LIBDIR)/jpeg-8a/jaricom.c \
	    -x c $(LIBDIR)/jpeg-8a/jcapimin.c \
	    -x c $(LIBDIR)/jpeg-8a/jcapistd.c \
	    -x c $(LIBDIR)/jpeg-8a/jcarith.c \
	    -x c $(LIBDIR)/jpeg-8a/jccoefct.c \
	    -x c $(LIBDIR)/jpeg-8a/jccolor.c \
	    -x c $(LIBDIR)/jpeg-8a/jcdctmgr.c \
	    -x c $(LIBDIR)/jpeg-8a/jchuff.c \
	    -x c $(LIBDIR)/jpeg-8a/jcinit.c \
	    -x c $(LIBDIR)/jpeg-8a/jcmainct.c \
	    -x c $(LIBDIR)/jpeg-8a/jcmarker.c \
	    -x c $(LIBDIR)/jpeg-8a/jcmaster.c \
	    -x c $(LIBDIR)/jpeg-8a/jcomapi.c \
	    -x c $(LIBDIR)/jpeg-8a/jcparam.c \
	    -x c $(LIBDIR)/jpeg-8a/jcprepct.c \
	    -x c $(LIBDIR)/jpeg-8a/jcsample.c \
	    -x c $(LIBDIR)/jpeg-8a/jctrans.c \
	    -x c $(LIBDIR)/jpeg-8a/jdapimin.c \
	    -x c $(LIBDIR)/jpeg-8a/jdapistd.c \
	    -x c $(LIBDIR)/jpeg-8a/jdarith.c \
	    -x c $(LIBDIR)/jpeg-8a/jdatadst.c \
	    -x c $(LIBDIR)/jpeg-8a/jdatasrc.c \
	    -x c $(LIBDIR)/jpeg-8a/jdcoefct.c \
	    -x c $(LIBDIR)/jpeg-8a/jdcolor.c \
	    -x c $(LIBDIR)/jpeg-8a/jddctmgr.c \
	    -x c $(LIBDIR)/jpeg-8a/jdhuff.c \
	    -x c $(LIBDIR)/jpeg-8a/jdinput.c \
	    -x c $(LIBDIR)/jpeg-8a/jdmainct.c \
	    -x c $(LIBDIR)/jpeg-8a/jdmarker.c \
	    -x c $(LIBDIR)/jpeg-8a/jdmaster.c \
	    -x c $(LIBDIR)/jpeg-8a/jdmerge.c \
	    -x c $(LIBDIR)/jpeg-8a/jdpostct.c \
	    -x c $(LIBDIR)/jpeg-8a/jdsample.c \
	    -x c $(LIBDIR)/jpeg-8a/jdtrans.c \
	    -x c $(LIBDIR)/jpeg-8a/jerror.c \
	    -x c $(LIBDIR)/jpeg-8a/jfdctflt.c \
	    -x c $(LIBDIR)/jpeg-8a/jfdctfst.c \
	    -x c $(LIBDIR)/jpeg-8a/jfdctint.c \
	    -x c $(LIBDIR)/jpeg-8a/jidctflt.c \
	    -x c $(LIBDIR)/jpeg-8a/jidctfst.c \
	    -x c $(LIBDIR)/jpeg-8a/jidctint.c \
	    -x c $(LIBDIR)/jpeg-8a/jmemansi.c \
	    -x c $(LIBDIR)/jpeg-8a/jmemmgr.c \
	    -x c $(LIBDIR)/jpeg-8a/jquant1.c \
	    -x c $(LIBDIR)/jpeg-8a/jquant2.c \
	    -x c $(LIBDIR)/jpeg-8a/jutils.c \
            -x c $(LIBDIR)/lpng141/png.c \
            -x c $(LIBDIR)/lpng141/pngerror.c \
            -x c $(LIBDIR)/lpng141/pngget.c \
            -x c $(LIBDIR)/lpng141/pngmem.c \
            -x c $(LIBDIR)/lpng141/pngpread.c \
            -x c $(LIBDIR)/lpng141/pngread.c \
            -x c $(LIBDIR)/lpng141/pngrio.c \
            -x c $(LIBDIR)/lpng141/pngrtran.c \
            -x c $(LIBDIR)/lpng141/pngrutil.c \
            -x c $(LIBDIR)/lpng141/pngset.c \
            -x c $(LIBDIR)/lpng141/pngtrans.c \
            -x c $(LIBDIR)/lpng141/pngwio.c \
            -x c $(LIBDIR)/lpng141/pngwrite.c \
            -x c $(LIBDIR)/lpng141/pngwtran.c \
            -x c $(LIBDIR)/lpng141/pngwutil.c

all: slag slagc gogo

slag: $(BINDIR)/slag

slagc: $(BINDIR)/slagc

gogo: $(BINDIR)/gogo

$(BINDIR)/slag: $(VM_H) $(VM_CPP) $(SRCDIR)/slag.cpp
	mkdir -p $(BINDIR)
	$(CC) -o $(BINDIR)/slag $(FLAGS) $(VM_DEFINES) $(VM_CPP) $(SRCDIR)/slag.cpp
	chmod 0775 $(BINDIR)/slag

$(BINDIR)/gogo: $(VM_H) $(VM_CPP) $(GOGO_CPP)
	mkdir -p $(BINDIR)
	$(CC) -o $(BINDIR)/gogo $(GOGO_FLAGS) $(VM_DEFINES) $(VM_CPP) $(GOGO_CPP) $(GOGO_C) $(ZLIB_C)
	chmod 0775 $(BINDIR)/gogo

$(BINDIR)/slagc: $(SLAGC_H) $(SLAGC_CPP) $(SRCDIR)/slagc.cpp
	$(CC) -o $(BINDIR)/slagc $(FLAGS) $(SLAGC_DEFINES) $(SLAGC_CPP) $(SRCDIR)/slagc.cpp
	chmod 0775 $(BINDIR)/slagc
	
$(BUILDDIR)/slagc_xc.h: $(SLAGC_ETC)
	mkdir -p $(BUILDDIR)
	$(BINDIR)/slag $(SLAGC_ETC) slagc.slag -cpp -src_path "$(SLAG_LIBS)/standard;$(SLAG_LIBS)/slagc" -dest_path $(BUILDDIR)

clean:
	rm -r $(BUILDDIR)

xclean: clean
	rm -r $(BINDIR)

