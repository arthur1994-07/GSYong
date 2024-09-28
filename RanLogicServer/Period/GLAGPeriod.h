#ifndef _GL_AG_PERIOD_H_
#define _GL_AG_PERIOD_H_

#pragma once

#include <algorithm>
#include <map>
#include <list>

#include "../../EngineLib/G-Logic/GLDefine.h"

struct SPERIOD
{
	DWORD dwBEGINTIME;
	DWORD dwENDTIME;
	DWORD dwCURTIME;

	SPERIOD() 
		: dwBEGINTIME(0)
		, dwENDTIME  (0)
		, dwCURTIME  (0)
	{
	}

	virtual ~SPERIOD() 
	{ 
		dwBEGINTIME = 0;
		dwENDTIME	= 0;
		dwCURTIME	= 0;
	}
};

struct SNONCONFRONTLIST
{
	struct SNONCONFRONT : public SPERIOD
	{
		SNATIVEID sMAPID;
		INT32	  nCHANNEL;
		BOOL	  bUNLIMITED;

		SNONCONFRONT()  
			: sMAPID	 ( NATIVEID_NULL() )
			, nCHANNEL	 ( 0               )
			, bUNLIMITED ( FALSE           )
		{
		}

		SNONCONFRONT( const SNATIVEID& sMapID, const INT32 nChannel )  
			: sMAPID     ( sMapID   )
			, nCHANNEL   ( nChannel )
			, bUNLIMITED ( FALSE    )
		{
		}

		~SNONCONFRONT() {}

		bool operator == ( const SNONCONFRONT& sValue )
		{
			if ( sValue.nCHANNEL == -1 )
			{
				return ( sMAPID == sValue.sMAPID );
			}

			return ( sMAPID == sValue.sMAPID && nCHANNEL == sValue.nCHANNEL );
		}

	};

	typedef std::list<SNONCONFRONT> LIST;
	typedef LIST::iterator			LIST_ITER;

	LIST sPeriodList;

	SNONCONFRONTLIST  ();
	~SNONCONFRONTLIST ();

public:
	void FrameMove(GLAgentServer* pServer, float fTime, float fElapsedTime);
	void CleanUp();

public:
	void Insert(GLAgentServer* pServer, const SNATIVEID& sMapID, const INT32 nChannel, const DWORD dwPeriod);
	void Delete(const SNATIVEID& sMapID, const INT32 nChannel);
	void Delete_Front();
	void Delete_Back();

public:
	BOOL IsNonConfront ( const SNATIVEID& sMapID, const INT32 nChannel );

};

class GLAGPeriod
{

private:
	SNONCONFRONTLIST m_NonConfrontList;

public:
	GLAGPeriod();
	~GLAGPeriod();

public:
	void FrameMove(GLAgentServer* pServer, float fTime, float fElapsedTime );
	void CleanUp();

public:
	SNONCONFRONTLIST* GetNonConfrontList () { return &m_NonConfrontList; }

};

#endif // _GL_AG_PERIOD_H_