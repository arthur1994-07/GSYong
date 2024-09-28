#pragma once
#include "../EngineLib/G-Logic/GLOGIC.h"
#include "../../RanLogic/GLogicFile.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/GLogicDataMan.h"

class DxSkinChar;


class SkinCharMultiData
{
	SkinCharMultiData()
	{
	}
	virtual ~SkinCharMultiData()
	{
	}

	std::vector<D3DXVECTOR3> m_vecPosXYZ;

public:
	static SkinCharMultiData& GetInstance()
	{
		{
			static SkinCharMultiData instance;
			return instance;
		}

	}
public:
	HRESULT LoadFile(const std::string& FileName)
	{
		m_vecPosXYZ.clear();

		try
		{
			std::string FullPath(GLOGIC::GetPath());
			FullPath.append(FileName);

			std::auto_ptr< sc::BaseStream > pStream(
				GLOGIC::openfile_basestream(
				GLOGIC::UseLogicZipFile(),
				GLogicData::GetInstance().GetGlogicZipFile(),
				FullPath.c_str(), 
				FileName.c_str(), 
				false,
				GLOGIC::UseLogicPackFile() ) );

			if( !pStream.get() )		
			{
				sc::writeLogError(
					sc::string::format(
					"ClonSkill::LoadFile(), File Open %1%", FullPath ) );

				return E_FAIL;
			}

			const size_t nSize = static_cast< size_t >( pStream->GetFileSize() );
			void* pBuffer = ::malloc(nSize);
			pStream->ReadBuffer(pBuffer, 0);
			CTypeSmartPtr _pSmartPtr(pBuffer);

			sc::lua_init();
			if (!sc::lua_doBuffer(static_cast<char*>(pBuffer), nSize, NULL) )
			{        
				MessageBox( NULL, sc::lua_getError().c_str(), FileName.c_str(), MB_OK );
				return false;
			}

			//if (!sc::lua_doFile(FullPath.c_str()))
			//{        
			//	::MessageBoxA(
			//		NULL,
			//		sc::lua_getError().c_str(),
			//		FileName.c_str(),
			//		MB_OK);
			//	return false;
			//}

			LuaPlus::LuaObject positionList = sc::lua_getGlobalFromName("positionList");
			for (LuaPlus::LuaTableIterator OptionIter(positionList); OptionIter; OptionIter.Next())
			{
				D3DXVECTOR3 PosXYZ;
				for (LuaPlus::LuaTableIterator XYZiter(OptionIter.GetValue()); XYZiter; XYZiter.Next())
				{
					int IntType = XYZiter.GetValue().GetInteger();

					PosXYZ.x = static_cast<float>(XYZiter.GetValue().GetInteger());
					XYZiter.Next();
					PosXYZ.y = static_cast<float>(XYZiter.GetValue().GetInteger());
					XYZiter.Next();
					PosXYZ.z = static_cast<float>(XYZiter.GetValue().GetInteger());
				}
				m_vecPosXYZ.push_back(PosXYZ);
			}
			return true;
		}
		catch (const LuaPlus::LuaException& e)
		{        
			MessageBox( NULL, e.GetErrorMessage(), FileName.c_str(), MB_OK );

			return false;
		}
	}
	std::vector<D3DXVECTOR3> GetVecPosition()
	{
		return m_vecPosXYZ;
	}

};

class SkinCharMulti
{
public:
	SkinCharMulti();
	SkinCharMulti( DxSkinCharData* pCharData, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP, DWORD dwFlags );
	virtual ~SkinCharMulti();

private:
	DxSkinCharData* m_pCharData;
	bool m_bAmbientUP;
	DWORD m_dwFlags;
	LPDIRECT3DDEVICEQ m_pd3dDevice;
	std::vector<DxSkinChar*> m_vecSkinCharMulti;
	BOOL m_bIsUseSkinCharMulti;
	int m_nSkinCount;

public:
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	BOOL SetUseSkinCharMulti( BOOL bUse, int nNum );

	void RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bOpaqueState, BOOL bHALF_VISIBLE, const D3DXVECTOR3 &vDiffuse, const D3DXVECTOR3 &vPointLight, BOOL bMy );

	BOOL PutPassiveEffect( const char* szEffFile, const D3DXVECTOR3 *pDir, float fTarDir=0.0f );
	BOOL OutEffect( std::string strEffFile );

	//void CalculateAmbientColor( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan, const D3DXVECTOR3& vPos );
	void SELECTSKILLANI( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE );
	BOOL IsANI_SELECTANI( const TCHAR* szAnim, DWORD dwFlags=NULL );
	BOOL IsANI_SELECTANI( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE, DWORD dwFlags=NULL, DWORD dwSelect=0 );
	void SetPosition( const D3DXVECTOR3& vPos );

	HRESULT FrameMove( const float fTime, 
		const float fElapsedTime,
		BOOL bNoneOverFrame,
		const D3DXMATRIX &matRot, 
		const BOOL bContinue=TRUE, 
		BOOL const bFreeze=FALSE, 
		const BOOL bAttackMode=TRUE, 
		const BOOL bBoardRender = FALSE );

	void ClearReserveAction();

	D3DXMATRIX D3DXmatrix(D3DXVECTOR3 vecPos);

	void ResetSkinCharMulti();
	void SetCharData ( DxSkinCharData* pCharData, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP, DWORD dwFlags );	// dwFlags - EMFLAGS_SetCharData
	void SetHairColor( WORD wColor );
	void ResetPiece ( int i );

	HRESULT SetPiece( 
		const TCHAR* szFile,
		LPDIRECT3DDEVICEQ pd3dDevice,
		DWORD dwFlag, 
		int nLevel, 
		EMSCD_FLAGS emFlags,
		BOOL bAttachSub=FALSE,
		EMPIECECHAR* pOutPiecetype=NULL,
		const char* pSelfBodyEffect=NULL );

	HRESULT SetPiece( 
		EMPIECECHAR emPieceChar,
		const std::string& strFileChange, 
		LPDIRECT3DDEVICEQ pd3dDevice,
		DWORD dwFlag, 
		int nLevel, 
		EMSCD_FLAGS emFlags,
		BOOL bAttachSub=FALSE,
		EMPIECECHAR* pOutPiecetype=NULL,
		const char* pSelfBodyEffect=NULL );

	void SELECTANI( const TCHAR* szAnim, DWORD dwFlags=NULL);
	void SELECTANI( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE, DWORD dwFlags=NULL, DWORD dwSelect=0);
	void DEFAULTANI();
	void TOSTARTTIME();
	void TOENDTIME();
	void SetPartRend( EMPIECECHAR emPIECE, BOOL bRender );
	void DOSHOCKMIX();
	void SetSkinCount( int nCount );
	void ResetSkinCount();
	BOOL IsUpdateSuit();


};