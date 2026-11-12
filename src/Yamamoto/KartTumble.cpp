#include "Yamamoto/KartTumble.h"

#include "Yamamoto/kartCtrl.h"

#include "Sato/ItemObjMgr.h"
#include "Sato/JPEffectPerformer.h"

#include "Shiraiwa/TKartThrower.h"

#include "JSystem/JAudio/JASFakeMatch2.h"

// The single-case CheckCrash() switches are deliberate. MWCC lowers them to
// `cmp; beq case; b default`, while an equivalent if/else drops the
// unconditional branch and no longer matches the target.

// comments inside functions are inline functions being called in that function

void KartTumble::Init(int index) {
    mBody = GetKartCtrl()->getKartBody(index);
    mFlags = 0;
    mUnk5 = 0;
    mTimer = 0;
    mDir.zero();
}

void KartTumble::MakeWanWanTumble(ItemObj *itemObj) {
    KartBody* body = mBody;
    ItemObjMgr* mgr = GetItemObjMgr();
    ItemObjMgr::KartHitList* hitList = mgr->getKartHitList(body->mMynum);
    if (mFlags & 1) return;
    switch (body->getChecker()->CheckCrash()) {
        case 0:
            {
                JGeometry::TVec3f force;
                JGeometry::TVec3f dir;
                JGeometry::TVec3f itemPos;
                body->_584 = 4;
                body->_588 = 0;
                mUnk5 = 0;
                hitList->mObjects[9]->getPos(&itemPos);
                body->getGame()->ItemWatchMan(itemObj);
                itemPos.y = body->mPos.y;
                force.sub(itemPos, body->mPos);
                force.length();
                body->_308.angle(force);
                dir.set(force);
                dir.scale(-1.0f);
                GetKartCtrl()->DevMatrixByVector(&mDir, &dir, body->_110);
                mDir.y = 0.0f;
                mDir.normalize();
                body->_594 = 0;
                body->_4b4 = 0.0f;
                body->_4bc = 0.680333f;
                body->getCrash()->NonRescue();
                body->mCarStatus |= 0x00102000;
                body->getCrash()->SaveDir();
                body->getGame()->MakeClear();
                body->getDamage()->SetBigDamageAnime();
                body->getItem()->FallItem();
                body->mVel.zero();
                body->_2cc.zero();
                body->mWg.zero();
                dir.normalize();
                force.set(dir);
                force.y = 0.0f;
                force.scale(80.0f);
                force.scale(body->_3a4);
                body->DoForce(&body->mPos, &force);
                mFlags |= 1;
                mTimer = 60;
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashStartSound();
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashVoice();
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, body->mMynum, body->mPos, true);
                body->getStrat()->DoMotor(MotorManager::MotorType_11);
                body->getCrash()->SetMatchlessTimer();
            }
            break;
        default:
            return;
    }
}

void KartTumble::MakeKameTumble(ItemObj *itemObj) {
    KartBody* body = mBody;
    ItemObjMgr* mgr = GetItemObjMgr();
    ItemObjMgr::KartHitList* hitList = mgr->getKartHitList(body->mMynum);
    if (mFlags & 1) return;
    switch (body->getChecker()->CheckCrash()) {
        case 0:
            {
                JGeometry::TVec3f force;
                JGeometry::TVec3f dir;
                JGeometry::TVec3f itemPos;
                body->_584 = 4;
                body->_588 = 0;
                mUnk5 = 0;
                hitList->mObjects[11]->getPos(&itemPos);
                itemPos.y = body->mPos.y;
                force.sub(itemPos, body->mPos);
                force.length();
                body->_308.angle(force);
                dir.set(force);
                dir.scale(-1.0f);
                GetKartCtrl()->DevMatrixByVector(&mDir, &dir, body->_110);
                mDir.y = 0.0f;
                mDir.normalize();
                body->_594 = 0;
                body->_4b4 = 0.0f;
                body->_4bc = 0.680333f;
                body->mCarStatus |= 0x00102000;
                body->getCrash()->SaveDir();
                body->getGame()->MakeClear();
                body->getDamage()->SetBigDamageAnime();
                body->getItem()->FallItem();
                body->mVel.zero();
                body->_2cc.zero();
                body->mWg.zero();
                dir.normalize();
                force.set(dir);
                force.y = 0.0f;
                force.scale(80.0f);
                force.scale(body->_3a4);
                body->DoForce(&body->mPos, &force);
                mFlags |= 1;
                mTimer = 60;
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashStartSound();
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, body->mMynum, body->mPos, true);
                body->getStrat()->DoMotor(MotorManager::MotorType_11);
                body->getGame()->ItemWatchMan(itemObj);
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashVoice();
                body->getCrash()->SetMatchlessTimer();
            }
            break;
        default:
            return;
    }
}

