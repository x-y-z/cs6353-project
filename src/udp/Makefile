DEFINES= `libnet-config --defines`
LIBS= `libnet-config --libs`
INCLUDE= -I/opt/local/include
LIB= -L/opt/local/lib

test: main.cpp udp.cpp
	g++ -Wall $(DEFINES) $(INCLUDE) $(LIB) $(LIBS) -o test main.cpp udp.cpp 

