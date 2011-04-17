#include "udp.h"

using std::cerr;
using std::cout;
using std::endl;

int udp::sendPacket()
{
    int ret = 0;
    u_short bport;
    u_short  eport;

    bport = packetArgs.dst_bport;
    eport = packetArgs.dst_eport;

    if (ifInitPayload == 0 || ifInitArgs == 0)
        return -1;

    for (cport = bport; cport <= eport; cport++)
    {
        ret += packetConstrUDP(); //step 2
        if (ret != 0)
            break;
        ret += packetConstrIP();  //step 3
        if (ret != 0)
            break;
        ret += packetInject();    //step 4
        if (ret != 0)
            break;
    }

    return ret;
}

int udp::networkInit()     //step 1
{
    network = libnet_init(
                LIBNET_RAW4,
                NULL,
                err_buf);
    if (network == NULL)
    {
        cerr<<"Can't open the network.\n";
        return -1;
    }

    return 0;

}

int udp::packetConstrUDP() //step 2 
{
    udp_t = libnet_build_udp(
            packetArgs.src_port,
            cport,
            LIBNET_UDP_H + packetArgs.payload_size,
            0,
            (u_int8_t *)(payload),
            packetArgs.payload_size,
            network,
            udp_t);

    if (udp_t == -1)
    {
        cerr<<"Can't build UDP header: "<<libnet_geterror(network)<<endl;
        return -1;
    }

    return 0;

}

int udp::packetConstrIPOpt()    //step 2.5
{
    if (ip_opt == NULL)
        return 0;

    ipo_t = libnet_build_ipv4_options(
            ip_opt,
            opt_len,
            network,
            ipo_t);

    if (ipo_t == -1)
    {
        cerr<<"Can't build IP options: "<<libnet_geterror(network)<<endl;
        return -1;
    }

    return 0;
}

int udp::packetConstrIP()    //step 3
{

    ip_t = libnet_build_ipv4(
            LIBNET_IPV4_H + LIBNET_UDP_H + packetArgs.payload_size,
            0,                  /* Type of Service*/
            242,                /* IP ID*/
            packetArgs.frag,    /* IP frag*/
            packetArgs.ttl,     /* TTL*/
            IPPROTO_UDP,        /* protocol*/
            0,                  /* checksum*/
            packetArgs.src_ip,
            packetArgs.dst_ip,
            NULL,               /* payload*/
            0,                  /* payload size*/
            network,            /* libnet handle*/
            ip_t);                /* libnet id*/

    if (ip_t == -1)
    {
        cerr<<"Can't build IP header: "<<libnet_geterror(network)<<endl;
        return -1;
    }
    return 0;
}

int udp::packetInject()    //step 4
{
    int ret;

    ret = libnet_write(network);

    if (ret == -1)
    {
        cerr<<"Write error: "<<libnet_geterror(network)<<endl;
        return -1;
    }
    else
    {
        cerr<<"Construction and injection completed, wrote all "
            <<ret
            <<" bytes\n";
    }

    return 0;
}

int udp::packetStat()       //step 4.5
{
    struct libnet_stats ls;

    libnet_stats(network, &ls);
    cerr<<"Packets sent: "<<ls.packets_sent<<endl<<
          "Packets erros: "<<ls.packet_errors<<endl<<
          "Bytes written: "<<ls.bytes_written<<endl;

    return 0;
}

int udp::memoryDeinit()    //step 5
{
    /*
     *  Shut down the interface.
     */
    libnet_destroy(network); 

    return 0;

}

