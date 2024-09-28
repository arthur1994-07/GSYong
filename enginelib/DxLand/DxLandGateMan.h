#pragma once

#include "../../InternalCommonLib/dxstdafx.h"
#include "../DxTools/DxCustomTypes.h"
#include "../DxTools/Collision.h"
#include "../G-Logic/GLDefine.h"

namespace sc {
    class BaseStream;
}

class sc::SerialFile;
class NavigationMesh;

class DxLandGate : public OBJAABB
{
public:
	enum
	{
		VERSION		= 0x0103,
		MAX_SZNAME	= 65,

		GATE_IN		= 0x0001,
		GATE_OUT	= 0x0002,

		GATETO_NSIZE = 8, // 값 변경시 버전 올려주고 해당 버전에 맞는 작업 해주어야 함;

		GEN_NULL	= -1,

		GATE_ALL	= (GATE_IN|GATE_OUT),
	};

	enum EMGENPOS_SEL
	{
		GEN_FIRST	= 1,
		GEN_RENDUM	= 2,
	};

	struct PROPERTY_100
	{
		char			m_szName[MAX_SZNAME];
		DWORD			m_dwGeteFlags;
		DWORD			m_GateID;

		SNATIVEID		m_ToMapID;
		DWORD			m_ToGateID;
		D3DXVECTOR2		m_vDiv;

		D3DXVECTOR3		m_vMax;
		D3DXVECTOR3		m_vMin;

		DWORD			m_dwStartPNum;

		PROPERTY_100()
		{
			memset( m_szName, 0, sizeof(char)*MAX_SZNAME );
		}
	};

    struct PROPERTY_101
    {
        char			m_szName[MAX_SZNAME];
        DWORD			m_GateID;
        DWORD			m_dwGeteFlags;

        DWORD			m_dwStartPNum;

        SNATIVEID		m_ToMapID;
        DWORD			m_ToGateID;

        D3DXVECTOR2		m_vDiv;
        D3DXVECTOR3		m_vMin;
        D3DXVECTOR3		m_vMax;

        PROPERTY_101()
        {
            memset( m_szName, 0, sizeof(char)*MAX_SZNAME );
        }
    };
	struct PROPERTY_102
	{
		char			m_szName[MAX_SZNAME];
		char			m_szEffName[MAX_SZNAME];
		DWORD			m_GateID;
		DWORD			m_dwGeteFlags;

		DWORD			m_dwStartPNum;

		SNATIVEID		m_ToMapID;
		DWORD			m_ToGateID;

		D3DXVECTOR2		m_vDiv;
		D3DXVECTOR3		m_vMin;
		D3DXVECTOR3		m_vMax;

		PROPERTY_102()
		{
			memset( m_szName, 0, sizeof(char)*MAX_SZNAME );
			memset( m_szEffName, 0, sizeof(char)*MAX_SZNAME );
		}
	};

	struct PROPERTY
	{
		char			m_szName[MAX_SZNAME];
        char			m_szEffName[MAX_SZNAME];
		DWORD			m_GateID;
		DWORD			m_dwGeteFlags;

		DWORD			m_dwStartPNum;

		char				m_szGateNameKey[GATETO_NSIZE][MAX_SZNAME];
		DWORD			m_nGateNameIndex[GATETO_NSIZE];
		SNATIVEID		m_ToMapID[GATETO_NSIZE];
		DWORD			m_ToGateID[GATETO_NSIZE];
		DWORD			m_nToGate;

		D3DXVECTOR2		m_vDiv;
		D3DXVECTOR3		m_vMin;
		D3DXVECTOR3		m_vMax;

		PROPERTY()
			: m_nToGate(0)
		{
			memset( m_szName, 0, sizeof(char)*MAX_SZNAME );
            memset( m_szEffName, 0, sizeof(char)*MAX_SZNAME );			
			ResetToGateMapID();
		}

		void ResetToGateMapID(void)
		{
			m_nToGate = 0;
			for ( DWORD _i(GATETO_NSIZE); _i--; )
			{
				memset( m_szGateNameKey[_i], 0, sizeof(char)*MAX_SZNAME );
				m_nGateNameIndex[_i] = -1;
				m_ToMapID[_i] = SNATIVEID(false);
				m_ToGateID[_i] = 0;
			}
		}
		void Assign( PROPERTY_102& sOldData )
		{
			StringCchCopy( m_szName, MAX_SZNAME, sOldData.m_szName );
			StringCchCopy( m_szEffName, MAX_SZNAME, sOldData.m_szEffName );
			m_GateID = sOldData.m_GateID;
			m_dwGeteFlags = sOldData.m_dwGeteFlags;

			m_dwStartPNum = sOldData.m_dwStartPNum;

			m_ToMapID[0] = sOldData.m_ToMapID;
			m_ToGateID[0] = sOldData.m_ToGateID;
			m_nToGate = 1;

			m_vDiv = sOldData.m_vDiv;
			m_vMin = sOldData.m_vMin;
			m_vMax = sOldData.m_vMax;
		}

