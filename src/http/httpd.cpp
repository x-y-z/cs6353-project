#include <iostream>
#include "parser.h"
#include <unistd.h>
#include <cstdlib>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../udp/udp.h"
#include "../tcp/tcp.h"
#include "../icmp/icmp.h"

using std::cerr;
using std::endl;

#define DEFAULT_PRT 3333
#define BUFFER_SZ   10240

int error(const char *msg)
{
   cerr<<msg<<endl;
   exit(1);
}

void *runClient(void *param);
void replyReq(char *rData, char *sData);
int getIndex(char **buffer);

int main(int argc, char **argv)
{
    int listenSock, accSock;
    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;
    int clntlen;
    int h_port;
    
    //check arugments
    if (argc != 2)
    {
        h_port = DEFAULT_PRT;
    }

    //construct a socket
    listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSock < 0)
        error("ERROR: cannot open socket\n");

    //set socket option
    int optval = 1;
    setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR,
               (const void *)&optval, sizeof(int));

    bzero((char *)&serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;

    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short) h_port);

    //binding address with socket
    if (bind(listenSock, (struct sockaddr *)&serveraddr,
                sizeof(serveraddr)) < 0)
        error("ERROR: binding error\n");

    //listen to hotel port
    if (listen(listenSock, 5) < 0)
        error("ERROR: listen error\n");
        
	clntlen = sizeof(clientaddr);

    while(1)
    {
        //accept new client
        accSock = accept(listenSock, 
                         (struct sockaddr *)&clientaddr, 
                         (socklen_t*)&clntlen);
        if (accSock < 0)
            error("ERROR: accept error\n");

        //send a new client to a thread
        pthread_t tid;
        pthread_create(&tid, NULL, runClient, (void *)&accSock);
    }
    return 0;

}


void *runClient(void *data)
{
    int clntSock;
    char buf[BUFFER_SZ];
    int readNum;
    int state = 0;

    memcpy(&clntSock, data, sizeof(clntSock));

    while (state != 2)
    {
        //read msg sent from the browser
        readNum = recv(clntSock, buf, BUFFER_SZ, 0);
        if (readNum < 0)
            error("ERROR: reading from socket error\n");


        fprintf(stderr,"LOG:Get msg: size:%d\n%s\n", readNum, buf);

        //GET method process
        if (strncmp(buf, "GET", 3) == 0)
        {
            char *fileBuf = NULL;

            int fileLen = getIndex(&fileBuf);


            int sendNum;

            sendNum = send(clntSock, fileBuf, fileLen, 0);
            if (sendNum < fileLen)
            {
                error("ERROR: sending file error\n");
            }

            if (fileBuf != NULL)
            {
                free(fileBuf);
                fileBuf = NULL;
            }

            state = 2;

        }
               //POST method process
        else if (strncmp(buf, "POST", 4) == 0)
        {

            char replyBuf[1024];
            replyReq(buf, replyBuf);
            fprintf(stderr, "LOG:Send msg:%s\n", replyBuf);
            send(clntSock, replyBuf, strlen(replyBuf), 0);

            state = 2;
        }
        else
        {
            error("ERROR: undefined operation\n");
        }
    }

    fprintf(stderr, "LOG:Finished\n");
    fprintf(stderr, "******************\n");
    close(clntSock);
}

int getIndex(char **buffer)
{
    FILE *index;
    long file_sz;

    index = fopen("index.html", "r");
    if (index == NULL)
        error("ERROR: index.html not found\n");

    fseek(index, 0, SEEK_END);
    file_sz = ftell(index);

    *buffer = (char *)malloc(file_sz);
    if (*buffer == NULL)
        error("ERROR: out of mem\n");

    rewind(index);

    int len = fread(*buffer, 1, file_sz, index);
    if (len != file_sz)
        error("ERROR: file reading error\n");

    fclose(index);

    return file_sz;
}


void replyReq(char *rData, char *sData)
{
    parser aParser;
    string iData(rData);
    comArg_t aComm;
    tcpArg_t aTcp;
    icmpArg_t aIcmp;
    char payload[1500];
    int len;

    aParser.getInput(iData);

    aComm = aParser.getCommonArg();
    aTcp = aParser.getTcpArg();
    aIcmp = aParser.getIcmpArg();
    aParser.getPayload(payload, len);
    
    switch (aComm.packet_t)
    {
        case TCP_T:
            {
                tcp aTcpS;
                aTcpS.setArgs(
                        aComm.src_prt,
                        aComm.dst_prtb,
                        aComm.dst_prte,
                        aTcp.seq,
                        aTcp.ack,
                        aTcp.control,
                        aTcp.win,
                        aTcp.urg,
                        aComm.src_ip,
                        aComm.dst_ip,
                        aComm.frag,
                        aComm.ttl
                        );
                aTcpS.setPayload(payload, len);
                if (aTcpS.sendPacket() == 0)
                    strcpy(sData, "Packet sending succeeded!");
                else
                    strcpy(sData, "Packet sending failed!");
            }
            break;
        case UDP_T:
            {
                udp aUdpS;
                aUdpS.setArgs(
                         aComm.frag, 
                         aComm.ttl, 
                         aComm.src_ip, 
                         aComm.dst_ip,
                         aComm.src_prt,
                         aComm.dst_prtb,
                         aComm.dst_prte);
                aUdpS.setPayload(payload, len);
                if (aUdpS.sendPacket() == 0)
                    strcpy(sData, "Packet sending succeeded!");
                else
                    strcpy(sData, "Packet sending failed!");
            }
            break;
        case ICMP_T:
            {
               icmp aIcmpS; 
               aIcmpS.setArgs(
                         aComm.frag,
                         aComm.ttl,
                         aComm.src_ip,
                         aComm.dst_ip,
                         aComm.src_prt,
                         aComm.dst_prtb,
                         aComm.dst_prte,
                         aIcmp.type,
                         aIcmp.code,
                         aIcmp.id,
                         aIcmp.seq,
                         aIcmp.mask,
                         aIcmp.gateway,
                         aIcmp.otime,
                         aIcmp.rtime,
                         aIcmp.ttime);
                aIcmpS.setPayload(payload, len);
                if (aIcmpS.sendPacket() == 0)
                    strcpy(sData, "Packet sending succeeded!");
                else
                    strcpy(sData, "Packet sending failed!");
            }
            break;
        default:
            break;
    }


}
