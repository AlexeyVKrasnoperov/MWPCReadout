#include "sensormap.h"

SensorMap::SensorMap()
{
    cnt = 0;
    size_t nWires[10] = {192,192,192,192,640,448,576,320,640,320};
    for( size_t w = 0 ; w < 17; w++)
        for( int c = 0; c < 10; c++ )
            ChamberProfiles[c][w].resize(nWires[c],0);
}

SensorMap::~SensorMap()
{
    clear();
}

void SensorMap::clear()
{
    QMutexLocker locker(&mutex);
    cnt = 0;
    for(iterator i = begin(); i != end(); ++i)
        delete (*i).second;
    map::clear();
    for( size_t w = 0 ; w < 17; w++)
        for( int c = 0; c < 10; c++ )
            fill(ChamberProfiles[c][w].begin(),ChamberProfiles[c][w].end(),0);
}

int SensorMap::addData(uint address, uint port, timeval *tv, uchar *data, int dataSize, uint & trgNum)
{
    QMutexLocker locker(&mutex);
    iterator i = find(address);
    Sensor *sensor = 0;
    if( i != end() )
        sensor = (*i).second;
    else
    {
        sensor = new Sensor(address, port);
        insert(value_type(address,sensor));
    }
    cnt++;
    return sensor->addData(tv,data,dataSize,trgNum);
}

bool SensorMap::analizeData(void)
{
    if(empty())
        return false;
    int cnt = 0;
    if( mutex.tryLock() )
    {
        for(iterator i = begin(); i != end(); ++i)
        {
            Sensor *s = (*i).second;
            if( s->analizeData() )
                cnt++;
            for( uint w = 0; w < 17; w++ )
            {
                for( uint bid = 0; bid < s->getNBoard(); bid++ )
                {
                    for( uint ch = 0; ch < 32; ch++ )
                    {
                        ChamberWire cw;
                        int wireCnts = s->getChamberWireCounts(bid,ch,w,cw);
                        if( (cw.chamber < NChambers) && (cw.wire < ChamberProfiles[cw.chamber][w].size()))
                            ChamberProfiles[cw.chamber][w][cw.wire] = wireCnts;
                    }
                }
            }
        }
        mutex.unlock();
    }
    return (cnt != 0);
}

const vector<uint> * SensorMap::getHist(uint id, int w)
{
    Sensor *s = findSensor(id);
    if( s == 0 )
        return 0;
    uint board = (id & 0x00FF0000) >> 16;
    uint hid   = (id & 0x0000F000);
    if( board <= s->getNBoard() )
    {
        board -= 1;
        if( hid == 0x1000 )
            return s->getWindowCnt(board);
        if( hid == 0x2000 )
            return s->getWireCnt(board,w);
    }
    return s->getRateCnt();
}

const vector<uint> * SensorMap::getChamberProfile(uint id, int w)
{
    return ( (id < NChambers) && (w < 17) ) ? &ChamberProfiles[id][w] : 0;
}

