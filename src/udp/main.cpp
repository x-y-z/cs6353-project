#include <iostream>
#include "udp.h"

int main()
{
    udp a;

    a.setPayload((char *)"hello world", 11);
    a.setArgs(0, 64, "192.168.0.11", "192.168.0.12", 12111, 55555, 55557);
    a.sendPacket();
    return 0;
}
