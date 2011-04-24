#include "parser.h"
#include <cstring>
#include <cstdlib>


int parser::getInput(const string content)
{
    int start;
    int end;
    string res;

    for (int i = 0; i < ARGNUM; i++)
    {
        switch (i)
        {
            case 0://type
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                if (res.compare("tcp") == 0)
                    commArg.packet_t = TCP_T;
                else if (res.compare("udp") == 0)
                    commArg.packet_t = UDP_T;
                else if (res.compare("icmp") == 0)
                    commArg.packet_t = ICMP_T;
                else commArg.packet_t = -1;
                break;
            case 1://srcip
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                strncpy(commArg.src_ip, res.c_str(), res.length());
                break;
            case 2://srcprt
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                commArg.src_prt = (u_short)atoi(res.c_str());
                break;
            case 3://dstip
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                strncpy(commArg.dst_ip, res.c_str(), res.length());
                break;
            case 4://dstprtb
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                commArg.dst_prtb = (u_short)atoi(res.c_str());
                break;
            case 5://dstprte
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                commArg.dst_prte = (u_short)atoi(res.c_str());
                break;
            case 6://frag
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                commArg.frag = (u_int)atoi(res.c_str());
                break;
            case 7://ttl
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                commArg.ttl = (u_int)atoi(res.c_str());
                break;
            case 8://tcp_seqnum
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                tcpArg.seq = (u_int)atoi(res.c_str());
                break;
            case 9://tcp_acknum
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                tcpArg.ack = (u_int)atoi(res.c_str());
                break;
            case 10://tcp_winsz
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                tcpArg.win = (u_short)atoi(res.c_str());
                break;
            case 11://tcp_urgptr
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                tcpArg.urg = (u_short)atoi(res.c_str());
                break;
            case 12://tcp_ctrlflag
                start = 0;
                tcpArg.control = 0;
                while ((start = content.find(argNames[i], start)) != -1)
                {
                    start = start + strlen(argNames[i]) + 1;
                    end = content.find_first_of('&', start);
                    if (end == string::npos) end = content.length();
                    res = content.substr(start, end - start);

                    if (res.compare("tcp_urg") == 0)
                        tcpArg.control = tcpArg.control | TH_URG;
                    else if (res.compare("tcp_ack") == 0)
                        tcpArg.control = tcpArg.control | TH_ACK;
                    else if (res.compare("tcp_psh") == 0)
                        tcpArg.control = tcpArg.control | TH_PUSH;
                    else if (res.compare("tcp_rst") == 0)
                        tcpArg.control = tcpArg.control | TH_RST;
                    else if (res.compare("tcp_syn") == 0)
                        tcpArg.control = tcpArg.control | TH_SYN;
                    else if (res.compare("tcp_fin") == 0)
                        tcpArg.control = tcpArg.control | TH_FIN;
                    tcpArg.control = tcpArg.control | (u_int)atoi(res.c_str());
                }
                break;
            case 13://icmp_type
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                icmpArg.type = (u_char)atoi(res.c_str());
                break;
            case 14://icmp_code
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                icmpArg.code = (u_char)atoi(res.c_str());
                break;
            case 15://icmp_id
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                icmpArg.id = (u_short)atoi(res.c_str());
                break;
            case 16://icmp_seq
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                icmpArg.seq = (u_short)atoi(res.c_str());
                break;
            case 17://icmp_submask
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                icmpArg.mask = (u_int)atoi(res.c_str());
                break;
            case 18://icmp_gateway
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                icmpArg.gateway = (u_int)atoi(res.c_str());
                break;
            case 19://icmp_ots
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                icmpArg.otime = (u_int)atoi(res.c_str());
                break;
            case 20://icmp_rts
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                icmpArg.rtime = (u_int)atoi(res.c_str());
                break;
            case 21://icmp_tts
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                icmpArg.ttime = (u_int)atoi(res.c_str());
                break;
            case 22://payload
                start = content.find(argNames[i]);
                start = start + strlen(argNames[i]) + 1;
                end = content.find_first_of('&', start);
                if (end == string::npos) end = content.length();
                res = content.substr(start, end - start);

                strncpy(payload, res.c_str(), res.length());
                payload_len = res.length();
                break;
            default:
                break;
        }
    }

}

comArg_t parser::getCommonArg()
{
    return commArg;
}

tcpArg_t parser::getTcpArg()
{
    return tcpArg;
}

icmpArg_t  parser::getIcmpArg()
{
    return icmpArg;
}

void parser::getPayload(char *aPayload, int &len)
{
    len = payload_len;
    strncpy(aPayload, payload, payload_len);
}

