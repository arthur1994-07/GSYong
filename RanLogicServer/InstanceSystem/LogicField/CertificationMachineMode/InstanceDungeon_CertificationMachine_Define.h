#pragma once

#include <vector>
#include <map>

#include "../../../../enginelib/G-Logic/TypeDefine.h"

namespace InstanceSystem
{

	namespace CertificationMachineMode
	{

		// �⺻ ���� ( NPC );
		enum EMFACTION_BASIC
		{
			EMFACTION_BASIC_ALLY,
			EMFACTION_BASIC_ENEMY,
			EMFACTION_BASIC_NEUTRAL,
		};


		// Ÿ�̸�;
		enum EMTIMER_ID
		{
			EMTIMER_ID_GAME_DURATION										= 1,				// ���� ����ð� Ÿ�̸�;
			EMTIMER_ID_SAFETIME												= 2,				// ����� ���� �� �����ð� Ÿ�̸�;
			EMTIMER_ID_DESTROY_WAITTIME										= 3,				// �δ� �ı� ���ð� Ÿ�̸�;
			EMTIMER_ID_EXIT_JOIN_BUFFTIME									= 4,				// ������� ���� Ÿ�̸�;
			EMTIMER_ID_EXIT_JOIN_DEBUFFTIME									= 5,				// �������� ���� Ÿ�̸�;

			EMTIMER_ID_CERTIFY_POINT										= 6,				// ���� ����Ʈ üũ�ð� Ÿ�̸�;
			EMTIMER_ID_TRY_CERTIFY_WAITTIME									= 7,				// ���� �õ� ���ð� Ÿ�̸�;
			EMTIMER_ID_CERTIFICATION_MACHINE_LOCKTIME						= 8,				// �������� ���ѽð� Ÿ�̸�;

			EMTIMER_ID_ACCELERATOR_RECHARGE									= 9,				// ���ӱ� ��Ȱ��ȭ�ð� Ÿ�̸�;
			EMTIMER_ID_ACCELERATOR_BUFFTIME									= 10,				// ���ӱ� �����ð� Ÿ�̸�;

			EMTIMER_ID_CERTIFY_SUCCESS										= 11,				// ������ ���� ����;
			EMTIMER_ID_CERTIFICATION_MACHINE_NEUTRALIZATION					= 12,				// ������ �߸�ȭ Ÿ�̸�;
			EMTIMER_ID_CERTIFICATION_MACHINE_ACTIVATE_ADDPOINT				= 13,				// �������� AddPoint ���� Ȱ�� Ÿ�̸�;
			EMTIMER_ID_CERTIFICATION_MACHINE_INACTIVATE_ADDPOINT			= 14,				// �������� AddPoint ���� ��Ȱ�� Ÿ�̸�;
			EMTIMER_ID_CONFIRM_CERTIFICATION								= 15,				// �������� ���� ���� Ȯ�� Ÿ�̸�;

			EMTIMER_ID_REWARD_WINNER										= 16,				// �¸��� ���� Ÿ�̸�;
			EMTIMER_ID_REWARD_PARTICIPANT									= 17,				// ������ ���� Ÿ�̸�;

			EMTIMER_ID_HELP_NOTIFY_1										= 18,				// ���� ���� Ÿ�̸�1;
			EMTIMER_ID_HELP_NOTIFY_2										= 19,				// ���� ���� Ÿ�̸�2;
			EMTIMER_ID_HELP_NOTIFY_3										= 20,				// ���� ���� Ÿ�̸�3;
			EMTIMER_ID_ACCELERATOR_CREATE_NOTIFY							= 21,				// ���ӱ� ���� ���� Ÿ�̸�;
			EMTIMER_ID_ACCELERATOR_CAPTURE_NOTIFY							= 22,				// ���ӱ� ���� ���� Ÿ�̸�;
			EMTIMER_ID_ACCELERATOR_CAPTURE_ADDPOINT_NOTIFY					= 23,				// ���ӱ� ���� �� ����Ʈ ���� ���� Ÿ�̸�;
			EMTIMER_ID_ACCELERATOR_RECHARGE_NOTIFY							= 24,				// ���ӱ� ��Ȱ��ȭ ���� Ÿ�̸�;
			EMTIMER_ID_CERTIFY_SUCCESS_NOTIFY								= 25,				// ���� ���� ���� Ÿ�̸�;
			EMTIMER_ID_CERTIFY_FAIL_NOTIFY									= 26,				// ������ �Ҽ� ���� ���� ���� Ÿ�̸�;
			EMTIMER_ID_CERTIFY_CAPTURED_FAIL_NOTIFY							= 27,				// �̹� ������ �Ǿ� �ִ� ���� ���� Ÿ�̸�;
			EMTIMER_ID_CERTIFY_DUPLICATE_FAIL_NOTIFY						= 28,				// �ߺ� �������� ���� ���� ���� Ÿ�̸�;
			EMTIMER_ID_CERTIFICATION_MACHINE_NEUTRALIZATION_WAIT_NOTIFY		= 29,				// ������ �߸�ȭ ���1 ���� Ÿ�̸�;
			EMTIMER_ID_CERTIFICATION_MACHINE_NEUTRALIZATION_NOTIFY			= 30,				// ������ �߸�ȭ ���� Ÿ�̸�;
			EMTIMER_ID_APPROACHED_GOAL_NOTIFY								= 31,				// ��ǥ �޼� ���� Ÿ�̸�;
			EMTIMER_ID_EXIT_GAME_NOTIFY										= 32,				// �������� ���� Ÿ�̸�;
			EMTIMER_ID_CONFIRM_CERTIFICATION_NOTIFY							= 33,				// �������� Ȯ�� ���� Ÿ�̸�;
			EMTIMER_ID_TRY_CERTIFY_NOTIFY									= 34,				// ���� �õ� ���� Ÿ�̸�;

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


		// ������ ����Ʈ�� �����ϱ� ���� MAP;
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


		// ������ ����Ʈ �� ��ŷ����;
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


		// SFACTION_POINT �� �񱳿���;
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