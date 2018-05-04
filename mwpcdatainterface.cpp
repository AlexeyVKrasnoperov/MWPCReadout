#include "mwpcdatainterface.h"
#include "programsettings.h"
#include <QDateTime>
#include <QDebug>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h> /* includes net/ethernet.h */


const int pcap_shift = sizeof(ether_header) + sizeof(iphdr) + sizeof(udphdr);

MWPCDataInterface::MWPCDataInterface(SensorMap *map, QObject *parent):QThread(parent),sensorMap(map)
{
    fout = 0;
    lastTrigger = -1;
    breakLoop  = false;
    compress   = ProgramSettings::isCompressionEnabled();
    write2file = ProgramSettings::isDataWritingEnabled();
    if( sensorMap != 0 )
        sensorMap->clear();
}

void MWPCDataInterface::openFile(void)
{
    if( write2file )
    {
        QString fname(ProgramSettings::getOutputDirName());
        fname += "/mwpc_";
        fname += QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
        fname += ".dat";
        if( compress )
            fname += ".gz";
        fout = fopen(fname.toLocal8Bit().data(),"wb");
        if( fout != 0 )
            qDebug() << "Open Data file:" << fname;
        else
        {
            qDebug() << "Cannot create Data file:" << fname;
            qDebug() << "Data writing disable";
            write2file = false;
        }

    }
}

void MWPCDataInterface::closeFile(void)
{
    if( fout != 0 )
    {
        fclose(fout);
        fout = 0;
        qDebug() << "Close Data file";
    }
}
