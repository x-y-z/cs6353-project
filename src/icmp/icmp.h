#ifndef __ICMP_H__
#define __ICMP_H__

#include <libnet.h>
#include <iostream>

using std::cout;

#define MAX_PAYLOAD_SZ  1500

typedef u_int u_int;

typedef struct argList_i{
    int payload_size;                   /* payload size*/
    u_long src_ip, dst_ip;              /* source ip, dest ip*/
    u_short src_port;                   /* source port*/
    u_short dst_bport;                  /* dest port begin*/
    u_short dst_eport;                  /* dest port end*/
    u_int ttl;                          /* time to live*/
    u_int frag;                         /* framentation bit*/
//DONE: add more arguments for icmp
    u_char type;			/*icmp type 8-bits*/
    u_char code;			/*icmp code 8-bits*/
    u_short id;				/*icmp id 16-bits*/
    u_short seq;			/*icmp seq 16-bits*/
    u_long mask;			/*subnet mask 32-bits*/
    u_long gateway;			/*redirect gateway 32-bits*/
    u_int otime;			/*timestamp originate time*/
    u_int rtime;			/*timestamp receive time*/
    u_int ttime;			/*timestamp transmit time*/
} argList_i;

//DONE: change to icmp 
class icmp
{
private:
    int packet_size;                    /* packet size*/
    libnet_t *network;                  /* network pointer*/
    
    char payload[MAX_PAYLOAD_SZ];     /* packet payload*/
    char err_buf[LIBNET_ERRBUF_SIZE];   /* error buffer*/

    u_char *ip_opt;
    int opt_len;

    argList_i packetArgs;
    u_short cport;                      /* current dst port*/

    u_int ifInitPayload;
    u_int ifInitArgs;

    libnet_ptag_t icmp_t;
    libnet_ptag_t ipo_t;
    libnet_ptag_t ip_t;

public:
    icmp():ip_opt(NULL), ifInitPayload(0), ifInitArgs(0),icmp_t(0),ipo_t(0),ip_t(0){};
    ~icmp()
    { 
        if (ip_opt != NULL) 
            delete []ip_opt;
    };

public:
    int setPayload(char *aPayload, int len = -1)
    {
        if (len < 0 || len > MAX_PAYLOAD_SZ)
            return -1;
        memset(payload, 0, MAX_PAYLOAD_SZ);
        memcpy(payload, aPayload, len);
        packetArgs.payload_size= len;
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
    };

//DONE: modify setArgs for enough arguments
//object arguments are always set, but not necessarily useful
    void setArgs(u_int fragm, 
                 u_int ttl, 
                 const char* srcIP, 
                 const char* dstIP,
                 u_short srcPort, 
                 u_short dstPortb, 
                 u_short dstPorte, 
                 u_char type, 
                 u_char code,  
                 u_short id, 
                 u_short seq,
		         u_long mask, 
                 u_long gateway, 
                 u_int otime, 
                 u_int rtime, 
                 u_int ttime )
    {
        setFragmentation(fragm);
        setTTL(ttl);
        setSrcIP(srcIP);
        setDstIP(dstIP);
        setSrcPort(srcPort);
        setDstPort(dstPortb, dstPorte);
        setType(type);
        setCode(code);
	    setId(id);
	    setSeq(seq);
	    setMask(mask);
	    setGateway(gateway);
	    setOtime(otime);
	    setRtime(rtime);
	    setTtime(ttime);  
        
        ifInitArgs = 1;
    };
    int sendPacket();

private:

    void setFragmentation(u_int flag){ packetArgs.frag = flag; };
    void setTTL(u_int flag){ packetArgs.ttl = flag; };
    void setSrcIP(const char* arg)
    { 
	 packetArgs.src_ip = libnet_name2addr4(
                                network, 
                                const_cast<char *>(arg), 
                                LIBNET_RESOLVE); 
        if (packetArgs.src_ip == (u_long)-1)
        {
            std::cerr<<"Bad source IP address: "<<arg<<std::endl;
            exit(1);
        }
    };
    void setDstIP(const char* arg)
    {
 	packetArgs.dst_ip = libnet_name2addr4(
                                network, 
                                const_cast<char *>(arg), 
                                LIBNET_RESOLVE); 
        if (packetArgs.dst_ip == (u_long)-1)
        {
            std::cerr<<"Bad destination IP address: "<<arg<<std::endl;
            exit(1);
        }
    };
    void setSrcPort(u_short arg){ packetArgs.src_port = arg; };
    void setDstPort(u_short begin, u_short end)
    { packetArgs.dst_bport = begin; packetArgs.dst_eport = end; };
  
    void setType(u_char arg){packetArgs.type = arg;};
    void setCode(u_char arg){packetArgs.code = arg;};
    void setId(u_short arg){packetArgs.id = arg;};
    void setSeq(u_short arg){packetArgs.seq = arg;};
    void setMask(u_long arg){packetArgs.mask = arg;};
    void setGateway(u_long arg){packetArgs.gateway = arg;};
    void setOtime(u_int otimet){ packetArgs.otime=otimet; };
    void setRtime(u_int rtimet){ packetArgs.rtime=rtimet; };
    void setTtime(u_int ttimet){ packetArgs.ttime=ttimet; };


private:
        int networkInit();          //step 1
        int packetConstrICMP();      //step 2 
	int packetConstrICMP_Echo();
	int packetConstrICMP_Mask();
	int packetConstrICMP_Redirect();
	int packetConstrICMP_TimeExceed();
	int packetConstrICMP_TimeStamp();
	int packetConstrICMP_Unreach();
        int packetConstrIPOpt();    //step 2.5
        int packetConstrIP();       //step 3
        int packetInject();         //step 4
        int packetStat();           //step 4.5
        int memoryDeinit();         //step 5
        
    
};


#endif
