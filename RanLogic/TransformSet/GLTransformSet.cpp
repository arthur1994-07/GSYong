#include "pch.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/Memory/SmartPtr.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/G-Logic/GLOGIC.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinDataDummy.h"

#include "../GLogicFile.h"
#include "../GLogicDataMan.h"
#include "../Character/GLCharDefine.h"

#include "GLTransformSet.h"

namespace GLTRANSFORMSET
{
	DWORD g_nTransformChaSet = 0;
	DWORD g_nTransformSet = 0;
	HRESULT LoadFile(const std::string& _FileName, const bool _bPastLoad)
	{		
		std::string FullPath(GLOGIC::GetPath());
		FullPath.append(_FileName);

		std::auto_ptr< sc::BaseStream > pStream(
			GLOGIC::openfile_basestream(
			GLOGIC::UseLogicZipFile(),
			GLogicData::GetInstance().GetGlogicZipFile(),
			FullPath.c_str(), 
			_FileName.c_str(), 
			false,
			GLOGIC::UseLogicPackFile() ) );

		if( !pStream.get() )		
		{
			sc::writeLogError(
				sc::string::format(
				"GLTRANSFORMSET::LoadFile(), File Open %1%", FullPath ) );

			return E_FAIL;
		}

		const size_t nSize = static_cast< size_t >( pStream->GetFileSize() );
		void* pBuffer = ::malloc(nSize);
		pStream->ReadBuffer(pBuffer, 0);
		CTypeSmartPtr _pSmartPtr(pBuffer);

		sc::lua_init();
		if ( !sc::lua_doBuffer(static_cast<char*>(pBuffer), nSize, NULL) )
			return E_FAIL;

		LuaPlus::LuaObject TransformSkinSetGroup = sc::lua_getGlobalFromName("TransformSkinSet");
		if ( (GLTRANSFORMSET::g_nTransformSet = TransformSkinSetGroup.GetCount())  > EMTRANSFORM_SET_MAXSIZE )
			GLTRANSFORMSET::g_nTransformSet = EMTRANSFORM_SET_MAXSIZE;
		

		DWORD dwCounter = 0;
		for ( LuaPlus::LuaTableIterator iterSkinSet(TransformSkinSetGroup); iterSkinSet; iterSkinSet.Next() )
		{
			// 아래의 COMMET는 COMMENT데이터에서 읽어오지 않는다;
			COMMENT::TRANSFORM_SET[dwCounter] = 
				sc::string::unicodeToAnsi((const wchar_t*)iterSkinSet.GetValue().GetWString());
			++dwCounter;
		}
		
		////////////// 연극부 변신 파일 리스트 //////////////////////////////////////////////////////////////////
		LuaPlus::LuaObject TransformCharSet = sc::lua_getGlobalFromName("TRANSFORM_CHAR_SET");
		if ( (GLTRANSFORMSET::g_nTransformChaSet = TransformCharSet.GetCount())  > EMTRANSFORM_CHAR_SET_MAXSIZE )
			GLTRANSFORMSET::g_nTransformChaSet = EMTRANSFORM_CHAR_SET_MAXSIZE;

		dwCounter = 0;

		for ( LuaPlus::LuaTableIterator iterSkinSet(TransformCharSet); iterSkinSet; iterSkinSet.Next(), dwCounter++  )
		{
			COMMENT::TFCHAR_SET[dwCounter] = 
				sc::string::unicodeToAnsi((const wchar_t*)iterSkinSet.GetValue().GetWString());

		}
		
		return S_OK;
	}
};

GLTransformSkinManager::GLTransformSkinManager(void)
{
	const unsigned int _nSize = GLTRANSFORMSET::g_nTransformSet;
	for ( unsigned int _i = _nSize; _i; --_i )
		m_pSkinCharData[_i - 1] = NULL;
}

GLTransformSkinManager::~GLTransformSkinManager(void)
{	
}

