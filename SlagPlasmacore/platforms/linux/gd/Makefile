GD_SRC   = ../../../libraries/native/gd-2.0.36RC1
JPEG_SRC = ../../../libraries/native/jpeg-8a
PNG_SRC  = ../../../libraries/native/libpng-1.5.4
ZLIB_SRC = ../../../libraries/native/zlib-1.2.3
BUILD_DIR = ../build/gd

GD_FLAGS=-O3 -w -DHAVE_LIBJPEG -DHAVE_LIBPNG -I $(PNG_SRC) -I $(JPEG_SRC) -I $(ZLIB_SRC)

GD_OFILES = \
 $(BUILD_DIR)/gd.o \
 $(BUILD_DIR)/gd_gd.o \
 $(BUILD_DIR)/gd_io.o \
 $(BUILD_DIR)/gd_io_dp.o \
 $(BUILD_DIR)/gd_io_file.o \
 $(BUILD_DIR)/gd_jpeg.o \
 $(BUILD_DIR)/gd_png.o \
 $(BUILD_DIR)/gd_security.o \
 $(BUILD_DIR)/gdhelpers.o \
 $(BUILD_DIR)/gdtables.o \
 $(BUILD_DIR)/jaricom.o \
 $(BUILD_DIR)/jcapimin.o \
 $(BUILD_DIR)/jcapistd.o \
 $(BUILD_DIR)/jcarith.o \
 $(BUILD_DIR)/jccoefct.o \
 $(BUILD_DIR)/jccolor.o \
 $(BUILD_DIR)/jcdctmgr.o \
 $(BUILD_DIR)/jchuff.o \
 $(BUILD_DIR)/jcinit.o \
 $(BUILD_DIR)/jcmainct.o \
 $(BUILD_DIR)/jcmarker.o \
 $(BUILD_DIR)/jcmaster.o \
 $(BUILD_DIR)/jcomapi.o \
 $(BUILD_DIR)/jcparam.o \
 $(BUILD_DIR)/jcprepct.o \
 $(BUILD_DIR)/jcsample.o \
 $(BUILD_DIR)/jctrans.o \
 $(BUILD_DIR)/jdapimin.o \
 $(BUILD_DIR)/jdapistd.o \
 $(BUILD_DIR)/jdarith.o \
 $(BUILD_DIR)/jdatadst.o \
 $(BUILD_DIR)/jdatasrc.o \
 $(BUILD_DIR)/jdcoefct.o \
 $(BUILD_DIR)/jdcolor.o \
 $(BUILD_DIR)/jddctmgr.o \
 $(BUILD_DIR)/jdhuff.o \
 $(BUILD_DIR)/jdinput.o \
 $(BUILD_DIR)/jdmainct.o \
 $(BUILD_DIR)/jdmarker.o \
 $(BUILD_DIR)/jdmaster.o \
 $(BUILD_DIR)/jdmerge.o \
 $(BUILD_DIR)/jdpostct.o \
 $(BUILD_DIR)/jdsample.o \
 $(BUILD_DIR)/jdtrans.o \
 $(BUILD_DIR)/jerror.o \
 $(BUILD_DIR)/jfdctflt.o \
 $(BUILD_DIR)/jfdctfst.o \
 $(BUILD_DIR)/jfdctint.o \
 $(BUILD_DIR)/jidctflt.o \
 $(BUILD_DIR)/jidctfst.o \
 $(BUILD_DIR)/jidctint.o \
 $(BUILD_DIR)/jmemansi.o \
 $(BUILD_DIR)/jmemmgr.o \
 $(BUILD_DIR)/jquant1.o \
 $(BUILD_DIR)/jquant2.o \
 $(BUILD_DIR)/jutils.o \
 $(BUILD_DIR)/png.o \
 $(BUILD_DIR)/pngerror.o \
 $(BUILD_DIR)/pngget.o \
 $(BUILD_DIR)/pngmem.o \
 $(BUILD_DIR)/pngpread.o \
 $(BUILD_DIR)/pngread.o \
 $(BUILD_DIR)/pngrio.o \
 $(BUILD_DIR)/pngrtran.o \
 $(BUILD_DIR)/pngrutil.o \
 $(BUILD_DIR)/pngset.o \
 $(BUILD_DIR)/pngtrans.o \
 $(BUILD_DIR)/pngwio.o \
 $(BUILD_DIR)/pngwrite.o \
 $(BUILD_DIR)/pngwtran.o \
 $(BUILD_DIR)/pngwutil.o

