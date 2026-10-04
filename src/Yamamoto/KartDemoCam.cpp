#include "Yamamoto/KartDemoCam.h"

#include "Yamamoto/kartCamera.h"

#include "Kaneshige/RaceMgr.h"

#include "Kameda/J2DManager.h"

#include "JSystem/JAudio/JASFakeMatch2.h"

#include "Kaneshige/Course/CrsGround.h"

// comments inside functions are inline functions being called in that function

static const f32 _rodataPrefix[4] = {0.0f, 0.0f, 0.0f, 1.0f};

void KartDemoCam::Init(KartCam *) {}

void KartDemoCam::ChaseFovy(const CrsData::Camera *cam) {
    f32 fovy = mCam->GetFovy();
    f32 camFovy2 = cam->getFovy2();
    if (camFovy2 == 0.0f) {
        mCam->SetFovy(cam->getFovy());
    }
    else {
        GetKartCtrl()->ChaseFnumber(&fovy, camFovy2, 0.0f);
        mCam->SetFovy(fovy);
    }
}

void KartDemoCam::DoInitCol() {
    _80.set(mCam->mCameraPos);
}

void KartDemoCam::DoCol() {
    CrsGround ground;
    JGeometry::TVec3f normal;
    ground.search(mCam->mCameraPos, _80);
    if (!(mBody->mCarStatus & 0x400000)) {
        if (ground.getAttribute() == 2 || ground.getAttribute() == 18) {
            f32 f = ground.getWallNormal(&normal, nullptr);
            mCam->mCameraPos.x += normal.x * f * 2.0f;
            mCam->mCameraPos.y += normal.y * f * 2.0f;
            mCam->mCameraPos.z += normal.z * f * 2.0f;
        }
        else {
            f32 height = ground.getHeight();
            f32 diff = (mCam->mCameraPos.y - 30.0f) - height;
            if (diff < 0.0f) {
                mCam->mCameraPos.y -= diff;
            }
        }
    }
    _80.set(mCam->mCameraPos);
}

void KartDemoCam::InitFixCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    local.set(0.0f, 1.0f, 0.0f);
    cam->getPosition(&local2);
    mCam->mCameraPos.set(local2);
    mCam->mCameraLookPos.set(mCam->mCameraPos);
    JGeometry::TVec3f local3;
    cam->getFrontDirection(&local3);
    mCam->mCameraLookPos.x += 100.0f * local3.x;
    mCam->mCameraLookPos.y += 100.0f * local3.y;
    mCam->mCameraLookPos.z += 100.0f * local3.z;
    mCam->SetFovy(cam->getFovy());
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("InitFixCam", 31);
}

void KartDemoCam::DoFixCam() {
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    JGeometry::TVec3f local;
    ChaseFovy(cam);
    local.set(0.0f, 1.0f, 0.0f);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("DoFixCam", 32);
}

void KartDemoCam::InitFixSearchCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    local.set(0.0f, 1.0f, 0.0f);
    cam->getPosition(&local2);
    mCam->mCameraPos.set(local2);
    mCam->mCameraLookPos.set(mBody->mPlayerPosMtx[0][3], mBody->mPlayerPosMtx[1][3], mBody->mPlayerPosMtx[2][3]);
    mCam->SetFovy(cam->getFovy());
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("InitFixSearchCam", 33);
}

void KartDemoCam::DoFixSearchCam() {
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    JGeometry::TVec3f local3;
    local.set(0.0f, 1.0f, 0.0f);
    local3.set(mBody->mPlayerPosMtx[0][3], mBody->mPlayerPosMtx[1][3], mBody->mPlayerPosMtx[2][3]);
    local2.sub(local3, mCam->mCameraLookPos);
    f32 len = local2.length();
    local2.normalize();
    len *= 0.3f;
    local2.scale(len);
    mCam->mCameraLookPos.add(local2);
    ChaseFovy(cam);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("DoFixSearchCam", 34);
}

