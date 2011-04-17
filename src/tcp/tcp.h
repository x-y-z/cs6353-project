#ifndef __TCP_H__
#define __TCP_H__

#include <libnet.h>
#include <iostream>

using std::cout;

#define MAX_PAYLOAD_SZ 1500

 /*     sp 	source port
	dp 	destination port
	seq 	sequence number
	ack 	acknowledgement number
	control 	control flags
	win 	window size
	sum 	checksum (0 for libnet to autofill)
	urg 	urgent pointer
	len 	total length of the TCP packet (for checksum calculation)
	payload 	
	payload_s 	payload length or 0
	l 	pointer to a libnet context
	ptag 	protocol tag to modify an existing header, 0 to build a new one 
*/

typedef struct argList_t{
		u_int16_t  	sp;   //source port
		u_int16_t  	dpbegin;
                u_int16_t  	dpend;   //destination port
		u_int32_t  	seq;    // sequence number
		u_int32_t  	ack;     // ack number
		u_int8_t  	control;   // control
		u_int16_t  	win;   //   winsize
		u_int16_t  	sum;   // check sum  0
		u_int16_t  	urg;     // pointer  0
		u_int16_t  	len;     // lens of the packet
		u_int8_t *  	payload;   // payload
		u_int32_t  	payload_s;   // lens of payload
                u_long   saddr;
                u_long   daddr;
                u_int ttl;            /* time to live*/
                u_int frag;           /* framentation bit*/
} argList_t;

class tcp
{
private:
    int packet_size; /* packet size*/
    libnet_t *network; /* network pointer*/
    
    char payload[MAX_PAYLOAD_SZ]; /* packet payload*/
    char err_buf[LIBNET_ERRBUF_SIZE]; /* error buffer*/
    int payloadsize;

    u_char *ip_opt;
    int opt_len;

    argList_t packetArgs;
    u_short cport;   /* current dst port*/

    u_int ifInitPayload;
    u_int ifInitArgs;

    libnet_ptag_t tcp_t;
    libnet_ptag_t ipo_t;
    libnet_ptag_t ip_t;


public:
    tcp():ip_opt(NULL), ifInitPayload(0), ifInitArgs(0),
          tcp_t(0), ipo_t(0), ip_t(0)
    {
        networkInit();
    };
    ~tcp()
    {
        if (ip_opt != NULL)
            delete []ip_opt;

        memoryDeinit();
    };

public:

    int setPayload(char *aPayload, int len = -1)
    {
        payloadsize=len;
        if (len < 0 || len > MAX_PAYLOAD_SZ)
            return -1;
        memset(payload, 0, MAX_PAYLOAD_SZ);
        memcpy(payload, aPayload, len);
        packetArgs.payload_s= len;
        ifInitPayload = 1;
        return 0;
    };

    int setIPOpt(u_char *aOpt, int len = -1)
    {
        if (len < 0)
        {
            ip_opt = NULL;
            return -1;
        }
        ip_opt = new u_char[len];
        memcpy(ip_opt, aOpt, len);
        opt_len = len;

        return 0;
    }
    void setArgs(
                u_int16_t  	sp,     //source port
		u_int16_t  	dpbegin,
                u_int16_t  	dpend,   //destination port
		u_int32_t  	seq,    // sequence number
		u_int32_t  	ack,     // ack number
		u_int8_t  	control,   // control
		u_int16_t  	win,   //   winsize
		u_int16_t  	urg,     // pointer  0
                char*   saddr,
                char*   daddr,
                u_int frag,
                u_int ttltime)
    {
          setDstPort(dpbegin,dpend) ;
          setWinSize(win);
          setSrcPort(sp);
          setSeq(seq);
          setAck(ack);
          setControl(control);
          setUrgent(urg);
          setSrcIP(saddr);
          setDstIP(daddr);
          setFragmentation(frag);
          setTTL(ttltime);
          ifInitArgs = 1;
    };
    void setTTL(u_int ttltime){ packetArgs.ttl = ttltime; };


    int sendPacket();

private:

    void setSrcIP(char *arg)
    {
        packetArgs.saddr = libnet_name2addr4(
                                network,
                                const_cast<char *>(arg),
                                LIBNET_RESOLVE);
        if (packetArgs.saddr== (u_long)-1)
        {
            std::cerr<<"Bad destination IP address: "<<arg<<std::endl;
            exit(1);
        }
    };
    void setFragmentation(u_int cflag){ packetArgs.frag = cflag; };
    void setDstIP(char * arg)
    {
        packetArgs.daddr = libnet_name2addr4(
                                network,
                              const_cast<char *>(arg),
                                LIBNET_RESOLVE);
        if (packetArgs.daddr == (u_long)-1)
        {
            std::cerr<<"Bad destination IP address: "<<arg<<std::endl;
            exit(1);
        }
    };
   void setDstPort(u_short begin, u_short end) { packetArgs.dpbegin = begin; packetArgs.dpend = end; };
   void setWinSize(u_int16_t size){ packetArgs.win=size; return; };
   void setSrcPort(u_short src){packetArgs.sp=src;};
   void setSeq(u_int32_t seqnumber){packetArgs.seq=seqnumber;};
   void setAck(u_int32_t acknumber){packetArgs.ack=acknumber;};
   void setControl(u_int8_t con){packetArgs.control=con;};
   void setUrgent(u_int16_t urgent){packetArgs.urg=urgent;};
   void setLengh(u_int16_t len){packetArgs.len=LIBNET_TCP_H; };

    

private:
    int networkInit(); //step 1
    int packetConstrTCP(); //step 2
    int packetConstrIPOpt(); //step 2.5
    int packetConstrIP(); //step 3
    int packetInject(); //step 4
    int packetStat(); //step 4.5
    int memoryDeinit(); //step 5
};


#endif
