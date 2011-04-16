#include <iostream>
#include "icmp.h"

int main()
{
    icmp a;
    a.setArgs( 0,   //u_int fragm,    
              64,   //u_int ttl, 
              "129.115.2.246",   //src ip 
              "192.168.0.2",  //const char* dstIP,
              12345,     //u_short srcPort, 
              55555,    // u_short dstPortb,
              55558,    //u_short dstPorte, 
              ICMP_MASKREPLY,  //u_char type, 
              23,     //u_char code,
              0,     //u_short sum, 
              12,    //u_short id, 
              12,   //u_short seq,
	      0,   //u_long mask, 
              0,    //u_long gateway,
              0,   //n_time otime, 
              0,    //n_time rtime, 
              0    //n_time ttime
           );
    cout<<"Args set\n";
    a.setPayload((u_char*)"hello world", 11);
    cout<<"payload set\n";
    a.sendPacket();
    cout<<"packet sent\n";
    return 0;
}
