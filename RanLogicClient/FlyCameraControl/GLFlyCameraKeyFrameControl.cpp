
#include "pch.h"

#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/G-Logic/GLDefine.h"

#include "../GLGaeaClient.h"

#include "./GLFlyCameraControl.h"
#include "./GLFlyCameraKeyFrameControl.h"

#include "../Widget/GLWidgetScript.h"

/*
struct FINDINDEX_FLYCAMERAKEY_VEC
{
	int nKey;
	FINDINDEX_FLYCAMERAKEY_VEC(const int Key):nKey(Key){}
	bool operator()(GLFlyCameraKeyFrameControl::KEYFRAME_VEC_VALUE_T& key) const		
	{
		if ( nKey == key.nKey )	return true;
		return false;
	}
};

struct SORTINDEX_FLYCAMERAKEY
{
public:
	bool operator()(const GLFlyCameraKeyFrameControl::KEYFRAME_VEC_VALUE_T& lhs
		, const GLFlyCameraKeyFrameControl::KEYFRAME_VEC_VALUE_T& rhs)
	{
		return lhs.nKey < rhs.nKey;
	}
};

GLFlyCameraKeyFrameControl::GLFlyCameraKeyFrameControl(GLGaeaClient* pGaeaClient, GLFlyCameraControl* pParentCtrl)
: m_pGaeaClient(pGaeaClient)
, m_pParentCtrl(pParentCtrl)
, m_nFPS(60)
, m_nTotalFrame(300)
, m_bPlaying(false)
, m_bRewind(false)
{
}

GLFlyCameraKeyFrameControl::~GLFlyCameraKeyFrameControl(void)
{
}

void GLFlyCameraKeyFrameControl::Init(void)
{	
}

void GLFlyCameraKeyFrameControl::MsgProcedures ( DWORD dwType, DWORD dwMsg )
{
	switch ( dwType )
	{
	case FlyCameraControl::EMFLYCAMERA_SET_CURRENTKEYFRAME:
		m_nCurrentKey = dwMsg;
		break;
	case FlyCameraControl::EMFLYCAMERA_ADD_KEYFRAMESLOT:
		{
			if ( !this->AddSlot(dwMsg) )
			{
				const char* strErrMsg = "err. Overlap Slot";

				GLWidgetScript::GetInstance().LuaCallFunc< void >( 
					NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_MsgError,
					"-s", strErrMsg );
			}
		}
		break;
	case FlyCameraControl::EMFLYCAMERA_DEL_KEYFRAMESLOT:
		{
			if ( !this->DelSlot(dwMsg) )
			{
				const char* strErrMsg = "err. Nonexistent Slot";

				GLWidgetScript::GetInstance().LuaCallFunc< void >( 
					NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_MsgError,
					"-s", strErrMsg );
			}
		}
		break;
	case FlyCameraControl::EMFLYCAMERA_SET_KEYPOSITION:
		{
			LuaTable tbPos = GLWidgetScript::GetInstance().LuaCallFunc< LuaTable >( 
				NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_GetKeyFramePos );

			D3DXVECTOR3 vPos( tbPos.get< float >( 1 ), tbPos.get< float >( 2 ), tbPos.get< float >( 3 ) );
			this->EditKeyFrameInfo( vPos );
		}
		break;
	case FlyCameraControl::EMFLYCAMERA_SET_TOTALFRAME:
		this->SetTotalFrame(dwMsg);
		break;
	case FlyCameraControl::EMFLYCAMERA_PLAY_ANIMATION:
		this->PlayVideo();
		break;
	case FlyCameraControl::EMFLYCAMERA_SET_FPS:
		m_nFPS = dwMsg;
		break;
	case FlyCameraControl::EMFLYCAMERA_REWIND_ANIMATION:
		this->PlayVideo(true);
		break;
	case FlyCameraControl::EMFLYCAMERA_STOP_ANIMATION:
		m_bPlaying = false;
		break;
	}
}

void GLFlyCameraKeyFrameControl::Update(float fTime, float fElapsedTime)
{
	if ( !m_bPlaying )
		return ;

	float fProgressTime = m_fProgressTime + (fElapsedTime * (m_bRewind ? -1.0f:1.0f));
	const int nKeySize = (int)m_vecKeyFrame.size();	

	if ( fProgressTime < 0.0f || (float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS < fProgressTime )
	{
		fProgressTime = m_bRewind == false ? (float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS:0.0f;
		m_bPlaying = m_bRewind = false;
	}

	// 마지막으로 입력된 프레임보다 전체 프레임 개수가 많을 경우, 앵커가 위치갱신을 제대로 못한다;
// 	const float fValidTime = (float)(m_vecKeyFrame[nKeySize-1].nKey - m_vecKeyFrame[0].nKey)/(float)m_nFPS;
// 	//if ( fValidTime + ((float)(m_vecKeyFrame[0].nKey)/(float)m_nFPS) < fProgressTime )
// 	if ( ((float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS < fProgressTime) )
// 	{
// 		fProgressTime = (float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS;
// 		m_bPlaying = false;
// 	}

//	const float fTimelinePos = m_bRewind ? (float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS - fProgressTime: fProgressTime;
//	const float fVaildStartT = m_bRewind ? (float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS : m_vecKeyFrame[0].nKey/(float)m_nFPS;
	const int nCurrentKey = fProgressTime * m_nFPS;

	// Play시 실행할 앵커가 없을 때, 유효한 앵커 위치까지 대기한다;
// 	if ( ( m_bRewind && ((float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS < fProgressTime) ) ||
// 		 (!m_bRewind && (fProgressTime<(float)m_vecKeyFrame[0].nKey/(float)m_nFPS) ) )
// 	{
// 		m_fProgressTime = fProgressTime;
// 		m_nCurrentKey = nCurrentKey;
// 		return;
// 	}

	// 상대적으로 움직이기 때문에 이전위치 정보도 필요함으로, 이전 시간도 실행할 앵커가 없을 때, 유효한 앵커 위치로 강제 세팅;
	if ( m_bRewind && ((float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS < m_fProgressTime) ) 
		m_fProgressTime = (float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS;
	else if	 (!m_bRewind && (m_fProgressTime<(float)m_vecKeyFrame[0].nKey/(float)m_nFPS) ) 
		m_fProgressTime = (float)m_vecKeyFrame[0].nKey/(float)m_nFPS;

	/*
	// Play 최초 시 0프레임에 데이터가 없을 경우, 앵커를 제대로 읽어오지 못함으로 건너뜀;
	if ( m_vecKeyFrame[0].nKey != 0 && fProgressTime < m_vecKeyFrame[0].nKey/m_nFPS )
	{
		m_fProgressTime = fProgressTime;
		m_nCurrentKey = nCurrentKey;
		return;
	}
	// 마지막으로 입력된 프레임보다 전체 프레임 개수가 많을 경우, 앵커가 위치갱신을 제대로 못한다;
	else if ( (float)m_vecKeyFrame[nKeySize-1].nKey/m_nFPS < fProgressTime )
	{
		fProgressTime = m_vecKeyFrame[nKeySize-1].nKey/m_nFPS;
		m_bPlaying = false;
	}	
	
	// 여기서 부터 진정한 업데이트;
	int nNextKey = nKeySize > nCurrentIdx+1 ? m_vecKeyFrame[nCurrentIdx+1].nKey : m_vecKeyFrame[nCurrentIdx].nKey;//-1;
	
	// 키위치가 바낄때에만 앵커 정보를 갱신한다, 마지막 프레임일때는 키정보를 갱신하지않는다;
	if ( nNextKey <= nCurrentKey && m_vecKeyFrame[nKeySize-1].nKey > nCurrentKey )
	{
		++nCurrentIdx;

		int nIDX = nCurrentIdx;
		KEYFRAME_VEC_VALUE_T& Key = m_vecKeyFrame[nCurrentIdx];
	
		const KEYFRAME_VEC_VALUE_T& PreviKey = nCurrentIdx == 0 ? m_vecKeyFrame[nIDX] : m_vecKeyFrame[--nIDX];
		const KEYFRAME_VEC_VALUE_T& FirstKey = nCurrentIdx == 0 ? m_vecKeyFrame[nIDX] : m_vecKeyFrame[++nIDX];
		const KEYFRAME_VEC_VALUE_T& SecondKey = nKeySize < 2 ? m_vecKeyFrame[nIDX] : m_vecKeyFrame[++nIDX];
		const KEYFRAME_VEC_VALUE_T& ThirdKey = (nCurrentIdx == nKeySize-2) || (nKeySize<3) ? m_vecKeyFrame[nIDX] : m_vecKeyFrame[++nIDX];
 		Anchors.vPreviAnchor = PreviKey;
 		Anchors.vStartAnchor = FirstKey;
 		Anchors.vDestAnchor = SecondKey;
 		Anchors.vNextAnchor = ThirdKey;
		Anchors = GetAnchors(fProgressTime);
	}

	D3DXVECTOR3 vNextPos, vBeforePos;
	// 다음 위치 구함;
	{
		FlyCameraControl::FlyCameraAnchors Anchors;
		Anchors = GetAnchors(fProgressTime);
		const unsigned int nOrigPoint = Anchors.vStartAnchor.nKey
			, nDestPoint = Anchors.vDestAnchor.nKey
			, nDividCount = nDestPoint - nOrigPoint;

		float _t1 = (float)(nCurrentKey-nOrigPoint)/nDividCount		// 1-t
			, _t2 = _t1 * _t1
			, _t3 = _t2 * _t1;

		const D3DXVECTOR3 v0 = Anchors.vPreviAnchor.sMoveToTargetID.vPos
			, v1 = Anchors.vStartAnchor.sMoveToTargetID.vPos
			, v2 = Anchors.vDestAnchor.sMoveToTargetID.vPos
			, v3 = Anchors.vNextAnchor.sMoveToTargetID.vPos;

		const float	fResult_X = FlyCameraControl::CATMULL_ROM_SPLINE(_t1, _t2, _t3, v0.x, v1.x, v2.x, v3.x)
			, fResult_Y = FlyCameraControl::CATMULL_ROM_SPLINE(_t1, _t2, _t3, v0.y, v1.y, v2.y, v3.y)
			, fResult_Z = FlyCameraControl::CATMULL_ROM_SPLINE(_t1, _t2, _t3, v0.z, v1.z, v2.z, v3.z);
		vNextPos = D3DXVECTOR3(fResult_X, fResult_Y, fResult_Z);
	}

	// 이전 시간대의 위치구함;
	{
		const float fBeforTime = m_fProgressTime;// m_bRewind ? (float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS - m_fProgressTime: m_fProgressTime;
		int nBeforeKey = static_cast<int>(fBeforTime*m_nFPS);
		FlyCameraControl::FlyCameraAnchors _beforeAnchors = GetAnchors(fBeforTime);
		unsigned int nOrigPoint = _beforeAnchors.vStartAnchor.nKey
			, nDestPoint = _beforeAnchors.vDestAnchor.nKey
			, nDividCount = nDestPoint - nOrigPoint;

		float _t1 = (float)(nBeforeKey-nOrigPoint)/nDividCount		// 1-t
			, _t2 = _t1 * _t1
			, _t3 = _t2 * _t1;

		const D3DXVECTOR3 v0 = _beforeAnchors.vPreviAnchor.sMoveToTargetID.vPos
			, v1 = _beforeAnchors.vStartAnchor.sMoveToTargetID.vPos
			, v2 = _beforeAnchors.vDestAnchor.sMoveToTargetID.vPos
			, v3 = _beforeAnchors.vNextAnchor.sMoveToTargetID.vPos;

		const float	fResult_X = FlyCameraControl::CATMULL_ROM_SPLINE(_t1, _t2, _t3, v0.x, v1.x, v2.x, v3.x)
			, fResult_Y = FlyCameraControl::CATMULL_ROM_SPLINE(_t1, _t2, _t3, v0.y, v1.y, v2.y, v3.y)
			, fResult_Z = FlyCameraControl::CATMULL_ROM_SPLINE(_t1, _t2, _t3, v0.z, v1.z, v2.z, v3.z);
		vBeforePos = D3DXVECTOR3(fResult_X, fResult_Y, fResult_Z);
	}

	m_vReservMove = vNextPos - vBeforePos;
	m_pParentCtrl->AddAfterAction(FlyCameraControl::EMFLYCAMERA_AFTER_ANIMATION_MOVE);

	// 현재 키 프레임이 변화되었으면 UI 프로그래스 업데이트
	if( m_nCurrentKey != nCurrentKey )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_UpdateProgress,
			"-n", nCurrentKey );
	}

	m_fProgressTime = fProgressTime;
	m_nCurrentKey = nCurrentKey;

	if ( m_fProgressTime <= 0.0f || (float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS <= m_fProgressTime )
	{
		m_bPlaying = m_bRewind = false;
	}
}

int GLFlyCameraKeyFrameControl::GetKeyFrameSize(void) const
{
	return static_cast<int>(m_vecKeyFrame.size());
}

int GLFlyCameraKeyFrameControl::GetKeyIndex(int nAt) const
{
	if ( static_cast<int>(m_vecKeyFrame.size()) > nAt )
		return m_vecKeyFrame[nAt].nKey;

	return -1;
}

const GLFlyCameraKeyFrameControl::KEYFRAME_VEC_VALUE_T& GLFlyCameraKeyFrameControl::GetKeyData(int nKey)
{
	//std::advance()
	static FlyCameraControl::FlyCameraKey sFlyKey;

 	if ( nKey < 0 || m_nTotalFrame < nKey )
 		return sFlyKey;

	KEYFRAME_VEC_ITR& itr = std::find_if(m_vecKeyFrame.begin()
		, m_vecKeyFrame.end()
		, FINDINDEX_FLYCAMERAKEY_VEC(nKey));
	if ( itr == m_vecKeyFrame.end() )
		return sFlyKey;

	return (*itr);
}

D3DXVECTOR3 GLFlyCameraKeyFrameControl::GetReserveMove(void)
{
	D3DXVECTOR3 vGet = m_vReservMove;
	m_vReservMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	return vGet; 
}


bool GLFlyCameraKeyFrameControl::AddSlot(int nKey)
{
	if ( nKey < 0 || m_nTotalFrame < nKey )
		return false;

	KEYFRAME_VEC_ITR& itr = std::find_if(m_vecKeyFrame.begin()
		, m_vecKeyFrame.end()
		, FINDINDEX_FLYCAMERAKEY_VEC(nKey));

	if ( itr != m_vecKeyFrame.end() )
		return false;

	KEYFRAME_VEC_VALUE_T key;
	key.nKey = nKey;
	m_vecKeyFrame.push_back(key);

	KEYFRAME_VEC_ITR& itrBegin = m_vecKeyFrame.begin(), itrEnd = m_vecKeyFrame.end();
	// m_vecKeyFrame.sort(SORTINDEX_FLYCAMERAKEY());
	std::sort(itrBegin, itrEnd, SORTINDEX_FLYCAMERAKEY());

	// UI에 키프레임 정보 보내기
	SendToWidgetKeyFrameInfo();

	return true;
}

bool GLFlyCameraKeyFrameControl::DelSlot(int nKey)
{
	if ( nKey < 0 || m_nTotalFrame < nKey )
		return false;

	KEYFRAME_VEC_ITR& itr = std::find_if(m_vecKeyFrame.begin()
		, m_vecKeyFrame.end()
		, FINDINDEX_FLYCAMERAKEY_VEC(nKey));

	if ( itr == m_vecKeyFrame.end() )
		return false;

	m_vecKeyFrame.erase(itr);
	return true;
}

void GLFlyCameraKeyFrameControl::EditKeyFrameInfo(D3DXVECTOR3 vPosition)
{
	KEYFRAME_VEC_ITR& itr = std::find_if(m_vecKeyFrame.begin()
		, m_vecKeyFrame.end()
		, FINDINDEX_FLYCAMERAKEY_VEC(m_nCurrentKey));
	if ( itr != m_vecKeyFrame.end() )
	{
		KEYFRAME_VEC_VALUE_T& key = (*itr);
		key.sMoveToTargetID.vPos = vPosition;
	}
}

void GLFlyCameraKeyFrameControl::PlayVideo(bool bRewind)
{
	if ( m_vecKeyFrame.size() == 0 )
	{
		const char* strErrMsg = "err. Key frame size is 0";

		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_MsgError,
			"-s", strErrMsg );

		return;
	}

	if ( m_vecKeyFrame[0].nKey != 0 )
	{
		const char* strErrMsg = "0 Key frame is Null!";

		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_MsgError,
			"-s", strErrMsg );

		return;
	}

	m_bRewind = bRewind;
	if ( m_bPlaying )
		return;
	
	if ( bRewind == true )
	{
		const int nSize = (int)m_vecKeyFrame.size();
		m_nCurrentKey = m_vecKeyFrame[nSize-1].nKey;
		m_fProgressTime = (float)m_nCurrentKey/(float)m_nFPS;
		m_bPlaying = true;
	}
	else
	{
		m_fProgressTime = 0.0f;
		m_bPlaying = true;
		m_nCurrentKey = 0;
	}
}

FlyCameraControl::FlyCameraAnchors GLFlyCameraKeyFrameControl::GetAnchors(float fProgressTime)
{
	const int nProgressKey = fProgressTime * m_nFPS;
	const int nKeySize = static_cast<int>(m_vecKeyFrame.size());
	int nIndex = 0;
	
	// Play시 시간이 안되서 실행할 앵커가 없을 때;
	if ( m_vecKeyFrame[0].nKey > nProgressKey )					// forward
		nIndex = 0;
	else if ( m_vecKeyFrame[nKeySize - 1].nKey < nProgressKey )	// rewind
		nIndex = nKeySize-1;
	else
	{
		for ( int i = 0; i < nKeySize-1; ++i, ++nIndex )
		{
			if ( nProgressKey >= m_vecKeyFrame[i].nKey &&
				nProgressKey <= m_vecKeyFrame[i+1].nKey )
			{
				break;
			}
		}
	}

	int nIDX = nIndex;

	const KEYFRAME_VEC_VALUE_T& PreviKey = nIndex == 0 ? m_vecKeyFrame[nIDX] : m_vecKeyFrame[--nIDX];
	const KEYFRAME_VEC_VALUE_T& FirstKey = nIndex == 0 ? m_vecKeyFrame[nIDX] : m_vecKeyFrame[++nIDX];
	const KEYFRAME_VEC_VALUE_T& SecondKey = nKeySize < 2 ? m_vecKeyFrame[nIDX] : m_vecKeyFrame[++nIDX];
	const KEYFRAME_VEC_VALUE_T& ThirdKey = (nIndex == nKeySize-2) || (nKeySize<3) ? m_vecKeyFrame[nIDX] : m_vecKeyFrame[++nIDX];

	FlyCameraControl::FlyCameraAnchors Anchors;
	Anchors.vPreviAnchor = PreviKey ;
	Anchors.vStartAnchor = FirstKey ;
	Anchors.vDestAnchor =  SecondKey;
	Anchors.vNextAnchor =  ThirdKey ;
	return Anchors;
}

bool GLFlyCameraKeyFrameControl::isPlayVideo(void)
{
	bool isEnd = false;
	if ( m_bRewind )
		isEnd = m_fProgressTime == 0;
	else
		isEnd = m_fProgressTime >= static_cast<float>(m_nTotalFrame/m_nFPS);

	return isEnd | m_bPlaying;
}

void GLFlyCameraKeyFrameControl::SendToWidgetKeyFrameInfo()
{
	LuaTable tbKeyFrame( GLWidgetScript::GetInstance().GetLuaState() );

	KEYFRAME_VEC_CITR iter = m_vecKeyFrame.begin();
	for( int i = 1; iter != m_vecKeyFrame.end(); ++iter, ++i )
		tbKeyFrame.set( i, iter->nKey );

	GLWidgetCallFuncValue arg;
	arg.SetTable( &tbKeyFrame );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_AddKeyFrame, &arg, 1 );
}
*/