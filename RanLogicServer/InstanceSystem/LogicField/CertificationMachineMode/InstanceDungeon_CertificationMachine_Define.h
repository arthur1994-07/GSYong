#pragma once

#include <vector>
#include <map>

#include "../../../../enginelib/G-Logic/TypeDefine.h"

namespace InstanceSystem
{

	namespace CertificationMachineMode
	{

		// 기본 진영 ( NPC );
		enum EMFACTION_BASIC
		{
			EMFACTION_BASIC_ALLY,
			EMFACTION_BASIC_ENEMY,
			EMFACTION_BASIC_NEUTRAL,
		};


		// 타이머;
		enum EMTIMER_ID
		{
			EMTIMER_ID_GAME_DURATION										= 1,				// 게인 진행시간 타이머;
			EMTIMER_ID_SAFETIME												= 2,				// 전장맵 입장 후 무적시간 타이머;
			EMTIMER_ID_DESTROY_WAITTIME										= 3,				// 인던 파괴 대기시간 타이머;
			EMTIMER_ID_EXIT_JOIN_BUFFTIME									= 4,				// 입장버프 해제 타이머;
			EMTIMER_ID_EXIT_JOIN_DEBUFFTIME									= 5,				// 입장디버프 해제 타이머;

			EMTIMER_ID_CERTIFY_POINT										= 6,				// 인증 포인트 체크시간 타이머;
			EMTIMER_ID_TRY_CERTIFY_WAITTIME									= 7,				// 인증 시도 대기시간 타이머;
			EMTIMER_ID_CERTIFICATION_MACHINE_LOCKTIME						= 8,				// 인증기의 제한시간 타이머;

			EMTIMER_ID_ACCELERATOR_RECHARGE									= 9,				// 가속기 재활성화시간 타이머;
			EMTIMER_ID_ACCELERATOR_BUFFTIME									= 10,				// 가속기 버프시간 타이머;

			EMTIMER_ID_CERTIFY_SUCCESS										= 11,				// 인증기 인증 성공;
			EMTIMER_ID_CERTIFICATION_MACHINE_NEUTRALIZATION					= 12,				// 인증기 중립화 타이머;
			EMTIMER_ID_CERTIFICATION_MACHINE_ACTIVATE_ADDPOINT				= 13,				// 인증기의 AddPoint 상태 활성 타이머;
			EMTIMER_ID_CERTIFICATION_MACHINE_INACTIVATE_ADDPOINT			= 14,				// 인증기의 AddPoint 상태 비활성 타이머;
			EMTIMER_ID_CONFIRM_CERTIFICATION								= 15,				// 인증기의 인증 상태 확인 타이머;

			EMTIMER_ID_REWARD_WINNER										= 16,				// 승리자 보상 타이머;
			EMTIMER_ID_REWARD_PARTICIPANT									= 17,				// 참가자 보상 타이머;

			EMTIMER_ID_HELP_NOTIFY_1										= 18,				// 게임 도움말 타이머1;
			EMTIMER_ID_HELP_NOTIFY_2										= 19,				// 게임 도움말 타이머2;
			EMTIMER_ID_HELP_NOTIFY_3										= 20,				// 게임 도움말 타이머3;
			EMTIMER_ID_ACCELERATOR_CREATE_NOTIFY							= 21,				// 가속기 생성 공지 타이머;
			EMTIMER_ID_ACCELERATOR_CAPTURE_NOTIFY							= 22,				// 가속기 점령 공지 타이머;
			EMTIMER_ID_ACCELERATOR_CAPTURE_ADDPOINT_NOTIFY					= 23,				// 가속기 점령 후 포인트 증가 공지 타이머;
			EMTIMER_ID_ACCELERATOR_RECHARGE_NOTIFY							= 24,				// 가속기 재활성화 공지 타이머;
			EMTIMER_ID_CERTIFY_SUCCESS_NOTIFY								= 25,				// 인증 성공 공지 타이머;
			EMTIMER_ID_CERTIFY_FAIL_NOTIFY									= 26,				// 인증을 할수 없는 상태 공지 타이머;
			EMTIMER_ID_CERTIFY_CAPTURED_FAIL_NOTIFY							= 27,				// 이미 인증이 되어 있는 상태 공지 타이머;
			EMTIMER_ID_CERTIFY_DUPLICATE_FAIL_NOTIFY						= 28,				// 중복 인증으로 인한 실패 공지 타이머;
			EMTIMER_ID_CERTIFICATION_MACHINE_NEUTRALIZATION_WAIT_NOTIFY		= 29,				// 인증기 중립화 대기1 공지 타이머;
			EMTIMER_ID_CERTIFICATION_MACHINE_NEUTRALIZATION_NOTIFY			= 30,				// 인증기 중립화 공지 타이머;
			EMTIMER_ID_APPROACHED_GOAL_NOTIFY								= 31,				// 목표 달성 공지 타이머;
			EMTIMER_ID_EXIT_GAME_NOTIFY										= 32,				// 게임종료 공지 타이머;
			EMTIMER_ID_CONFIRM_CERTIFICATION_NOTIFY							= 33,				// 인증상태 확인 공지 타이머;
			EMTIMER_ID_TRY_CERTIFY_NOTIFY									= 34,				// 인증 시도 공지 타이머;

