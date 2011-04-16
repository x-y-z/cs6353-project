#ifndef __PARSER_H__
#define __PARSER_H__
#include <libnet.h>
#include <string>

using std::string;

#define TCP     1
#define UDP     2
#define ICMP    3

#define ARGNUM  22

const char argNames[ARGNUM][15] =
{
    "type", 
    "srcip", "srcprt", "dstip", "dstprtb", "dstprte", "frag", "ttl",
    "tcp_seqnum", "tcp_acknum", "tcp_winsz", "tcp_urgptr", 
    "icmp_type", "icmp_code", "icmp_id", "icmp_seq", "icmp_submask",
    "icmp_gateway", "icmp_ots", "icmp_rts", "icmp_tts",
    "payload"
};

typedef struct comArg_t{
    int packet_t;
    u_long src_ip;
    u_long dst_ip;
    u_short src_prt;
    u_short dst_prt;
    u_int ttl;
    u_int frag;
} comArg_t;

typedef struct tcpArg_t{
    u_int seq;
    u_int ack;
    u_char control;
    u_short win;
    u_short urg;
} tcpArg_t;

typedef struct icmpArg_t{
    u_char type;
    u_char code;
    u_short id;
    u_short seq;
    u_int mask;
    u_int gateway;
    u_int otime;
    u_int rtime;
    u_int ttime;
} icmpArg_t;

class parser
{
private:
    comArg_t commArg;
    tcpArg_t tcpArg;
    icmpArg_t icmpArg;

public:
    parser(){};
    ~parser(){};
public:
    int getInput(const string content);
    void getCommonArg(comArg_t *aArg = NULL);
    void getTcpArg(tcpArg_t *aArg = NULL);
    void getIcmpArg(icmpArg_t *aArg = NULL);

}


#endif
