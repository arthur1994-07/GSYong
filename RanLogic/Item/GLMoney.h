#ifndef GLMONEY_H_
#define GLMONEY_H_

#pragma once

#include "../../SigmaCore/Container/GLList.h"
#include "../../EngineLib/DxTools/DxCustomTypes.h"
#include "../../EngineLib/DxLand/DxLandDef.h"
#include "../../EngineLib/G-Logic/GLQuadTree.h"
#include "../../EngineLib/G-Logic/GLDefine.h"

#include "../Party/GLPartyDefine.h"
#include "../Character/GLCharDefine.h"
#include "../Item/GLItemDef.h"

//	Note :MONEY�� �⺻ ���� ����.
//
struct SDROP_MONEY
{
	SNATIVEID	sMapID;					//	
	DWORD		dwCeID;					//	�� ID.
	DWORD		dwGlobID;				//	���� �޸� �ε�����.
	D3DXVECTOR3	vPos;					//	�� ��ġ.

	LONGLONG	lnAmount;				//	��ġ.

	BOOL		bCanGet;				//  ������ ���� ���� ����

	SDROP_MONEY () :
		dwCeID(0),
		dwGlobID(0),
		vPos(0,0,0),
		lnAmount(0),
		bCanGet(FALSE)
	{
	}
};

#ifndef GLLandMan
	class GLLandMan;
	class GLLandNode;
	typedef SQUADNODE<GLLandNode>		LANDQUADNODE;
#endif //GLLandMan

#ifndef GLLandManClient
	class GLClientNode;
	class GLLandNode;
	typedef SQUADNODE<GLClientNode>		CLIENTQUADNODE;
#endif //GLLandMan


//	Note : Server �� CMoneyDrop ���� Ŭ����.
//
class CMoneyDrop
{
public:
	union
	{
		struct
		{
		SDROP_MONEY			sDrop;
		};

		struct
		{
		SNATIVEID			sMapID;					//	�� ID.
		DWORD				dwCeID;					//	�� ID.
		DWORD				dwGlobID;				//	���� �޸� �ε�����.
		D3DXVECTOR3			vPos;					//	�� ��ġ.
		LONGLONG			lnAmount;				//	��ġ.
		};
	};

public:
	float					fAge;			//	��� �ð�.

	EMGROUP					emGroup;		//	�ӽ� �������� ����.
	DWORD					dwHoldGID;		//	�ӽ� ������ ��.

	bool					bDropMonster;	// ����Ʈ�� ��� üũ, 0: ĳ���� 1: ����

	LANDQUADNODE*			pQuadNode;		//	���� Ʈ�� ���. ( ���� ������ ���� �����ϱ� ������. )
	sc::SGLNODE<CMoneyDrop*>*	pCellList;		//	�� ����Ʈ ���.
	//sc::SGLNODE<CMoneyDrop*>*	pGlobList;		//	���� ����Ʈ ���.

public:
	bool IsTakeItem(const GLPartyID& dwPartyID, const DWORD dwGaeaID);
	bool IsDropOut();

	void Update ( float fElapsedTime );

public:
	CMoneyDrop(void) :
		fAge(0.0f),
		dwHoldGID(GAEAID_NULL),

		pQuadNode(NULL),
		pCellList(NULL),
		//pGlobList(NULL),
		bDropMonster(FALSE)
	{
	}
};

typedef CMoneyDrop* PMONEYDROP;

typedef sc::CGLLIST<PMONEYDROP>		MONEYDROPLIST;
typedef sc::SGLNODE<PMONEYDROP>		MONEYDROPNODE;

class DxSimMesh;
class DxLandMan;
struct ActorNameCollision;

//	Note : Client �� CMoneyDrop ���� Ŭ����.
//
class CMoneyClientDrop
{
public:
	union
	{
		struct
		{
		SDROP_MONEY			sDrop;
		};

		struct
		{
		SNATIVEID			sMapID;					//	�� ID.
		DWORD				dwCeID;					//	�� ID.
		DWORD				dwGlobID;				//	���� �޸� �ε�����.
		D3DXVECTOR3			vPos;					//	�� ��ġ.
		LONGLONG			lnAmount;				//	��ġ.
		BOOL				bCanGet;				//  ������ ���� ���� ����
		};
	};

public:
	float						fAge;			//	��� �ð�.
	float						fNextMsgDelay;	//	�̹� �������� �޽��� ������ ��� �������� �õ����ɽð� ����.

	D3DXMATRIX					matWld;			//	������ ��ġ.
	DxSimMesh*					pMoneyMesh;

	//CLIENTQUADNODE*				pQuadNode;		//	���� Ʈ�� ���. ( ���� ������ ���� �����ϱ� ������. )
	//sc::SGLNODE<CMoneyClientDrop*>*	pCellList;		//	�� ����Ʈ ���.
	//sc::SGLNODE<CMoneyClientDrop*>*	pGlobList;		//	���� ����Ʈ ���.

public:
	CMoneyClientDrop(void);

private:
	std::tr1::shared_ptr<ActorNameCollision>	m_spNameCollision;

public:
	BOOL	IsVisibleDisplayName() const;
	bool IsCollision ( const D3DXVECTOR3 &vFromPt, const D3DXVECTOR3 &vTargetPt ) const;
	bool IsCollision ( const CLIPVOLUME &cv ) const;
	void FrameMove( const boost::shared_ptr<DxLandMan>& spLandMan, float fElapsedTime );
	HRESULT RenderItem ( LPDIRECT3DDEVICEQ pd3dDevice, D3DMATERIALQ* pMaterials=NULL );
};

typedef CMoneyClientDrop*			PMONEYCLIENTDROP;

typedef sc::CGLLIST<PMONEYCLIENTDROP>	MONEYCLIENTDROPLIST;
typedef sc::SGLNODE<PMONEYCLIENTDROP>	MONEYCLIENTDROPNODE;

#endif // GLMONEY_H_