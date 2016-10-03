SUPER_CPP_HEADERS = \
  ../../SuperCPP/SuperCPPStringTable.h \
  ../../SuperCPP/SuperCPPIntTable.h

FLAGS  = -Wall

INCLUDE_FOLDERS = -I ../../SuperCPP

all: compile run

compile: a.out

a.out: Test.cpp $(SUPER_CPP_HEADERS)
	g++ $(FLAGS) $(INCLUDE_FOLDERS) Test.cpp -o a.out

run:
	./a.out

clean:
	rm -f a.out

