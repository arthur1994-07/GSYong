#pragma once

#include "./FlyCameraDefine.h"

class GLFlyCameraKeyFrameControl
{
public:
	// 확실히 랜덤 액세스가 필요할때가 많다, 그렇지만 리스트를 사용하는데 있어서 이점도 생각해보면..;
	typedef std::vector<FlyCameraControl::FlyCameraKey>		KEYFRAME_VEC;
	typedef KEYFRAME_VEC::iterator			KEYFRAME_VEC_ITR;
	typedef KEYFRAME_VEC::const_iterator	KEYFRAME_VEC_CITR;
	typedef KEYFRAME_VEC::value_type		KEYFRAME_VEC_VALUE_T;

private:
	// UI->, Keyproc->;
	std::queue<FlyCameraControl::SMsg>	m_MsgQueue;

	KEYFRAME_VEC		m_vecKeyFrame;

	int m_nFPS;
	int m_nTotalFrame;
	float m_fProgressTime;
	int m_nCurrentKey;
	int m_nCurrentIndex;

	bool m_bPlaying;
	bool m_bRewind;
	D3DXVECTOR3 m_vReservMove;
	D3DXVECTOR3 m_vReservLook;

public:
	GLFlyCameraKeyFrameControl(void);
	virtual ~GLFlyCameraKeyFrameControl(void);

public:
	virtual void Init(void);
	virtual void Update(float fTime, float fElapsedTime);
	virtual void Render(float fTime, float fElapsedTime);

	void AddMessage(FlyCameraControl::SMsg* sMsg);
	bool PopMessage(FlyCameraControl::SMsg* sMsg);
	virtual void MsgProcedures (void);

private:
	D3DXVECTOR3 GetCatmullromSplineCalc(float fProgressTime, bool bMove);

public:
	bool isPlayVideo(void);
	int GetFPS() { return m_nFPS; }
	int GetCurrentFrame(void) { return m_nCurrentKey; }
	int GetTotalFrame() { return m_nTotalFrame; }
	D3DXVECTOR3 GetReserveMove(void);
	D3DXVECTOR3 GetReserveLook(void);

	int GetKeyFrameSize(void) const;

	// GetKeyIndex, GetKeyData 헷갈리지말자;
	// GetKeyIndex는 인덱스를 인자로 받는다;
	// GetKeyDatas는 프레임 위치를 인자로 받는다;
	int GetKeyFrame(int nAt) const;
	const KEYFRAME_VEC_VALUE_T& GetKeyData(int nKey);
	const KEYFRAME_VEC& GetKeyFrameList(void) { return m_vecKeyFrame; }

	void ResetAll(void);

	HRESULT Load(CString strPath);
	HRESULT Save(CString strPath);

private:
	bool AddSlot(int nKey);
	bool DelSlot(int nKey);
	void EditKeyFrameInfo(D3DXVECTOR3 vec, bool bPos);

	void SetTotalFrame(int nTotal)	{m_nTotalFrame = nTotal;}
	void PlayVideo(bool bRewind = false);
	FlyCameraControl::FlyCameraAnchors GetAnchors(float fProgressTime);
	void SortKeyframelist(void);

public :
	// 클라 재정의 필요함수;
	void SendToWidgetKeyFrameInfo();
};