#include "sensor.h"
#include <string.h>
#include <stdio.h>
#include <QUdpSocket>
#include <arpa/inet.h>
#include "programsettings.h"

const int Sensor::dataShift = sizeof(timeval) + sizeof(SensorHeader);

Sensor::Sensor(uint _address, uint _port):address(_address),port(_port),name(inet_ntoa(*((in_addr*)(&_address))))
{
    testMode = false;
    clear();
    rateCnt.resize(ProgramSettings::getRateHistorySize());
    setTestMode(false);
}

Sensor::~Sensor(void)
{
    clear();
}

void Sensor::clear(void)
{
    sendCnt   =  0;
    recvCnt   =  0;
    Ip        =  0;
    firstTrg  = -1;
    firstTime =  0;
    lastTimeBin = -1;
    fill(rateCnt.begin(),rateCnt.end(),0);
    fill(boardCnt.begin(),boardCnt.end(),0);
    fill(boardTrg.begin(),boardTrg.end(),0);
    for(size_t k = 0; k < 17; k++)
        for(size_t i = 0; i < wireCnt[k].size(); i++)
            fill(wireCnt[k][i].begin(),wireCnt[k][i].end(),0);
    for(size_t i = 0; i < windowCnt.size(); i++)
        fill(windowCnt[i].begin(),windowCnt[i].end(),0);
    //
    deque::clear();
}


int Sensor::addData(timeval *tv,uchar *data, int dataSize, uint & trgNum)
{
    //
    SensorHeader *h = (SensorHeader*)(data);
//    if( firstTrg == -1 )
//    {
//        firstTrg = h->trg_num;
//        sendCnt = 1;
//    }
//    else
//        sendCnt = h->trg_num - firstTrg + 1;
    sendCnt = h->trg_num;

    //
    if( Ip == 0 )
    {
        Ip = h->ip;
        for(size_t w = 0; w < 17; w++)
            wireCnt[w].resize(getNBoard(),vector<uint>(32,0));
        boardCnt.resize(getNBoard(),0);
        boardTrg.resize(getNBoard(),0);
        windowCnt.resize(getNBoard(),vector<uint>(15,0));
    }
    //
    trgNum = 0;
    Board *b = (Board*)(data + sizeof(SensorHeader));
    for( uint i = 0; i < getNBoard(); i++, b++ )
        trgNum = qMax(trgNum,b->trg_num);
    //
    int sz = sizeof(SensorHeader) + sizeof(Board)*getNBoard();
    if( dataSize > sz )
        sz = dataSize;
    //
    push_back(vector<uchar>(sz + sizeof(timeval)));
    uchar *ptr = back().data();
    memcpy(ptr,tv,sizeof(timeval));
    memcpy(ptr+sizeof(timeval),data,sz);
    if( firstTrg == -1 ) recvCnt = sendCnt;
    else recvCnt++;
    // recvCnt++;
    return sz;
}

bool Sensor::analizeData(void)
{
    if( empty() )
        return false;
    //
    uchar * data = front().data();
    timeval *tv  = (timeval*)data;
    if( firstTime == 0 )
        firstTime = tv->tv_sec;
    int idx = (tv->tv_sec-firstTime)%rateCnt.size();
    if( lastTimeBin > idx )
        fill(rateCnt.begin(),rateCnt.end(),0);
    rateCnt[idx]++;
    lastTimeBin = idx;
    //
    SensorHeader *h = (SensorHeader*)(data + sizeof(timeval));
    Board *b = (Board*)((char*)h + sizeof(SensorHeader));
    uint trg = 0;
    for( uint i = 0; i < getNBoard() ; i++ )
    {
        uint id0 = (b->id&0x0F) - 1;
        if( id0 != i )
            break;
        if( b->trg_num == 0 )
            break;
        if( trg == 0 )
            trg = b->trg_num;
        else if( trg != b->trg_num )
        {
            //qDebug() << name << hex << b->id << "Trigger number mismatch !!!" << trg << b->trg_num << "  " << dec << trg << b->trg_num;
        }
        //
        uint *cntAll = wireCnt[16].at(i).data();
        int fw =  h->winmask&0x0F;
        int lw = (h->winmask>>4)&0x0F;
        for( int j = fw; j <= lw; j++ )
        {
            uint *cnt    = wireCnt[j].at(i).data();
            for( int k = 0; k < 32; k++ )
            {
                if( (b->data[j] & (1<<k) ) != 0 )
                {
                    if((Ip == 136)&&(k == 24)&&(i == 4))
                        continue;
                    else if((Ip == 138)&&(k == 27)&&(i == 5))
                        continue;
                    else if((Ip == 150)&&(k == 6)&&(i == 2))
                        continue;
                    else if((Ip == 131)&&(k == 28)&&(i == 4))
                        continue;
                    else if((Ip == 133)&&(k == 29)&&(i == 4))
                        continue;
                    else if(((Ip == 129)||(Ip == 132))&&(k == 31)&&(i == 4))
                        continue;
                    else
                    {
                        *(cnt+k) += 1;
                        *(cntAll+k) += 1;
                        boardCnt[i]++;
                        windowCnt[i][j]++;
                    }
                }
            }
        }
        //
        boardTrg[i] = trg;
        b++;
    }
    //
    pop_front();
    //
    return true;
}

void Sensor::writeDatagram(const char w)
{
    QUdpSocket socket;
    socket.writeDatagram(&w,1,QHostAddress(name),port);
    //    qDebug() << "Write" << hex << w << "to" << name << "port" << port;
}

void Sensor::setTestMode(bool mode)
{
    testMode = mode;
    writeDatagram(testMode ? 1 : 0);
}
