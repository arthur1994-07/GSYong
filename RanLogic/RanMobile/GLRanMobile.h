#pragma once

#include "../../enginelib/G-Logic/TypeDefine.h"

namespace NSRanMobile
{
	// �� ����� ���� ������
	struct SRewardItem
	{
		// ��ȯ ������ ID
		SNATIVEID sID;
		// ��ȯ ������ ����
		WORD wCount;
		// ��ȯ�� �ʿ��� MPoint
		DWORD dwNecessaryMPoint;
		// ����ġ ���� ����
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
		// ����� Ȩ������ URL
		std::string m_strHomePageURL;
		// �ȵ���̵� �ٿ�ε� URL
		std::string m_strAndroidDownloadURL;
		// ������ �ٿ�ε� URL
		std::string m_strIOSDownloadURL;

		// ù ������ �ʿ� �ݾ�
		LONGLONG m_llRecognizePrice;
		// ���� �ð� ����� �ʿ� �ݾ�
		LONGLONG m_llExtendPrice;

		// �ּ� ���� �ð�
		int m_nFirstTime;
		// ���� �ð�
		int m_nExtendTime;

		// ���� �ִ� ���� ����
		int m_nMaxSendJewelCount;

		// ���� ���� ������
		SNATIVEID m_sDailyRewardID;
		// ���� ���� ������ ����
		WORD m_wDailyRewardCount;

		// ���� ������
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