void KartDemoCam::InitFixChaseCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    JGeometry::TVec3f local3;
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    local.set(0.0f, 1.0f, 0.0f);
    cam->getPosition(&mCam->mCameraPos);
    local3.set(mBody->mPlayerPosMtx[0][3], mBody->mPlayerPosMtx[1][3], mBody->mPlayerPosMtx[2][3]);
    mCamKartDiff.sub(mCam->mCameraPos, local3);
    mCamKartDiffLength = mCamKartDiff.length();
    mCamKartDiff.normalize();
    mCam->mCameraPos.x = mCamKartDiff.x * mCamKartDiffLength + local3.x;
    mCam->mCameraPos.y = mCamKartDiff.y * mCamKartDiffLength + local3.y;
    mCam->mCameraPos.z = mCamKartDiff.z * mCamKartDiffLength + local3.z;
    mCam->mCameraLookPos.set(mCam->mCameraPos);
    JGeometry::TVec3f local4;
    cam->getFrontDirection(&local4);
    mCam->mCameraLookPos.x += 100.0f * local4.x;
    mCam->mCameraLookPos.y += 100.0f * local4.y;
    mCam->mCameraLookPos.z += 100.0f * local4.z;
    mCam->SetFovy(cam->getFovy());
    DoInitCol();
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("InitFixChaseCam", 35);
}

void KartDemoCam::DoFixChaseCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    JGeometry::TVec3f local3;
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    ChaseFovy(cam);
    local.set(0.0f, 1.0f, 0.0f);
    local3.set(mBody->mPlayerPosMtx[0][3], mBody->mPlayerPosMtx[1][3], mBody->mPlayerPosMtx[2][3]);
    mCam->mCameraPos.x = mCamKartDiff.x * mCamKartDiffLength + local3.x;
    mCam->mCameraPos.y = mCamKartDiff.y * mCamKartDiffLength + local3.y;
    mCam->mCameraPos.z = mCamKartDiff.z * mCamKartDiffLength + local3.z;
    mCam->mCameraLookPos.set(mCam->mCameraPos);
    JGeometry::TVec3f local4;
    cam->getFrontDirection(&local4);
    mCam->mCameraLookPos.x += 100.0f * local4.x;
    mCam->mCameraLookPos.y += 100.0f * local4.y;
    mCam->mCameraLookPos.z += 100.0f * local4.z;
    DoCol();
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("DoFixChaseCam", 36);
}

void KartDemoCam::InitChaseCam() {
    JGeometry::TVec3f local;
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    local.set(0.0f, 1.0f, 0.0f);
    mCam->mCameraLookPos.set(mBody->mPlayerPosMtx[0][3], mBody->mPlayerPosMtx[1][3] + 30.0f, mBody->mPlayerPosMtx[2][3]);
    cam->getPosition(&mCam->mCameraPos);
    mCamKartDiff.sub(mCam->mCameraPos, mCam->mCameraLookPos);
    mCamKartDiffLength = mCamKartDiff.length();
    mCamKartDiff.normalize();
    mCam->mCameraPos.x = mCamKartDiff.x * mCamKartDiffLength + mCam->mCameraLookPos.x;
    mCam->mCameraPos.y = mCamKartDiff.y * mCamKartDiffLength + mCam->mCameraLookPos.y;
    mCam->mCameraPos.z = mCamKartDiff.z * mCamKartDiffLength + mCam->mCameraLookPos.z;
    mCam->SetFovy(cam->getFovy());
    DoInitCol();
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("InitChaseCam", 37);
}

void KartDemoCam::DoChaseCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    JGeometry::TVec3f local3;
    JGeometry::TVec3f local4;
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    ChaseFovy(cam);
    local.set(0.0f, 1.0f, 0.0f);
    local4.set(mBody->mPlayerPosMtx[0][3], mBody->mPlayerPosMtx[1][3] + 30.0f, mBody->mPlayerPosMtx[2][3]);
    local3.sub(local4, mCam->mCameraLookPos);
    f32 len = local3.length();
    local3.normalize();
    len *= 0.6f;
    local3.scale(len);
    mCam->mCameraLookPos.add(local3);
    mCam->mCameraPos.x = mCamKartDiff.x * mCamKartDiffLength + mCam->mCameraLookPos.x;
    mCam->mCameraPos.y = mCamKartDiff.y * mCamKartDiffLength + mCam->mCameraLookPos.y;
    mCam->mCameraPos.z = mCamKartDiff.z * mCamKartDiffLength + mCam->mCameraLookPos.z;
    DoCol();
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("DoChaseCam", 38);
}

void KartDemoCam::InitFixPathCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    if (cam->getPathID() < 0) {
        mCam->InitOutView();
        return;
    }
    CrsData::PathData* pathData = crsData->getPathData((u16)cam->getPathID());
    mCam->mCameraPos.set(crsData->getPointData((u16)cam->getPathID(), 0)->pos);
    local.set(0.0f, 1.0f, 0.0f);
    JGeometry::TVec3f local3;
    cam->getFrontDirection(&local3);
    KartCam* kc = mCam;
    f32 angle = atan2f(local3.x, local3.z);
    GetKartCtrl()->PolarToCross(&kc->mCameraPos, &kc->mCameraLookPos, 50.0f, 0.0f, angle);
    mCam->SetFovy(cam->getFovy());
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("InitFixPathCam", 39);
}

