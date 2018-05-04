#ifndef MWPCDATAINTERFACE_H
#define MWPCDATAINTERFACE_H

#include <QThread>
#include "sensormap.h"

class MWPCDataInterface : public QThread
{
    Q_OBJECT
protected:
    FILE *fout;
    long lastTrigger;
    bool compress;
    bool write2file;
    bool breakLoop;
    void openFile(void);
    void closeFile(void);
    void stop(void)
    {
        if( isRunning() )
        {
            breakLoop = true;
            wait();
        }
    }
public:
    MWPCDataInterface(SensorMap *map, QObject *parent);
    virtual ~MWPCDataInterface(){}
    void setLastTrigger(long t)
    {
        lastTrigger = t;
    }
protected:
    SensorMap *sensorMap;
};

extern const int pcap_shift;

#endif // MWPCDATAINTERFACE_H