        void Assign( PROPERTY_101& sOldData )
        {
            StringCchCopy( m_szName, MAX_SZNAME, sOldData.m_szName );
            m_GateID = sOldData.m_GateID;
            m_dwGeteFlags = sOldData.m_dwGeteFlags;

            m_dwStartPNum = sOldData.m_dwStartPNum;

            m_ToMapID[0] = sOldData.m_ToMapID;
            m_ToGateID[0] = sOldData.m_ToGateID;
			m_nToGate = 1;

            m_vDiv = sOldData.m_vDiv;
            m_vMin = sOldData.m_vMin;
            m_vMax = sOldData.m_vMax;
        }

		void Assign( PROPERTY_100& sOldData )
		{
			StringCchCopy( m_szName, MAX_SZNAME, sOldData.m_szName );            
			m_GateID = sOldData.m_GateID;
			m_dwGeteFlags = sOldData.m_dwGeteFlags;

			m_dwStartPNum = sOldData.m_dwStartPNum;

			m_ToMapID[0] = sOldData.m_ToMapID;
			m_ToGateID[0] = sOldData.m_ToGateID;
			m_nToGate = 1;

			m_vDiv = sOldData.m_vDiv;
			m_vMin = sOldData.m_vMin;
			m_vMax = sOldData.m_vMax;
		}
	};

protected:
	union
	{
		struct
		{
			PROPERTY		m_Prop;
		};
		struct
		{			
			char			m_szName[MAX_SZNAME];
            char			m_szEffName[MAX_SZNAME];
			DWORD			m_GateID;
			DWORD			m_dwGeteFlags;

			DWORD			m_dwStartPNum;

			char				m_szGateNameKey[GATETO_NSIZE][MAX_SZNAME];
			DWORD			m_nGateNameIndex[GATETO_NSIZE];
			SNATIVEID		m_ToMapID[GATETO_NSIZE];
			DWORD			m_ToGateID[GATETO_NSIZE];
			DWORD			m_nToGate;			

			D3DXVECTOR2		m_vDiv;
			D3DXVECTOR3		m_vMin;
			D3DXVECTOR3		m_vMax;
		};
	};

    bool            m_bEvent;

public:
	LPD3DXVECTOR3	m_pvStartPos;

public:
	DWORD			m_dwLastUsedPos;

public:
	void SetGateNameKey ( const DWORD nIndex, const char* szGateNameKey ) { StringCchCopy( m_szGateNameKey[nIndex], MAX_SZNAME, szGateNameKey ); }
	void SetName ( const char* szName )	{ StringCchCopy( m_szName, MAX_SZNAME, szName ); }
    void SetEffName ( const char* szEffName )	{ StringCchCopy( m_szEffName, MAX_SZNAME, szEffName ); }
	void SetGateNameIndex ( const DWORD nIndex, DWORD dwIndex ) { m_nGateNameIndex[nIndex] = dwIndex; }
	void SetGate ( DWORD dwFlags, DWORD GateID, const D3DXVECTOR2 &vDiv );

	void SetGateID ( DWORD GateID )		{ m_GateID = GateID; }
	void SetPos ( const D3DXVECTOR3 &vPos );
	void SetBox ( const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin );
	BOOL MakeStartPos ( NavigationMesh* pNaviMesh );
    void SetEvent(bool bEvent) { m_bEvent = bEvent; }
    bool GetEvent() { return m_bEvent; }    
	
	char* GetName ()						{ return m_szName; }
    char* GetEffName ()					{ return m_szEffName; }	
	DWORD GetFlags ()					{ return m_dwGeteFlags; }
	DWORD GetGateID ()					{ return m_GateID; }