void KartDemoCam::DoFixPathCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    JGeometry::TVec3f local3;
    KartCam* kc;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    CrsData::PointData* pointData = crsData->getPointData((u16)cam->getPathID(), 1);
    local.set(0.0f, 1.0f, 0.0f);
    if (cam->getPathID() < 0) {
        mCam->OutView();
        return;
    }
    local2.set(pointData->pos);
    local3.sub(local2, mCam->mCameraPos);
    int pathSpeed = cam->getPathSpeed();
    if (pathSpeed == 0) {
        pathSpeed = 1;
    }
    if (pathSpeed > 100) {
        pathSpeed = 100;
    }
    f32 scale = 0.0005f * (f32)pathSpeed * local3.length();
    local3.normalize();
    local3.scale(scale);
    mCam->mCameraPos.add(local3);
    ChaseFovy(cam);
    JGeometry::TVec3f local4;
    cam->getFrontDirection(&local4);
    kc = mCam;
    f32 angle = atan2f(local4.x, local4.z);
    GetKartCtrl()->PolarToCross(&kc->mCameraPos, &kc->mCameraLookPos, 50.0f, 0.0f, angle);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("DoFixPathCam", 40);
}

void KartDemoCam::InitChasePathCam() {
    JGeometry::TVec3f local;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    if (cam->getPathID() < 0) {
        mCam->InitOutView();
        return;
    }
    CrsData::PathData* pathData = crsData->getPathData((u16)cam->getPathID());
    mCam->mCameraPos.set(crsData->getPointData((u16)cam->getPathID(), 0)->pos);
    local.set(0.0f, 1.0f, 0.0f);
    mCam->mCameraLookPos.set(mBody->mPlayerPosMtx[0][3], mBody->mPlayerPosMtx[1][3] + 30.0f, mBody->mPlayerPosMtx[2][3]);
    mCam->SetFovy(cam->getFovy());
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("InitChasePathCam", 41);
}

void KartDemoCam::DoChasePathCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    JGeometry::TVec3f local3;
    JGeometry::TVec3f local4;
    JGeometry::TVec3f local5;
    local.set(0.0f, 1.0f, 0.0f);
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    CrsData::PointData* pointData = crsData->getPointData((u16)cam->getPathID(), 1);
    if (cam->getPathID() < 0) {
        mCam->OutView();
        return;
    }
    local2.set(pointData->pos);
    local5.sub(local2, mCam->mCameraPos);
    int pathSpeed = cam->getPathSpeed();
    if (pathSpeed == 0) {
        pathSpeed = 1;
    }
    if (pathSpeed > 100) {
        pathSpeed = 100;
    }
    f32 scale = 0.0005f * (f32)pathSpeed * local5.length();
    local5.normalize();
    local5.scale(scale);
    mCam->mCameraPos.add(local5);
    local4.set(mBody->mPlayerPosMtx[0][3], mBody->mPlayerPosMtx[1][3] + 30.0f, mBody->mPlayerPosMtx[2][3]);
    local3.sub(local4, mCam->mCameraLookPos);
    f32 len = local3.length();
    local3.normalize();
    len *= 0.6f;
    local3.scale(len);
    mCam->mCameraLookPos.add(local3);
    ChaseFovy(cam);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("DoChasePathCam", 42);
}

void KartDemoCam::InitFixSplCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    KartCam* kc;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    if (cam->getPathID() < 0) {
        mCam->InitOutView();
        return;
    }
    mCamKartDiffLength = 0.0f;
    *(u16*)&_3a[4] = 1;
    CrsData::PathData* pathData = crsData->getPathData((u16)cam->getPathID());
    mCam->_1b0 = 0.0f;
    for (int i = 0; i < 4; i++) {
        mCam->mPathData[i].set(crsData->getPointData((u16)cam->getPathID(), (u16)i)->pos);
    }
    mCam->mPathSpline->setAll(mCam->mPathData);
    mCam->mCameraPos.set(mCam->mPathData[0]);
    mCam->_1b0 = 0.0f;
    local.set(0.0f, 1.0f, 0.0f);
    JGeometry::TVec3f local3;
    cam->getFrontDirection(&local3);
    kc = mCam;
    f32 angle = atan2f(local3.x, local3.z);
    GetKartCtrl()->PolarToCross(&kc->mCameraPos, &kc->mCameraLookPos, 50.0f, 0.0f, angle);
    mCam->SetFovy(cam->getFovy());
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("InitFixSplCam", 43);
}