void GLTransformSkinManager::Release(void)
{
	const unsigned int _nSize = GLTRANSFORMSET::g_nTransformSet;	
	for ( unsigned int _i = _nSize; _i; --_i )	
	{
		if ( m_pSkinCharData[_i - 1] )
		{
			DxSkinCharDataContainer::GetInstance().ReleaseData(COMMENT::TRANSFORM_SET[_i - 1].c_str());
			m_pSkinCharData[_i - 1] = NULL;
		}		
	}	
}
GLTransformSkinManager& GLTransformSkinManager::GetInstance(void)
{
	static GLTransformSkinManager _Instance;
	return _Instance;
}

DxSkinCharData* const GLTransformSkinManager::GetSkinCharData(const unsigned int iIndex)
{	
	return m_pSkinCharData[iIndex];
}

HRESULT GLTransformSkinManager::LoadFile(LPDIRECT3DDEVICEQ pd3dDevice)
{
	GLTransformSkinManager::Release();
	const unsigned int _nSize = GLTRANSFORMSET::g_nTransformSet;	
	for ( unsigned int _i = _nSize; _i; --_i )
	{
		m_pSkinCharData[_i - 1] = DxSkinCharDataContainer::GetInstance().LoadData(
			COMMENT::TRANSFORM_SET[_i - 1].c_str(), pd3dDevice);
	}
	
	return S_OK;
}

GLTransformSet::GLTransformSet(LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP, EMSCD_FLAGS emFlags) :
m_pd3dDevice(pd3dDevice),
m_bAmbientUP(bAmbientUP),
m_emFlags(emFlags),
m_iTransformIndex(0),
m_iDistinguishIndex(0)
{
	m_pDxSkinChar = new DxSkinChar;
}

GLTransformSet::~GLTransformSet(void)
{
	SAFE_DELETE(m_pDxSkinChar);
}

/**
 * 변신 상태인 경우 해당 변신 DXSkinChar* 반환, 아닌경우 NULL 반환;
 *
 * @param iTransformIndex				[in] 변신ID, 최대 갯수를 넘을수 없음;
 * @param bTransformed					[in] 변신 켬/끔;
 */
DxSkinChar* GLTransformSet::GetTransformSkinChar(const unsigned int iTransformIndex, const bool bTransformed)
{
	if ( bTransformed ) 
	{ // 변신인 경우 인덱스 예외 처리 후 해당 인덱스에 맞는 SkinChar 반환;
		if ( iTransformIndex > (GLTRANSFORMSET::g_nTransformSet - 1) )
			return NULL;
		return GLTransformSet::SwapTransformSkin(iTransformIndex);
	}

	return NULL;
}

/**
 * GetTransformSkinChar() 함수에서 호출하는 내부 함수; 
 * 기존에 들고 있던 SkinChar와 같다면 그대로 반환;
 * 다르다면 새로운 SkinChar 세팅 후 반환;
 *
 * @param iTransformIndex				[in] 변신ID;
*/
DxSkinChar* GLTransformSet::SwapTransformSkin(const unsigned int iTransformIndex)
{
	if ( m_iTransformIndex != iTransformIndex )
	{
		DxSkinCharData* const _pSkinChar = GLTransformSkinManager::GetInstance().GetSkinCharData(iTransformIndex);			

		if ( _pSkinChar == NULL)
			return NULL;

		m_pDxSkinChar->SetCharData(_pSkinChar, m_pd3dDevice, m_bAmbientUP, m_emFlags);
		m_pDxSkinChar->DEFAULTANI();	
		m_pDxSkinChar->StopLoopSound();			

		m_iTransformIndex = iTransformIndex;		
	}
	return m_pDxSkinChar;
}


HRESULT GLTransformSet::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	m_pd3dDevice = pd3dDevice;
	return m_pDxSkinChar->InitDeviceObjects(pd3dDevice);
}
HRESULT GLTransformSet::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	if ( m_pDxSkinChar )
		return m_pDxSkinChar->RestoreDeviceObjects(pd3dDevice);

	return S_OK;
}
HRESULT GLTransformSet::InvalidateDeviceObjects(void)
{
	if ( m_pDxSkinChar )
		return m_pDxSkinChar->InvalidateDeviceObjects();

	return S_OK;
}
HRESULT GLTransformSet::DeleteDeviceObjects(void)
{	
	if ( m_pDxSkinChar )
		return m_pDxSkinChar->DeleteDeviceObjects();

	return S_OK;
}

