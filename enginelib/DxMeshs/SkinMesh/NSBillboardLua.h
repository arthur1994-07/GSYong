#pragma once

class BILLBOARD_CHF;
class BILLBOARD_CHF_DATA;

namespace NSBillboard
{
	//////////////////////////////////////////////////////////////////////////
	//								Path
	void SetTexturePath( const TCHAR* pName );
	const TCHAR* GetTexturePath();
	//								Path
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//						CharEdit ������ �����.
	void ImportBillboard( const BILLBOARD_CHF_DATA*	pBillboardChfData );
	const BILLBOARD_CHF_DATA* GetBillboardChfData();
	//						CharEdit ������ �����.
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//							pushBillboard
	void CleanUp();
	boost::shared_ptr<BILLBOARD_CHF> pushBillboard( const TCHAR* pNameChf );
	//							pushBillboard
	//////////////////////////////////////////////////////////////////////////
};

namespace NSBillboardLua
{
	struct BILLBOARD
	{
		TSTRING	m_strBillboardData;
		TSTRING	m_strBillboardTexture;
	};

	void SetPath( const TCHAR* pName );
	const TCHAR* GetPath();

	BOOL LuaLoad();
	const BILLBOARD* FindBillboard( const TCHAR* szNameChf );
};