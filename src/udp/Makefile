DEFINES= `libnet-config --defines`
LIBS= `libnet-config --libs`
INCLUDE= -I/opt/local/include
LIB= -L/opt/local/lib

test: main.cpp udp.cpp
	g++ -o test main.cpp udp.cpp -Wall $(DEFINES) $(LIBS) $(INCLUDE) $(LIB)
