HEADERS = Bard.h BardDefines.h BardUtil.h BardAllocator.h BardObject.h BardString.h BardReader.h BardLoader.h \
	  BardType.h BardProperty.h BardMethod.h BardMM.h BardVM.h BardProcessor.h BardOpcodes.h \
	  BardEventQueue.h \
	  BardCoreLibrary.h BardArray.h BardList.h
OBJECTS = Bard.o BardUtil.o BardAllocator.o BardObject.o BardString.o BardReader.o BardLoader.o \
	  BardType.o BardProperty.o BardMethod.o BardMM.o BardVM.o BardProcessor.o BardOpcodes.o \
	  BardEventQueue.o \
	  BardCoreLibrary.o BardArray.o BardList.o
FLAGS   = -Wall #-fno-strict-aliasing

b4: $(HEADERS) $(OBJECTS)
	gcc $(FLAGS) $(OBJECTS) -o b4
	chmod a+x b4

clean:
	rm -f *.o

Bard.o: $(HEADERS) Bard.c
	gcc -c $(FLAGS) Bard.c

BardUtil.o: $(HEADERS) BardUtil.c
	gcc -c $(FLAGS) BardUtil.c

BardAllocator.o: $(HEADERS) BardAllocator.c
	gcc -c $(FLAGS) BardAllocator.c

BardObject.o: $(HEADERS) BardObject.c
	gcc -c $(FLAGS) BardObject.c

BardString.o: $(HEADERS) BardString.c
	gcc -c $(FLAGS) BardString.c

BardReader.o: $(HEADERS) BardReader.c
	gcc -c $(FLAGS) BardReader.c

BardLoader.o: $(HEADERS) BardLoader.c
	gcc -c $(FLAGS) BardLoader.c

BardType.o: $(HEADERS) BardType.c
	gcc -c $(FLAGS) BardType.c

BardProperty.o: $(HEADERS) BardProperty.c
	gcc -c $(FLAGS) BardProperty.c

BardMethod.o: $(HEADERS) BardMethod.c
	gcc -c $(FLAGS) BardMethod.c

BardMM.o: $(HEADERS) BardMM.c
	gcc -c $(FLAGS) BardMM.c

BardVM.o: $(HEADERS) BardVM.c BardOpcodes.h
	gcc -c $(FLAGS) BardVM.c

BardProcessor.o: $(HEADERS) BardProcessor.c BardOpcodes.h
	gcc -c $(FLAGS) BardProcessor.c

BardOpcodes.o: $(HEADERS) BardOpcodes.c BardOpcodes.h
	gcc -c $(FLAGS) BardOpcodes.c

BardEventQueue.o: $(HEADERS) BardEventQueue.c
	gcc -c $(FLAGS) BardEventQueue.c

BardCoreLibrary.o: $(HEADERS) BardCoreLibrary.c
	gcc -c $(FLAGS) BardCoreLibrary.c

BardArray.o: $(HEADERS) BardArray.c
	gcc -c $(FLAGS) BardArray.c

BardList.o: $(HEADERS) BardList.c
	gcc -c $(FLAGS) BardList.c

