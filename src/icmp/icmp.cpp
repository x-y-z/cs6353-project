#include "icmp.h"

using std::cerr;
using std::cout;
using std::endl;

int icmp::sendPacket()
{
    int ret = 0;
    u_short bport;
    u_short  eport;
    bport = packetArgs.dst_bport;
    eport = packetArgs.dst_eport;
    if (ifInitPayload == 0 || ifInitArgs == 0)
        return -1;
  //  cout<<"here\n";
    for (cport = bport; cport <= eport; cport++)
    {
        ret += packetConstrICMP(); //step 2
        if (ret != 0)
            break;
      //  cout<<"here\n";
        ret += packetConstrIP();  //step 3
        if (ret != 0)
            break;
       // cout<<"here\n";
        ret += packetInject();    //step 4
        if (ret != 0)
            break;
    }
    return ret;
}

int icmp::networkInit()     //step 1
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

  
int icmp::packetConstrICMP() //step 2 
{
    switch(packetArgs.type)
    {
	case ICMP_ECHOREPLY:
	case ICMP_ECHO: packetConstrICMP_Echo();
			break;
	case ICMP_MASKREQ:
	case ICMP_MASKREPLY: 
                       // cout<<"icmp maskreply\n";
                        packetConstrICMP_Mask();                             
			break;
	case ICMP_REDIRECT: packetConstrICMP_Redirect();
			break;
	case ICMP_TIMXCEED: packetConstrICMP_TimeExceed();
			break;
	case ICMP_TSTAMP:
	case ICMP_TSTAMPREPLY: packetConstrICMP_TimeStamp();
			break;
	case ICMP_UNREACH: packetConstrICMP_Unreach();
			break;
	default: cerr<<"Wrong type: "<<packetArgs.type<<endl;
	return -1;
    }

    if (icmp_t == -1)
    {
	cerr<<"Can't build ICMP header: "<<libnet_geterror(network)<<endl;
	return -1;
    }
    return 0;
}

//subroutines implement different icmp types respectively

int icmp::packetConstrICMP_Echo()
{
	icmp_t = libnet_build_icmpv4_echo(
			packetArgs.type,
			0,		          /*code should be 0*/
			0,		          /*checksum = 0 for libnet auto fill*/
			packetArgs.id,
			packetArgs.seq,
			(uint8_t *)payload,
			packetArgs.payload_size,
			network,
			icmp_t);	
           return 0;
}
int icmp::packetConstrICMP_Mask()
{
       // cout<<"in construct packet mask reply\n";
	icmp_t = libnet_build_icmpv4_mask(
			packetArgs.type,
			0,		        /*code should be 0*/
			0,		        /*checksum = 0 for libnet auto fill*/
			packetArgs.id,
			packetArgs.seq,
			packetArgs.mask,	 /*subnet mask*/
			(uint8_t *)payload,
			packetArgs.payload_size,
			network,
			icmp_t);	
          return 0;
}

int icmp::packetConstrICMP_Redirect()
{
	icmp_t = libnet_build_icmpv4_redirect(
			packetArgs.type,
			packetArgs.code,	 /*code should be one of 4*/
			0,		         /*checksum = 0 for libnet auto fill*/
			packetArgs.gateway,
			(uint8_t *)payload,
			packetArgs.payload_size,
			network,
			icmp_t);	
           return 0;
}

int icmp::packetConstrICMP_TimeExceed()
{
	icmp_t = libnet_build_icmpv4_timeexceed(
			packetArgs.type,
			packetArgs.code,	/*code should be one of 2*/
			0,		/*checksum = 0 for libnet auto fill*/
			(uint8_t *)payload,
			packetArgs.payload_size,
			network,
			icmp_t);	
          return 0;
}

int icmp::packetConstrICMP_TimeStamp()
{
	icmp_t = libnet_build_icmpv4_timestamp(
			packetArgs.type,
			0,		/*code should be 0*/
			0,		/*checksum = 0 for libnet auto fill*/
			packetArgs.id,
			packetArgs.seq,
			packetArgs.otime,
			packetArgs.rtime,
			packetArgs.ttime,
			(uint8_t *)payload,
			packetArgs.payload_size,
			network,
			icmp_t);	
        return 0;
}
int icmp::packetConstrICMP_Unreach()
{
	icmp_t = libnet_build_icmpv4_unreach(
			packetArgs.type,
			packetArgs.code,	/*code should be one of 16*/
			0,		     /*checksum = 0 for libnet auto fill*/
			(uint8_t *)payload,
			packetArgs.payload_size,
			network,
			icmp_t);	
        return 0;
}

int icmp::packetConstrIPOpt()    //step 2.5
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

int icmp::packetConstrIP()    //step 3
{
    u_short icmp_hd_s;		/*different icmp type has diff head size*/
    u_char tos = 0;		/*different icmp type has diff tos*/
    switch(packetArgs.type)
    {
	case ICMP_ECHOREPLY:
	case ICMP_ECHO: icmp_hd_s = LIBNET_ICMPV4_ECHO_H;
			break;
	case ICMP_MASKREQ:
	case ICMP_MASKREPLY: icmp_hd_s = 0;		//todo: I don't know?
			break;
	case ICMP_REDIRECT: icmp_hd_s = 0;
			tos = IPTOS_LOWDELAY | IPTOS_THROUGHPUT;
			break;
	case ICMP_TIMXCEED: icmp_hd_s = 0;
			tos = IPTOS_LOWDELAY | IPTOS_THROUGHPUT;
			break;
	case ICMP_TSTAMP:
	case ICMP_TSTAMPREPLY: icmp_hd_s = LIBNET_ICMPV4_TS_H;
			break;
	case ICMP_UNREACH: icmp_hd_s = 0;
			tos = IPTOS_LOWDELAY | IPTOS_THROUGHPUT;
			break;
	default: cerr<<"Wrong type: "<<packetArgs.type<<endl;
	return -1;
    }
    ip_t = libnet_build_ipv4(
            LIBNET_IPV4_H + icmp_hd_s + packetArgs.payload_size,
            tos,                  /* Type of Service*/
            packetArgs.id,                /* IP ID*/
            packetArgs.frag,    /* IP frag*/
            packetArgs.ttl,       /* TTL*/
            IPPROTO_ICMP,         /* protocol*/
            0,                    /* checksum*/
            packetArgs.src_ip,
            packetArgs.dst_ip,
            0,                   /* payload*/
            0,                    /* payload size*/
            network,              /* libnet handle*/
            ip_t);                /* libnet id*/
    if (ip_t == -1)
    {
        cerr<<"Can't build IP header: "<<libnet_geterror(network)<<endl;
        return -1;
    }
    return 0;
}

int icmp::packetInject()    //step 4
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

int icmp::memoryDeinit()    //step 5
{
    /*
     *  Shut down the interface.
     */
    libnet_destroy(network); 
    return 0;

}

