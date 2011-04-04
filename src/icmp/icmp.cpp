#include "icmp.h"

int icmp::sendPacket()
{
    int ret;
    int bport;
    int eport;

    bport = packetArgs.dst_bport;
    eport = packetArgs.dst_eport;

    if (ifInitPayload == 0 || ifInitArgs == 0)
        return -1;

    memoryInit();      //step 1
    networkInit();     //step 2
    packetConstrIP();  //step 3
    
    while (libnet_plist_chain_next_pair(plist_p, &bport, &eport))
    {
        while (!(bport > eport) && bport != 0)
        {
            cport = bport++;

            packetConstrUDP(); //step 3.5
            packetChecksum();  //step 4
            packetInject();    //step 5
        }
    }
    memoryDeinit();    //step 6

    return 0;
}

int icmp::memoryInit();      //step 1
{
    int packet_size = LIBNET_IP_H + LIBNET_UDP_H + packetArgs.payload_size;

    if (libnet_init_packet(packet_size, &packet) == -1)
    {
        libnet_error(LIBNET_ERR_FATAL, "libnet_init_packet failed\n");
    }

    return 0;

}

int icmp::networkInit();     //step 2
{
    network = libnet_open_raw_sock(IPPROTO_RAW);
    if (network == -1)
    {
        libnet_error(LIBNET_ERR_FATAL, "Can't open the network.\n");
        return -1;
    }

    return 0;

}

int icmp::packetConstrIP();    //step 3
{
    libnet_build_ip(LIBNET_UDP_H + packetArgs.payload_size,
                    0,
                    242,
                    packetArgs.frag,
                    packetArgs.ttl,
                    IPPROTO_UDP,
                    packetArgs.src_ip,
                    packetArgs.dst_ip,
                    NULL,
                    0,
                    packet);

    return 0;
}

int icmp::packetConstrICMP(); //step 3.5 
{
    libnet_build_udp(packetArgs.src_port,
                     cport,
                     payload,
                     packetArgs.payload_size,
                     packet + LIBNET_IP_H);

    return 0;

}

int icmp::packetChecksum();  //step 4
{
    if (libnet_do_checksum(packet, IPPROTO_UDP, LIBNET_UDP_H) == -1)
    {
        libnet_error(LIBNET_ERR_FATAL, "udp header checksum error\n");
        return -1;
    }

    return 0;

}

int icmp::packetInject();    //step 5
{
    int ret;

    ret = libnet_write_ip(network, packet, packet_size);

    if (ret < packet_size)
    {
        libnet_error(LN_ERR_WARNING, 
                     "libnet_write_ip only wrote %d bytes\n", ret);
    }
    else
    {
        cout<<"construction and injection completed, wrote all "
            <<ret
            <<" bytes\n";
    }

}

int icmp::memoryDeinit();    //step 6
{
    /*
     *  Shut down the interface.
     */
     if (libnet_close_raw_sock(network) == -1)
     {
         libnet_error(LN_ERR_WARNING, 
                      "libnet_close_raw_sock couldn't close the interface");
     }
         

     /*
     *  Free packet memory.
     */
     libnet_destroy_packet(&packet); 

     return 0;

}

