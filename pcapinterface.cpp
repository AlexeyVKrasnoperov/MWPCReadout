#include "pcapinterface.h"
#include <stdio.h>
#include <pcap.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sensormap.h>
#include <QDateTime>
#include "programsettings.h"
#include "qcompressor.h"
#include <QDebug>

PCapInterface::PCapInterface(const char *dev, SensorMap *map, QObject *parent):MWPCDataInterface(map,parent),devName(dev)
{
    start();
}

pcap_t * PCapInterface::openInterface(const char *dev)
{
    char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */
    pcap_t * handle = pcap_open_live(dev, 8192, 0, 1000, errbuf);
    //
    if(handle == 0)
    {
        qCritical() << QString("Couldn't open device %1: %2").arg(dev).arg(errbuf);
        return 0;
    }
    //
    const char * filter = "udp and dst port 24720 and not src net 192.168.0.1";
    bpf_program fcode;
    //
    if( pcap_compile(handle,&fcode,filter,1,0xffffff) < 0 )
    {
        closeInterface(handle);
        return 0;
    }
    //
    if( pcap_setfilter(handle, &fcode) < 0 )
    {
        closeInterface(handle);
        return 0;
    }
    pcap_freecode(&fcode);
    qDebug() << "Open device:" << devName;
    return handle;
}

void PCapInterface::closeInterface(pcap_t *handle)
{
    if( handle != 0 )
    {
        pcap_close(handle);
        qDebug() << "Close device:" << devName;
    }
}

void PCapInterface::run(void)
{
    pcap_t *handle = openInterface(devName.toLocal8Bit());
    if( handle == 0 )
        return;
    breakLoop = false;
    while( ! breakLoop )
    {
       if( ! getNextPacket(handle) )
           usleep(1);
    }
    closeFile();
    closeInterface(handle);
}

bool PCapInterface::getNextPacket(pcap_t *handle)
{
    pcap_pkthdr *header;
    const u_char *packet;
    int res = pcap_next_ex( handle, &header, &packet);
    if( res == 1 )
    {
        /* lets start with the ether header... */
        ether_header * eptr = (struct ether_header *) packet;
        /* check to see if we have an ip packet */
        if( ntohs(eptr->ether_type) != ETHERTYPE_IP )
            return false;
        iphdr *iptr = (iphdr*)(eptr+1);
        udphdr *uptr = (udphdr*)(iptr+1);
        //
        int sp = ntohs(uptr->uh_sport);
        int dp = ntohs(uptr->uh_dport);
        if( dp != 0x6090)
            return false;
        int data_size = header->len - pcap_shift;
        uchar *data = (uchar*)(uptr+1);
        int sz = 0;
        uint trgNum = 0;
        if( sensorMap != 0 )            
            sz = sensorMap->addData(iptr->saddr,sp,&header->ts,data,data_size,trgNum);
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
        if( (fout != 0) && (sz > 0) )
        {
            QByteArray input;
            header->len = pcap_shift + sz;
            input.append((char*)header,sizeof(pcap_pkthdr));
            input.append((char*)packet,header->len);
            if( compress )
            {
                QByteArray output;
                QCompressor::gzipCompress(input,output);
                fwrite(output.data(),output.size(),1,fout);
            }
            else
                fwrite(input.data(),input.size(),1,fout);
        }
        //
        return true;
    }
    return false;
}