void KartTumble::MakeStarTumble() {
    KartBody* body = mBody;
    if (mFlags & 1) return;
    switch (body->getChecker()->CheckCrash()) {
        case 0:
            {
                JGeometry::TVec3f force;
                JGeometry::TVec3f dir;
                JGeometry::TVec3f otherKartPos;
                body->_584 = 4;
                body->_588 = 0;
                mUnk5 = 0;
                otherKartPos.set(((KartBody*)body->mUnkSub10c)->mPos);
                otherKartPos.y = body->mPos.y;
                force.sub(otherKartPos, body->mPos);
                dir.set(force);
                dir.scale(-1.0f);
                GetKartCtrl()->DevMatrixByVector(&mDir, &dir, body->_110);
                mDir.y = 0.0f;
                mDir.normalize();
                body->_594 = 0;
                body->_4b4 = 0.0f;
                body->_4bc = 0.680333f;
                body->getCrash()->NonRescue();
                body->mCarStatus |= 0x00102000;
                body->getCrash()->SaveDir();
                body->getGame()->MakeClear();
                body->getDamage()->SetBigDamageAnime();
                body->getItem()->ReleseWanWan();
                body->mVel.zero();
                body->_2cc.zero();
                body->mWg.zero();
                dir.normalize();
                force.set(dir);
                force.y = 0.0f;
                force.scale(80.0f);
                force.scale(body->_3a4);
                body->DoForce(&body->mPos, &force);
                mFlags |= 1;
                mTimer = 60;
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashStartSound();
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashVoice();
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, body->mMynum, body->mPos, true);
                body->getStrat()->DoMotor(MotorManager::MotorType_11);
                body->getCrash()->SetMatchlessTimer();
            }
            break;
        default:
            return;
    }
}

void KartTumble::MakeDashTumble() {
    KartBody* body = mBody;
    JGeometry::TVec3f force;
    JGeometry::TVec3f angleDiff;
    JGeometry::TVec3f otherPrevPos;
    otherPrevPos.set(((KartBody*)body->mUnkSub10c)->mPrevPos);
    force.sub(otherPrevPos, body->mPrevPos);
    angleDiff.sub(body->_308, ((KartBody*)body->mUnkSub10c)->_308);
    f32 angle = angleDiff.angle(force);
    if (!((180.0f * angle) / 3.141f < 90.0f)) {
        force.normalize();
        force.y = 0.0f;
        force.scale(500.0f);
        force.scale(body->_3a4);
        body->DoForce(&body->mPos, &force);
    }
}

void KartTumble::DoTumble() {
    KartBody* body = mBody;
    GeographyObj* obj = GetGeoObjMgr()->getKartReactHitObjectList(body->mMynum)[11];
    if (mFlags & 1) return;
    if (obj == 0) return;
    switch (body->getChecker()->CheckCrash()) {
        case 0:
            {
                JGeometry::TVec3f force;
                JGeometry::TVec3f dir;
                JGeometry::TVec3f hitNormal;
                JGeometry::TVec3f objPos;
                body->_584 = 4;
                body->_588 = 0;
                mUnk5 = 0;
                obj->getPosition(&objPos);
                obj->getColScaleRadius();
                GetGeoObjMgr()->getKartHitDepthNormalObj(body->mMynum);
                hitNormal.set(*GetGeoObjMgr()->getKartHitRefVecNormalObj(body->mMynum));
                obj->getVelocity(&dir);
                dir.y = 0.0f;
                objPos.y = body->mPos.y;
                force.sub(objPos, body->mPos);
                dir.set(force);
                dir.scale(-1.0f);
                GetKartCtrl()->DevMatrixByVector(&mDir, &dir, body->_110);
                mDir.y = 0.0f;
                mDir.normalize();
                body->_594 = 0;
                body->_4b4 = 0.0f;
                body->_4bc = 0.680333f;
                body->mCarStatus |= 0x00102000;
                body->getCrash()->SaveDir();
                body->getGame()->MakeClear();
                body->getDamage()->SetBigDamageAnime();
                body->getItem()->FallItem();
                body->ObjectReflection(&hitNormal);
                body->mVel.zero();
                body->_2cc.zero();
                body->mWg.zero();
                dir.normalize();
                force.set(dir);
                force.y = 0.0f;
                force.scale(80.0f);
                force.scale(body->_3a4);
                body->DoForce(&body->mPos, &force);
                mFlags |= 1;
                mTimer = 300;
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashStartSound();
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashVoice();
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, body->mMynum, body->mPos, true);
                body->getStrat()->DoMotor(MotorManager::MotorType_11);
                body->getCrash()->SetMatchlessTimer();
            }
            break;
        default:
            return;
    }
}

