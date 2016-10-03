SUPER_CPP_HEADERS = \
  ../../SuperCPP/SuperCPP.h \
  ../../SuperCPP/SuperCPPStringBuilder.h \
  ../../SuperCPP/SuperCPPDataBuilder.h \
  ../../SuperCPP/SuperCPPDataReader.h

SUPER_CPP_SOURCE = \
  ../../SuperCPP/SuperCPP.cpp \
  ../../SuperCPP/SuperCPPStringBuilder.cpp \
  ../../SuperCPP/SuperCPPDataBuilder.cpp \
  ../../SuperCPP/SuperCPPDataReader.cpp

all: compile run

compile: a.out

a.out: Test.cpp $(SUPER_CPP_HEADERS) $(SUPER_CPP_SOURCE)
	g++ -Wall -I ../../SuperCPP $(SUPER_CPP_SOURCE) Test.cpp -o a.out

run:
	./a.out

clean:
	rm -f a.out