			EMTIMER_ID_NEXT,
		};


		// Callback Function;
		const std::string	STR_CFUNCTION_GAMEHELP1										= "EventCMMode_GameHelp1";
		const std::string	STR_CFUNCTION_GAMEHELP2										= "EventCMMode_GameHelp2";
		const std::string	STR_CFUNCTION_GAMEHELP3										= "EventCMMode_GameHelp3";
		const std::string	STR_CFUNCTION_ACCELERATOR_CREATE							= "EventCMMode_AcceleratorCreate";
		const std::string	STR_CFUNCTION_ACCELERATOR_CAPTURE							= "EventCMMode_AcceleratorCapture";
		const std::string	STR_CFUNCTION_ACCELERATOR_CAPTURE_ADDPOINT					= "EventCMMode_AcceleratorCaptureAddPoint";
		const std::string	STR_CFUNCTION_ACCELERATOR_RECHARGE							= "EventCMMode_AcceleratorRecharge";
		const std::string	STR_CFUNCTION_CERTIFY_SUCCESS								= "EventCMMode_CertifySuccess";
		const std::string	STR_CFUNCTION_CERTIFY_FAIL									= "EventCMMode_CertifyFail";
		const std::string	STR_CFUNCTION_CERTIFY_DUPLICATE_FAIL						= "EventCMMode_CertifyDuplicateFail";
		const std::string	STR_CFUNCTION_CERTIFY_CAPTURED_FAIL							= "EventCMMode_CertifyCapturedFail";
		const std::string	STR_CFUNCTION_CERTIFICATION_MACHINE_NEUTRALIZATION_WAIT		= "EventCMMode_CertificationMachineNeutralizationWait";
		const std::string	STR_CFUNCTION_CERTIFICATION_MACHINE_NEUTRALIZATION			= "EventCMMode_CertificationMachineNeutralization";
		const std::string	STR_CFUNCTION_APPROACHED_GOAL								= "EventCMMode_ApproachedGoal";
		const std::string	STR_CFUNCTION_EXIT_GAME										= "EventCMMode_ExitGame";
		const std::string	STR_CFUNCTION_CONFIRM_CERTIFICATION							= "EventCMMode_ConfirmCertification";
		const std::string	STR_CFUNCTION_TRY_CERTIFY									= "EventCMMode_TryCertify";


		// 진영의 포인트를 갱신하기 위한 MAP;
		typedef		std::map< DWORD, DWORD >					MAP_FACTION_POINT;
		typedef		MAP_FACTION_POINT::iterator					MAP_FACTION_POINT_ITER;
		typedef		MAP_FACTION_POINT::const_iterator			MAP_FACTION_POINT_CITER;
		typedef		MAP_FACTION_POINT::value_type				MAP_FACTION_POINT_VALUE;


		struct SRESULT_LOG
		{
			DWORD dwFactionID;
			DWORD dwPoint;
			DWORD dwCertificationMachineCount;
			DWORD dwCertifyCount;

			SRESULT_LOG ( void )
				: dwFactionID ( 0 )
				, dwPoint ( 0 )
				, dwCertificationMachineCount ( 0 )
				, dwCertifyCount ( 0 )
			{

			}
		};


		// 진영의 포인트 및 랭킹정보;
		struct SFACTION_POINT 
		{
			DWORD dwFactionID;
			DWORD dwFactionPoint;
			DWORD dwFactionRealPoint;
			DWORD dwRanking;
			bool bApproachGoal;

			SFACTION_POINT ( const DWORD _dwFactionID
				, DWORD _dwFactionPoint )
				: dwFactionID ( _dwFactionID )
				, dwFactionPoint ( _dwFactionPoint )
				, dwFactionRealPoint ( 0 )
				, dwRanking ( 0 )
				, bApproachGoal ( false )
			{

			}
		};

		typedef		std::vector< SFACTION_POINT >		VEC_FACTION_POINT;
		typedef		VEC_FACTION_POINT::iterator			VEC_FACTION_POINT_ITER;


		// SFACTION_POINT 의 비교연산;
		class CCompare_SFACTION_POINT
		{
		public:
			CCompare_SFACTION_POINT ()
			{
			}

		public:
			bool operator () ( const SFACTION_POINT& sFactionPointA, const SFACTION_POINT& sFactionPointB )
			{
				return sFactionPointA.dwFactionRealPoint > sFactionPointB.dwFactionRealPoint;
			}
		};

	}

}