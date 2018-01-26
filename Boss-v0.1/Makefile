CC=g++
CFLAGS = -Wall

CPP_FILES = $(shell find Source/CPP/*.cpp | grep -v temp)
H_FILES   = $(shell find Source/CPP/*.h | grep -v temp)

O_FILES = Build/Test.o $(CPP_FILES:Source/CPP/%.cpp=Build/%.o)

all: Build a.out
	./a.out

Build:
	@mkdir -p Build

a.out: $(O_FILES)
	$(CC) $(CFLAGS) $(O_FILES) -o a.out

Build/Test.o: Test.cpp $(H_FILES)
	$(CC) $(CFLAGS) -c $< -o $@

Build/%.o: Source/CPP/%.cpp $(H_FILES)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf Build
