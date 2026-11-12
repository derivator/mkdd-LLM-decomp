#include "Osako/LANPlayInfo.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/KartInfo.h"
#include "Osako/NetGameMgr.h"
#include "Osako/SystemRecord.h"
#include "kartEnums.h"
LANPlayInfo gLANPlayInfo;
static const ECourseID caRaceCrsID[16] = {
    LUIGI_CIRCUIT, PEACH_BEACH, BABY_PARK, DRY_DRY_DESERT,
    MUSHROOM_BRIDGE, MARIO_CIRCUIT, DAISY_CRUISER, WALUIGI_STADIUM,
    SHERBET_LAND, MUSHROOM_CITY, YOSHI_CIRCUIT, DK_MOUNTAIN,
    WARIO_COLOSSEUM, DINO_DINO_JUNGLE, BOWSER_CASTLE, RAINBOW_ROAD
};

static const ECourseID caBattleCrsID[6] = {
    MINI_7, MINI_3, MINI_2, MINI_8, MINI_1, MINI_5
};

static const ECharID caCharID[18] = {
    BABY_MARIO, BABY_LUIGI, PARATROOPA, KOOPATROOPA, PEACH, DAISY,
    MARIO, LUIGI, WARIO, WALUIGI, YOSHI, BIRDO, DONKEY_KONG, DIDDY_KONG,
    BOWSER, BOWSER_JR, TOAD, TOADETTE
};

void LANPlayInfo::init() {
    NetGameMgr *mgr = NetGameMgr::ptr();
    mMode = 0;
    mPower = 0;
    mCrsMode = 0;
    mStage = 0;
    mLap = 0;
    mLapSel = 0;
    mTag = 0;
    mDispIdx = 0;
    mValueMode = 0;
    mCrsIdx = 0;
    mMapID = 0;
    mMask = 0;
    if (!(mgr->get12ec() & 0x80)) {
        mMask |= 0x10;
    }
    if (!(mgr->get12ec() & 0x100)) {
        mMask |= 0x20;
    }
    mMapMask = 0;
    if (!(mgr->get12ec() & 0x8)) {
        mMapMask |= 0x1000;
        mMapMask |= 0x2000;
        mMapMask |= 0x4000;
        mMapMask |= 0x8000;
    }
    for (int i = 0; i < 8; i++) {
        mOrder[i] = 1;
    }
    mRnd.setSeed(NetGameMgr::ptr()->get12e9());
}

ECourseID LANPlayInfo::getCrsID() {
    switch ((int)mMode) {
    case 0:
        switch (mCrsMode) {
        case 0:
            return caRaceCrsID[getRandomMap(16, mMapMask)];
        case 1:
            return caRaceCrsID[mStage];
        case 2:
            if (mCrsIdx >= 16) {
                mCrsIdx = 0;
            }
            if (!(NetGameMgr::ptr()->get12ec() & 0x8) && mCrsIdx >= 12) {
                mCrsIdx = 0;
            }
            return caRaceCrsID[mCrsIdx++];
        case 3:
            if (mCrsIdx >= 4) {
                mCrsIdx = 0;
            }
            return caRaceCrsID[mCrsIdx++];
        case 4:
            if (mCrsIdx >= 4) {
                mCrsIdx = 0;
            }
            return caRaceCrsID[mCrsIdx++ + 4];
        case 5:
            if (mCrsIdx >= 4) {
                mCrsIdx = 0;
            }
            return caRaceCrsID[mCrsIdx++ + 8];
        case 6:
            if (mCrsIdx >= 4) {
                mCrsIdx = 0;
            }
            return caRaceCrsID[mCrsIdx++ + 12];
        default:
            goto invalid;
        }
    case 1:
    case 2:
        switch (mCrsMode) {
        case 0:
            return caBattleCrsID[getRandomMap(6, mMask)];
        case 1:
            return caBattleCrsID[mStage];
        case 2:
            if (!(NetGameMgr::ptr()->get12ec() & 0x80) && mCrsIdx == 4) {
                mCrsIdx++;
            }
            if (!(NetGameMgr::ptr()->get12ec() & 0x100) && mCrsIdx == 5) {
                mCrsIdx++;
            }
            if (mCrsIdx >= 6) {
                mCrsIdx = 0;
            }
            return caBattleCrsID[mCrsIdx++];
        default:
            goto invalid;
        }
    default:
    invalid:
        return CRS_ID_INV;
    }
}
ResMgr::CourseOrder LANPlayInfo::getCrsOrder() {
    return ResMgr::mcOrder_1;
}

void LANPlayInfo::resumeInfo(LANSelectMode *mode) {
    mode->getModeItem()->setIdx(mMode);
    mode->getPowerItem()->setIdx(mPower);
    mode->getLapItem()->setIdx(mLapSel);
    mode->getOrderItem()->setIdx(mCrsMode);
    mode->getTagItem()->setIdx(mTag);
    mode->getDisplayItem()->setIdx(mDispIdx);
    mode->getCourseSelectItem()->changeValueMode((LANSelectMode::SelectableItemSw::ValueMode)mValueMode);
    mode->getCourseSelectItem()->setIdx(mStage);
    mode->getCourseSelectItem()->setIdxSw(mLap);
}