gd: $(BUILD_DIR) $(BUILD_DIR)/gd.a

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/gd.a: $(GD_OFILES)
	ar rcs $(BUILD_DIR)/gd.a $(GD_OFILES)

$(BUILD_DIR)/gd.o: $(GD_SRC)/gd.c
	gcc $(GD_FLAGS) -c $(GD_SRC)/gd.c -o $(BUILD_DIR)/gd.o

$(BUILD_DIR)/gd_gd.o: $(GD_SRC)/gd_gd.c
	gcc $(GD_FLAGS) -c $(GD_SRC)/gd_gd.c -o $(BUILD_DIR)/gd_gd.o

$(BUILD_DIR)/gd_io.o: $(GD_SRC)/gd_io.c
	gcc $(GD_FLAGS) -c $(GD_SRC)/gd_io.c -o $(BUILD_DIR)/gd_io.o

$(BUILD_DIR)/gd_io_dp.o: $(GD_SRC)/gd_io_dp.c
	gcc $(GD_FLAGS) -c $(GD_SRC)/gd_io_dp.c -o $(BUILD_DIR)/gd_io_dp.o

$(BUILD_DIR)/gd_io_file.o: $(GD_SRC)/gd_io_file.c
	gcc $(GD_FLAGS) -c $(GD_SRC)/gd_io_file.c -o $(BUILD_DIR)/gd_io_file.o

$(BUILD_DIR)/gd_jpeg.o: $(GD_SRC)/gd_jpeg.c
	gcc $(GD_FLAGS) -c $(GD_SRC)/gd_jpeg.c -o $(BUILD_DIR)/gd_jpeg.o

$(BUILD_DIR)/gd_png.o: $(GD_SRC)/gd_png.c
	gcc $(GD_FLAGS) -c $(GD_SRC)/gd_png.c -o $(BUILD_DIR)/gd_png.o

$(BUILD_DIR)/gd_security.o: $(GD_SRC)/gd_security.c
	gcc $(GD_FLAGS) -c $(GD_SRC)/gd_security.c -o $(BUILD_DIR)/gd_security.o

$(BUILD_DIR)/gdhelpers.o: $(GD_SRC)/gdhelpers.c
	gcc $(GD_FLAGS) -c $(GD_SRC)/gdhelpers.c -o $(BUILD_DIR)/gdhelpers.o

$(BUILD_DIR)/gdtables.o: $(GD_SRC)/gdtables.c
	gcc $(GD_FLAGS) -c $(GD_SRC)/gdtables.c -o $(BUILD_DIR)/gdtables.o

$(BUILD_DIR)/jaricom.o: $(JPEG_SRC)/jaricom.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jaricom.c -o $(BUILD_DIR)/jaricom.o

$(BUILD_DIR)/jcapimin.o: $(JPEG_SRC)/jcapimin.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jcapimin.c -o $(BUILD_DIR)/jcapimin.o

$(BUILD_DIR)/jcapistd.o: $(JPEG_SRC)/jcapistd.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jcapistd.c -o $(BUILD_DIR)/jcapistd.o

$(BUILD_DIR)/jcarith.o: $(JPEG_SRC)/jcarith.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jcarith.c -o $(BUILD_DIR)/jcarith.o

$(BUILD_DIR)/jccoefct.o: $(JPEG_SRC)/jccoefct.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jccoefct.c -o $(BUILD_DIR)/jccoefct.o

$(BUILD_DIR)/jccolor.o: $(JPEG_SRC)/jccolor.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jccolor.c -o $(BUILD_DIR)/jccolor.o

$(BUILD_DIR)/jcdctmgr.o: $(JPEG_SRC)/jcdctmgr.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jcdctmgr.c -o $(BUILD_DIR)/jcdctmgr.o

$(BUILD_DIR)/jchuff.o: $(JPEG_SRC)/jchuff.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jchuff.c -o $(BUILD_DIR)/jchuff.o

$(BUILD_DIR)/jcinit.o: $(JPEG_SRC)/jcinit.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jcinit.c -o $(BUILD_DIR)/jcinit.o

$(BUILD_DIR)/jcmainct.o: $(JPEG_SRC)/jcmainct.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jcmainct.c -o $(BUILD_DIR)/jcmainct.o

$(BUILD_DIR)/jcmarker.o: $(JPEG_SRC)/jcmarker.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jcmarker.c -o $(BUILD_DIR)/jcmarker.o

$(BUILD_DIR)/jcmaster.o: $(JPEG_SRC)/jcmaster.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jcmaster.c -o $(BUILD_DIR)/jcmaster.o

