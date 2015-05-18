BSTINCL = /home/andreyharitonov/boost_1_57_0/install-dir/include
BSTLIB = /home/andreyharitonov/boost_1_57_0/install-dir/lib

all:
	g++ main.cpp keccak.cpp -lboost_serialization -lboost_filesystem -lboost_system -I$(BSTINCL) -L$(BSTLIB) -std=c++11 -o andrew
