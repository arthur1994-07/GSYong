
#pragma	once

#include "./UIGroup.h"

class CSwapImage : public CUIGroup
{
protected:
	enum
	{
		SWAPIMAGE_NONE = NO_ID,
	};

public:
	CSwapImage (EngineDeviceMan* pEngineDevice);
	virtual	~CSwapImage ();

public:
	virtual HRESULT		InitDeviceObjects   ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual void		SetImage		( const char* szControlKeyword );
	virtual void		SetImageEx		( const char* szControlKeyword, const UINT32 nTexPosX, const UINT32 nTexPosY, const UINT32 nTexWidth, const UINT32 nTexHeight );
	virtual void		SetImageTexture	( const char* szTextureName );
	void				SetImageClass   ( DWORD iClassId );
	void				SetImageSchool  ( DWORD School );

public:
	virtual void		SetImageResizing ( const char* szControlKeyword );

private:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

};