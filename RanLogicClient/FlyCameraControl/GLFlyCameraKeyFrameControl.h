
#pragma once
/*
#include "../../RanLogic/FlyCamera/FlyCameraDefine.h"

class GLGaeaClient;
class GLFlyCameraControl;

class GLFlyCameraKeyFrameControl
{
public:
	// 확실히 랜덤 액세스가 필요할때가 많다, 그렇지만 리스트를 사용하는데 있어서 이점도 생각해보면..;
	typedef std::vector<FlyCameraControl::FlyCameraKey>		KEYFRAME_VEC;
	typedef KEYFRAME_VEC::iterator			KEYFRAME_VEC_ITR;
	typedef KEYFRAME_VEC::const_iterator	KEYFRAME_VEC_CITR;
	typedef KEYFRAME_VEC::value_type		KEYFRAME_VEC_VALUE_T;

private:
	GLGaeaClient*		m_pGaeaClient;
	GLFlyCameraControl*	m_pParentCtrl;

	KEYFRAME_VEC		m_vecKeyFrame;

	int m_nFPS;
	int m_nTotalFrame;
	float m_fProgressTime;
	int m_nCurrentKey;
	int m_nCurrentIndex;

	bool m_bPlaying;
	bool m_bRewind;
	D3DXVECTOR3 m_vReservMove;

public:
	GLFlyCameraKeyFrameControl(GLGaeaClient* pGaeaClient, GLFlyCameraControl* pParentCtrl);
	virtual ~GLFlyCameraKeyFrameControl(void);

public:
	void Init(void);
	void Update(float fTime, float fElapsedTime);
	void MsgProcedures (FlyCameraControl::SMsg* msg);

public:
	int GetKeyFrameSize(void) const;
	int GetKeyIndex(int nAt) const;
	const KEYFRAME_VEC_VALUE_T& GetKeyData(int nKey);
	bool isPlayVideo(void);//	{ return m_bPlaying; }
	int GetFPS() { return m_nFPS; }
	int GetCurrentFrame(void) { return m_nCurrentKey; }
	int GetTotalFrame() { return m_nTotalFrame; }
	D3DXVECTOR3 GetReserveMove(void);

private:
	bool AddSlot(int nKey);
	bool DelSlot(int nKey);
	void EditKeyFrameInfo(D3DXVECTOR3 vPos);

	void SetTotalFrame(int nTotal)	{m_nTotalFrame = nTotal;}
	void PlayVideo(bool bRewind = false);
	FlyCameraControl::FlyCameraAnchors GetAnchors(float fProgressTime);

public :
	void SendToWidgetKeyFrameInfo();
};
*/