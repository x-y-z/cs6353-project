#ifndef __TCP_H__
#define __TCP_H__

#include <libnet.h>
#include <iostream>

using std::cout;

#define MAX_PAYLOAD_SZ  1500

//add other necessary arguments
typedef struct argList{
    int payload_size;                   /* payload size*/
    u_long src_ip, dst_ip;              /* source ip, dest ip*/
    u_short src_port;                   /* source port*/
    u_short dst_bport;                  /* dest port begin*/
    u_short dst_eport;                  /* dest port end*/
    u_int ttl;                          /* time to live*/
    u_int frag;                         /* framentation bit*/

} argList;

class tcp
{
private:
    int packet_size;                    /* packet size*/
    int network;
    
    u_char payload[MAX_PAYLOAD_SZ];     /* packet payload*/
    u_char *packet;                     /* pointer to packet buffer*/
    char err_buf[LIBNET_ERRBUF_SIZE];   /* error buffer*/
    struct libnet_plist_chain plist;    /* plist chain*/
    struct libnet_plist_chain *plist_p; /* plist chain pointer*/

    argList packetArgs;
    u_short cport;                      /* current dst port*/

    u_int ifInitPayload;
    u_int ifInitArgs;

public:
    tcp():ifInitPayload(0), ifInitArgs(0){};
    ~tcp(){};

public:
    int setPayload(u_char *aPayload, int len = -1)
    {
        if (len < 0 || len > MAX_PAYLOAD_SZ)
            return -1;

        memset(payload, 0, MAX_PAYLOAD_SZ);
        memcpy(payload, aPayload, len);
        packetArgs.packet_size = len;

        ifInitPayload = 1;

        return 0;
    };
    void setArgs(u_int fragm, u_int ttl, 
                 u_long srcIP, u_long dstIP,
                 u_short srcPort, u_short dstPortb, u_short dstPorte
                 //other arguments
                 )
    {
        //add other arguments initialization
        setFragmentation(fragm);
        setTTl(ttl);
        setSrcIP(srcIP);
        setDstIP(dstIP);
        setSrcPort(srcPort);
        setDstPortRange(dstPortb, dstPorte);

        ifInitArgs = 1;
    };
    int sendPacket();
private:
    void setFragmentation(u_int flag){ packetArgs.frag = flag; };
    void setTTL(u_int flag){ packetArgs.ttl = flag; };
    void setSrcIP(u_long arg){ packetArgs.src_ip = arg; };
    void setDstIP(u_long arg){ packetArgs.dst_ip = arg; };
    void setSrcPort(u_short arg){ packetArgs.src_port = arg; };
    void setDstPort(u_short begin, u_short end)
    { packetArgs.dst_bport = begin; packetArgs.dst_eport = end; };
    

private:
    int memoryInit();      //step 1
    int networkInit();     //step 2
    int packetConstrIP();  //step 3
    int packetConstrTCP(); //step 3.5 
    int packetChecksum();  //step 4
    int packetInject();    //step 5
    int memoryDeinit();    //step 6
        
    
};


#endif