DxSkinCharData* GLTransformSet::GetDistinguishCharData( WORD wSex, const UINT iDisIndex, const bool bDistinguish )
{
	if( bDistinguish && m_iDistinguishIndex != iDisIndex )
	{
		if ( iDisIndex > (GLTRANSFORMSET::g_nTransformChaSet - 1) )
			return NULL;

		DxSkinCharData* const _pSkinChar = GLDistinguishSkinManager::GetInstance().GetSkinCharSet( wSex, iDisIndex);

		if( _pSkinChar == NULL )	return NULL;

		m_iDistinguishIndex = iDisIndex;

		return _pSkinChar;
	}

	return NULL;
}

void GLTransformSet::CancelDistinguish ( void )
{
	m_iDistinguishIndex = 0;
}

BOOL GLTransformSet::AutoResetDisting( const UINT iDisIndex, const bool bDistinguish )
{
	if( !bDistinguish && m_iDistinguishIndex > 0 )
	{
		m_iDistinguishIndex = 0;

		return TRUE;
	}

	return FALSE;
}

/** 연극부 *****************************************************************************************************************/
GLDistinguishSkinManager::GLDistinguishSkinManager(void)
{
	/*****************************************************************/
	const unsigned int iSize = GLTRANSFORMSET::g_nTransformChaSet;

	for( unsigned int i = 0; i < EMTRANSFORM_CHAR_SEX ; i++ )
	{
		for ( unsigned int j = iSize; j; --j )
		{
			m_pSkinCharSet[i][j] = NULL;
		}
	}	
}

GLDistinguishSkinManager::~GLDistinguishSkinManager(void)
{	
	ReleaseAniSet();
}

void GLDistinguishSkinManager::Release(void)
{	
	const unsigned int iSize = GLTRANSFORMSET::g_nTransformChaSet;	
	/*	
	for ( unsigned int _i = iSize; _i; --_i )	
	{
		if ( m_pSkinCharSet[_i - 1] )
		{
			DxSkinCharDataContainer::GetInstance().ReleaseData(COMMENT::TFCHAR_SET[_i - 1].c_str());
			m_pSkinCharSet[_i - 1] = NULL;
		}		
	}
	*/
	
	for( unsigned int i = 0 ; i < EMTRANSFORM_CHAR_SEX; i++ )
	{
		for ( unsigned int j = iSize; j; --j )
		{
			if ( m_pSkinCharSet[i][j - 1] )
			{
				DxSkinCharDataContainer::GetInstance().ReleaseData(COMMENT::TFCHAR_SET[j - 1].c_str());
				m_pSkinCharSet[i][j - 1] = NULL;
			}		
		}
	}
}

void GLDistinguishSkinManager::ReleaseAniSet(void)
{
	map_iter _iter = m_mapAniSet.begin();

	while( _iter != m_mapAniSet.end() )
	{
		_iter->second.ClearAniSet();

		++_iter;
	}

	_iter = m_mapAniSetW.begin();

	while( _iter != m_mapAniSetW.end() )
	{
		_iter->second.ClearAniSet();

		++_iter;
	}
}

GLDistinguishSkinManager& GLDistinguishSkinManager::GetInstance(void)
{
	static GLDistinguishSkinManager _Instance;
	return _Instance;
}

DxSkinCharData* const GLDistinguishSkinManager::GetSkinCharSet(WORD wSex, const unsigned int iIndex)
{	
	WORD wSexTemp = (wSex == 1) ? 0 : 1;

	return m_pSkinCharSet[wSexTemp][iIndex];
}

