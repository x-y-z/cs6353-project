#include "udp.h"

using std::cerr;
using std::cout;
using std::endl;

int udp::sendPacket()
{
    //int ret;
    u_short bport;
    u_short  eport;

    bport = packetArgs.dst_bport;
    eport = packetArgs.dst_eport;

    if (ifInitPayload == 0 || ifInitArgs == 0)
        return -1;

    networkInit();     //step 1
    
    for (cport = bport; cport <= eport; cport++)
    {
        packetConstrUDP(); //step 2
        packetConstrIP();  //step 3
        packetInject();    //step 4
    }
    memoryDeinit();    //step 5

    return 0;
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
    libnet_ptag_t udp = 0;
    udp = libnet_build_udp(
            packetArgs.src_port,
            cport,
            LIBNET_UDP_H + packetArgs.payload_size,
            0,
            payload,
            packetArgs.payload_size,
            network,
            udp);

    if (udp == -1)
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

    libnet_ptag_t ipo = 0;
    ipo = libnet_build_ipv4_options(
            ip_opt,
            opt_len,
            network,
            ipo);

    if (ipo == -1)
    {
        cerr<<"Can't build IP options: "<<libnet_geterror(network)<<endl;
        return -1;
    }

    return 0;
}

int udp::packetConstrIP()    //step 3
{
    libnet_ptag_t ip = 0;

    ip = libnet_build_ipv4(
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
            ip);                /* libnet id*/

    if (ip == -1)
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
        cerr<<"construction and injection completed, wrote all "
            <<ret
            <<" bytes\n";
    }

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

