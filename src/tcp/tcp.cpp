#include "tcp.h"

using std::cerr;
using std::cout;
using std::endl;


int tcp::sendPacket()
{
    int ret;
    u_short bport;
    u_short eport;

    bport = packetArgs.dpbegin;
    eport = packetArgs.dpend;

    if (ifInitPayload == 0 || ifInitArgs == 0)
        return -1;
    
    for (cport = bport; cport <= eport; cport++)
    {
        ret = packetConstrTCP(); //step 2
        ret = packetConstrIP();  //step 3
        ret = packetInject();    //step 4
        cout<<"packet injected!\n";
    }
    memoryDeinit(); //step 5

    return 0;
}

int tcp::networkInit() //step 1
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

/*
libnet_ptag_t libnet_build_tcp 	( 	
                u_int16_t  	sp,
		u_int16_t  	dp,
		u_int32_t  	seq,
		u_int32_t  	ack,
		u_int8_t  	control,
		u_int16_t  	win,
		u_int16_t  	sum,
		u_int16_t  	urg, 
		u_int16_t  	len,
		u_int8_t *  	payload,
		u_int32_t  	payload_s,
		libnet_t *  	l,
		libnet_ptag_t  	ptag	 
	) 			
        sp 	source port
	dp 	destination port
	seq 	sequence number
	ack 	acknowledgement number
	control 	control flags
	win 	window size
	sum 	checksum (0 for libnet to autofill)
	urg 	urgent pointer
	len 	total length of the TCP packet (for checksum calculation)
	payload 	
	payload_s 	payload length or 0
	l 	pointer to a libnet context
	ptag 	protocol tag to modify an existing header, 0 to build a new one 
*/

int tcp::packetConstrTCP()      //step 2
{
   tcp_t = libnet_build_tcp(packetArgs.sp,    /* src port */
                  cport,                                 /* destination port */
                  packetArgs.seq,    /* sequence number */       
                  packetArgs.ack,                                 /* acknowledgement */
                  packetArgs.control,               /* control flags */
                  packetArgs.win,                    /* window */
                  packetArgs.sum,                     /* checksum - 0 = autofill */
                  packetArgs.urg,                    /* urgent */
                  packetArgs.len,         /* header length */
                  (u_int8_t *)payload,                /* payload */
                  payloadsize,                   /* payload length */
                  network,                   /* libnet context */
                  tcp_t);                 /* protocol tag */

    if (tcp_t == -1)
    {
        cerr<<"Can't build TCP header: "<<libnet_geterror(network)<<endl;
        return -1;
    }

    return 0;

}



int tcp::packetConstrIPOpt() //step 2.5
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


int tcp::packetConstrIP() //step 3
{

  ip_t = libnet_build_ipv4 (LIBNET_TCP_H + LIBNET_IPV4_H + payloadsize,     /* length */
            0,                             /* Type of Service*/
            242,                           /* IP ID*/
            packetArgs.frag,                /* IP frag*/
            packetArgs.ttl,               /* TTL*/
            IPPROTO_TCP,                   /* protocol*/
            0,                            /* checksum*/
            packetArgs.saddr,
            packetArgs.daddr,
            NULL,                         /* payload*/
            0,                             /* payload size*/
            network,                      /* libnet handle*/
            ip_t);                        /* libnet id*/

     if (ip_t == -1)
     {
       fprintf (stderr,
          "Unable to build IPv4 header: %s\n", libnet_geterror (network));
       exit (1);
     }
    return 0;
}

int tcp::packetInject() //step 4
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

int tcp::packetStat() //step 4.5
{
    struct libnet_stats ls;

    libnet_stats(network, &ls);
    cerr<<"Packets sent: "<<ls.packets_sent<<endl<<
          "Packets erros: "<<ls.packet_errors<<endl<<
          "Bytes written: "<<ls.bytes_written<<endl;
    return 0;
}

int tcp::memoryDeinit() //step 5
{
    /*
* Shut down the interface.
*/
    libnet_destroy(network);

    return 0;

}