void KartDemoCam::DoFixSplCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    JGeometry::TVec3f local3;
    JGeometry::TVec3f local4;
    KartCam* kc;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    crsData->getPointData((u16)cam->getPathID(), 1);
    local.set(0.0f, 1.0f, 0.0f);
    if (cam->getPathID() < 0) {
        mCam->OutView();
        return;
    }
    mCam->mPathSpline->setAll(mCam->mPathData);
    mCam->mPathSpline->getBezierPoint(&local3, mCam->_1b0);
    mCam->mCameraPos.set(local3);
    u16 k = cam->getPathSpeed();
    if (k == 0) {
        k = 1;
    }
    if (k > 100) {
        k = 100;
    }
    k = 100 / k;
    GetKartCtrl()->ChaseFcnvge4(&mCamKartDiffLength, (u16*)&_3a[4], 1.0f, k);
    mCam->_1b0 = mCamKartDiffLength;
    ChaseFovy(cam);
    JGeometry::TVec3f local5;
    cam->getFrontDirection(&local5);
    kc = mCam;
    f32 angle = atan2f(local5.x, local5.z);
    GetKartCtrl()->PolarToCross(&kc->mCameraPos, &kc->mCameraLookPos, 50.0f, 0.0f, angle);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("DoFixSplCam", 44);
}

void KartDemoCam::InitChaseSplCam() {
    JGeometry::TVec3f local;
    CrsData::Camera* cam;
    CrsData* crsData;
    crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    if (cam->getPathID() < 0) {
        mCam->InitOutView();
        return;
    }
    mCamKartDiffLength = 0.0f;
    *(u16*)&_3a[4] = 1;
    crsData->getPathData((u16)cam->getPathID());
    mCam->_1b0 = 0.0f;
    for (int i = 0; i < 4; i++) {
        mCam->mPathData[i].set(crsData->getPointData((u16)cam->getPathID(), (u16)i)->pos);
    }
    mCam->mPathSpline->setAll(mCam->mPathData);
    mCam->mCameraPos.set(mCam->mPathData[0]);
    mCam->_1b0 = 0.0f;
    mCam->_e5 = 0;
    local.set(0.0f, 1.0f, 0.0f);
    mCam->mCameraLookPos.set(mBody->mPlayerPosMtx[0][3], mBody->mPlayerPosMtx[1][3] + 30.0f, mBody->mPlayerPosMtx[2][3]);
    mCam->SetFovy(cam->getFovy());
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("InitChaseSplCam", 45);
}

void KartDemoCam::DoChaseSplCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f diff;
    JGeometry::TVec3f lookPos;
    JGeometry::TVec3f d;
    JGeometry::TVec3f bezier;
    JGeometry::TVec3f f;

    local.set(0.0f, 1.0f, 0.0f);
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mCam->mCurCamIdx);
    crsData->getPointData((u16)cam->getPathID(), 1);
    if (cam->getPathID() < 0) {
        mCam->OutView();
        return;
    }
    mCam->mPathSpline->setAll(mCam->mPathData);
    mCam->mPathSpline->getBezierPoint(&bezier, mCam->_1b0);
    mCam->mCameraPos.set(bezier);
    u16 k = (u16)cam->getPathSpeed();
    if (k == 0) k = 1;
    if (k > 100) k = 100;
    k = 100 / k;
    GetKartCtrl()->ChaseFcnvge4(&mCamKartDiffLength, (u16*)&_3a[4], 1.0f, k);
    mCam->_1b0 = mCamKartDiffLength;
    lookPos.set(mBody->mPlayerPosMtx[0][3], mBody->mPlayerPosMtx[1][3] + 30.0f, mBody->mPlayerPosMtx[2][3]);
    diff.sub(lookPos, mCam->mCameraLookPos);
    f32 len = diff.length();
    diff.normalize();
    len *= 0.6f;
    diff.scale(len);
    mCam->mCameraLookPos.add(diff);
    ChaseFovy(cam);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("DoChaseSplCam", 46);
}

void KartDemoCam::InitFixPalaCam() {
    // void CrsData::Camera::getTargetPos2(JGeometry::TVec3<float> *) const {}
    // void CrsData::Camera::getTargetPos(JGeometry::TVec3<float> *) const {}
    mCam->DegubCamera("InitFixPalaCam", 47);
}

void KartDemoCam::DoFixPalaCam() {
    mCam->DegubCamera("DoFixPalaCam", 48);
}

