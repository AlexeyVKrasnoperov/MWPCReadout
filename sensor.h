#ifndef SENSOR_H
#define SENSOR_H

#include <QTypeInfo>
#include <QString>
#include <vector>
#include <deque>
#include <map>
using namespace std;

struct SensorHeader
{
    char title[8];
    uchar ip;
    uchar winmask;
    uchar cardmask;
    u_int32_t trg_num;
} __attribute__((packed));


struct Board
{
    uchar id;
    u_int32_t trg_num;
    u_int32_t data[16];
} __attribute__((packed));

struct ChamberWire
{
    uint chamber;
    uint wire;
};


class Sensor: public deque<vector<uchar> >
{
protected:
    int lastTimeBin;
    vector<uint> rateCnt;
    vector<vector<uint> > wireCnt[17];
    vector<vector<uint> > windowCnt;
    vector<uint> boardCnt;
    vector<uint> boardTrg;
    const uint address;
    uchar Ip;
    const uint   port;
    static const int dataShift;
    const QString name;
    int firstTrg;
    int sendCnt;
    int recvCnt;
    time_t firstTime;
    bool testMode;
    void writeDatagram(const char w);
public:
    Sensor(uint _address, uint _port);
    ~Sensor(void);
    inline uint getNBoard(void)
    {
        if( (Ip == 129) || (Ip == 132) )
            return 7;
        if( (Ip == 150) || (Ip == 146) )
            return 4;
        if( (Ip == 135) || (Ip == 136) || (Ip == 137) || (Ip == 138))
            return 6;
        return 5;
    }
    //
    inline bool getChamberWire(uint bid, uint k, ChamberWire & cw)
    {
        //*********** Chamber 1
        if( Ip == 138 )
          {
            cw.chamber = 0;
            if( bid < 3 )
              cw.wire = 2*(32*bid + (31-k));
            else
              cw.wire = 2*(32*(5-bid) + (31-k)) + 1;
            return true;
          }
        //*********** Chamber 2
        if( Ip == 137 )
          {
            cw.chamber = 1;
            if( bid < 3 )
              cw.wire = 2*(32*bid + k) + 1;
            else
              cw.wire = 2*(32*(5-bid) + k);
            return true;
          }
        //*********** Chamber 3  // New mapping
        if( Ip == 136 )
          {
            cw.chamber = 2;
            //            if( bid < 3 )
            //              cw.wire = 2*(32*(2-bid) + (31-k));
            //            else
            //              cw.wire = 2*(32*(bid-3) + (31-k)) + 1;
            if( bid < 3 )
                cw.wire = 2*(32*bid + (31-k));
            else
                cw.wire = 2*(32*(5-bid) + (31-k)) + 1;
            return true;
          }
        //*********** Chamber 4  // New mapping
        if( Ip == 135 )
          {
            cw.chamber = 3;
            //            if( bid < 3 )
            //              cw.wire = 2*(32*(2-bid) + (31-k));
            //            else
            //              cw.wire = 2*(32*(bid-3) + (31-k)) + 1;
            if( bid < 3 )
              cw.wire = 2*(32*bid + k) + 1;
            else
              cw.wire = 2*(32*(5-bid) + k);

            return true;
          }
        //*********** Chamber 5
        if( Ip == 130 )
          {
            cw.chamber = 4;
            cw.wire = 2*(32*bid + (31-k));
            return true;
          }
        else if( Ip == 131 )
          {
            cw.chamber = 4;
            cw.wire = 2*(32*(bid+5) + (31-k));
            return true;
          }
        else if( Ip == 133 )
          {
            cw.chamber = 4;
            cw.wire = 2*(32*(bid+5) + (31-k)) + 1;
            return true;
          }
        else if( Ip == 134 )
          {
            cw.chamber = 4;
            cw.wire = 2*(32*bid + (31-k)) + 1;
            return true;
          }
        //*********** Chamber 6
        if( Ip == 129 )
          {
            cw.chamber = 5;
            cw.wire = 2*(32*bid + k);
            return true;
          }
        else if( Ip == 132 )
          {
            cw.chamber = 5;
            cw.wire = 2*(32*bid + k)+1;
            return true;
          }
        //
        //*********** Chamber 7
        if( Ip == 146 )
        {
            cw.chamber = 6;
            cw.wire = 2*(32*bid + (31-k));
            return true;
        }
        else if( Ip == 147 )
        {
            cw.chamber = 6;
            cw.wire = 2*(32*(bid+4) + (31-k));
            return true;
        }
        else if( Ip == 149 )
        {
            cw.chamber = 6;
            cw.wire = 2*(32*(bid+4) + (31-k)) + 1;
            return true;
        }
        else if( Ip == 150 )
        {
            cw.chamber = 6;
            cw.wire = 2*(32*bid + (31-k)) + 1;
            return true;
        }
        //*********** Chamber 8
        if( Ip == 145 )
        {
            cw.chamber = 7;
            cw.wire = 2*(32*bid + (31-k));
            return true;
        }
        else if( Ip == 148 )
        {
            cw.chamber = 7;
            cw.wire = 2*(32*bid + (31-k))+1;
            return true;
        }
        //*********** Chamber 9  // New mapping
        if( Ip == 152 )
        {
            cw.chamber = 8;
            cw.wire = 2*(32*bid + (31-k));
            return true;
        }
        else if( Ip == 153 )
        {
            cw.chamber = 8;
            cw.wire = 2*(32*(bid+5) + (31-k));
            return true;
        }
        else if( Ip == 155 )
        {
            cw.chamber = 8;
            cw.wire = 2*(32*(bid+5) + (31-k)) + 1;
            return true;
        }
        else if( Ip == 156 )
        {
            cw.chamber = 8;
            cw.wire = 2*(32*bid + (31-k)) + 1;
            return true;
        }
        //*********** Chamber 10  // New mapping
        if( Ip == 151 )
        {
            cw.chamber = 9;
            cw.wire = 2*(32*bid + (31-k));
            return true;
        }
        else if( Ip == 154 )
        {
            cw.chamber = 9;
            cw.wire = 2*(32*bid + (31-k))+1;
            return true;
        }
        //
        cw.chamber = -1;
        cw.wire = -1;
        //
        return false;
    }
    //
    uint getChamberWireCounts(uint bid, uint k, uint w, ChamberWire & cw)
    {
        if( ! getChamberWire(bid,k,cw) )
            return -1;
        if( (w < 17) && (bid < wireCnt[w].size()) && (k < wireCnt[w][bid].size()))
            return wireCnt[w][bid][k];
        return -1;
    }
    //
    void clear();
    int addData(timeval *tv, uchar *data, int dataSize, uint & trgNum);
    bool analizeData(void);
    int getSendCnt(void)
    {
        return sendCnt;
    }
    int getRecvCnt(void)
    {
        return recvCnt;
    }
    const QString & getName(void) const
    {
        return name;
    }
    uint getAddress(void)
    {
        return address;
    }
    const vector<uint> * getRateCnt(void)
    {
        return &rateCnt;
    }
    uint getBoardCnt(int i)
    {
        return (i < int(boardCnt.size())) ? boardCnt[i] : 0;
    }
    uint getBoardTrg(int i)
    {
        return (i < int(boardTrg.size())) ? boardTrg[i] : 0;
    }
    uchar getIp(void)
    {
        return Ip;
    }
    const vector<uint> * getWindowCnt(int b)
    {
        return (b < int(windowCnt.size())) ? &windowCnt.at(b) : 0;
    }
    const vector<uint> * getWireCnt(int b, int w = 16)
    {
        return (b < int(wireCnt[w].size())) ? &wireCnt[w].at(b) : 0;
    }
    long getBranchTrigger(void)
    {
        long t = -1;
        for(uint i = 0; i < getNBoard(); i++)
        {
            if( t == -1 )
            {
                t = getBoardTrg(i);
                continue;
            }
            if( t != getBoardTrg(i) )
                return -1;
        }
        return t;
    }
    bool getTestMode(void)
    {
        return testMode;
    }
    void setTestMode(bool mode);
};

#endif // SENSOR_H
