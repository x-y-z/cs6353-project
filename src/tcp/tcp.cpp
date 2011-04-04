#include "tcp.h"

int tcp::sendPacket()
{
    int ret;

    memoryInit();      //step 1
    networkInit();     //step 2
    packetConstrIP();  //step 3
    
    packetConstrtcp(); //step 3.5
    packetChecksum();  //step 4
    packetInject();    //step 5
    memoryDeinit();    //step 6

    return 0;
}

int tcp::memoryInit();      //step 1
{
    int packet_size = LIBNET_IP_H + LIBNET_TCP_H + packetArgs.payload_size;

    if (libnet_init_packet(packet_size, &packet) == -1)
    {
        libnet_error(LIBNET_ERR_FATAL, "libnet_init_packet failed\n");
    }

    return 0;

}

int tcp::networkInit();     //step 2
{
    network = libnet_open_raw_sock(IPPROTO_RAW);
    if (network == -1)
    {
        libnet_error(LIBNET_ERR_FATAL, "Can't open the network.\n");
        return -1;
    }

    return 0;

}

int tcp::packetConstrIP();    //step 3
{
    libnet_build_ip();

    return 0;
}

int tcp::packetConstrUDP(); //step 3.5 
{
    libnet_build_tcp(
                     );

    return 0;

}

int tcp::packetChecksum();  //step 4
{

    return 0;

}

int tcp::packetInject();    //step 5
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

int tcp::memoryDeinit();    //step 6
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

