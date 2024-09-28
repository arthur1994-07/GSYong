#pragma once

#include "GLContrlBaseMsg.h"
#include <string>
#include <vector>
#include "../../SigmaCore/MsgPack/MinMsgPack.h"

#include "../InstanceSystem/GLInstanceSystemContentsDefine.h"

#include "../Country/GLCountryDefine.h"

// �δ� �������� �޽���[2/7/2014 hsshin]
// �δ� �������� ���� �޽����� NET_GENERIC ��� NET_INSTANCE_CONTENTS_MESSAGE�� ��ӹ޾�
// �ִ��� �� ���� �޽������� ���յ��� ���߰� �δ� ������ ��ü�� ������ �����ϵ��� ��;
namespace InstanceSystem
{
    enum EMCONTENTS_MESSAGE_TYPE
    {
        // Rn CDM
        EMMSG_RNCDM_PROGRESS_UPDATE_FC,
        EMMSG_RNCDM_PROGRESS_OFF_FC,
        EMMSG_RNCDM_RANK_INFO_FC,
        EMMSG_RNCDM_RANK_SHOW_FC,
        EMMSG_RNCDM_MAP_INFO_FC,
        EMMSG_RNCDM_MAP_IMAGE_FC,
        EMMSG_RNCDM_BATTLESTATE_FC,
        EMMSG_RNCDM_START_LOGO_SHOW_FC,
        EMMSG_RNCDM_END_LOGO_SHOW_FC,
        EMMSG_RNCDM_TOP_CLUB_INFO_FC,
		EMMSG_RNCDM_DOUBLE_POINT_FC,
        EMMSG_RNCDM_PREVIOUS_INFO_FAC,

        // Capture The Flag
        EMMSG_CTF_FLAG_UPDATE_FC,

        // Competition Result UI
        EMMSG_COMPETITION_RESULT_TAB_FC,
        EMMSG_COMPETITION_RESULT_INFO_FC,
        EMMSG_COMPETITION_RESULT_END_INFO_FC,

        // Guidance Club Battle
        EMMSG_GUIDANCE_UI_AUTH,
        EMMSG_GUIDANCE_UI_PROGRESS,
        EMMSG_GUIDANCE_UI_RANK,
        EMMSG_GUIDANCE_UI_RANK_MY_INFO,
        EMMSG_GUIDANCE_UI_AUTH_INFO,

		// CTI
		EMMSG_CTI_UI_AUTH,
		EMMSG_CTI_UI_PROGRESS,
		EMMSG_CTI_UI_RANK,
		EMMSG_CTI_UI_RANK_MY_INFO,
		EMMSG_CTI_UI_AUTH_INFO,
    };
}

namespace GLMSG
{
    // �ν��Ͻ� ���̽� �޼���;
    struct NET_INSTANCE_CONTENTS_MESSAGE : public NET_MSG_GENERIC
    {
        const InstanceSystem::EMCONTENTS_MESSAGE_TYPE emType;

        NET_INSTANCE_CONTENTS_MESSAGE(const DWORD nSize, const InstanceSystem::EMCONTENTS_MESSAGE_TYPE _emType)
            : NET_MSG_GENERIC(NET_MSG_GCTRL_INSTANCE_CONTENTS, nSize)
            , emType(_emType)
        {
        }
    };

    //! ���� : NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE�� ��ӹ��� ��� ����ü�� ���
    // MSGPACK_DEFINE ����� emType�� �� ù��°�� ���� ���Ŀ� �ڽ��� �ڷ����� �־�����Ѵ�.
    struct NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE // : public NET_MSG_GENERIC
    {
        DWORD emType;

        MSGPACK_DEFINE(emType);

        NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE()
            : emType(0)
        {
        }

        NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE(const InstanceSystem::EMCONTENTS_MESSAGE_TYPE _emType)
            : emType(_emType)
        {
        }
    };


    //--------------------------------------------------------------------------
    //------------------ Renewal Club Death Match ------------------------------
    //--------------------------------------------------------------------------
    struct NET_INSTANCE_RNCDM_PROGRESS_UPDATE_FC : public NET_INSTANCE_CONTENTS_MESSAGE
    {
        struct clubInfo
        {
            DWORD   dwClubMakrVer;
            DWORD   dwClubDbNum;
            DWORD   dwClubScore;
            char    szClubName[CLUB_NAME_LENGTH];
        };

