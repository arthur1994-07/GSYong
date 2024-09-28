#pragma once

#include <string>
#include <vector>
#include "../../SigmaCore/MsgPack/MinMsgPack.h"

namespace InstanceSystem
{
    //--------------------------------------------------------------------------
    //------------------ Renewal Club Death Match ------------------------------
    //--------------------------------------------------------------------------
    struct CDM_MEMBER_INFO
    {
        int nEmCharIndex;
        std::string strCharName;

        MSGPACK_DEFINE(nEmCharIndex, strCharName);
        CDM_MEMBER_INFO()
            : nEmCharIndex(0)
        {
        }
    };

    struct RNCDM_RANK_SLOT
    {
        int			nRank;
        DWORD		dwClubMarkVER;
        DWORD		dwClubDBNum;
        std::string	strClubName;
        std::string	strClubMasterName;
        DWORD		dwPoint;
        DWORD		dwKill;
        DWORD		dwDeath;
        DWORD		dwRebirth;
        std::vector<DWORD> vecReWard;
        std::vector<CDM_MEMBER_INFO> vecCdmMemberInfo;

        MSGPACK_DEFINE(nRank, dwClubMarkVER, dwClubDBNum, strClubName, strClubMasterName, dwPoint, dwKill, dwDeath, dwRebirth, vecReWard, vecCdmMemberInfo);
        //NET_MSG_GCTRL_RNCDM_RANK_SHOW_FC
        RNCDM_RANK_SLOT ()
            : nRank(0)
            , dwClubMarkVER(0)
            , dwClubDBNum(0)
            , dwPoint(0)
            , dwKill(0)
            , dwDeath(0)
            , dwRebirth(0)
        {
        }
    };






    //--------------------------------------------------------------------------
    //------------------ RnCompetitionResult -----------------------------------
    //--------------------------------------------------------------------------
	enum COMPETITION
	{
		COMPETITION_CDM,
		COMPETITION_CTFLAG,
		COMPETITION_CTFIELD,
		COMPETITION_GUIDANCE,
		COMPETITION_TOURNAMENT,
		COMPETITION_CTI,
		COMPETITION_NSIZE,
	};

    struct RnCompetitionResultTitleObj
    {
        enum EM_CONTENT_TYPE
        {
            EM_CONTENT_INT          = 0,
            EM_CONTENT_FLOAT,
            EM_CONTENT_STRING,
            EM_CONTENT_ICON_CLASS,
            EM_CONTENT_ICON_SCHOOL,
            EM_CONTENT_ICON_CLUBMARK,
            EM_CONTENT_ICON_WINLOSE,
			EM_CONTENT_ICON_COUNTRYMARK,

            // 보상;
            EM_CONTENT_REWARD               = 0x1000,
            EM_CONTENT_REWARD_CONTRIBUTION  = 0x0001,
            EM_CONTENT_REWARD_EXP           = 0x0002,
            EM_CONTENT_REWARD_MONEY         = 0x0004,
            EM_CONTENT_REWARD_ITEM          = 0x0008,
            EM_CONTENT_REWARD_BUFF          = 0x0010,
        };

        int emItemType;
        std::string strTitleName;
        int nBoxSize;

		RnCompetitionResultTitleObj()
			: emItemType(0)
			, nBoxSize(0)
		{
		}

		RnCompetitionResultTitleObj( int _contentType, const int _boxSize, const char* _titleName)
			: emItemType(_contentType)
			, nBoxSize(_boxSize)
		{
			strTitleName = _titleName;
		}


        MSGPACK_DEFINE(emItemType, strTitleName, nBoxSize);
    };

    struct RnCompetitionResultTitle
    {
        // 전장 결과 제목 (1~12개);
        std::vector<RnCompetitionResultTitleObj> vecTitleObj;

        MSGPACK_DEFINE(vecTitleObj);
        
        __forceinline void AddObj(const int _emContentType, const DWORD _dwBoxSize, const char* _pText)
        {
            InstanceSystem::RnCompetitionResultTitleObj _obj;
            _obj.emItemType = _emContentType;
            _obj.nBoxSize = _dwBoxSize;
            _obj.strTitleName = _pText; //"순위";
            vecTitleObj.push_back(_obj);
        }
    };

