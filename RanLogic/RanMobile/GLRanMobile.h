#pragma once

#include "../../enginelib/G-Logic/TypeDefine.h"

namespace NSRanMobile
{
	// 란 모바일 보상 아이템
	struct SRewardItem
	{
		// 교환 아이템 ID
		SNATIVEID sID;
		// 교환 아이템 개수
		WORD wCount;
		// 교환에 필요한 MPoint
		DWORD dwNecessaryMPoint;
		// 경험치 압축 비율
		float fExpCompressRatio;

		SRewardItem()
			: sID( false )
			, wCount( 0 )
			, dwNecessaryMPoint( 0 )
			, fExpCompressRatio( 0.0f )
		{
		}

		SRewardItem( WORD wMID, WORD wSID, WORD wCnt, DWORD dwMPoint, float fExpRatio )
			: sID( wMID, wSID )
			, wCount( wCnt )
			, dwNecessaryMPoint( dwMPoint )
			, fExpCompressRatio( fExpRatio )
		{
		}
	};
	typedef std::vector< SRewardItem >					VEC_REWARD;
	typedef std::vector< SRewardItem >::iterator		ITER_REWARD;
	typedef std::vector< SRewardItem >::const_iterator	CITER_REWARD;

	class GLRanMobile
	{
	private :
		GLRanMobile();
		~GLRanMobile();

	public :
		static GLRanMobile& GetInstance();

	protected :
		// 모바일 홈페이지 URL
		std::string m_strHomePageURL;
		// 안드로이드 다운로드 URL
		std::string m_strAndroidDownloadURL;
		// 아이폰 다운로드 URL
		std::string m_strIOSDownloadURL;

		// 첫 연동시 필요 금액
		LONGLONG m_llRecognizePrice;
		// 연동 시간 연장시 필요 금액
		LONGLONG m_llExtendPrice;

		// 최소 연동 시간
		int m_nFirstTime;
		// 연동 시간
		int m_nExtendTime;

		// 보석 최대 전송 개수
		int m_nMaxSendJewelCount;

		// 일일 보상 아이템
		SNATIVEID m_sDailyRewardID;
		// 일일 보상 아이템 개수
		WORD m_wDailyRewardCount;

		// 보상 아이템
		VEC_REWARD m_vecReawrd;

	public :
		bool Load();

	public :
		const std::string& GetHomePageURL() { return m_strHomePageURL; }
		const std::string& GetAndroidDownloadURL() { return m_strAndroidDownloadURL; }
		const std::string& GetIOSDownloadURL() { return m_strIOSDownloadURL; }

		LONGLONG GetRecognizePrice() { return m_llRecognizePrice; }
		LONGLONG GetExtendPrice() { return m_llExtendPrice; }
		int GetFirstTime() { return m_nFirstTime; }
		int GetExtendTime() { return m_nExtendTime; }
		int GetMaxSendJewelCount() { return m_nMaxSendJewelCount; }

		const SNATIVEID& GetDailyItemID() { return m_sDailyRewardID; }
		WORD GetDailyItemCount() { return m_wDailyRewardCount; }

		const VEC_REWARD& GetReward() { return m_vecReawrd; }
	};

	//GLRanMobile& Get() { return GLRanMobile::GetInstance(); }
};