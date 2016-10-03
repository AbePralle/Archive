SUPER_CPP_HEADERS = \
  ../../SuperCPP/SuperCPPResourceBank.h

all: compile run

compile: a.out

a.out: Test.cpp $(SUPER_CPP_HEADERS)
	g++ -Wall -I ../../SuperCPP Test.cpp -o a.out

run:
	./a.out

clean:
	rm -f a.out