HRESULT GLDistinguishSkinManager::LoadFile(LPDIRECT3DDEVICEQ pd3dDevice)
{
	GLDistinguishSkinManager::Release();
	
	const unsigned int iSize = GLTRANSFORMSET::g_nTransformChaSet;

	for( unsigned int i = iSize; i; --i)
	{
		if( (i - 1) != 0 )
		{
			std::string strData = COMMENT::TFCHAR_SET[i - 1].c_str();
			std::string strTemp = strData;

			std::string::size_type iNum = strData.find(".chf");

			if( iNum != std::string::npos ) {
				boost::algorithm::erase_first( strData, ".chf" );		
				strTemp = strData;
			}

			std::string strM = strTemp + "_m.chf";
			std::string strW = strTemp + "_w.chf";

			m_pSkinCharSet[0][i - 1] = DxSkinCharDataContainer::GetInstance().LoadData(	strM.c_str(), pd3dDevice);
			m_pSkinCharSet[1][i - 1] = DxSkinCharDataContainer::GetInstance().LoadData(	strW.c_str(), pd3dDevice);

		}
		else
		{
			m_pSkinCharSet[0][i - 1] = NULL;
			m_pSkinCharSet[1][i - 1] = NULL;
		}
	}

	return S_OK;
}

void GLDistinguishSkinManager::LoadAniSet()
{
	const unsigned int iSize = GLTRANSFORMSET::g_nTransformChaSet;

	for( unsigned int i = iSize; i; --i)
	{
		if( (i - 1) != 0 )
		{
			std::string strData = COMMENT::TFCHAR_SET[i - 1].c_str();
			std::string strTemp = strData;

			std::string::size_type iNum = strData.find(".chf");

			if( iNum != std::string::npos ) {
				boost::algorithm::erase_first( strData, ".chf" );		
				strTemp = strData;
			}

			std::string strM = strTemp + "_m.chf";
			std::string strW = strTemp + "_w.chf";

			LoadAniSet( 0, i - 1, strM.c_str() ); // 남
			LoadAniSet( 1, i - 1, strW.c_str() ); // 여
		}
	}
}

void GLDistinguishSkinManager::LoadAniSet( WORD wSex, DWORD dwIndex, const char* pszName )
{
	if( pszName == NULL || dwIndex == 0 ) return;

	DxSkinDataDummy DataDummy;
	BOOL bOK = DataDummy.LoadFile(pszName);
	if (!bOK)
	{
		std::string Msg(sc::string::format("[%1%] Distinguish Skin File Load Fail.", pszName));
		MessageBox(NULL, Msg.c_str(), "ERROR", MB_OK);
		return;
	}

	size_t nSIZE = DataDummy.m_vecANIMINFO.size();

	for ( size_t n=0; n<nSIZE; ++n )
	{
		const SANIMCONINFO *pAnimConInfo = DataDummy.m_vecANIMINFO[n];

		SANIATTACK sAniAttack;
		sAniAttack.Assign ( pAnimConInfo );

		if( wSex == 0 ) {
			m_mapAniSet[dwIndex].m_ANIMATION[pAnimConInfo->m_MainType][pAnimConInfo->m_SubType].push_back ( sAniAttack );
		}
		else
		if( wSex == 1 ) {
			m_mapAniSetW[dwIndex].m_ANIMATION[pAnimConInfo->m_MainType][pAnimConInfo->m_SubType].push_back ( sAniAttack );
		}
	}	
}

GLDistinguishSkinManager::sTransAni* GLDistinguishSkinManager::GetAniSet( WORD wSex, const unsigned int iIndex )
{
	if( iIndex == 0 ) return NULL;

	WORD wSexTemp = (wSex == 1) ? 0 : 1;

	if( wSexTemp == 0 )
	{
		map_iter _iter = m_mapAniSet.find( iIndex );

		if( _iter == m_mapAniSet.end() ) return NULL;

		return &(_iter->second);
	}
	else
	if( wSexTemp == 1 )
	{
		map_iter _iter = m_mapAniSetW.find( iIndex );

		if( _iter == m_mapAniSetW.end() ) return NULL;

		return &(_iter->second);
	}
	
	return NULL;
}