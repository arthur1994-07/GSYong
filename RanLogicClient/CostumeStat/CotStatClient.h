#pragma once

class GLGaeaClient;

namespace NSCostumeStat
{
	enum EMQuestion
	{
		EMQuestion_Replace = 0,
		EMQuestion_Expiration,
	};

	enum EMError
	{
		EMError_NoPoint = 0,
	};
}

class CCostumeStatClient
{
public :
	CCostumeStatClient(GLGaeaClient* pGaeaClient);
	~CCostumeStatClient();

protected :
	GLGaeaClient*		m_pGaeaClient;

	typedef std::map< COSTUME_STATS, WORD >	MInvestPoint;		// < 능력치 정의 열거값, 투자한 포인트 >
	typedef MInvestPoint::iterator			MIter;
	typedef MInvestPoint::value_type		MValueType;

	MInvestPoint	m_InvestPoint;

	__int64			m_iHavePoint;
	__int64			m_iSubPoint;

	int				m_iCurSuit;
	
	const SINVENITEM* m_pSrcItem;
	const SINVENITEM* m_pDstItem;

public :
	void SetHavePoint( __int64 iPt );
	void SetSubPoint( __int64 iPt );
	void SetSuit(EMSUIT eSuit );
	void SetItem(const SINVENITEM* pSrc, const SINVENITEM* pDst);

	__int64 GetRemainPoint();
	__int64 GetHavePoint() const { return m_iHavePoint; }
	__int64 GetSubPoint()  const { return m_iSubPoint;  }
	  int   GetSuit()      const { return m_iCurSuit;   }

    void RestMember();
	void SetNumeric( COSTUME_STATS eSt, WORD wValue );
	void ResetStatPos(COSTUME_STATS eSt);

	bool IsNumeric(COSTUME_STATS eSt);
	bool IsInvestEnable(COSTUME_STATS eSt);

	WORD GetNumeric(COSTUME_STATS eSt);

	const SINVENITEM* GetSrcItem() { return m_pSrcItem; }	
	const SINVENITEM* GetDstItem() { return m_pDstItem; }

	bool  GetData( GLMSG::SNET_INVEN_COSTUME_STATS::SINVESTINFO* pData );

public :
	bool SetItemInfo( const SINVENITEM* pSrc, const SINVENITEM* pDst );
	WORD GetMaxStatPoint( COSTUME_STATS emStat );
	double GetStatInc( COSTUME_STATS emStat );
};