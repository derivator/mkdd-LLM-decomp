#include "Yamamoto/KartDemoCam.h"

#include "Yamamoto/kartCamera.h"

#include "Kaneshige/RaceMgr.h"

#include "Kameda/J2DManager.h"

#include "JSystem/JAudio/JASFakeMatch2.h"

#include "Kaneshige/Course/CrsGround.h"

// comments inside functions are inline functions being called in that function

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
    // void CrsData::Camera::getPathSpeed() const {}
}

void KartDemoCam::InitChasePathCam() {}

void KartDemoCam::DoChasePathCam() {}

void KartDemoCam::InitFixSplCam() {}

void KartDemoCam::DoFixSplCam() {}

void KartDemoCam::InitChaseSplCam() {}

void KartDemoCam::DoChaseSplCam() {}

void KartDemoCam::InitFixPalaCam() {
    // void CrsData::Camera::getTargetPos2(JGeometry::TVec3<float> *) const {}
    // void CrsData::Camera::getTargetPos(JGeometry::TVec3<float> *) const {}
}

void KartDemoCam::DoFixPalaCam() {}

void KartDemoCam::InitDemoPathCam() {}

void KartDemoCam::DoDemoPathCam() {}

void KartDemoCam::InitDemoParallelView() {}

void KartDemoCam::DemoParallelView() {}

void KartDemoCam::InitDemoGoalView() {}

void KartDemoCam::DemoGoalView() {}

void KartDemoCam::MapView() {}

void KartDemoCam::ObjectView() {}

void KartDemoCam::DemoInitID() {
    // void CrsData::Camera::getKind() const {}
}

void KartDemoCam::InitID() {}

void KartDemoCam::DoDemoView() {}

void KartDemoCam::LiveViewChange() {}

void KartDemoCam::DemoView() {
    // void RaceMgr::requestDemoCameraID() const {}
}

void KartDemoCam::DoPathEnd(JGeometry::TVec3<float> *, float *) {}

void KartDemoCam::InitStartFixPathCam() {}

void KartDemoCam::DoStartFixPathCam() {}

void KartDemoCam::InitStartPathCam() {}

void KartDemoCam::DoStartPathCam() {}

void KartDemoCam::InitStartLookPathCam() {}

void KartDemoCam::DoStartLookPathCam() {}

void KartDemoCam::InitStartOnlyPathCam() {}

void KartDemoCam::DoStartOnlyPathCam() {}

void KartDemoCam::InitStartFixCam() {}

void KartDemoCam::DoStartFixCam() {}

void KartDemoCam::InitStartChasePathCam() {}

void KartDemoCam::DoStartChasePathCam() {}

void KartDemoCam::InitVsLastCam() {}

void KartDemoCam::VsLastCam() {}

void KartDemoCam::InitLastCam() {
    // void TJugem::getPosition() const {}
}
void KartDemoCam::LastCam() {}

void KartDemoCam::InitStartDemoCam() {}

void KartDemoCam::StartDemoCam() {}

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