void LANPlayInfo::saveInfo(LANSelectMode *mode) {
    mMode = mode->getModeItem()->getIdx();
    mPower = mode->getPowerItem()->getIdx();
    mLapSel = mode->getLapItem()->getIdx();
    mCrsMode = mode->getOrderItem()->getIdx();
    mTag = mode->getTagItem()->getIdx();
    mDispIdx = mode->getDisplayItem()->getIdx();
    mValueMode = mode->getCourseSelectItem()->getValueMode();
    mStage = mode->getCourseSelectItem()->getIdx();
    mLap = mode->getCourseSelectItem()->getIdxSw();
    mCrsIdx = 0;
    mMapID = 0xFFFF;
    for (int i = 0; i < 8; i++) {
        mOrder[i] = 1;
    }
}

u8 LANPlayInfo::getRandomMap(int max, u32 map) {
    if ((((mMapID) & (( 1 << max ) - 1) ) == (( 1 << max ) - 1))) {
        mMapID = map;
    }
    u8 ret = getMaskedRandom(max, mMapID);
    mMapID |= (1 << ret);
    return ret;
}

void LANPlayInfo::shuffleKart(int playerCount) {
    u32 mask18 = 0;
    u32 maskPlayer = 0;
    if (!(NetGameMgr::ptr()->get12ec() & 1)) {
        mask18 |= 0x30000;
    }
    for (int i = 0; i < playerCount; i++) {
        u8 front = getMaskedRandom(18, mask18);
        mKarts[i].front = front;
        mask18 |= 1 << front;
        u8 back = getMaskedRandom(18, mask18);
        mKarts[i].back = back;
        mask18 |= 1 << back;
        u8 frontWeight = KartInfo::getDriverWeight(caCharID[mKarts[i].front]);
        u8 backWeight = KartInfo::getDriverWeight(caCharID[mKarts[i].back]);
        if (frontWeight >= backWeight) {
            mKarts[i].kart = getRideKart(mKarts[i].front);
        } else {
            mKarts[i].kart = getRideKart(mKarts[i].back);
        }
        u8 pos = getMaskedRandom(playerCount, maskPlayer);
        mKarts[i].pos = pos;
        maskPlayer |= 1 << pos;
    }
}

u8 LANPlayInfo::getMaskedRandom(int max, u32 mask) {




































































































































    JUT_ASSERT(!(((mask) & (( 1 << max ) - 1) ) == (( 1 << max ) - 1)));

    u8 ret = 0;
    const int n = max * mRnd.get_ufloat_1() + 1;

    for (int i = 0; i < n; i++) {
        do {
            if (++ret >= max) {
                ret = 0;
            }
        } while (mask & (1 << ret));
    }
    return ret;
}

EKartID LANPlayInfo::getRideKart(u8 idx) {
    switch (idx) {
    case 1:
        if (SystemRecord::tstSecretKart(RATTLE_BUGGY, NetGameMgr::ptr()->getSecretKartFlags())) {
            return RATTLE_BUGGY;
        }
        goto k5;
    case 0:
    k5:
        return GOO_GOO_BUGGY;
    case 2:
        if (SystemRecord::tstSecretKart(PARA_WING, NetGameMgr::ptr()->getSecretKartFlags())) {
            return PARA_WING;
        }
        goto k3;
    case 3:
    k3:
        return KOOPA_DASHER;
    case 5:
        if (SystemRecord::tstSecretKart(BLOOM_COACH, NetGameMgr::ptr()->getSecretKartFlags())) {
            return BLOOM_COACH;
        }
        goto k4;
    case 4:
    k4:
        return HEART_COACH;
    case 9:
        if (SystemRecord::tstSecretKart(WALUIGI_RACER, NetGameMgr::ptr()->getSecretKartFlags())) {
            return WALUIGI_RACER;
        }
        goto k7;
    case 7:
    k7:
        if (SystemRecord::tstSecretKart(GREEN_FIRE, NetGameMgr::ptr()->getSecretKartFlags())) {
            return GREEN_FIRE;
        }
        goto k0;
    case 6:
    k0:
        return RED_FIRE;
    case 8:
        return WARIO_CAR;
    case 11:
        if (SystemRecord::tstSecretKart(TURBO_BIRDO, NetGameMgr::ptr()->getSecretKartFlags())) {
            return TURBO_BIRDO;
        }
        goto k2;
    case 10:
    k2:
        return TURBO_YOSHI;
    case 12:
        return DK_JUMBO;
    case 13:
        if (SystemRecord::tstSecretKart(BARREL_TRAIN, NetGameMgr::ptr()->getSecretKartFlags())) {
            return BARREL_TRAIN;
        }
        return GOO_GOO_BUGGY;
    case 14:
        return KOOPA_KING;
    case 15:
        if (SystemRecord::tstSecretKart(BULLET_BLASTER, NetGameMgr::ptr()->getSecretKartFlags())) {
            return BULLET_BLASTER;
        }
        return KOOPA_DASHER;
    case 17:
        if (SystemRecord::tstSecretKart(TOADETTE_KART, NetGameMgr::ptr()->getSecretKartFlags())) {
            return TOADETTE_KART;
        }
        goto k16;
    case 16:
    k16:
        return TOAD_KART;
    default:
        return RED_FIRE;
    }
}

ECharID LANPlayInfo::getFront(int i) {
    return caCharID[mKarts[i].front];
}

ECharID LANPlayInfo::getBack(int i) {
    return caCharID[mKarts[i].back];
}

int LANPlayInfo::getDisplayNum() {
    switch (mDispIdx) {
    case 0:
        return 1;
    case 1:
        return 2;
    case 2:
        return 4;
    default:
        return 0;
    }
}

#include "JSystem/JAudio/JASFakeMatch2.h"