void KartDemoCam::InitDemoPathCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = crsData->getCameraData(mCam->mCurCamIdx);
    *(u16*)&_3a[2] = 0;
    crsData->getPathData((u16)cam->getPathID());
    mCam->mCameraPos.set(crsData->getPointData((u16)cam->getPathID(), 0)->pos);
    local.set(0.0f, 1.0f, 0.0f);
    mCam->SetFovy(cam->getFovy());
    cam->getTargetPos(&mCam->mCameraLookPos);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("InitDemoPathCam", 49);
}

void KartDemoCam::DoDemoPathCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    JGeometry::TVec3f local3;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = crsData->getCameraData(mCam->mCurCamIdx);
    CrsData::PointData* pointData = crsData->getPointData((u16)cam->getPathID(), 1);
    local.set(0.0f, 1.0f, 0.0f);
    local2.set(pointData->pos);
    local3.sub(local2, mCam->mCameraPos);
    f32 scale = 0.0005f * (f32)cam->getPathSpeed();
    scale *= local3.length();
    local3.normalize();
    local3.scale(scale);
    mCam->mCameraPos.add(local3);
    cam->getTargetPos2(&local3);
    local3.sub(local3, mCam->mCameraLookPos);
    f32 scale2 = 0.01f * local3.length();
    local3.normalize();
    local3.scale(scale2);
    mCam->mCameraLookPos.add(local3);
    ChaseFovy(cam);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("DoDemoPathCam", 50);
}

void KartDemoCam::InitDemoParallelView() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    JGeometry::TVec3f local3;
    mCam->SetFovy(viewdata[mCam->mPtr].fovY);
    local.set(mBody->mPlayerPosMtx[0][3], mBody->mPlayerPosMtx[1][3], mBody->mPlayerPosMtx[2][3]);
    mCam->mCameraLookPos.sub(local, mBody->_350);
    GetKartCtrl()->PolarToCross(&mCam->mCameraLookPos, &mCam->mCameraPos, 500.0f, 0.35f, 0.0f);
    local2.set(0.0f, 1.0f, 0.0f);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local2, &mCam->mCameraLookPos);
    mCam->DegubCamera("InitDemoParallelView", 51);
}

void KartDemoCam::DemoParallelView() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    JGeometry::TVec3f local3;
    mCam->SetFovy(viewdata[mCam->mPtr].fovY);
    local.set(mBody->mPlayerPosMtx[0][3], mBody->mPlayerPosMtx[1][3], mBody->mPlayerPosMtx[2][3]);
    mCam->mCameraLookPos.sub(local, mBody->_350);
    GetKartCtrl()->PolarToCross(&mCam->mCameraLookPos, &mCam->mCameraPos, 500.0f, 0.35f, 0.0f);
    local2.set(0.0f, 1.0f, 0.0f);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local2, &mCam->mCameraLookPos);
    mCam->DegubCamera("DemoParallelView", 52);
}

void KartDemoCam::InitDemoGoalView() {
    mCam->DegubCamera("InitDemoGoalView", 53);
}

void KartDemoCam::DemoGoalView() {
    mCam->DegubCamera("DemoGoalView", 54);
}

void KartDemoCam::MapView() {
    JUTReport(0, 0, "Y_DIR%7.0f", 0.0f);
    JUTReport(0, 0, "X_DIR%7.0f", 0.0f);
    JUTReport(0, 0, "FOVY%7.1f", 0.0f);
}

void KartDemoCam::ObjectView() {
    JUTReport(0, 0, "RaceMgr.h", 0);
    JUTReport(0, 0, "range over: %d <= index=%d < %d", 0, 0, 0);
}

void KartDemoCam::DemoInitID() {
    // void CrsData::Camera::getKind() const {}
}

void KartDemoCam::InitID() {}

void KartDemoCam::DoDemoView() {}

void KartDemoCam::LiveViewChange() {}

void KartDemoCam::DemoView() {
    // void RaceMgr::requestDemoCameraID() const {}
}

bool KartDemoCam::DoPathEnd(JGeometry::TVec3f* pathPtr, f32* out) {
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = crsData->getCameraData(mStartId);
    f32 f = 0.0005f * (f32)cam->getPathSpeed();
    f *= pathPtr->length();
    if (cam->getTime() != 0) {
        if (*(u16*)&_3a[2] < 65535) (*(u16*)&_3a[2])++;
        if (*(u16*)&_3a[2] < cam->getTime()) goto fail;
        if (_38 & 0x100) { SetMapNextID(); return true; }
        else if (_38 & 0x200) { return SetStaffNextID(); }
        else { SetNextID(); return true; }
    } else {
        if (f >= 5.0f) goto fail;
        if (_38 & 0x100) { SetMapNextID(); return true; }
        else if (_38 & 0x200) { return SetStaffNextID(); }
        else { SetNextID(); return true; }
    }
fail:
    *out = f;
    return false;
}