        enum
        {
            NSIZE = (NET_DATA_BUFSIZE - sizeof(NET_INSTANCE_CONTENTS_MESSAGE) - sizeof(int) - sizeof(int) - sizeof(int) - sizeof(DWORD) ) / sizeof(clubInfo) ,
            HEADER_NSIZE = sizeof(NET_INSTANCE_CONTENTS_MESSAGE) + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(DWORD) ,
        };

        int		nHour;  // ����ð�;
        int		nMin;
        int		nSecond;

        DWORD   nClub; // Ŭ�� ���� ����;

        clubInfo sClubInfo[NSIZE];

        NET_INSTANCE_RNCDM_PROGRESS_UPDATE_FC ()
            : NET_INSTANCE_CONTENTS_MESSAGE(sizeof(NET_INSTANCE_RNCDM_PROGRESS_UPDATE_FC), InstanceSystem::EMMSG_RNCDM_PROGRESS_UPDATE_FC)
            , nHour(0)
            , nMin(0)
            , nSecond(0)
            , nClub(0)
        {
            dwSize = HEADER_NSIZE;

            memset(sClubInfo, 0, sizeof(sClubInfo));

            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_RNCDM_PROGRESS_UPDATE_FC)<=NET_DATA_BUFSIZE);
        }

        const bool addClub(const DWORD _dwMarVer, const DWORD _dwClubDbNum, const DWORD _dwClubScore, const char* const _szClubName)
        {
            if ( nClub == NSIZE )
                return false;
            sClubInfo[nClub].dwClubMakrVer = _dwMarVer;
            sClubInfo[nClub].dwClubDbNum = _dwClubDbNum;
            sClubInfo[nClub].dwClubScore = _dwClubScore;
            memcpy(sClubInfo[nClub].szClubName, _szClubName, CLUB_NAME_LENGTH);

            dwSize += sizeof(clubInfo);
            ++nClub;

            if ( nClub == NSIZE )
                return false;

            return true;
        }

        void reset()
        {
            nClub = 0;
            dwSize = HEADER_NSIZE;
        }

        const bool isValid()
        {
            return nClub != 0;
        }
    };

    struct NET_INSTANCE_RNCDM_PROGRESS_OFF_FC : public NET_INSTANCE_CONTENTS_MESSAGE
    {
        DWORD	dwNumberOneClubMarkVER;
        DWORD	dwNumberOneClubDbNum;
        char	szNumberOneClubName[CLUB_NAME_LENGTH];
        int		nNumberOneScore;
        int		nMyRank;
        DWORD	dwMyScore;
        int		nHour;
        int		nMin;
        int		nSecond;

        NET_INSTANCE_RNCDM_PROGRESS_OFF_FC ()
            : NET_INSTANCE_CONTENTS_MESSAGE( sizeof(NET_INSTANCE_RNCDM_PROGRESS_OFF_FC), InstanceSystem::EMMSG_RNCDM_PROGRESS_OFF_FC)
            , dwNumberOneClubMarkVER(0)
            , dwNumberOneClubDbNum(0)
            , nNumberOneScore(0)
            , nMyRank(0)
            , dwMyScore(0)
            , nHour(0)
            , nMin(0)
            , nSecond(0)
        {
            memset (szNumberOneClubName, 0, sizeof(char) * CLUB_NAME_LENGTH);
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_RNCDM_PROGRESS_OFF_FC)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_RNCDM_RANK_INFO_FC : public NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE
    {
        InstanceSystem::RNCDM_RANK_SLOT sRnCDMRankSlot;
        MSGPACK_DEFINE(emType, sRnCDMRankSlot);
        //NET_MSG_GCTRL_RNCDM_RANK_INFO_FC
        NET_INSTANCE_RNCDM_RANK_INFO_FC ()
            : NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE(InstanceSystem::EMMSG_RNCDM_RANK_INFO_FC)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_RNCDM_RANK_INFO_FC)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_RNCDM_RANK_SHOW_FC : public NET_INSTANCE_CONTENTS_MESSAGE
    {
        NET_INSTANCE_RNCDM_RANK_SHOW_FC()
            : NET_INSTANCE_CONTENTS_MESSAGE(sizeof(NET_INSTANCE_RNCDM_RANK_SHOW_FC), InstanceSystem::EMMSG_RNCDM_RANK_SHOW_FC)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_RNCDM_RANK_SHOW_FC)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_RNCDM_MAP_INFO_FC : public NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE
    {
        std::string			strMapName; //���̸�
        int					nTimeLimit; //�ð� ����
        int					nPlayerLimite; // ���� �ο�
        int					nLevelLimit; // ���� ���� ����

        MSGPACK_DEFINE(emType, strMapName, nTimeLimit, nPlayerLimite, nLevelLimit);

        NET_INSTANCE_RNCDM_MAP_INFO_FC ()
            : NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE(InstanceSystem::EMMSG_RNCDM_MAP_INFO_FC)
            , nTimeLimit(0)
            , nPlayerLimite(0)
            , nLevelLimit(0)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_RNCDM_MAP_INFO_FC)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_RNCDM_MAP_IMAGE_FC : public NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE
    {
        std::string			strMapImageName; //�� �̹��� �̸�

        MSGPACK_DEFINE(emType, strMapImageName);

        NET_INSTANCE_RNCDM_MAP_IMAGE_FC ()
            : NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE(InstanceSystem::EMMSG_RNCDM_MAP_IMAGE_FC)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_RNCDM_MAP_IMAGE_FC)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_RNCDM_TIME_FC : public NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE
    {
        int					nStartTime; //���� �ð�
        int					nEndTime; // ���� �ð�

        MSGPACK_DEFINE(emType, nStartTime, nEndTime);

        NET_INSTANCE_RNCDM_TIME_FC ()
            : NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE(InstanceSystem::EMMSG_RNCDM_BATTLESTATE_FC)
            , nStartTime(0)
            , nEndTime(0)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_RNCDM_TIME_FC)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_RNCDM_START_LOGO_SHOW_FC : public NET_INSTANCE_CONTENTS_MESSAGE
    {
        NET_INSTANCE_RNCDM_START_LOGO_SHOW_FC()
            : NET_INSTANCE_CONTENTS_MESSAGE(sizeof(NET_INSTANCE_RNCDM_START_LOGO_SHOW_FC), InstanceSystem::EMMSG_RNCDM_START_LOGO_SHOW_FC)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_RNCDM_START_LOGO_SHOW_FC)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_RNCDM_END_LOGO_SHOW_FC : public NET_INSTANCE_CONTENTS_MESSAGE
    {
        NET_INSTANCE_RNCDM_END_LOGO_SHOW_FC()
            : NET_INSTANCE_CONTENTS_MESSAGE(sizeof(NET_INSTANCE_RNCDM_END_LOGO_SHOW_FC), InstanceSystem::EMMSG_RNCDM_END_LOGO_SHOW_FC)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_RNCDM_END_LOGO_SHOW_FC)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_RNCDM_TOP_CLUB_INFO_FC : public NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE
    {
        std::vector<DWORD>	vecClubDbNum; //Ŭ�� DBNUM
        std::vector<std::string>	vecClubName; //Ŭ�� �̸�
        std::vector<std::string>	vecClubMasterName; //Ŭ�� ������ �̸�
        std::vector<DWORD>	vecClubMarkVer; //Ŭ����ũ ����

        MSGPACK_DEFINE(emType, vecClubDbNum, vecClubName, vecClubMasterName, vecClubMarkVer);

        NET_INSTANCE_RNCDM_TOP_CLUB_INFO_FC ()
            : NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE(InstanceSystem::EMMSG_RNCDM_TOP_CLUB_INFO_FC)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_RNCDM_TOP_CLUB_INFO_FC)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_RNCDM_PREVIOUS_INFO_FAC : public NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE
    {
        __time64_t tStartTime;
        __time64_t tEndTime;
        std::vector<DWORD>	vecClubDbNum; // Ŭ�� DBNUM;
        std::vector<DWORD>	vecClubRank; // Ŭ�� ����;

        MSGPACK_DEFINE(emType, tStartTime, tEndTime, vecClubDbNum, vecClubRank);

        NET_INSTANCE_RNCDM_PREVIOUS_INFO_FAC ()
            : NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE(InstanceSystem::EMMSG_RNCDM_PREVIOUS_INFO_FAC)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_RNCDM_PREVIOUS_INFO_FAC)<=NET_DATA_BUFSIZE);
        }
    };








    //--------------------------------------------------------------------------
    //------------------ Capture The Flag --------------------------------------
    //--------------------------------------------------------------------------
    struct NET_INSTANCE_CTF_FLAG_UPDATE_FC : public NET_INSTANCE_CONTENTS_MESSAGE
    {
        enum
        {
            EM_BLUE_FLAG_VISIBLE_ON,
            EM_RED_FLAG_VISIBLE_ON,
            EM_BLUE_FLAG_VISIBLE_OFF,
            EM_RED_FLAG_VISIBLE_OFF,
            EM_ALL_FLAG_VISIBLE_OFF,

			// ��ü Navagation On / Off ó��;
			EM_NAVIGATION_ON,
			EM_NAVIGATION_OFF,
        };
        STARGETID sTargetID;
        int nFlagType;
        bool bBlink;

        NET_INSTANCE_CTF_FLAG_UPDATE_FC()
            : NET_INSTANCE_CONTENTS_MESSAGE(sizeof(NET_INSTANCE_CTF_FLAG_UPDATE_FC), InstanceSystem::EMMSG_CTF_FLAG_UPDATE_FC)
            , nFlagType(EM_ALL_FLAG_VISIBLE_OFF)
            , bBlink(false)
        {

        }
    };



    //--------------------------------------------------------------------------
    //------------------ RnCompetitionResult -----------------------------------
    //--------------------------------------------------------------------------
    
    // ���� ��� �� ���� : ��� �Ǵ� 1�� ����;
    struct NET_INSTANCE_COMPETITION_RESULT_TAB_FC : public NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE
    {
		unsigned int typeCompetition;
        InstanceSystem::RnCompetitionResultTabInfo sTabInfo;
        MSGPACK_DEFINE(emType, typeCompetition, sTabInfo);

        NET_INSTANCE_COMPETITION_RESULT_TAB_FC()
            : NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE(InstanceSystem::EMMSG_COMPETITION_RESULT_TAB_FC)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_COMPETITION_RESULT_TAB_FC)<=NET_DATA_BUFSIZE);
        };
    };

    // ���� ��� �� ���� �׸���� ���� : �׸� ������ŭ ����;
    struct NET_INSTANCE_COMPETITION_RESULT_INFO_FC : public NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE
    {
		unsigned int typeCompetition;
        unsigned int nTabNumber; // �ش��׸��� �� ��ȣ;
        InstanceSystem::RnCompetitionResultContent sContents; // �׸� ����;
        MSGPACK_DEFINE(emType, typeCompetition, nTabNumber, sContents);

        NET_INSTANCE_COMPETITION_RESULT_INFO_FC()
            : NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE(InstanceSystem::EMMSG_COMPETITION_RESULT_INFO_FC)
            , nTabNumber(0)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_COMPETITION_RESULT_INFO_FC)<=NET_DATA_BUFSIZE);
        };
    };

    struct NET_INSTANCE_COMPETITION_RESULT_END_INFO_FC : public NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE
    {
		unsigned int typeCompetition;
        float fOutTime; // �ش��׸��� �� ��ȣ;
        DWORD nCloseButtonParam;
        MSGPACK_DEFINE(emType, typeCompetition, fOutTime, nCloseButtonParam);

        NET_INSTANCE_COMPETITION_RESULT_END_INFO_FC()
            : NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE(InstanceSystem::EMMSG_COMPETITION_RESULT_END_INFO_FC)
            , fOutTime(0)
            , nCloseButtonParam(0)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_COMPETITION_RESULT_END_INFO_FC)<=NET_DATA_BUFSIZE);
        };
    };

	struct NET_INSTANCE_RNCDM_DOUBLE_POINT_FC : public NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE
	{
		BOOL bVisible;
		MSGPACK_DEFINE(emType, bVisible);

		NET_INSTANCE_RNCDM_DOUBLE_POINT_FC()
			: NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE(InstanceSystem::EMMSG_RNCDM_DOUBLE_POINT_FC)
			, bVisible(FALSE)
		{
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_RNCDM_DOUBLE_POINT_FC)<=NET_DATA_BUFSIZE);
		};
	};



    //--------------------------------------------------------------------------
    //------------------ Guidance Club Battle ----------------------------------
    //--------------------------------------------------------------------------
    struct NET_INSTANCE_GUIDANCE_UI_AUTH_FC : public NET_INSTANCE_CONTENTS_MESSAGE
    {
        bool bVisible;  // UI ��� ����;
        float fAuthTime;  // ���� �ð� (bVisible�� true�϶�);
        //int iResult;  // ���� ���;

        NET_INSTANCE_GUIDANCE_UI_AUTH_FC()
            : NET_INSTANCE_CONTENTS_MESSAGE(sizeof(NET_INSTANCE_GUIDANCE_UI_AUTH_FC), InstanceSystem::EMMSG_GUIDANCE_UI_AUTH)
        {

        }
    };

    struct NET_INSTANCE_GUIDANCE_UI_PROGRESS_FC : public NET_INSTANCE_CONTENTS_MESSAGE
    {
        int iNameIndex;  // ����� XML GameWord Index;
        int iProgressIndex;  // ������� XML GameWord Index;
        int iPoint;  // ��ǥ ǥ��Ʈ;
        float fRemainTime;  // ���� �ð�;

        NET_INSTANCE_GUIDANCE_UI_PROGRESS_FC()
            : NET_INSTANCE_CONTENTS_MESSAGE(sizeof(NET_INSTANCE_GUIDANCE_UI_PROGRESS_FC), InstanceSystem::EMMSG_GUIDANCE_UI_PROGRESS)
        {

        }
    };

    struct NET_INSTANCE_GUIDANCE_UI_RANK_FC : public NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE
    {
        struct RankInfo
        {
            std::string clubName;
            int iPoint;
            int iRank;
            MSGPACK_DEFINE(clubName, iPoint, iRank);
        };

        std::vector<NET_INSTANCE_GUIDANCE_UI_RANK_FC::RankInfo> rankVector;

        MSGPACK_DEFINE(emType, rankVector);

        NET_INSTANCE_GUIDANCE_UI_RANK_FC()
            : NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE(InstanceSystem::EMMSG_GUIDANCE_UI_RANK)
        {
        }
    };
    typedef std::vector<NET_INSTANCE_GUIDANCE_UI_RANK_FC::RankInfo> NET_GUIDANCE_RANK_VEC;

    struct NET_INSTANCE_GUIDANCE_UI_RANK_MY_INFO_FC : public NET_INSTANCE_CONTENTS_MESSAGE
    {
        int iMyClubPoint;
        int iMyClubRank;

        NET_INSTANCE_GUIDANCE_UI_RANK_MY_INFO_FC()
            : NET_INSTANCE_CONTENTS_MESSAGE(sizeof(NET_INSTANCE_GUIDANCE_UI_PROGRESS_FC), InstanceSystem::EMMSG_GUIDANCE_UI_RANK_MY_INFO)
        {
        }
    };

    struct NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC : public NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE
    {
        struct AuthClubInfo
        {
            DWORD clubID;
            float fXpos;
            float fZpos;
            MSGPACK_DEFINE(clubID, fXpos, fZpos);
        };

        struct AcceleratorInfo
        {
            float fXpos;
            float fZpos;
            MSGPACK_DEFINE(fXpos, fZpos);
        };

        DWORD dwGuidanceMapID;
		DWORD dwIconType;
        std::vector<NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC::AuthClubInfo> vecAuthClub;
        std::vector<NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC::AcceleratorInfo> vecAccelerator;
		bool bAllianceClub;

        MSGPACK_DEFINE(emType, dwGuidanceMapID, dwIconType, vecAuthClub, vecAccelerator, bAllianceClub);

        NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC()
            : NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE(InstanceSystem::EMMSG_GUIDANCE_UI_AUTH_INFO)
			, dwGuidanceMapID( 0 )
			, dwIconType ( 0 )
			, bAllianceClub ( true )
        {
        }
    };
    typedef std::vector<NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC::AuthClubInfo> NET_GUIDANCE_AUTH_CLUB_VEC;


	/************************************************************************/
	/* CTI Message                                                          */
	/************************************************************************/
	struct NET_INSTANCE_CTI_UI_AUTH_FC : public NET_INSTANCE_CONTENTS_MESSAGE
	{
		bool bVisible;  // UI ��� ����;
		float fAuthTime;  // ���� �ð� (bVisible�� true�϶�);
		//int iResult;  // ���� ���;

		NET_INSTANCE_CTI_UI_AUTH_FC()
			: NET_INSTANCE_CONTENTS_MESSAGE(
			sizeof(NET_INSTANCE_CTI_UI_AUTH_FC),
			InstanceSystem::EMMSG_CTI_UI_AUTH )
		{

		}
	};

	struct NET_INSTANCE_CTI_UI_PROGRESS_FC : public NET_INSTANCE_CONTENTS_MESSAGE
	{
		int iNameIndex;  // ����� XML GameWord Index;
		int iProgressIndex;  // ������� XML GameWord Index;
		int iPoint;  // ��ǥ ǥ��Ʈ;
		float fRemainTime;  // ���� �ð�;

		NET_INSTANCE_CTI_UI_PROGRESS_FC()
			: NET_INSTANCE_CONTENTS_MESSAGE(
			sizeof(NET_INSTANCE_CTI_UI_PROGRESS_FC),
			InstanceSystem::EMMSG_CTI_UI_PROGRESS )
		{

		}
	};

	struct NET_INSTANCE_CTI_UI_RANK_FC : public NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE
	{
		struct RankInfo
		{
			std::string countryName;
			int iPoint;
			int iRank;

			MSGPACK_DEFINE( countryName, iPoint, iRank );
		};

		std::vector<NET_INSTANCE_CTI_UI_RANK_FC::RankInfo> rankVector;

		MSGPACK_DEFINE( emType, rankVector );

		NET_INSTANCE_CTI_UI_RANK_FC()
			: NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE(
			InstanceSystem::EMMSG_CTI_UI_RANK )
		{
		}
	};
	typedef std::vector< NET_INSTANCE_CTI_UI_RANK_FC::RankInfo > NET_CTI_RANK_VEC;

	struct NET_INSTANCE_CTI_UI_RANK_MY_INFO_FC : public NET_INSTANCE_CONTENTS_MESSAGE
	{
		int iMyCountryPoint;
		int iMyCountryRank;

		NET_INSTANCE_CTI_UI_RANK_MY_INFO_FC()
			: NET_INSTANCE_CONTENTS_MESSAGE(
			sizeof( NET_INSTANCE_CTI_UI_RANK_MY_INFO_FC ),
			InstanceSystem::EMMSG_CTI_UI_RANK_MY_INFO )
		{
		}
	};

	struct NET_INSTANCE_CTI_UI_MINIMAP_FC : public NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE
	{
		struct AuthCountryInfo
		{
			Country::SCOUNTRY_INFO sCountryInfo;
			float fXpos;
			float fZpos;

			MSGPACK_DEFINE( sCountryInfo, fXpos, fZpos );
		};

		struct AcceleratorInfo
		{
			float fXpos;
			float fZpos;

			MSGPACK_DEFINE( fXpos, fZpos );
		};

		DWORD dwGuidanceMapID;

		std::vector< NET_INSTANCE_CTI_UI_MINIMAP_FC::AuthCountryInfo > vecAuthCountry;
		std::vector< NET_INSTANCE_CTI_UI_MINIMAP_FC::AcceleratorInfo > vecAccelerator;

		MSGPACK_DEFINE( emType, dwGuidanceMapID, vecAuthCountry, vecAccelerator );

		NET_INSTANCE_CTI_UI_MINIMAP_FC ()
			: NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE(
			InstanceSystem::EMMSG_CTI_UI_AUTH_INFO )
		{
		}
	};
	typedef std::vector< NET_INSTANCE_CTI_UI_MINIMAP_FC::AuthCountryInfo > NET_CTI_AUTH_COUNTRY_VEC;

}