    struct RnCompetitionResultContentRewardObj
    {
        struct RewardItem
        {
            SNATIVEID itemID;
            WORD wAmount;
            MSGPACK_DEFINE(itemID,wAmount);
        };

        std::string strContributionPoint;
        std::string strExpReward;
        std::string strMoneyReward;
        std::vector<SNATIVEID> vecBuffReward;
        std::vector<RewardItem> vecItemReward;

        MSGPACK_DEFINE(strContributionPoint, strExpReward, strMoneyReward, vecBuffReward, vecItemReward);
    };

    struct RnCompetitionResultContentObj
    {
        std::string ContentItemString;
        int ContentItemInt;
        float ContentItemFloat;

		RnCompetitionResultContentObj()
			: ContentItemInt(0)
			, ContentItemFloat(0.0f)
		{

		}

		RnCompetitionResultContentObj( const int _intValue )
			: ContentItemInt(_intValue)
			, ContentItemFloat(0.0f)
		{

		}

		RnCompetitionResultContentObj( const float _floatValue )
			: ContentItemInt(0)
			, ContentItemFloat(_floatValue)
		{

		}

		RnCompetitionResultContentObj( const char* _strValue)
			: ContentItemInt(0)
			, ContentItemFloat(0.0f)
		{
			ContentItemString = _strValue;
		}

        MSGPACK_DEFINE(ContentItemString, ContentItemInt, ContentItemFloat);
    };

    struct RnCompetitionResultContent
    {
        // 전장 결과 내용 (1~12개);
        DWORD dwKeyID;
        std::vector<RnCompetitionResultContentObj> vecResultObj;
        std::vector<RnCompetitionResultContentRewardObj> vecRewardObj;

        MSGPACK_DEFINE(dwKeyID, vecResultObj, vecRewardObj);

        void AddContent(const char* _pText)
        {
            if (_pText == NULL )
                return;

            RnCompetitionResultContentObj _obj;
            _obj.ContentItemString = _pText;
            vecResultObj.push_back(_obj);
        }
        void AddContent(const int _iValue)
        {
            RnCompetitionResultContentObj _obj;
            _obj.ContentItemInt = _iValue;
            vecResultObj.push_back(_obj);
        }
        void AddContent(const float _fValue)
        {
            RnCompetitionResultContentObj _obj;
            _obj.ContentItemFloat = _fValue;
            vecResultObj.push_back(_obj);
        }
        void AddReward(RnCompetitionResultContentRewardObj& _obj)
        {
            vecRewardObj.push_back(_obj);
        }
    };

    struct RnCompetitionResultTabInfo
    {
        enum
        {
            EM_RESULT_SOR_NON = 0,
            EM_RESULT_SOR_ASC,
            EM_RESULT_SOR_DESC,
        };
        enum
        {
            EM_MY_RESULT_NONE = 0,
            EM_MY_RESULT_CHAR_DBNUM,
            EM_MY_RESULT_CHAR_GAEAID,
            EM_MY_RESULT_CLUB,
            EM_MY_RESULT_PARTY,
			EM_MY_RESULT_COUNTRY,
        };

        std::string strTabName;  // 탭 이름;
        WORD emSortType;  // 정렬 방식;
        WORD wSortTitleNum;  // 정렬 기준 타이틀 번호 (주의 사항 : RnCompetitionResultContentObj 번호를 뜻함);
        WORD emMyResultType;  // 내 결과 항목 타입;

        RnCompetitionResultTitle sTitle;  // 결과 제목;        

        RnCompetitionResultTabInfo()
            : emSortType(0)
            , wSortTitleNum(0)
            , emMyResultType(0)
        {}

        MSGPACK_DEFINE(strTabName, emSortType, wSortTitleNum, emMyResultType, sTitle);
    };

    struct RnCompetitionResultTab
    {
        RnCompetitionResultTabInfo sTabInfo;
        std::vector<RnCompetitionResultContent> vecContent;  // 결과 내용;
    };

    struct RnCompetotionResultInfo
    {
        float fOutTime;  // 강제 퇴장 시간;
        std::vector<RnCompetitionResultTab> vecTab;

        RnCompetotionResultInfo()
            : fOutTime(0.0f)
        {}
    };
}