#pragma once

#include <vector>

namespace NSEffectLua
{
	// emEndCondition 	-- ��������
	enum EM_END_CONDITION
	{
		END_CONDITION_NONE = 0,	//			= 0	-- ���������� ����.  �ܺο��� �����Ǿ߸� �Ѵ�.
		END_CONDITION_AUTO = 1,	// 			= 1	-- m_fNextStepTime Ÿ���� �Ǹ� ���ᰡ �ȴ�.
	};

	//-- emScreenEffectType
	enum EM_SCREENEFFECT
	{
		SCREENEFFECT_NONE			= 0,	//		= 0	-- �ƹ��۾��� ���� �ʴ´�.
		SCREENEFFECT_BASE_TEXTURE	= 1,	//		= 1	-- SCREENEFFECT Texture ������.
	};

	// STEP_DATA
	struct STEP_DATA
	{
		EM_END_CONDITION	m_emEndCondition;				// ���� ����
		BOOL				m_bEgpInFrontOfEye;				// egp ����. ��ũ�� �տ��� ������ �ǵ��� matTrans �� ����.
		TSTRING				m_strEgpFile;					// ���� play �� .epg ����
		EM_SCREENEFFECT		m_emScreenEffectType;			// type ����.
		TSTRING				m_strScreenEffectTexture;		// ScreeenEffect ���� ���̴� Texture
		TSTRING				m_strScreenEffectShader;		// ScreeenEffect ���� ���̴� Shader
		float				m_fScreenEffectCreatingTime;	// ScreeenEffect �� ���� �ɶ� �ð� ����
		float				m_fScreenEffectDestroyingTime;	// ScreeenEffect �� ���� �ɶ� �ð� ����
		float				m_fNextStepTime;				// ���� �������� �̵��ϴ� �ð�

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