
#include "pch.h"

#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxTools/EditMeshs.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../SigmaCore/File/TxtFile.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/String/StringFormatW.h"
#include "./GLFlyCameraKeyFrameControl.h"

//#include "../Widget/GLWidgetScript.h"

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

GLFlyCameraKeyFrameControl::GLFlyCameraKeyFrameControl(void)
: m_nFPS(60)
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

void GLFlyCameraKeyFrameControl::AddMessage(FlyCameraControl::SMsg* sMsg)
{
	m_MsgQueue.push(*sMsg);
}

bool GLFlyCameraKeyFrameControl::PopMessage(FlyCameraControl::SMsg* sMsg)
{
	if ( !sMsg )	return false;
	if ( m_MsgQueue.empty() )	return false;
	*sMsg = m_MsgQueue.front();
	m_MsgQueue.pop();
	return true;
}

void GLFlyCameraKeyFrameControl::MsgProcedures (void)
{
	FlyCameraControl::SMsg msg;
	while ( PopMessage(&msg))
	{
		DWORD dwType(msg.type), dwMsg(msg.dwMsg);
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

					// 				GLWidgetScript::GetInstance().LuaCallFunc< void >( 
					// 					NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_MsgError,
					// 					"-s", strErrMsg );
				}
			}
			break;
		case FlyCameraControl::EMFLYCAMERA_DEL_KEYFRAMESLOT:
			{
				if ( !this->DelSlot(dwMsg) )
				{
					const char* strErrMsg = "err. Nonexistent Slot";

					// 				GLWidgetScript::GetInstance().LuaCallFunc< void >( 
					// 					NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_MsgError,
					// 					"-s", strErrMsg );
				}
			}
			break;
		case FlyCameraControl::EMFLYCAMERA_SET_KEYPOSITION:
			{
				m_nCurrentKey = (int)msg.vec.w;
				D3DXVECTOR3 vec(msg.vec.x, msg.vec.y, msg.vec.z);
				this->EditKeyFrameInfo( vec, true );
			}
			break;

		case FlyCameraControl::EMFLYCAMERA_SET_KEYLOOKAT:
			{
				m_nCurrentKey = (int)msg.vec.w;
				D3DXVECTOR3 vec(msg.vec.x, msg.vec.y, msg.vec.z);
				this->EditKeyFrameInfo( vec, false );
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
		case FlyCameraControl::EMFLYCAMERA_EDIT_KEYFRAME:
			{
				FlyCameraControl::FlyCameraKey& sKeyData = (FlyCameraControl::FlyCameraKey&)GetKeyData(msg.dwMsg);
				sKeyData.nKey = msg.dwData;
				SortKeyframelist();
			}
			break;
		}
	}
}

