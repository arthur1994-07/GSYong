#pragma once

#include "../../SigmaCore/File/SerialFile.h"
#include "../../EngineLib/Common/ByteStream.h"

struct STAXI_STATION
{
public:
	enum
	{
		VERSION	= 0x0100,		
	};

public:
	DWORD		dwLINKID;
	bool		bUsed;
	DWORD		dwMAPID;
	WORD		wPosX;
	WORD		wPosY;
	DWORD		dwNPCID;
	DWORD		dwMapCharge;
	std::string	strMAP;
	std::string	strSTATION;

	STAXI_STATION ()
		: dwLINKID(0)
		, bUsed(false )
		, dwMAPID(UINT_MAX)
		, wPosX(0)
		, wPosY(0)
		, dwNPCID(UINT_MAX)
		, dwMapCharge(0)
	{
	}

	STAXI_STATION& operator= ( const STAXI_STATION& value )
	{
		bUsed = value.bUsed;
		dwLINKID = value.dwLINKID;
        dwMAPID = value.dwMAPID;
		wPosX = value.wPosX;
		wPosY = value.wPosY;
		dwNPCID = value.dwNPCID;
		strMAP = value.strMAP;
		dwMapCharge = value.dwMapCharge;
		strSTATION = value.strSTATION;

		return *this;
	}

	BOOL LoadFile( sc::BaseStream &SFile );
	BOOL SaveFile( sc::SerialFile &SFile );

	static void SaveCsvHead ( std::fstream &SFile );
	bool        SaveCsv  ( std::fstream &SFile );
	bool	    LoadCsv  ( CStringArray &StrArray );
};

typedef std::vector<STAXI_STATION> VEC_STATION;
typedef VEC_STATION::iterator	   VEC_STATION_ITER;

struct STAXI_MAP
{
	DWORD						dwMAPID;
	std::string					strMAP;
	VEC_STATION					vecTaxiStation;

	STAXI_MAP () :
		dwMAPID(0)
	{
	}

	STAXI_MAP& operator= ( const STAXI_MAP& value )
	{
		dwMAPID = value.dwMAPID;
		strMAP = value.strMAP;
		vecTaxiStation = value.vecTaxiStation;
		return *this;
	}

	DWORD GetStationNum ()
	{
		return (DWORD) vecTaxiStation.size();
	}

	STAXI_STATION* GetStation ( DWORD dwIndex )
	{
		
		if ( vecTaxiStation.size() <= dwIndex )
			return NULL;

		return &vecTaxiStation[dwIndex];
	}
};

typedef std::vector<STAXI_MAP>	TAXI_VECTOR;
typedef TAXI_VECTOR::iterator	TAXI_VECTOR_ITER;

class GLTaxiStation 
{
public:
	enum
	{
		VERSION	= 0x0100,		
	};

	static const char* _FILEHEAD;

public:
	virtual ~GLTaxiStation(void);

protected:
	std::string				m_strPATH;
	char					m_szFileName[MAX_PATH];

public:
	void SetPath ( std::string strPATH )	{ m_strPATH = strPATH; }
	std::string GetPath ()					{ return m_strPATH.c_str(); }
	DWORD GetBasicCharge() { return m_dwBasicCharge; }

public:
	TAXI_VECTOR	m_vecTaxiMap;
	DWORD		m_dwBasicCharge;

protected:
	GLTaxiStation(void);	
	GLTaxiStation( const GLTaxiStation& value);	

public:
    HRESULT LoadFile(const std::string& FileName);
	HRESULT LoadFile_OLD(const char* szFile, bool bToolMode);
	HRESULT SaveFile ( const char* szFile );

	bool    SaveCsv  ( std::fstream &SFile );
	bool	LoadCsv  ( std::string& strFileName );

	void    insert( STAXI_STATION &sSTATION );
	
public:
	int StationCount();
	void CleanUp();

	DWORD GetTaxiMapNum();

	STAXI_MAP* GetTaxiMap ( int nIndex );
	STAXI_STATION* GetStation ( int nMapIndex, int nStationIndex );

public:
	static GLTaxiStation& GetInstance ();
};
