#include <iostream>
#include "tcp.h"

int main()
{
    tcp a;
    a.setPayload((char *)"hello world", 11);
    a.setArgs(  12111,     //source port
		55555,
                55557,  //destination port
		libnet_get_prand (LIBNET_PRu16),    // sequence number
		0,     // ack number
		TH_SYN,  // control
		7,       //   winsize
		0,       // check sum  0
		0,       // pointer  0
		LIBNET_TCP_H,     // lens of the packet
                "172.24.18.79",
                "192.168.0.2",
                 0,
                 255);
    a.sendPacket();
    cout<<"packet sent finished.\n";
    return 0;
}
