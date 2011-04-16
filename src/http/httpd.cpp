#include <iostream>
#include "parser.h"

int main(int argc, char **argv)
{
    parser aParser;
    string tmp("type=tcp&srcip=192.168.0.1&srcprt=1123&dstip=126.124.145.111&dstprtb=45&dstprte=56&frag=0&ttl=62&tcp_seqnum=12&tcp_acknum=23&tcp_winsz=123&tcp_urgptr=123&tcp_ctrlflag=tcp_urg&tcp_ctrlflag=tcp_psh&tcp_ctrlflag=tcp_rst&tcp_ctrlflag=tcp_syn&tcp_ctrlflag=tcp_fin&icmp_type=12&icmp_code=12&icmp_id=22&icmp_seq=22&icmp_submask=22&icmp_gateway=22&icmp_ots=22&icmp_rts=22&icmp_tts=22&payload=hello+world");
    comArg_t a;
    tcpArg_t b;
    icmpArg_t c;
    char payload[500];
    int len;

    aParser.getInput(tmp);
    a = aParser.getCommonArg();
    b = aParser.getTcpArg();
    c = aParser.getIcmpArg();
    aParser.getPayload(payload, len);

    return 0;

}
