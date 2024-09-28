#ifndef _DROP_CROW_H_
#define _DROP_CROW_H_

#pragma once

#include "../../EngineLib/G-Logic/GLDefine.h"

struct SDROP_CROW
{
    enum { CROW_GEM=0x0001, };

    SNATIVEID			sNativeID;	//	Crow 고유 ID.

    SNATIVEID			sMapID;		//	맵 ID.
    DWORD				dwCeID;		//	셀 ID.
    DWORD				dwGlobalID;	//	생성 메모리 인덱스용.
    D3DXVECTOR3			vPos;		//	위치.
    D3DXVECTOR3			vDir;		//	방향.
    FLOAT				fScale;		//  크기.

    // 서버 클라이언트간 몹의 위치 동기화를 위해
    D3DXVECTOR3			vStartPos;  // 이동시 시작위치. 추가된거
    NavigationPath::WAYPOINT sNextWaypoint; // 다음 웨이포인트. 추가된거

    DWORD				dwNowHP;	//	생명량.
    WORD				wNowMP;		//	정신량.
    FLOAT               fMultipleHP;//  HP 배수

	FLOAT				fRestorationHP_Plus;
	FLOAT				fRestorationMP_Plus;
	FLOAT				fRestorationSP_Plus;

	FLOAT				fInfluenceY;

	//
	//mjeon.CaptureTheField
	//
	DWORD				arrHP4School[SCHOOL_NUM];
	WORD				wSchoolAuthed;

    DWORD				dwFLAGS;	//	기타 속성.
    EMACTIONTYPE		emAction;	//	행동.
    DWORD				dwActState;	//	행동 상태.

    STARGETID			TargetID;	//	목표 유닛, 위치.

    SDROP_SKILLFACT		sSKILLFACT[CROW_SKILLFACT_SIZE];
    SSTATEBLOW			sSTATEBLOWS[EMBLOW_MULTI];

    SDROP_CROW () 
        : sMapID(NATIVEID_NULL())
        , dwCeID(0)
        , dwGlobalID(0)
        , vPos(0,0,0)
        , vDir(1,0,0)
        , dwNowHP(0)
        , wNowMP(0)
        , dwFLAGS(NULL)
        , emAction(GLAT_IDLE)
        , dwActState(0)
		, wSchoolAuthed(SCHOOL_NONE)
        , fMultipleHP(1.f)
		, fRestorationHP_Plus(0.f)
		, fRestorationMP_Plus(0.f)
		, fRestorationSP_Plus(0.f)
		, fInfluenceY(0.f)
    {
		ZeroMemory(arrHP4School, sizeof(arrHP4School));
    }
};

#endif // _DROP_CROW_H_
