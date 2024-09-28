#ifndef _GLTRANSFORMSET_H_
#define _GLTRANSFORMSET_H_

#include <xstring>
#include "../../EngineLib/DxMeshs/SkinMesh/DxPieceDefine.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAniControl.h"
#include "../RanLogic/Character/GLCharDefine.h"

namespace GLTRANSFORMSET
{	
	extern DWORD g_nTransformChaSet;
	extern DWORD g_nTransformSet;
	extern HRESULT LoadFile(const std::string& _FileName, const bool _bPastLoad);
};

class DxSkinChar;
class DxSkinCharData;
class GLTransformSkinManager
{
	
public:
	DxSkinCharData* const GetSkinCharData(const unsigned int iIndex);
	
	HRESULT LoadFile(LPDIRECT3DDEVICEQ pd3dDevice);
	

private:
	void Release(void);

public:
	static GLTransformSkinManager& GetInstance(void);
	~GLTransformSkinManager(void);

private:
	GLTransformSkinManager(void);

private:
	DxSkinCharData* m_pSkinCharData[EMTRANSFORM_SET_MAXSIZE];	
};

class GLTransformSet
{
public:
	DxSkinChar* GetTransformSkinChar(const unsigned int iTransformIndex, const bool bTransformed);

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects(void);
	HRESULT DeleteDeviceObjects(void);

	// 楷必何 函厘 包访
	BOOL AutoResetDisting( const UINT iDisIndex, const bool bDistinguish );
	DxSkinCharData* GetDistinguishCharData( WORD wSex, const UINT iDisIndex, const bool bDistinguish );

	void CancelDistinguish ( void );

public:
	GLTransformSet(LPDIRECT3DDEVICEQ pd3dDevice, bool _bAmbientUP, EMSCD_FLAGS _emFlags);
	virtual ~GLTransformSet(void);	

private:
	DxSkinChar* SwapTransformSkin(const unsigned int _iTransformIndex);

private:
	LPDIRECT3DDEVICEQ m_pd3dDevice;
	EMSCD_FLAGS m_emFlags;
	DxSkinChar* m_pDxSkinChar;

	unsigned int m_iTransformIndex;
	bool m_bAmbientUP;	

	UINT m_iDistinguishIndex;
};

/******* 楷必何侩 ********************************************************************************************************/

#define DISTINGUISH_INST	GLDistinguishSkinManager::GetInstance()

class GLDistinguishSkinManager
{
public :
	// 楷必何 局聪悸
	struct sTransAni
	{
		VECANIATTACK		m_ANIMATION[AN_TYPE_SIZE][AN_SUB_00_SIZE];

		void ClearAniSet()
		{
			for ( int i=0; i<AN_SUB_00_SIZE; ++i ) {
				for ( int j=0; j<AN_TYPE_SIZE; ++j )
				{
					m_ANIMATION[j][i].clear ();
				}
			}
		}
	};

	typedef std::map<DWORD, sTransAni>	map_transAni;
	typedef map_transAni::iterator		map_iter;
public:
	// 楷必何 函脚
	DxSkinCharData* const GetSkinCharSet(WORD wSex, const unsigned int iIndex);

	HRESULT LoadFile(LPDIRECT3DDEVICEQ pd3dDevice );

	sTransAni* GetAniSet( WORD wSex, const unsigned int iIndex );

	void LoadAniSet();

private:
	void Release(void);
	void ReleaseAniSet(void);

public:
	static GLDistinguishSkinManager& GetInstance(void);
	~GLDistinguishSkinManager(void);

private:
	GLDistinguishSkinManager(void);

	void	LoadAniSet( WORD wSex, DWORD dwIndex, const char* pszName );

private:
	DxSkinCharData* m_pSkinCharSet[EMTRANSFORM_CHAR_SEX][EMTRANSFORM_CHAR_SET_MAXSIZE];

	map_transAni	m_mapAniSet;
	map_transAni	m_mapAniSetW;
};
#endif
