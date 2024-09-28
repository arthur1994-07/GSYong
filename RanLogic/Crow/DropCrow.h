#ifndef _DROP_CROW_H_
#define _DROP_CROW_H_

#pragma once

#include "../../EngineLib/G-Logic/GLDefine.h"

struct SDROP_CROW
{
    enum { CROW_GEM=0x0001, };

    SNATIVEID			sNativeID;	//	Crow ���� ID.

    SNATIVEID			sMapID;		//	�� ID.
    DWORD				dwCeID;		//	�� ID.
    DWORD				dwGlobalID;	//	���� �޸� �ε�����.
    D3DXVECTOR3			vPos;		//	��ġ.
    D3DXVECTOR3			vDir;		//	����.
    FLOAT				fScale;		//  ũ��.

    // ���� Ŭ���̾�Ʈ�� ���� ��ġ ����ȭ�� ����
    D3DXVECTOR3			vStartPos;  // �̵��� ������ġ. �߰��Ȱ�
    NavigationPath::WAYPOINT sNextWaypoint; // ���� ��������Ʈ. �߰��Ȱ�

    DWORD				dwNowHP;	//	����.
    WORD				wNowMP;		//	���ŷ�.
    FLOAT               fMultipleHP;//  HP ���

	FLOAT				fRestorationHP_Plus;
	FLOAT				fRestorationMP_Plus;
	FLOAT				fRestorationSP_Plus;

	FLOAT				fInfluenceY;

	//
	//mjeon.CaptureTheField
	//
	DWORD				arrHP4School[SCHOOL_NUM];
	WORD				wSchoolAuthed;

    DWORD				dwFLAGS;	//	��Ÿ �Ӽ�.
    EMACTIONTYPE		emAction;	//	�ൿ.
    DWORD				dwActState;	//	�ൿ ����.

    STARGETID			TargetID;	//	��ǥ ����, ��ġ.

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