void KartDemoCam::InitStartFixPathCam() {
    JGeometry::TVec3f local;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = crsData->getCameraData(mStartId);
    crsData->getPathData((u16)cam->getPathID());
    mCam->mCameraPos.set(crsData->getPointData((u16)cam->getPathID(), 0)->pos);
    local.set(0.0f, 1.0f, 0.0f);
    mCam->SetFovy(cam->getFovy());
    cam->getTargetPos(&mCam->mCameraLookPos);
    *(u16*)&_3a[2] = 0;
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
}

void KartDemoCam::DoStartFixPathCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    JGeometry::TVec3f local3;
    f32 f;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = crsData->getCameraData(mStartId);
    CrsData::PointData* pointData = crsData->getPointData((u16)cam->getPathID(), 1);
    local.set(0.0f, 1.0f, 0.0f);
    local2.set(pointData->pos);
    local3.sub(local2, mCam->mCameraPos);
    if (DoPathEnd(&local3, &f)) return;
    local3.normalize();
    local3.scale(f);
    mCam->mCameraPos.add(local3);
    ChaseFovy(cam);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
}

void KartDemoCam::InitStartPathCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = crsData->getCameraData(mStartId);
    *(u16*)&_3a[2] = 0;
    crsData->getPathData((u16)cam->getPathID());
    mCam->mCameraPos.set(crsData->getPointData((u16)cam->getPathID(), 0)->pos);
    local.set(0.0f, 1.0f, 0.0f);
    mCam->SetFovy(cam->getFovy());
    cam->getTargetPos(&mCam->mCameraLookPos);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
}

void KartDemoCam::DoStartPathCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    JGeometry::TVec3f local3;
    f32 f;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = crsData->getCameraData(mStartId);
    CrsData::PointData* pointData = crsData->getPointData((u16)cam->getPathID(), 1);
    local.set(0.0f, 1.0f, 0.0f);
    local2.set(pointData->pos);
    local3.sub(local2, mCam->mCameraPos);
    if (DoPathEnd(&local3, &f)) return;
    local3.normalize();
    local3.scale(f);
    mCam->mCameraPos.add(local3);
    cam->getTargetPos2(&local3);
    local3.sub(local3, mCam->mCameraLookPos);
    f = 0.01f * local3.length();
    local3.normalize();
    local3.scale(f);
    mCam->mCameraLookPos.add(local3);
    ChaseFovy(cam);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
}

void KartDemoCam::InitStartLookPathCam() {
    JGeometry::TVec3f local;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = crsData->getCameraData(mStartId);
    *(u16*)&_3a[2] = 0;
    cam->getPosition(&mCam->mCameraPos);
    local.set(0.0f, 1.0f, 0.0f);
    mCam->SetFovy(cam->getFovy());
    cam->getTargetPos(&mCam->mCameraLookPos);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
}

void KartDemoCam::DoStartLookPathCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    JGeometry::TVec3f local3;
    f32 f;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = crsData->getCameraData(mStartId);
    local.set(0.0f, 1.0f, 0.0f);
    cam->getTargetPos2(&local3);
    local3.sub(local3, mCam->mCameraLookPos);
    if (DoPathEnd(&local3, &f)) return;
    local3.normalize();
    local3.scale(f);
    mCam->mCameraLookPos.add(local3);
    ChaseFovy(cam);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
}

void KartDemoCam::InitStartOnlyPathCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    JGeometry::TVec3f local3;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = crsData->getCameraData(mStartId);
    *(u16*)&_3a[2] = 0;
    crsData->getPathData((u16)cam->getPathID());
    mCam->mCameraPos.set(crsData->getPointData((u16)cam->getPathID(), 0)->pos);
    local.set(0.0f, 1.0f, 0.0f);
    mCam->SetFovy(cam->getFovy());
    cam->getFrontDirection(&local3);
    mCam->mCameraLookPos.x = mCam->mCameraPos.x + 10.0f * local3.x;
    mCam->mCameraLookPos.y = mCam->mCameraPos.y + 10.0f * local3.y;
    mCam->mCameraLookPos.z = mCam->mCameraPos.z + 10.0f * local3.z;
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
}

