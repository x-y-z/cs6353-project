DEFINES= `libnet-config --defines`
LIBS= `libnet-config --libs`


test: main.cpp udp.cpp
	g++ -o test main.cpp udp.cpp -Wall $DEFINES $LIBS
