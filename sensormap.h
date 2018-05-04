#ifndef SENSORMAP_H
#define SENSORMAP_H

#include <QMutex>
#include "sensor.h"
#include <map>
using namespace std;

class SensorMap: public map<uint,Sensor*>
{
public:
    enum { NChambers = 10 };
protected:
    uint cnt;
    QMutex mutex;
    vector<uint> ChamberProfiles[10][17];
public:
    SensorMap();
    virtual ~SensorMap();
    int addData(uint address, uint port, timeval *tv, uchar *data, int dataSize, uint & trgNum);
    bool analizeData(void);
    virtual void clear();
    QMutex * getMutex()
    {
        return &mutex;
    }
    const vector<uint> * getHist(uint id, int w = 16);
    const vector<uint> * getChamberProfile(uint id, int w = 16);
    Sensor *findSensor(uint id)
    {
        uint mask = id & 0xFF000000;
        for(iterator i = begin(); i != end(); ++i)
        {
            if( ((*i).first & mask ) == mask )
            {
                return (*i).second;
                break;
            }
        }
        return 0;
    }
};

#endif // SENSORMAP_H