void KartDemoCam::DoStartOnlyPathCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    JGeometry::TVec3f local3;
    JGeometry::TVec3f local4;
    f32 f;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = crsData->getCameraData(mStartId);
    CrsData::PointData* pointData = crsData->getPointData((u16)cam->getPathID(), 1);
    local.set(0.0f, 1.0f, 0.0f);
    local2.set(pointData->pos);
    local3.sub(local2, mCam->mCameraPos);
    if (DoPathEnd(&local3, &f)) return;
    local3.normalize();
    local3.scale(f);
    mCam->mCameraPos.add(local3);
    cam->getFrontDirection(&local4);
    mCam->mCameraLookPos.x = mCam->mCameraPos.x + 10.0f * local4.x;
    mCam->mCameraLookPos.y = mCam->mCameraPos.y + 10.0f * local4.y;
    mCam->mCameraLookPos.z = mCam->mCameraPos.z + 10.0f * local4.z;
    ChaseFovy(cam);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
}

void KartDemoCam::InitStartFixCam() {
    JGeometry::TVec3f local;
    JGeometry::TVec3f local2;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = crsData->getCameraData(mStartId);
    *(u16*)&_3a[2] = 0;
    local.set(0.0f, 1.0f, 0.0f);
    cam->getPosition(&local2);
    mCam->mCameraPos.set(local2);
    mCam->mCameraLookPos.set(mCam->mCameraPos);
    JGeometry::TVec3f local3;
    cam->getFrontDirection(&local3);
    mCam->mCameraLookPos.x += 100.0f * local3.x;
    mCam->mCameraLookPos.y += 100.0f * local3.y;
    mCam->mCameraLookPos.z += 100.0f * local3.z;
    mCam->SetFovy(cam->getFovy());
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("InitFixCam", 31);
}

void KartDemoCam::DoStartFixCam() {
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = crsData->getCameraData(mStartId);
    JGeometry::TVec3f local;
    local.zero();
    f32 f = 0.0f;
    if (DoPathEnd(&local, &f)) return;
    JGeometry::TVec3f local2;
    ChaseFovy(cam);
    local2.set(0.0f, 1.0f, 0.0f);
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local2, &mCam->mCameraLookPos);
    mCam->DegubCamera("DoFixCam", 32);
}

void KartDemoCam::InitStartChasePathCam() {
    JGeometry::TVec3f local;
    CrsData* crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mStartId);
    crsData->getPathData((u16)cam->getPathID());
    mCam->mCameraPos.set(crsData->getPointData((u16)cam->getPathID(), 0)->pos);
    local.set(0.0f, 1.0f, 0.0f);
    mCam->mCameraLookPos.set(mBody->mPlayerPosMtx[0][3], mBody->mPlayerPosMtx[1][3] + 30.0f, mBody->mPlayerPosMtx[2][3]);
    mCam->SetFovy(cam->getFovy());
    C_MTXLookAt(mCam->mMtx, &mCam->mCameraPos, &local, &mCam->mCameraLookPos);
    mCam->DegubCamera("InitChasePathCam", 41);
}

void KartDemoCam::DoStartChasePathCam() {}

void KartDemoCam::InitVsLastCam() {
    mCam->DegubCamera("InitVsLastCam", 30);
}

void KartDemoCam::VsLastCam() {
    mCam->DegubCamera("VsLastCam", 29);
}

void KartDemoCam::InitLastCam() {
    // void TJugem::getPosition() const {}
    mCam->DegubCamera("InitLastCam", 28);
}
void KartDemoCam::LastCam() {
    mCam->DegubCamera("LastCam", 27);
}

void KartDemoCam::InitStartDemoCam() {
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mStartId);
    cam->getKind();
    switch (cam->getKind()) {
        case 4:
            InitStartFixPathCam();
            mCam->DegubCamera("InitStartFixPathCam", 19);
            break;
        case 5:
            InitStartPathCam();
            mCam->DegubCamera("InitStartPathCam", 20);
            break;
        case 6:
            InitStartLookPathCam();
            mCam->DegubCamera("InitStartLookPathCam", 21);
            break;
        case 1:
            InitStartOnlyPathCam();
            mCam->DegubCamera("InitStartOnlyPathCam", 22);
            break;
    }
}

void KartDemoCam::InitStartMapCam() {
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mStartId);
    cam->getKind();
    switch (cam->getKind()) {
        case 4:
            InitStartFixPathCam();
            mCam->DegubCamera("InitStartFixPathCam", 11);
            break;
        case 5:
            InitStartPathCam();
            mCam->DegubCamera("InitStartPathCam", 12);
            break;
        case 6:
            InitStartLookPathCam();
            mCam->DegubCamera("InitStartLookPathCam", 13);
            break;
        case 1:
            InitStartOnlyPathCam();
            mCam->DegubCamera("InitStartOnlyPathCam", 14);
            break;
    }
}

