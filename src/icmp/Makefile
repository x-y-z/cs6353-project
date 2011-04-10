DEFINES= `libnet-config --defines`
LIBS= `libnet-config --libs`
#INCLUDE= -I/opt/local/include
#LIB= -L/opt/local/lib

test: main.cpp udp.cpp
	g++ -Wall $(DEFINES) $(LIBS) -o test main.cpp icmp.cpp 

