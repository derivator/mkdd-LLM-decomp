#ifndef LANPLAYINFO_H
#define LANPLAYINFO_H

#include "JSystem/JMath/JMath.h"
#include "Osako/LANSelectMode.h"
#include "Osako/ResMgr.h"
#include "kartEnums.h"
#include "types.h"

class LANPlayInfo {
public:
    void init();                       // 0x80218580
    ECourseID getCrsID();              // 0x8021865c
    ResMgr::CourseOrder getCrsOrder(); // 0x80218904
    void resumeInfo(LANSelectMode *);  // 0x8021890c
    void saveInfo(LANSelectMode *);    // 0x802189a4
    u8 getRandomMap(int, u32);         // 0x80218a48
    void shuffleKart(int);             // 0x80218ab4
    u8 getMaskedRandom(int, u32);      // 0x80218be4
    EKartID getRideKart(u8);           // 0x80218d10
    ECharID getFront(int);             // 0x80218ec4
    ECharID getBack(int);              // 0x80218ee4
    int getDisplayNum();               // 0x80218f04
    // Inline/Unused
    LANPlayInfo() : mRnd(0) {}

private:
    u8 mMode;      // 0x00
    u8 mPower;     // 0x01
    u8 mCrsMode;   // 0x02
    u8 mStage;     // 0x03
    u8 mLap;       // 0x04
    u8 mLapSel;    // 0x05
    u8 mTag;       // 0x06
    u8 mDispIdx;   // 0x07
    u8 mValueMode; // 0x08
    u8 mCrsIdx;    // 0x09
    u8 _a[2];      // 0x0a
    u32 mMapID;    // 0x0c
    u32 mMask;     // 0x10
    u32 mMapMask;  // 0x14
    JMARandFast mRnd; // 0x18
    struct {
        u8 kart;   // +0
        u8 front;  // +1
        u8 back;   // +2
        u8 pos;    // +3
    } mKarts[8];   // 0x1c
    u8 mOrder[8];  // 0x3c
};
extern LANPlayInfo gLANPlayInfo;

#endif // LANPLAYINFO_H
