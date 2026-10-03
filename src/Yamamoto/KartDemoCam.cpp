#include "Yamamoto/KartDemoCam.h"

#include "Yamamoto/kartCamera.h"

#include "Kaneshige/RaceMgr.h"

#include "Kameda/J2DManager.h"

#include "JSystem/JAudio/JASFakeMatch2.h"

// comments inside functions are inline functions being called in that function

void KartDemoCam::Init(KartCam *) {}

void KartDemoCam::ChaseFovy(const CrsData::Camera *) {
    // void KartCam::SetFovy(float) {}
    // void CrsData::Camera::getFovy() const {}
    // void CrsData::Camera::getFovy2() const {}
}

void KartDemoCam::DoInitCol() {
    _80.set(mCam->mCameraPos);
}

void KartDemoCam::DoCol() {}

void KartDemoCam::InitFixCam() {
    // void CrsData::Camera::getPosition(JGeometry::TVec3<float> *) const {}
}

void KartDemoCam::DoFixCam() {}

void KartDemoCam::InitFixSearchCam() {}

void KartDemoCam::DoFixSearchCam() {}

void KartDemoCam::InitFixChaseCam() {}

void KartDemoCam::DoFixChaseCam() {}

void KartDemoCam::InitChaseCam() {}

void KartDemoCam::DoChaseCam() {}

void KartDemoCam::InitFixPathCam() {
    // void CrsData::Camera::getPathID() const {}
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

void KartDemoCam::SetLastDemo() {}

void KartDemoCam::SetRaceCam() {
    _38 = 0;
    mCam->mMode = 0;
    mCam->InitOutView();
    if (mCam->mCamNo == 0) {
        mCam->mCtrl->mBitfield |= 2;
    }
}

void KartDemoCam::SetNextID() {}

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

void KartDemoCam::InitStartID(bool) {}

void KartDemoCam::StartDemoView() {
    StartDemoCam();
}

void KartDemoCam::InitStartMapCam() {}

void KartDemoCam::StartMapCam() {}

void KartDemoCam::SetMapNextID() {}

void KartDemoCam::SetUpMapID() {}

void KartDemoCam::InitStaffView() {}

void KartDemoCam::StaffView() {}

void KartDemoCam::SetStaffNextID() {}

void KartDemoCam::SetUpStaffID() {
    mStartId = RaceMgr::getManager()->getCourse()->getCrsData()->getStartCameraID();
    _38 |= 0x200;
}
