#pragma once

#include <boost/weak_ptr.hpp>

#include "./DxEffectSingleType.h"

#include "../../DxLand/DxLandMan.h"

struct DxEffSingleType
{
	DWORD	TYPEID;
	char	NAME[MAX_PATH];

	DxEffSingleType *pNext;

	DxEffSingleType() :
		TYPEID(0xffffffff),
		pNext(NULL)
	{
		memset( NAME, 0, sizeof(char)*MAX_PATH );
	}

	~DxEffSingleType ()
	{
		SAFE_DELETE(pNext);
	}
};

//	Note : �̱��� �������� �Ǿ� �ִ�. (���� ��ü�� �����Ѵ�. )
//			����Ʈ ������ ����ؼ� ��ϵ� ����Ʈ���� �̸���, TYPE_ID�� ����Ʈ�� ������
//			����Ʈ�� ���� �ڿ����� �ε��ϰ� �����Ѵ�. ( static ��� �Լ����� ȣ���Ͽ� �̼ҽ��� �о 
//			�ʱ�ȭ �Ͽ� static��� ������ �����صθ�, ���� �Լ��� ȣ���Ͽ� ���ҽ��� �����. )
//
class DxEffSingleMan
{
protected:
	void RegistType ( const DWORD TypeID, const char *Name );

protected:
	DxEffSingleType	*pEffectTypeList;

public:
	DxEffSingleType* GetEffectList ()	{ return pEffectTypeList; }

protected:
	boost::weak_ptr<DxLandMan>	m_wpLandMan;

public:
	void SetLandMan ( boost::shared_ptr<DxLandMan>& spLandMan )		{ m_wpLandMan = spLandMan; }
	boost::shared_ptr<DxLandMan> GetLandMan()						{ return m_wpLandMan.lock(); }

public:
	HRESULT OneTimeSceneInit();
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT FinalCleanup();

protected:
	DxEffSingleMan(void);

public:
	~DxEffSingleMan(void);

public:
	static DxEffSingleMan& GetInstance();
};

//DxEffSingleMan::GetInstance().GetLandMan()->GetOctree();