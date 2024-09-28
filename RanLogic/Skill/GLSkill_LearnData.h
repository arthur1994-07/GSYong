#pragma once

namespace SKILL
{
	//	래밸 별 습득 조건.
	struct SLEARN_LVL_101	//	데이터 포맷 변경 진행
	{
		DWORD				dwSKP;					//	요구 스킬 포인트.
		DWORD				dwLEVEL;				//	요구 Level.
		SCHARSTATS			sSTATS;					//	요구 Stats.

		DWORD				dwSKILL_LVL;			//	요구 보유스킬 래벨.

		SLEARN_LVL_101 (void);
	};

	struct SLEARN_LVL_102	//	데이터 포맷 변경 진행
	{
		DWORD				dwLEVEL;				//	요구 Level.
		SCHARSTATS			sSTATS;					//	요구 Stats.		
		DWORD				dwSKP;					//	요구 스킬 포인트.
		DWORD				dwSKILL_LVL;			//	요구 보유스킬 래벨.

		SLEARN_LVL_102 (void);
	};

	struct SLEARN_LVL_103	//	스킬습득 조건 추가( 한가지 만족 시 가능 유무 )
	{
		DWORD				dwLEVEL;				//	요구 Level.
		SCHARSTATS			sSTATS;					//	요구 Stats.		
		DWORD				dwSKP;					//	요구 스킬 포인트.
		DWORD				dwSKILL_LVL;			//	요구 보유스킬 래벨.

		SLEARN_LVL_103 (void);
	};

	struct SLEARN_LVL_104	//	스킬습득 조건 추가( 한가지 만족 시 가능 유무 )
	{
		DWORD				dwLEVEL;				//	요구 Level.
		SCHARSTATS			sSTATS;					//	요구 Stats.		
		DWORD				dwSKP;					//	요구 스킬 포인트.
		DWORD				dwSKILL_LVL;			//	요구 보유스킬 래벨.
		DWORD				dwReqMoney;				//  요구 돈;
		SLEARN_LVL_104 (void);
	};

	struct SLEARN_LVL : public SLEARN_LVL_104
	{
		SLEARN_LVL (void);
		void Assign ( SLEARN_LVL_101& sOldData );
		void Assign ( SLEARN_LVL_102& sOldData );
		void Assign ( SLEARN_LVL_103& sOldData );
	};
};