$(BUILD_DIR)/jcomapi.o: $(JPEG_SRC)/jcomapi.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jcomapi.c -o $(BUILD_DIR)/jcomapi.o

$(BUILD_DIR)/jcparam.o: $(JPEG_SRC)/jcparam.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jcparam.c -o $(BUILD_DIR)/jcparam.o

$(BUILD_DIR)/jcprepct.o: $(JPEG_SRC)/jcprepct.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jcprepct.c -o $(BUILD_DIR)/jcprepct.o

$(BUILD_DIR)/jcsample.o: $(JPEG_SRC)/jcsample.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jcsample.c -o $(BUILD_DIR)/jcsample.o

$(BUILD_DIR)/jctrans.o: $(JPEG_SRC)/jctrans.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jctrans.c -o $(BUILD_DIR)/jctrans.o

$(BUILD_DIR)/jdapimin.o: $(JPEG_SRC)/jdapimin.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdapimin.c -o $(BUILD_DIR)/jdapimin.o

$(BUILD_DIR)/jdapistd.o: $(JPEG_SRC)/jdapistd.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdapistd.c -o $(BUILD_DIR)/jdapistd.o

$(BUILD_DIR)/jdarith.o: $(JPEG_SRC)/jdarith.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdarith.c -o $(BUILD_DIR)/jdarith.o

$(BUILD_DIR)/jdatadst.o: $(JPEG_SRC)/jdatadst.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdatadst.c -o $(BUILD_DIR)/jdatadst.o

$(BUILD_DIR)/jdatasrc.o: $(JPEG_SRC)/jdatasrc.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdatasrc.c -o $(BUILD_DIR)/jdatasrc.o

$(BUILD_DIR)/jdcoefct.o: $(JPEG_SRC)/jdcoefct.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdcoefct.c -o $(BUILD_DIR)/jdcoefct.o

$(BUILD_DIR)/jdcolor.o: $(JPEG_SRC)/jdcolor.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdcolor.c -o $(BUILD_DIR)/jdcolor.o

$(BUILD_DIR)/jddctmgr.o: $(JPEG_SRC)/jddctmgr.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jddctmgr.c -o $(BUILD_DIR)/jddctmgr.o

$(BUILD_DIR)/jdhuff.o: $(JPEG_SRC)/jdhuff.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdhuff.c -o $(BUILD_DIR)/jdhuff.o

$(BUILD_DIR)/jdinput.o: $(JPEG_SRC)/jdinput.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdinput.c -o $(BUILD_DIR)/jdinput.o

$(BUILD_DIR)/jdmainct.o: $(JPEG_SRC)/jdmainct.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdmainct.c -o $(BUILD_DIR)/jdmainct.o

$(BUILD_DIR)/jdmarker.o: $(JPEG_SRC)/jdmarker.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdmarker.c -o $(BUILD_DIR)/jdmarker.o

$(BUILD_DIR)/jdmaster.o: $(JPEG_SRC)/jdmaster.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdmaster.c -o $(BUILD_DIR)/jdmaster.o

$(BUILD_DIR)/jdmerge.o: $(JPEG_SRC)/jdmerge.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdmerge.c -o $(BUILD_DIR)/jdmerge.o

$(BUILD_DIR)/jdpostct.o: $(JPEG_SRC)/jdpostct.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdpostct.c -o $(BUILD_DIR)/jdpostct.o

$(BUILD_DIR)/jdsample.o: $(JPEG_SRC)/jdsample.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdsample.c -o $(BUILD_DIR)/jdsample.o

$(BUILD_DIR)/jdtrans.o: $(JPEG_SRC)/jdtrans.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jdtrans.c -o $(BUILD_DIR)/jdtrans.o

$(BUILD_DIR)/jerror.o: $(JPEG_SRC)/jerror.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jerror.c -o $(BUILD_DIR)/jerror.o

$(BUILD_DIR)/jfdctflt.o: $(JPEG_SRC)/jfdctflt.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jfdctflt.c -o $(BUILD_DIR)/jfdctflt.o

$(BUILD_DIR)/jfdctfst.o: $(JPEG_SRC)/jfdctfst.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jfdctfst.c -o $(BUILD_DIR)/jfdctfst.o

$(BUILD_DIR)/jfdctint.o: $(JPEG_SRC)/jfdctint.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jfdctint.c -o $(BUILD_DIR)/jfdctint.o

$(BUILD_DIR)/jidctflt.o: $(JPEG_SRC)/jidctflt.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jidctflt.c -o $(BUILD_DIR)/jidctflt.o

$(BUILD_DIR)/jidctfst.o: $(JPEG_SRC)/jidctfst.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jidctfst.c -o $(BUILD_DIR)/jidctfst.o

