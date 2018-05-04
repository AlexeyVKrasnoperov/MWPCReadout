#ifndef DATASOCKET_H
#define DATASOCKET_H

#include "mwpcdatainterface.h"

class UDPSocketInterface: public MWPCDataInterface
{
    Q_OBJECT
protected:
    struct DataHeader
    {
        time_t      tv_sec;
        time_t      tv_usec;
        unsigned int  ip;
        unsigned int len;
        DataHeader(void)
        {
            tv_sec  = 0;
            tv_usec = 0;
            ip      = 0;
            len     = 0;
        }
        DataHeader(const timeval & tv, unsigned int _ip, unsigned int _len)
        {
            fill(tv,_ip,_len);
        }
        void fill(const timeval & tv, unsigned int _ip, unsigned int _len)
        {
            tv_sec = tv.tv_sec;
            tv_usec = tv.tv_usec;
            ip  = _ip;
            len = _len;
        }
    };
    const int port;
    virtual void run(void);
    int openSocket(void);
public:
    UDPSocketInterface(SensorMap *map, QObject *parent = 0);
    virtual ~UDPSocketInterface()
    {
        stop();
    }
};

#endif // DATASOCKET_H
