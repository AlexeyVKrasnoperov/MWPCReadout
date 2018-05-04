#include "dataanalizer.h"
#include "sensormap.h"

DataAnalizer::DataAnalizer(SensorMap *s, QObject *parent) :
    QThread(parent), sensorMap(s)
{
    breakLoop = false;
    start();
}

DataAnalizer::~DataAnalizer()
{
    if( isRunning() )
    {
        breakLoop = true;
        wait();
    }

}

void DataAnalizer::run(void)
{
    breakLoop = false;
    while( ! breakLoop )
    {
        if( ! sensorMap->analizeData() )
            usleep(1);
    }
}