void GLFlyCameraKeyFrameControl::Update(float fTime, float fElapsedTime)
{
	this->MsgProcedures();

	if ( !m_bPlaying )
		return ;

	float fProgressTime = m_fProgressTime + (fElapsedTime * (m_bRewind ? -1.0f:1.0f));
	const int nKeySize = (int)m_vecKeyFrame.size();	

	if ( fProgressTime < 0.0f || (float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS < fProgressTime )
	{
		fProgressTime = m_bRewind == false ? (float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS:0.0f;
		m_bPlaying = m_bRewind = false;
	}

	const int nCurrentKey = fProgressTime * m_nFPS;

	// 상대적으로 움직이기 때문에 이전위치 정보도 필요함으로, 이전 시간도 실행할 앵커가 없을 때, 유효한 앵커 위치로 강제 세팅;
	if ( m_bRewind && ((float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS < m_fProgressTime) ) 
		m_fProgressTime = (float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS;
	else if	 (!m_bRewind && (m_fProgressTime<(float)m_vecKeyFrame[0].nKey/(float)m_nFPS) ) 
		m_fProgressTime = (float)m_vecKeyFrame[0].nKey/(float)m_nFPS;

	// 이동변화량;
	{
		D3DXVECTOR3 vNextPos, vBeforePos;
		// 다음 위치 구함;
		{
			vNextPos = GetCatmullromSplineCalc(fProgressTime, true);
		}

		// 이전 시간대의 위치구함;
		{
			const float fBeforTime = m_fProgressTime;
			vBeforePos = GetCatmullromSplineCalc(fBeforTime, true);
		}

		m_vReservMove = vNextPos - vBeforePos;
	}

	// 보는곳 변화량
	// 이동변화량;
	{
		D3DXVECTOR3 vNextPos, vBeforePos;
		// 다음 위치 구함;
		vNextPos = GetCatmullromSplineCalc(fProgressTime, false);

		// 이전 시간대의 위치구함;
		const float fBeforTime = m_fProgressTime;
		vBeforePos = GetCatmullromSplineCalc(fBeforTime, false);

		m_vReservLook = vNextPos - vBeforePos;
	}



	// Parent가 여러 객체가 될 수있다; InGameClient, Tool
	// 템플릿화 시키자;
	// m_pParentCtrl->AddAfterAction(FlyCameraControl::EMFLYCAMERA_AFTER_ANIMATION_MOVE);

	// 현재 키 프레임이 변화되었으면 UI 프로그래스 업데이트
	if( m_nCurrentKey != nCurrentKey )
	{
// 		GLWidgetScript::GetInstance().LuaCallFunc< void >(
// 			NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_UpdateProgress,
// 			"-n", nCurrentKey );
	}

	m_fProgressTime = fProgressTime;
	m_nCurrentKey = nCurrentKey;

	if ( m_fProgressTime <= 0.0f || (float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS <= m_fProgressTime )
	{
		m_bPlaying = m_bRewind = false;
	}
}

void GLFlyCameraKeyFrameControl::Render(float fTime, float fElapsedTime)
{
	if ( m_bPlaying )
		return ;

	const INT nKeySize(m_vecKeyFrame.size()), nRenderSize(nKeySize*100);
	
	if ( nKeySize >= 2 )
	{
		const float nStartTime((float)m_vecKeyFrame[0].nKey/(float)m_nFPS)
			, nEndTime((float)m_vecKeyFrame[nKeySize-1].nKey/(float)m_nFPS)
			, nAllTime(nEndTime-nStartTime)
			, fRenderUnitTime((float)nAllTime/(float)nRenderSize);

		float fProgressTime = 0.0f;
		for ( INT i = 1; i < nRenderSize; ++i )
		{
			float fNextTime(fProgressTime+fRenderUnitTime);
			D3DXVECTOR3 vNext, vBefore;
			// Camera Frome
			vNext = GetCatmullromSplineCalc(fNextTime, true);
			vBefore = GetCatmullromSplineCalc(fProgressTime, true);
			EDITMESHS::RENDERLINE( vBefore, vNext );

			// Camera Lookat
			vNext = GetCatmullromSplineCalc(fNextTime, false);
			vBefore = GetCatmullromSplineCalc(fProgressTime, false);
			EDITMESHS::RENDERLINE( vBefore, vNext );

			fProgressTime += fRenderUnitTime;
		}
	}

	for ( int i = 0; i < nKeySize; ++i )
	{
		EDITMESHS::RENDERSPHERE(m_vecKeyFrame[i].sLookToTargetID.vPos, D3DXVECTOR3(2.0f,2.0f,2.0f));
		EDITMESHS::RENDERSPHERE(m_vecKeyFrame[i].sMoveToTargetID.vPos, D3DXVECTOR3(2.0f,2.0f,2.0f), FALSE, NULL, 0xffff0000);

		// FromPt와 LookatPt와 연결선
		EDITMESHS::RENDERLINE( m_vecKeyFrame[i].sLookToTargetID.vPos
			, m_vecKeyFrame[i].sMoveToTargetID.vPos, 0xffff8888 );

		CString strAncher;
		strAncher.Format("Anchor-%d", i);
		D3DXVECTOR3 vScreen(DxViewPort::GetInstance().ComputeVec3Project(&m_vecKeyFrame[i].sMoveToTargetID.vPos, NULL));
		CDebugSet::ToPos(vScreen.x, vScreen.y, "%s", strAncher.GetString());
	}
	//CDebugSet::m_pFont->DrawText ( XPos, info.YPos[i], fontColor, info.szInfo[i] );
}

// 
D3DXVECTOR3 GLFlyCameraKeyFrameControl::GetCatmullromSplineCalc(float fProgressTime, bool bMove)
{
	const int nKey = fProgressTime * m_nFPS;
	FlyCameraControl::FlyCameraAnchors Anchors;
	Anchors = GetAnchors(fProgressTime);
	const unsigned int nOrigPoint = Anchors.vStartAnchor.nKey
		, nDestPoint = Anchors.vDestAnchor.nKey
		, nDividCount = nDestPoint - nOrigPoint;

	float _t1 = (float)(nKey-nOrigPoint)/nDividCount		// 1-t
		, _t2 = _t1 * _t1
		, _t3 = _t2 * _t1;

	D3DXVECTOR3 v0, v1, v2, v3;
	if (bMove == true)
	{
		v0 = Anchors.vPreviAnchor.sMoveToTargetID.vPos;
		v1 = Anchors.vStartAnchor.sMoveToTargetID.vPos;
		v2 = Anchors.vDestAnchor.sMoveToTargetID.vPos;
		v3 = Anchors.vNextAnchor.sMoveToTargetID.vPos;
	}
	else
	{
		v0 = Anchors.vPreviAnchor.sLookToTargetID.vPos;
		v1 = Anchors.vStartAnchor.sLookToTargetID.vPos;
		v2 = Anchors.vDestAnchor.sLookToTargetID.vPos;
		v3 = Anchors.vNextAnchor.sLookToTargetID.vPos;
	}

	const float	fResult_X = FlyCameraControl::CATMULL_ROM_SPLINE(_t1, _t2, _t3, v0.x, v1.x, v2.x, v3.x)
		, fResult_Y = FlyCameraControl::CATMULL_ROM_SPLINE(_t1, _t2, _t3, v0.y, v1.y, v2.y, v3.y)
		, fResult_Z = FlyCameraControl::CATMULL_ROM_SPLINE(_t1, _t2, _t3, v0.z, v1.z, v2.z, v3.z);
	
	return D3DXVECTOR3(fResult_X, fResult_Y, fResult_Z);
}

int GLFlyCameraKeyFrameControl::GetKeyFrameSize(void) const
{
	return static_cast<int>(m_vecKeyFrame.size());
}

int GLFlyCameraKeyFrameControl::GetKeyFrame(int nAt) const
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

D3DXVECTOR3 GLFlyCameraKeyFrameControl::GetReserveLook(void)
{
	D3DXVECTOR3 vGet = m_vReservLook;
	m_vReservLook = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	return vGet; 
}

bool GLFlyCameraKeyFrameControl::AddSlot(int nKey)
{
	if ( nKey < 0 )
		return false;

	KEYFRAME_VEC_ITR& itr = std::find_if(m_vecKeyFrame.begin()
		, m_vecKeyFrame.end()
		, FINDINDEX_FLYCAMERAKEY_VEC(nKey));

	if ( itr != m_vecKeyFrame.end() )
		return false;

	KEYFRAME_VEC_VALUE_T key;
	key.nKey = nKey;
	m_vecKeyFrame.push_back(key);

	SortKeyframelist();

	if ( m_nTotalFrame < nKey )
		m_nTotalFrame = nKey;

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
	if ( nKey == m_nTotalFrame && m_vecKeyFrame.empty() == false )
		m_nTotalFrame = (*m_vecKeyFrame.rbegin()).nKey;

	return true;
}

void GLFlyCameraKeyFrameControl::EditKeyFrameInfo(D3DXVECTOR3 vec, bool bPos)
{
	KEYFRAME_VEC_ITR& itr = std::find_if(m_vecKeyFrame.begin()
		, m_vecKeyFrame.end()
		, FINDINDEX_FLYCAMERAKEY_VEC(m_nCurrentKey));
	if ( itr != m_vecKeyFrame.end() )
	{
		KEYFRAME_VEC_VALUE_T& key = (*itr);
		if ( bPos )
			key.sMoveToTargetID.vPos = vec;
		else
			key.sLookToTargetID.vPos = vec;
	}
}

void GLFlyCameraKeyFrameControl::PlayVideo(bool bRewind)
{
	if ( m_vecKeyFrame.size() == 0 )
	{
		const char* strErrMsg = "err. Key frame size is 0";

// 		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
// 			NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_MsgError,
// 			"-s", strErrMsg );

		return;
	}

	if ( m_vecKeyFrame[0].nKey != 0 )
	{
		const char* strErrMsg = "0 Key frame is Null!";

// 		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
// 			NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_MsgError,
// 			"-s", strErrMsg );

		return;
	}

	m_bRewind = bRewind;
// 	if ( m_bPlaying )
// 		return;
	
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

	m_vReservMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vReservLook = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
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

void GLFlyCameraKeyFrameControl::SortKeyframelist(void)
{
	KEYFRAME_VEC_ITR& itrBegin = m_vecKeyFrame.begin(), itrEnd = m_vecKeyFrame.end();
	std::sort(itrBegin, itrEnd, SORTINDEX_FLYCAMERAKEY());
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
// 	LuaTable tbKeyFrame( GLWidgetScript::GetInstance().GetLuaState() );
// 
// 	KEYFRAME_VEC_CITR iter = m_vecKeyFrame.begin();
// 	for( int i = 1; iter != m_vecKeyFrame.end(); ++iter, ++i )
// 		tbKeyFrame.set( i, iter->nKey );
// 
// 	GLWidgetCallFuncValue arg;
// 	arg.SetTable( &tbKeyFrame );
// 
// 	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
// 		NSWIDGET_SCRIPT::g_strFunc_FlyCamCtrl_AddKeyFrame, &arg, 1 );
}

void GLFlyCameraKeyFrameControl::ResetAll(void)
{
	m_vecKeyFrame.clear();
}

HRESULT GLFlyCameraKeyFrameControl::Load(CString strPath)
{
	sc::lua_destroyState();
	sc::lua_init();
	if (!sc::lua_doFile(strPath.GetString()))
		return E_FAIL;

	// 모든 키프레임 데이터 지운다;
	ResetAll();

	// 키프레임 데이터 로드;
	try
	{
		LuaPlus::LuaObject luaObjKeyframelist = sc::lua_getGlobalFromName("keyframelist");
		for ( LuaPlus::LuaTableIterator Iter(luaObjKeyframelist); Iter; Iter.Next() )
		{
			LuaPlus::LuaObject luaObjKeyframe = Iter.GetValue();
			LuaPlus::LuaObject luaObjKeyframe_key = luaObjKeyframe.GetByName("nKeyFrame");
			LuaPlus::LuaObject luaObjKeyframe_eyepos = luaObjKeyframe.GetByName("vEyepos");
			LuaPlus::LuaObject luaObjKeyframe_lookat = luaObjKeyframe.GetByName("vLookat");

			// 프레임;
			const INT nKeyFrame(luaObjKeyframe_key.GetInteger());

			D3DXVECTOR3 vEyepos, vLookat;

			// 위치;
			LuaPlus::LuaTableIterator luaItrEyepos(luaObjKeyframe_eyepos);
			LuaPlus::LuaObject luaObjEyepos = luaItrEyepos.GetValue();
			vEyepos.x = luaObjEyepos.GetFloat();	luaItrEyepos.Next();	luaObjEyepos = luaItrEyepos.GetValue();
			vEyepos.y = luaObjEyepos.GetFloat();	luaItrEyepos.Next();	luaObjEyepos = luaItrEyepos.GetValue();
			vEyepos.z = luaObjEyepos.GetFloat();

			// 보는곳;
			LuaPlus::LuaTableIterator luaItrLookat(luaObjKeyframe_lookat);
			LuaPlus::LuaObject luaObjLookat = luaItrLookat.GetValue();
			vLookat.x = luaObjLookat.GetFloat();	luaItrLookat.Next();	luaObjLookat = luaItrLookat.GetValue();
			vLookat.y = luaObjLookat.GetFloat();	luaItrLookat.Next();	luaObjLookat = luaItrLookat.GetValue();
			vLookat.z = luaObjLookat.GetFloat();

			FlyCameraControl::FlyCameraKey keyData;
			keyData.nKey = nKeyFrame;
			keyData.sMoveToTargetID.vPos = vEyepos;
			keyData.sLookToTargetID.vPos = vLookat;
			m_vecKeyFrame.push_back(keyData);
		}

		SortKeyframelist();
	}
	catch( const LuaPlus::LuaException& e )
	{
		std::string ErrMsg(
			sc::string::format(
			"Plz check %1%, %2%",
			strPath.GetString(),
			e.GetErrorMessage()));
		sc::ErrorMessageBox(ErrMsg, false);
		return E_FAIL;
	}

	sc::lua_destroyState();
	sc::lua_init();
	return S_OK;
}

HRESULT GLFlyCameraKeyFrameControl::Save(CString strPath)
{
	sc::TxtFile LuaFile(strPath.GetString(), true, sc::TxtFile::UTF16LE);
	LuaFile.write(L"-------------------------------------------------------------------------------");
	LuaFile.write(L"-- FlyCamera keyframe list 세팅파일");
	LuaFile.write(L"-- 2016-07-28 gbgim");
	LuaFile.write(L"--");
	LuaFile.write(L"-- 주의)");
	LuaFile.write(L"-- 1) 대/소 문자를 확실하게 구분해야 합니다.");
	LuaFile.write(L"-- 2) 1, 0, true, false, null 을 확실하게 구분해야 합니다.");
	LuaFile.write(L"-- 3) UCS-2 Little Endian 로 저장해야 합니다.");
	LuaFile.write(L"-- 4) 문자열을 작성할때는 L\"문자열\" 로 작성하십시오. (unicode)");
	LuaFile.write(L"-------------------------------------------------------------------------------");
	LuaFile.write(L"keyframelist =");
	LuaFile.write(L"{");
	KEYFRAME_VEC_ITR itr = m_vecKeyFrame.begin()
		, itrEnd = m_vecKeyFrame.end();
	for ( ; itr != itrEnd; ++itr )
	{
		const FlyCameraControl::FlyCameraKey& keyData = *itr;
		LuaFile.write(L"    {");
		LuaFile.write(sc::string::wformat(L"        nKeyFrame = %1%,", keyData.nKey));
		const D3DXVECTOR3 vEyepos(keyData.sMoveToTargetID.vPos)
			, vLookat(keyData.sLookToTargetID.vPos);
		LuaFile.write(sc::string::wformat(L"        vEyepos = {%1%, %2%, %3%},", vEyepos.x, vEyepos.y, vEyepos.z));
		LuaFile.write(sc::string::wformat(L"        vLookat = {%1%, %2%, %3%},", vLookat.x, vLookat.y, vLookat.z));
		LuaFile.write(L"    },");
	}
	LuaFile.write(L"}");
	return S_OK;
}