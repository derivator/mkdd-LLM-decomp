#include "Kameda/SceneMap.h"
#include "Kameda/Scene.h"
#include "Kameda/SceneCourseSelect.h"
#include "Kameda/SequenceInfo.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/RaceInfo.h"
#include "Osako/AppMgr.h"
#include "Osako/AwardApp.h"
#include "Osako/RaceApp.h"
#include "Osako/ResMgr.h"
#include "Osako/SequenceApp.h"
#include "Osako/SystemFile.h"
#include "Osako/SystemRecord.h"
#include "mathHelper.h"

SceneMap::SceneMap(JKRArchive *archive, JKRHeap *heap) : Scene(archive, heap) {
    init();
}

void SceneMap::init() {
    mState = 0;
}

SceneMap::~SceneMap() {}

void SceneMap::draw() {}

void SceneMap::calc() {
    switch (mState) {
    case 0:
        map();
        break;
    case 1:
        nextScene();
        break;
    case 2:
        nextRace();
        break;
    case 3:
        nextAward();
        break;
    case 4:
        nextEnding();
        break;
    case 5:
        nextTrueEnding();
        break;
    default:
#line 108
        JUT_ASSERT(0);
        break;
    }
}

void SceneMap::map() {
    switch (gSequenceInfo._364) {
    case 0:
        if (!gSequenceInfo.isEndGP()) {
            int cup = gSequenceInfo.mMapInfo.cup;
            int course = gSequenceInfo.mMapInfo.course;
            int cc = course;
            if (gSequenceInfo.mMapInfo.cup == 4) {
                cup = gSequenceInfo.getCupNo(course);
                cc = gSequenceInfo._3cc[course] % 4;
            }
            mState = 2;
            if (gRaceInfo.mStatusNum < 2) {
                ResMgr::loadCourseData((&SceneCourseSelect::mCourseData[cup*4])[cc].courseID, ResMgr::mcOrder_2);
            }
            else {
                ResMgr::loadCourseData((&SceneCourseSelect::mCourseData[cup*4])[cc].courseID, ResMgr::mcOrder_1);
            }
        }
        else {
            mState = 3;
            ResMgr::loadCourseData(AWARD, ResMgr::mcOrder_2);
        }
        break;
    case 1:
        if (gSequenceInfo._360 != 0) {
            gSystemFile._6024 = 1;
        }
        if (gSequenceInfo._35e != 0) {
            mState = 4;
            ResMgr::loadCourseData(ENDING, ResMgr::mcOrder_2);
        }
        else if (gSequenceInfo._35f != 0) {
            mState = 5;
            ResMgr::loadCourseData(ENDING, ResMgr::mcOrder_2);
        }
        else {
            mState = 1;
            gSequenceInfo._364 = 0;
            if (gSequenceInfo._35d != 0) {
                mNextScene = SCENE_SECRET;
            }
            else {
                mNextScene = SCENE_TITLE;
            }
        }
        break;
    case 2:
        mState = 1;
        gSequenceInfo._364 = 0;
        if (gSequenceInfo._35d != 0) {
            mNextScene = SCENE_SECRET;
        }
        else {
            mNextScene = SCENE_TITLE;
        }
        break;
    default:
#line 182
        JUT_ASSERT(0);
        break;
    }
}

void SceneMap::nextScene() {
    if (SequenceApp::ptr()->ready(mNextScene)) {
        SequenceApp::ptr()->setNextScene(mNextScene);
    }
}

