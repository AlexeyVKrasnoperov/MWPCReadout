#ifndef PCAPINTERFACE_H
#define PCAPINTERFACE_H

#include "mwpcdatainterface.h"
#include <QThread>
#include <pcap.h>
#include <vector>
using namespace std;

class PCapInterface : public MWPCDataInterface
{
    Q_OBJECT
protected:
    QString devName;
    virtual void run(void);
    pcap_t * openInterface(const char *dev);
    void closeInterface(pcap_t *handle);
    bool getNextPacket(pcap_t *handle);
public:
    explicit PCapInterface(const char *dev, SensorMap *map, QObject *parent = 0);
    virtual ~PCapInterface()
    {
        stop();
    }
};

#endif // PCAPINTERFACE_H