void KartDemoCam::StartDemoCam() {
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mStartId);
    if (_38 & 4) {
        LastCam();
        return;
    }
    switch (cam->getKind()) {
        case 4:
            DoStartFixPathCam();
            mCam->DegubCamera("DoStartFixPathCam", 23);
            break;
        case 5:
            DoStartPathCam();
            mCam->DegubCamera("DoStartPathCam", 24);
            break;
        case 6:
            DoStartLookPathCam();
            mCam->DegubCamera("DoStartLookPathCam", 25);
            break;
        case 1:
            DoStartOnlyPathCam();
            mCam->DegubCamera("DoStartOnlyPathCam", 26);
            break;
    }
}

void KartDemoCam::SetLastDemo() {
    _38 |= 4;
    *(u16*)&_3a[2] = 0;
    _3a[0] = 1;
    if (mCam->mCamNo == 0) {
        mCam->mCtrl->mBitfield |= 2;
    }
    InitLastCam();
}

void KartDemoCam::SetRaceCam() {
    _38 = 0;
    mCam->mMode = 0;
    mCam->InitOutView();
    if (mCam->mCamNo == 0) {
        mCam->mCtrl->mBitfield |= 2;
    }
}

void KartDemoCam::SetNextID() {
    RaceMgr::getManager()->getCourse()->getCrsData();
    mStartId = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mStartId)->getNextCameraID();
    if (mStartId >= 0) {
        InitStartDemoCam();
    } else {
        if (!(_38 & 0xC)) {
            GETJ2DManager()->startPreRace();
        }
        _38 |= 4;
        _3a[0] = 0;
    }
}

void KartDemoCam::DoExecution() {
    if (_38 & 1) {
        if (_38 & 4) {
            SetRaceCam();
        } else {
            if (!(_38 & 8)) {
                GETJ2DManager()->startPreRace();
            }
            _38 |= 8;
        }
    }
}

bool KartDemoCam::GetStartID() {
    mStartId = RaceMgr::getManager()->getCourse()->getCrsData()->getStartCameraID();
    if (mCam->mMode == 2) return 1;
    return mStartId != -1;
}

void KartDemoCam::InitStartID(bool last) {
    _38 = 1;
    if (last) {
        SetLastDemo();
    } else {
        InitStartDemoCam();
    }
}

void KartDemoCam::StartDemoView() {
    StartDemoCam();
}

void KartDemoCam::StartMapCam() {
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mStartId);
    switch (cam->getKind()) {
        case 4:
            InitStartFixPathCam();
            mCam->DegubCamera("DoStartFixPathCam", 15);
            break;
        case 5:
            InitStartPathCam();
            mCam->DegubCamera("DoStartPathCam", 16);
            break;
        case 6:
            InitStartLookPathCam();
            mCam->DegubCamera("DoStartLookPathCam", 17);
            break;
        case 1:
            InitStartOnlyPathCam();
            mCam->DegubCamera("DoStartOnlyPathCam", 18);
            break;
    }
}

void KartDemoCam::SetMapNextID() {
    CrsData* cd = RaceMgr::getManager()->getCourse()->getCrsData();
    mStartId = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mStartId)->getNextCameraID();
    if (mStartId < 0) {
        mStartId = cd->getStartLoserCameraID();
    }
    InitStartMapCam();
}

void KartDemoCam::SetUpMapID() {
    mStartId = RaceMgr::getManager()->getCourse()->getCrsData()->getStartLoserCameraID();
    if (mStartId >= 0) {
        InitStartMapCam();
        _38 |= 0x100;
    }
}

void KartDemoCam::InitStaffView() {}

void KartDemoCam::StaffView() {}

bool KartDemoCam::SetStaffNextID() {
    RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::Camera* cam = RaceMgr::getManager()->getCourse()->getCrsData()->getCameraData(mStartId);
    if (cam->getNextCameraID() < 0) {
        return 0;
    }
    mStartId = cam->getNextCameraID();
    InitStaffView();
    return 1;
}

void KartDemoCam::SetUpStaffID() {
    mStartId = RaceMgr::getManager()->getCourse()->getCrsData()->getStartCameraID();
    _38 |= 0x200;
}

void CrsData::Camera::getTargetPos(JGeometry::TVec3f *out) const {
    out->set(mTargetPos);
}

void CrsData::Camera::getTargetPos2(JGeometry::TVec3f *out) const {
    out->set(mTargetPos2);
}
