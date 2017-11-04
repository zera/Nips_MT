CPPFLAGS	= -O3 -std=c++0x
CC			= g++
MM			= framework/MurmurHash3.cpp
B2			= framework/blake2b-ref.c
CH			= framework/city.cc

default : all

all : testtime testsim testfhash speed20 testnews20 testmnist news20format

testfhash : fhashtest.cpp
	${CC} ${CPPFLAGS} fhashtest.cpp ${MM} ${CH} ${B2} -o testfhash

testtime : timetest.cpp
	${CC} ${CPPFLAGS} timetest.cpp ${MM} ${B2} ${CH} -o testtime

testsim : simtest.cpp
	${CC} ${CPPFLAGS} simtest.cpp ${MM} ${B2} ${CH} -o testsim

testnews20 : news20_test.cpp
	${CC} ${CPPFLAGS} news20_test.cpp ${MM} ${B2} ${CH} -o testnews20

testmnist : mnist_test.cpp
	${CC} ${CPPFLAGS} mnist_test.cpp ${MM} ${B2} ${CH} -o testmnist

speed20 : news20_speed.cpp
	${CC} ${CPPFLAGS} news20_speed.cpp ${MM} ${B2} ${CH} -o speed20

news20format : news20_change_format.cpp
	${CC} ${CPPFLAGS} news20_change_format.cpp -o news20format

clean :
	rm -f testtime testsim testfhash speed20 testnews20 testmnist news20format
	rm -f *.o
	rm -f *.exe
