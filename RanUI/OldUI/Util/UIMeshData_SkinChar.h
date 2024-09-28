#pragma once

// bjju 11.03.08

#include "./UIMeshData.h"
#include "../../../EngineLib/DxMeshs/DxAniKeys.h"
#include "../../../EngineLib/DxMeshs/SkinMesh/DxPieceDefine.h"

class DxSkinChar;

class CUIMeshData_SkinChar : public IUIMeshData
{

public:
    CUIMeshData_SkinChar();
    virtual ~CUIMeshData_SkinChar();

public:
    virtual HRESULT InitDeviceObjects       ( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual HRESULT RestoreDeviceObjects    ( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual HRESULT InvalidateDeviceObjects ();
    virtual HRESULT DeleteDeviceObjects     ();

public:
    virtual void       FrameMove  ( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime );
    virtual void       Render     ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual const BOOL Load       ( LPDIRECT3DDEVICEQ pd3dDevice, const std::string& szFile );

public:
    HRESULT            SetPiece      ( const TCHAR* szFile, DWORD dwFlag=0x0, int nLevel=0, bool bCharacterData=FALSE, BOOL bAttachSub=FALSE, EMPIECECHAR* pOutPiecetype=NULL);
    void               ResetPiece    ( const EMPIECECHAR emPiece );
	bool               IsSettingPiece( const EMPIECECHAR emPiece );

    const BOOL         SetEffect     ( const TCHAR* szFile, const EMPIECECHAR emPieceChar, D3DXVECTOR3& vDir );

    const BOOL         SelectAni     ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE, DWORD dwFlags=NULL, DWORD dwSelect=0 );
    const BOOL         SetMainColor  ( const EMPIECECHAR emPiece, const DWORD dwColor );
    const BOOL         SetSubColor   ( const EMPIECECHAR emPiece, const DWORD dwColor );
 //   void               SetAttackMode ( const BOOL bEnable ) { m_bAttackMode = bEnable; }
public:
    void               GetDefaultOverlayColor ( const EMPIECECHAR emPiece, DWORD& dwMainColor, DWORD& dwSubColor );
    const float        GetHeight ();

	void SetPeaceMode(BOOL isPeace);
private:
	float		m_fTime;
    DxSkinChar* m_pSkinChar;
//	DxVehicleData	m_VehicleData;		
private:
    BOOL        m_bAttackMode;

};