	char* GetGateNameKey (const DWORD nIndex = 0)			{ return nIndex > (GATETO_NSIZE -1) ? m_szGateNameKey[0] : m_szGateNameKey[nIndex]; }
	DWORD GetGateNameIndex (const DWORD nIndex = 0)		{ return nIndex > (GATETO_NSIZE -1) ? m_nGateNameIndex[0] : m_nGateNameIndex[nIndex]; }
	SNATIVEID GetToMapID (const DWORD nIndex = 0)	{ return nIndex > (GATETO_NSIZE -1) ? m_ToMapID[0] : m_ToMapID[nIndex]; }
	DWORD GetToGateID (const DWORD nIndex = 0)		{ return nIndex > (GATETO_NSIZE -1) ? m_ToGateID[0] : m_ToGateID[nIndex]; }

	const DWORD GetNGateToMapID(void) const;
	const bool SetToGateMapID(const SNATIVEID mapID, const DWORD dwGateID, const DWORD dwNameIndex, const char* const szGateName, const DWORD dwIndex = 0);
	const DWORD AddToGateMapID(const SNATIVEID mapID, const DWORD dwGateID, const DWORD dwNameIndex, const char* const szGateName);
	const bool ResetToGateMapID(const DWORD dwIndex);
	void ResetToGateMapID(void);

	D3DXVECTOR2 GetDiv ()					{ return m_vDiv; }
	DWORD GetStartPNum ()					{ return m_dwStartPNum; }
	LPD3DXVECTOR3 GetStartPos ()			{ return m_pvStartPos; }

public:
	DxLandGate*		m_pNext;

	//	Note : OBJAABB
	//
public:
	void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
	{
		vMax = m_vMax; vMin = m_vMin;
	}

	D3DXVECTOR3 GetMax ()	{ return m_vMax; }
	D3DXVECTOR3 GetMin ()	{ return m_vMin; }

	BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
	{
		D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
		return COLLISION::IsWithInPoint ( vDivMax, vDivMin, vPos );
	}

	BOOL IsDetectDivision ( D3DXVECTOR3 vPos )
	{
		return COLLISION::IsWithInPoint ( m_vMax, m_vMin, vPos );       
	}

	OBJAABB* GetNext ()		{ return m_pNext; }

public:
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxLandGate(void)
		: m_dwGeteFlags(GATE_ALL)
		, m_GateID(0)
		, m_nToGate(0)
		, m_vDiv(12.0f,12.0f)
		, m_vMax(40,40,40)
		, m_vMin(0,0,0)
		, m_dwStartPNum(0)
		, m_pvStartPos(NULL)
		, m_dwLastUsedPos(0)
		, m_pNext(NULL)
		, m_bEvent(false)
	{
		memset( m_szName, 0, sizeof(char)*MAX_SZNAME );
        memset( m_szEffName, 0, sizeof(char)*MAX_SZNAME );
		ResetToGateMapID();
	}
	~DxLandGate ()
	{
		SAFE_DELETE_ARRAY(m_pvStartPos);

		SAFE_DELETE(m_pNext);
	}

public:
	D3DXVECTOR3 GetGenPos ( DxLandGate::EMGENPOS_SEL emGenSel );
	void SetObjRotate90();

public:
	HRESULT Load ( sc::BaseStream	&SFile );
	HRESULT Save ( sc::SerialFile	&SFile );

public:
	void operator= (const DxLandGate &LandGate );
};
typedef DxLandGate* PDXLANDGATE;

class DxLandGateMan
{
public:
	static BOOL		m_bREND;

protected:	
	DWORD			m_dwNumLandGate;
	PDXLANDGATE		m_pLandGateList;
	OBJAABBNode*	m_pLandGateTree;

public:
	void AddLandGate ( PDXLANDGATE pLandGate );
	void DelLandGate ( char* szName );

public:
	PDXLANDGATE FindLandGate ( const char* szName );
	PDXLANDGATE FindLandGate ( DWORD dwGateID );    
	void BuildTree ();
	void SetObjRotate90();

public:
	PDXLANDGATE DetectGate ( const D3DXVECTOR3 &vPos );

public:
	const PDXLANDGATE GetListLandGate() { return m_pLandGateList; }
	DWORD		GetNumLandGate	()		{ return m_dwNumLandGate; }

public:
	HRESULT Load ( sc::BaseStream	&SFile );
	HRESULT Save ( sc::SerialFile	&SFile );

	static VOID SaveCsvHead( std::fstream &SFile );
	VOID SaveCsv( std::fstream &SFile );
	VOID LoadCsv( CStringArray &StrArray );

public:
	void CleanUp ();

public:
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxLandGateMan(void);
	~DxLandGateMan(void);

public:
	DxLandGateMan& operator= (const DxLandGateMan &LandGateMan );
};