void KartTumble::DoPakunTumble() {
    KartBody* body = mBody;
    GeographyObj* obj = GetGeoObjMgr()->getKartReactHitObjectList(body->mMynum)[18];
    if (mFlags & 1) return;
    if (obj == 0) return;
    switch (body->getChecker()->CheckCrash()) {
        case 0:
            {
                JGeometry::TVec3f throwDir;
                JGeometry::TVec3f dir;
                JGeometry::TVec3f hitNormal;
                JGeometry::TVec3f objPos;
                body->_584 = 4;
                body->_588 = 0;
                mUnk5 = 0;
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown6, body->mMynum, body->mPos, false);
                obj->getPosition(&objPos);
                obj->getColScaleRadius();
                GetGeoObjMgr()->getKartHitDepthNormalObj(body->mMynum);
                hitNormal.set(*GetGeoObjMgr()->getKartHitRefVecNormalObj(body->mMynum));
                obj->getKartThrowDirPow(&throwDir, 0, body->mMynum);
                dir.set(throwDir);
                GetKartCtrl()->DevMatrixByVector(&mDir, &dir, body->_110);
                mDir.y = 0.0f;
                dir.normalize();
                body->_594 = 0;
                body->_4b4 = 0.0f;
                body->_4bc = 0.680333f;
                body->mCarStatus |= 0x00102000;
                body->getCrash()->SaveDir();
                body->getGame()->MakeClear();
                body->getDamage()->SetBigDamageAnime();
                body->getItem()->FallItem();
                body->mVel.zero();
                body->_2cc.zero();
                body->mWg.zero();
                throwDir.y = 0.0f;
                throwDir.normalize();
                throwDir.scale(body->_3a4);
                body->DoForce(&body->mPos, &throwDir);
                mFlags |= 1;
                mTimer = 120;
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashStartSound();
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashVoice();
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, body->mMynum, body->mPos, true);
                body->getStrat()->DoMotor(MotorManager::MotorType_11);
                body->getCrash()->SetMatchlessTimer();
            }
            break;
        default:
            return;
    }
}

void KartTumble::DoHanaTumble() {
    KartBody* body = mBody;
    GeographyObj* obj = GetGeoObjMgr()->getKartReactHitObjectList(body->mMynum)[15];
    if (mFlags & 1) return;
    if (obj == 0) return;
    switch (body->getChecker()->CheckCrash()) {
        case 0:
            {
                JGeometry::TVec3f force;
                JGeometry::TVec3f dir;
                JGeometry::TVec3f hitNormal;
                JGeometry::TVec3f objPos;
                body->_584 = 4;
                body->_588 = 0;
                mUnk5 = 0;
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown6, body->mMynum, body->mPos, false);
                obj->getPosition(&objPos);
                obj->getColScaleRadius();
                GetGeoObjMgr()->getKartHitDepthNormalObj(body->mMynum);
                hitNormal.set(*GetGeoObjMgr()->getKartHitRefVecNormalObj(body->mMynum));
                obj->getVelocity(&dir);
                dir.y = 0.0f;
                objPos.y = body->mPos.y;
                force.sub(objPos, body->mPos);
                dir.set(force);
                dir.scale(-1.0f);
                GetKartCtrl()->DevMatrixByVector(&mDir, &dir, body->_110);
                mDir.y = 0.0f;
                mDir.normalize();
                body->_594 = 0;
                body->_4b4 = 0.0f;
                body->_4bc = 0.680333f;
                body->mCarStatus |= 0x00102000;
                body->getCrash()->SaveDir();
                body->getGame()->MakeClear();
                body->getDamage()->SetBigDamageAnime();
                body->getItem()->FallItem();
                body->ObjectReflection(&hitNormal);
                body->mVel.zero();
                body->_2cc.zero();
                body->mWg.zero();
                dir.normalize();
                force.set(dir);
                force.y = 0.0f;
                force.scale(80.0f);
                force.scale(body->_3a4);
                body->DoForce(&body->mPos, &force);
                mFlags |= 1;
                mTimer = 120;
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashStartSound();
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashVoice();
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, body->mMynum, body->mPos, true);
                body->getStrat()->DoMotor(MotorManager::MotorType_11);
                body->getCrash()->SetMatchlessTimer();
            }
            break;
        default:
            return;
    }
}

