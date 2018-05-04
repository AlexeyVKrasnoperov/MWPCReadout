#include "udpsocketinterface.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <sys/time.h>
#include <qcompressor.h>
#include "sensormap.h"
#include "programsettings.h"
using namespace std;

#include <QDebug>

UDPSocketInterface::UDPSocketInterface(SensorMap *map, QObject *parent):MWPCDataInterface(map,parent),port(0x6090)
{
    start();
}

int UDPSocketInterface::openSocket(void)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, IPPROTO_UDP);
    if(sockfd == -1)
    {
        qDebug() << "Can't create the data socket";
        return sockfd;
    }
    sockaddr_in si_me;
    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    // bind socket to port
    if( bind(sockfd , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1 )
    {
        qDebug() << QString("Can't bind the port %1").arg(port);
        close(sockfd);
        sockfd = -1;
        return sockfd;
    }
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    return sockfd;
}

void UDPSocketInterface::run()
{
    int sockfd = openSocket();
    if( sockfd == -1 )
        return;
    //
    fd_set readfds;
    timeval tv;
    sockaddr_in src_addr;
    socklen_t addrlen = sizeof(src_addr);
    breakLoop = false;
    vector<unsigned char> data(2048,0);
    unsigned char *dataPtr = data.data() + pcap_shift;
    size_t dataSize = data.size() - pcap_shift;
    uint trgNum = 0;
    while( ! breakLoop )
    {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        tv.tv_sec  = 0;
        tv.tv_usec = 1;
        int rv = select(sockfd + 1, &readfds, NULL, NULL, &tv);
        if( rv != 1 )
            continue;
        rv = recvfrom(sockfd, dataPtr, dataSize, 0,(sockaddr*)&src_addr,&addrlen);
        if( (sensorMap == 0) && (rv <= 0) )
            continue;
        gettimeofday(&tv,0);
        sensorMap->addData(src_addr.sin_addr.s_addr,ntohs(src_addr.sin_port),&tv,dataPtr,rv,trgNum);
        //
        if( trgNum < lastTrigger )
        {
            closeFile();
            lastTrigger = -1;
        }
        //
        if( write2file && (fout == 0) )
            openFile();
        //
        if( fout != 0 )
        {
            rv += pcap_shift;
            QByteArray input;
            DataHeader header(tv,ntohl(src_addr.sin_addr.s_addr),rv);
            input.append((char*)&header,sizeof(DataHeader));
            input.append((char*)data.data(),rv);
            if( compress )
            {
                QByteArray output;
                QCompressor::gzipCompress(input,output);
                fwrite(output.data(),output.size(),1,fout);
            }
            else
                fwrite(input.data(),input.size(),1,fout);
        }
    }
    //
    if(sockfd != -1)
    {
        close(sockfd);
        sockfd = -1;
    }
    closeFile();
}
