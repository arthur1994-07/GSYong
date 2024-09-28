#pragma once

#include <vector>

namespace NSEffectLua
{
	// emEndCondition 	-- 종료조건
	enum EM_END_CONDITION
	{
		END_CONDITION_NONE = 0,	//			= 0	-- 종료조건이 없다.  외부에서 삭제되야만 한다.
		END_CONDITION_AUTO = 1,	// 			= 1	-- m_fNextStepTime 타임이 되면 종료가 된다.
	};

	//-- emScreenEffectType
	enum EM_SCREENEFFECT
	{
		SCREENEFFECT_NONE			= 0,	//		= 0	-- 아무작업도 하지 않는다.
		SCREENEFFECT_BASE_TEXTURE	= 1,	//		= 1	-- SCREENEFFECT Texture 렌더링.
	};

	// STEP_DATA
	struct STEP_DATA
	{
		EM_END_CONDITION	m_emEndCondition;				// 종료 조건
		BOOL				m_bEgpInFrontOfEye;				// egp 파일. 스크린 앞에서 렌더링 되도록 matTrans 값 변경.
		TSTRING				m_strEgpFile;					// 따로 play 될 .epg 파일
		EM_SCREENEFFECT		m_emScreenEffectType;			// type 종류.
		TSTRING				m_strScreenEffectTexture;		// ScreeenEffect 에서 쓰이는 Texture
		TSTRING				m_strScreenEffectShader;		// ScreeenEffect 에서 쓰이는 Shader
		float				m_fScreenEffectCreatingTime;	// ScreeenEffect 가 시작 될때 시간 동작
		float				m_fScreenEffectDestroyingTime;	// ScreeenEffect 가 종료 될때 시간 동작
		float				m_fNextStepTime;				// 다음 조건으로 이동하는 시간

		STEP_DATA()
			: m_emEndCondition(END_CONDITION_AUTO)
			, m_bEgpInFrontOfEye(FALSE)
			, m_strEgpFile("")
			, m_emScreenEffectType(SCREENEFFECT_NONE)
			, m_strScreenEffectTexture("")
			, m_strScreenEffectShader("")
			, m_fScreenEffectCreatingTime(0.f)
			, m_fScreenEffectDestroyingTime(0.f)
			, m_fNextStepTime(0.f)
		{
		}
	};
	typedef std::vector<boost::shared_ptr<STEP_DATA>>	VEC_STEP_DATA;

	// Loading
	BOOL LuaLoad();

	// Find
	const VEC_STEP_DATA* Find( const TCHAR* szName );
};