$(BUILD_DIR)/jidctint.o: $(JPEG_SRC)/jidctint.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jidctint.c -o $(BUILD_DIR)/jidctint.o

$(BUILD_DIR)/jmemansi.o: $(JPEG_SRC)/jmemansi.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jmemansi.c -o $(BUILD_DIR)/jmemansi.o

$(BUILD_DIR)/jmemmgr.o: $(JPEG_SRC)/jmemmgr.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jmemmgr.c -o $(BUILD_DIR)/jmemmgr.o

$(BUILD_DIR)/jquant1.o: $(JPEG_SRC)/jquant1.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jquant1.c -o $(BUILD_DIR)/jquant1.o

$(BUILD_DIR)/jquant2.o: $(JPEG_SRC)/jquant2.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jquant2.c -o $(BUILD_DIR)/jquant2.o

$(BUILD_DIR)/jutils.o: $(JPEG_SRC)/jutils.c
	gcc $(GD_FLAGS) -c $(JPEG_SRC)/jutils.c -o $(BUILD_DIR)/jutils.o

$(BUILD_DIR)/png.o: $(PNG_SRC)/png.c
	gcc $(GD_FLAGS) -c $(PNG_SRC)/png.c -o $(BUILD_DIR)/png.o

$(BUILD_DIR)/pngerror.o: $(PNG_SRC)/pngerror.c
	gcc $(GD_FLAGS) -c $(PNG_SRC)/pngerror.c -o $(BUILD_DIR)/pngerror.o

$(BUILD_DIR)/pngget.o: $(PNG_SRC)/pngget.c
	gcc $(GD_FLAGS) -c $(PNG_SRC)/pngget.c -o $(BUILD_DIR)/pngget.o

$(BUILD_DIR)/pngmem.o: $(PNG_SRC)/pngmem.c
	gcc $(GD_FLAGS) -c $(PNG_SRC)/pngmem.c -o $(BUILD_DIR)/pngmem.o

$(BUILD_DIR)/pngpread.o: $(PNG_SRC)/pngpread.c
	gcc $(GD_FLAGS) -c $(PNG_SRC)/pngpread.c -o $(BUILD_DIR)/pngpread.o

$(BUILD_DIR)/pngread.o: $(PNG_SRC)/pngread.c
	gcc $(GD_FLAGS) -c $(PNG_SRC)/pngread.c -o $(BUILD_DIR)/pngread.o

$(BUILD_DIR)/pngrio.o: $(PNG_SRC)/pngrio.c
	gcc $(GD_FLAGS) -c $(PNG_SRC)/pngrio.c -o $(BUILD_DIR)/pngrio.o

$(BUILD_DIR)/pngrtran.o: $(PNG_SRC)/pngrtran.c
	gcc $(GD_FLAGS) -c $(PNG_SRC)/pngrtran.c -o $(BUILD_DIR)/pngrtran.o

$(BUILD_DIR)/pngrutil.o: $(PNG_SRC)/pngrutil.c
	gcc $(GD_FLAGS) -c $(PNG_SRC)/pngrutil.c -o $(BUILD_DIR)/pngrutil.o

$(BUILD_DIR)/pngset.o: $(PNG_SRC)/pngset.c
	gcc $(GD_FLAGS) -c $(PNG_SRC)/pngset.c -o $(BUILD_DIR)/pngset.o

$(BUILD_DIR)/pngtrans.o: $(PNG_SRC)/pngtrans.c
	gcc $(GD_FLAGS) -c $(PNG_SRC)/pngtrans.c -o $(BUILD_DIR)/pngtrans.o

$(BUILD_DIR)/pngwio.o: $(PNG_SRC)/pngwio.c
	gcc $(GD_FLAGS) -c $(PNG_SRC)/pngwio.c -o $(BUILD_DIR)/pngwio.o

$(BUILD_DIR)/pngwrite.o: $(PNG_SRC)/pngwrite.c
	gcc $(GD_FLAGS) -c $(PNG_SRC)/pngwrite.c -o $(BUILD_DIR)/pngwrite.o

$(BUILD_DIR)/pngwtran.o: $(PNG_SRC)/pngwtran.c
	gcc $(GD_FLAGS) -c $(PNG_SRC)/pngwtran.c -o $(BUILD_DIR)/pngwtran.o

$(BUILD_DIR)/pngwutil.o: $(PNG_SRC)/pngwutil.c
	gcc $(GD_FLAGS) -c $(PNG_SRC)/pngwutil.c -o $(BUILD_DIR)/pngwutil.o