void SceneMap::nextRace() {
    if (ResMgr::isFinishLoadingArc(ResMgr::mcArcCourse)) {
        if (SequenceApp::ptr()->checkFinishAllLoading()) {
            AppMgr::deleteCurrentApp();
            for (int kartNo = 0; kartNo < gRaceInfo.mKartNum; kartNo++) {
                gRaceInfo.setStartPosIdx(kartNo, gSequenceInfo.getStartNo(kartNo));
                gRaceInfo.setPointTable(kartNo, gSequenceInfo.getRankPoint(kartNo));
            }
            if (gRaceInfo.mRaceMode == 2) {
                gRaceInfo.mGpStageNo = gSequenceInfo.mMapInfo.course;
            }
            int slotType = gSystemRecord.mItemSlotType;
            int vsLapNum = gSystemRecord.mVsLapNum;
            gRaceInfo.mItemSlotType = slotType;
            gRaceInfo.mVsLapNum = vsLapNum;
            gRaceInfo.mGpCup = SceneCourseSelect::mRaceGpCup[gSequenceInfo.mMapInfo.cup];
            RaceApp::call();
        }
    }
}

void SceneMap::nextAward() {
    if (ResMgr::isFinishLoadingArc(ResMgr::mcArcCourse)) {
        if (SequenceApp::ptr()->checkFinishAllLoading()) {
            AppMgr::deleteCurrentApp();
            int local[8];
            int local2[8][2];
            ERaceLevel raceLevel = gRaceInfo.getRaceLevel();
            ERaceGpCup cup = SceneCourseSelect::mRaceGpCup[gSequenceInfo.mMapInfo.cup];
            for (int kartNo = 0; kartNo < 8; kartNo++) {
                for (int driverNo = 0; driverNo < 2; driverNo++) {
                    local2[kartNo][driverNo] = gRaceInfo.getKartInfo(kartNo)->getDriverCharID(driverNo);
                }
                local[kartNo] = gRaceInfo.getKartInfo(kartNo)->getKartID();
            }
            int rank = gSequenceInfo.getPlayerRankGP();
            gSequenceInfo.setSecretFlag();
            if ((cup == 4 || cup == 3) && rank == 0 && (gSystemRecord.mGameFlag & (1 << 6))) {
                gSequenceInfo._35f = 1;
            }
            else if ((cup == 4 || cup == 3) && rank >= 0 && rank < 3) {
                gSequenceInfo._35e = 1;
            }
            gRaceInfo.setRace(AWARD_DEMO, 3, 0, 1, 1);
            gRaceInfo.mDemoNextPhase = PHASE_11;
            gSequenceInfo._364 = 1;
            gRaceInfo.mGpCup = cup;
            gRaceInfo.setRaceLevel(raceLevel);
            if (rank >= 0 && rank < 3) {
                gRaceInfo.mAwardKartNo = rank;
            }
            for (int kartNo = 0; kartNo < gRaceInfo.mKartNum; kartNo++) {
                gRaceInfo.setKart(kartNo, (EKartID)((kartNo != 0) ? local[gSequenceInfo.getRank(kartNo)] : 20), (ECharID)local2[gSequenceInfo.getRank(kartNo)][0], NULL, (ECharID)local2[gSequenceInfo.getRank(kartNo)][1], NULL);
            }
            AwardApp::call();
        }
    }
}

void SceneMap::nextEnding() {
    if (ResMgr::isFinishLoadingArc(ResMgr::mcArcCourse)) {
        if (SequenceApp::ptr()->checkFinishAllLoading()) {
            AppMgr::deleteCurrentApp();
            gRaceInfo.settingForStaffRoll(false);
            gRaceInfo.mDemoNextPhase = PHASE_11;
            gSequenceInfo._364 = 2;
            RaceApp::call();
        }
    }
}

void SceneMap::nextTrueEnding() {
    if (ResMgr::isFinishLoadingArc(ResMgr::mcArcCourse)) {
        if (SequenceApp::ptr()->checkFinishAllLoading()) {
            AppMgr::deleteCurrentApp();
            gRaceInfo.settingForStaffRoll(true);
            gRaceInfo.mDemoNextPhase = PHASE_11;
            gSequenceInfo._364 = 2;
            RaceApp::call();
        }
    }
}

#include "JSystem/JAudio/JASFakeMatch2.h"
