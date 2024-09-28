#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <list>
#include <map>
#include <string>

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../Common/ByteStream.h"

#include "DxEffChar.h"
#include "../../DxMeshs/SkinMesh/DxSkinChar.h"

class DxEffCharData
{
public:
	struct SEFFDATA
	{
		se::ByteStream	m_byteStream;

		SEFFDATA ()
		{
		}
	};

private:
	typedef std::list<SEFFDATA*>			EFFDATALIST;
	typedef std::list<SEFFDATA*>::iterator	EFFDATALIST_ITER;

protected:
	std::string		m_strFileName;
	EMPIECECHAR		m_emType;
	EFFDATALIST		m_listEffData;

public:
	EMPIECECHAR GetPieceType ()								{ return m_emType; }
	void SetFileName ( std::string strFileName )			{ m_strFileName = strFileName; }

public:
	BOOL SetPart ( DxCharPart *pCharPart, LPDIRECT3DDEVICEQ	pd3dDevice, const D3DXVECTOR3 *pDir=NULL, float fTarDir=0.0f );
	BOOL SetPiece ( DxSkinPiece *pSkinPiece, LPDIRECT3DDEVICEQ	pd3dDevice, const D3DXVECTOR3 *pDir=NULL, float fTarDir=0.0f );

public:
	void ConvertTPoint( LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL Import ( EMPIECECHAR _emType, GLEFFCHAR_VEC &vecEFFECT );
	void CleanUp ();

public:
	BOOL LoadFile(sc::BaseStream& SFile);
	BOOL SaveFile(sc::SerialFile& SFile);

    BOOL LoadFile(const std::string& FileName);
	BOOL SaveFile(const char* szFile);

public:
	DxEffCharData(void);
	~DxEffCharData(void);
};
typedef DxEffCharData* PEFFCHARDATA;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------


class DxEffCharDataArray
{
protected:
	std::string		m_strFileName;
	PEFFCHARDATA	m_pEffCharDataArray[PIECE_SAVE_SIZE];

public:
	BOOL SetSkinPart ( PDXCHARPART pCharPart, EMPIECECHAR emType, LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3 *pDir=NULL, float fTarDir=0.0f );
	BOOL SetSkinChar ( DxSkinChar *pSkinChar, LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3 *pDir=NULL, float fTarDir=0.0f );
	BOOL SetSkinCharData ( DxSkinCharData *pSkinCharData, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void ConvertTPoint( LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL Import ( DxSkinCharData *pSkinCharData, LPDIRECT3DDEVICEQ pd3dDevice );
	void CleanUp ();

public:
    BOOL LoadFile(const std::string& FileName);
	BOOL SaveFile(const char* szFile);

public:
	DxEffCharDataArray(void)
	{
		memset ( m_pEffCharDataArray, 0x00, sizeof(PEFFCHARDATA)*PIECE_SAVE_SIZE );
	}

	~DxEffCharDataArray(void)
	{
		CleanUp ();
	}
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

class DxEffcharDataMan
{
protected:
	std::string			m_strPath;
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	void SetPath ( char *szPath )	{ m_strPath = szPath; }
	const char* GetPath ()			{ return m_strPath.c_str(); }
	
private:
    typedef std::tr1::unordered_map<std::string, DxEffCharData*> EFFDATA_MAP;
	typedef EFFDATA_MAP::iterator		                         EFFDATA_MAP_ITER;    
    typedef EFFDATA_MAP::value_type		                         EFFDATA_MAP_VALUE;

    typedef std::tr1::unordered_map<std::string, DxEffCharDataArray*> EFFDATA_ARRAYMAP;
	typedef EFFDATA_ARRAYMAP::iterator	                              EFFDATA_ARRAYMAP_ITER;    
    typedef EFFDATA_ARRAYMAP::value_type                              EFFDATA_ARRAYMAP_VALUE;

protected:
	EFFDATA_MAP			m_mapEffData;
	EFFDATA_ARRAYMAP	m_mapEffDataArray;

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	BOOL PutEffect ( DxCharPart    *pCharPart, const char* szEffFile, const D3DXVECTOR3 *pDir, float fTarDir=0.0f );
	BOOL PutEffect ( DxAttBoneLink *pAttLink , const char* szEffFile, const D3DXVECTOR3 *pDir, float fTarDir=0.0f );
	BOOL PutEffect ( DxSkinChar    *pSkinChar, const char* szEffFile, const D3DXVECTOR3 *pDir, float fTarDir=0.0f );

	BOOL PutPassiveEffect ( DxSkinChar *pSkinChar, const char* szEffFile, const D3DXVECTOR3 *pDir, float fTarDir=0.0f );
	void OutEffect ( DxSkinChar *pSkinChar, std::string strEffFile );

	void CleanUp ();

private:
	DxEffcharDataMan ()
	{
	}

public:
	~DxEffcharDataMan ()
	{
		CleanUp ();
	}

public:
	static DxEffcharDataMan& GetInstance();
};


