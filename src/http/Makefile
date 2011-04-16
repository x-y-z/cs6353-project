CC=g++
LN_DEF=`libnet-config --defines`
LN_LIB=`libnet-config --libs`
LN_INC=-I/opt/local/include
LN_LB=-L/opt/local/lib
CFLAGS=-lpthread $(LN_DEF) $(LN_LIB) $(LN_INC) $(LN_LB)

httpd: httpd.cpp parser.cpp ../udp/udp.cpp ../tcp/tcp.cpp ../icmp/icmp.cpp
	$(CC) $(CFLAGS) $^ -o $@
