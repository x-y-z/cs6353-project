#ifndef __UDP_H__
#define __UDP_H__

#include <libnet.h>
#include <iostream>

using std::cout;

#define MAX_PAYLOAD_SZ  1500

typedef struct argList{
    int payload_size;                   /* payload size*/
    u_long src_ip, dst_ip;              /* source ip, dest ip*/
    u_short src_port;                   /* source port*/
    u_short dst_bport;                  /* dest port begin*/
    u_short dst_eport;                  /* dest port end*/
    u_int ttl;                          /* time to live*/
    u_int frag;                         /* framentation bit*/


} argList;

class udp
{
private:
    int packet_size;                    /* packet size*/
    libnet_t *network;                  /* network pointer*/
    
    char payload[MAX_PAYLOAD_SZ];     /* packet payload*/
    char err_buf[LIBNET_ERRBUF_SIZE];   /* error buffer*/

    u_char *ip_opt;
    int opt_len;

    argList packetArgs;
    u_short cport;                      /* current dst port*/

    u_int ifInitPayload;
    u_int ifInitArgs;

    libnet_ptag_t udp_t;
    libnet_ptag_t ipo_t;
    libnet_ptag_t ip_t;
public:
    udp():ip_opt(NULL), ifInitPayload(0), ifInitArgs(0),
          udp_t(0), ipo_t(0), ip_t(0)
    {
        networkInit();
    };
    ~udp()
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
    }
    void setArgs(u_int fragm, u_int ttl, 
                 const char *srcIP, const char *dstIP,
                 u_short srcPort, u_short dstPortb, u_short dstPorte)
    {
        setFragmentation(fragm);
        setTTL(ttl);
        setSrcIP(srcIP);
        setDstIP(dstIP);
        setSrcPort(srcPort);
        setDstPort(dstPortb, dstPorte);

        ifInitArgs = 1;
    };
    int sendPacket();
private:
    void setFragmentation(u_int flag){ packetArgs.frag = flag; };
    void setTTL(u_int flag){ packetArgs.ttl = flag; };
    void setSrcIP(const char *arg)
    { 
        packetArgs.src_ip = libnet_name2addr4(
                                network, 
                                const_cast<char *>(arg), 
                                LIBNET_RESOLVE); 
        if (packetArgs.src_ip == (u_long)-1)
        {
            std::cerr<<"Bad destination IP address: "<<arg<<std::endl;
            exit(1);
        }
    };
    void setDstIP(const char * arg)
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
    

private:
    int networkInit();          //step 1
    int packetConstrUDP();      //step 2 
    int packetConstrIPOpt();    //step 2.5
    int packetConstrIP();       //step 3
    int packetInject();         //step 4
    int packetStat();           //step 4.5
    int memoryDeinit();         //step 5
        
    
};


#endif