void KartTumble::MakePoiHanaTumble() {
    KartBody* body = mBody;
    TKartThrower* obj = (TKartThrower*)GetGeoObjMgr()->getKartReactHitObjectList(body->mMynum)[14];
    if (mFlags & 1) return;
    if (obj == 0) return;
    switch (body->getChecker()->CheckCrash()) {
        case 0:
            {
                JGeometry::TVec3f force;
                JGeometry::TVec3f dir;
                JGeometry::TVec3f hitNormal;
                JGeometry::TVec3f objPos;
                body->_584 = 13;
                body->_588 = 0;
                mUnk5 = 0;
                obj->getPosition(&objPos);
                obj->getColScaleRadius();
                GetGeoObjMgr()->getKartHitDepthNormalObj(body->mMynum);
                hitNormal.set(*GetGeoObjMgr()->getKartHitRefVecNormalObj(body->mMynum));
                obj->getVelocity(&dir);
                dir.y = 0.0f;
                objPos.y = body->mPos.y;
                force.sub(objPos, body->mPos);
                force.length();
                body->_308.angle(force);
                dir.set(force);
                dir.scale(-1.0f);
                GetKartCtrl()->DevMatrixByVector(&mDir, &dir, body->_110);
                mDir.y = 0.0f;
                mDir.normalize();
                body->_594 = 0;
                body->_4b4 = 0.0f;
                body->_4bc = 0.191888f;
                body->mCarStatus |= 0x0000000800100000;
                body->getCrash()->SaveDir();
                body->getGame()->MakeClear();
                body->getDamage()->SetBigDamageAnime();
                body->getItem()->FallItem();
                body->mVel.zero();
                body->_2cc.zero();
                body->mWg.zero();
                JGeometry::TVec3f throwDir;
                f32 throwPow = (f32)obj->getThrowPow();
                f32 forceScale = 5.0f;
                throwPow = throwPow * forceScale;
                throwPow = throwPow * body->_3a4;
                obj->getThrowDir(&throwDir, body->mMynum);
                body->_2cc.x += throwPow * throwDir.x;
                body->_2cc.y += throwPow * throwDir.y;
                body->_2cc.z += throwPow * throwDir.z;
                mFlags |= 1;
                mTimer = 120;
                if (obj->getKind() == 3402) {
                    GetKartCtrl()->getKartSound(body->mMynum)->DoShootSound();
                }
                GetKartCtrl()->getKartSound(body->mMynum)->DoShootVoice();
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, body->mMynum, body->mPos, true);
                body->getStrat()->DoMotor(MotorManager::MotorType_12);
                body->getCrash()->SetMatchlessTimer();
            }
            break;
        default:
            return;
    }
}

void KartTumble::DoShootCrashCrl() {
    KartBody* body = mBody;
    if (!(body->mCarStatus & 0x800000000)) {
        GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashSound();
    }
    switch (body->_588) {
        case 1:
            break;
        case 0:
            body->_594++;
            if (body->_594 < 80 && body->_594 > 20 && body->getTouchNum() >= 3) {
                body->_588 = 1;
            } else if (body->_594 >= 80) {
                body->_588 = 1;
            }
            GetKartCtrl()->ChaseFnumber(&body->_4b4, body->_4bc, 0.8f);
            if (!((s64)(s32)mFlags & 0x400)) {
                if (body->_594 < 10) {
                    body->mVel.y += 10.0f;
                }
                if (body->_594 <= 20) {
                    body->mWg.scale(0.4f);
                    body->mWg.x += body->_4b4 * mDir.z;
                    body->mWg.z += body->_4b4 * (-mDir.x);
                }
            }
            break;
    }
}

void KartTumble::DoTumbleCrl() {
    if (mTimer != 0) {
        (mTimer)--;
    } else {
        mFlags &= 0xFE;
    }
}

void KartTumble::DoAfterTumbleCrl() {
    KartBody* body = mBody;
    if (!(body->mCarStatus & 0x800000000)) {
        GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashSound();
    }
    switch (body->_588) {
        case 1:
            break;
        case 0:
            body->_594++;
            if (body->_594 < 80 && body->_594 > 20 && body->getTouchNum() >= 3) {
                body->_588 = 1;
            } else if (body->_594 >= 80) {
                body->_588 = 1;
            }
            GetKartCtrl()->ChaseFnumber(&body->_4b4, body->_4bc, 0.8f);
            if (!((s64)(s32)mFlags & 0x400)) {
                if (body->_594 <= 20) {
                    body->mWg.scale(0.4f);
                    body->mWg.x += body->_4b4 * mDir.z;
                    body->mWg.z += body->_4b4 * (-mDir.x);
                }
            }
            break;
    }
}
