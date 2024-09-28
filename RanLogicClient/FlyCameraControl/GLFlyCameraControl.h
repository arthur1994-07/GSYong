
#pragma once

#include "../../RanLogic/FlyCamera/FlyCameraDefine.h"
#include "../../RanLogic/FlyCamera/GLFlyCameraKeyFrameControl.h"
#include "../Widget/GLWidgetScript.h"
#include <queue>

class GLGaeaClient;
class GLFlyCameraBaseMode;

//  [6/24/2014 gbgim];
// - 자유 시점 카메라 로직;
class GLFlyCameraControl
{
private:
	GLGaeaClient*	m_pGaeaClient;
	// FlyCamera Control활성화시, 캐릭터 기준으로 GetViewAround를 하지않고, 카메라기준으로;
	float		m_fLastMsgMoveSendT;
	
private:	// 이동, 회전, 보간값등 수치값;
	float		m_fMoveAmount;	// 힘;
	float		m_fMoveInterT;
	float		m_fMoveInterValue;
	D3DXVECTOR3 m_vMoveInterValue;

	float		m_fRotateAmount;
	float		m_fRotateInterT;
	float		m_fRotateInterValue;
	D3DXVECTOR3 m_vRotateInterValue;

	float	m_fTargetChangeInterT;

	// UI->, Keyproc->;
	std::queue<FlyCameraControl::SMsg>	m_MsgQueue;
	// Keyboard move->;
	std::list<DWORD>					m_lstReservAction;
	// Mode->, KeyFrameAnimation->;
	std::list<DWORD>					m_lstAfterAction;

	GLFlyCameraBaseMode*		m_pCameraMode;
	GLFlyCameraKeyFrameControl*	m_pKeyFrameControl;

public:
	float GetMoveVelo(void) const { return m_fMoveAmount; }
	float GetMoveInterTime(void) const { return m_fMoveInterT; }
	float GetRotateVelo(void) const { return m_fRotateAmount; }
	float GetRotateInterTime(void) const { return m_fRotateInterT; }
	float GetTargetChangeInterTime(void) const { return m_fTargetChangeInterT; }
	const D3DXVECTOR3& GetMoveInterValue(void) const { return m_vMoveInterValue; }
	const D3DXVECTOR3& GetRotateInterValue(void) const { return m_vRotateInterValue; }

	void SetMoveVelo( const float fVelo )	{ m_fMoveAmount = fVelo; }
	void SetMoveInterTime( const float fInterTime ) { m_fMoveInterT = fInterTime; }
	void SetRotateVelo( const float fVelo )	{ m_fRotateAmount = fVelo; }
	void SetRotateInterTime( const float fInterTime ) { m_fRotateInterT = fInterTime; }
	void SetTargetInterTime(const float fInterTime ) { m_fTargetChangeInterT = fInterTime; }

private:
	GLFlyCameraControl(void);
	virtual ~GLFlyCameraControl(void);
public:
	static GLFlyCameraControl& GetInstance(void);

public:
	void InitFlyCameraControl(GLGaeaClient* const pGaeaClient);
	void FrameMove(const float fTime, const float fElapsedTime);
	void BeginUpdate(const float fTime, const float fElapsedTime);
	void Update(const float fTime, const float fElapsedTime);

	void AddMessage(DWORD type, DWORD dwMsg);
	bool PopMessage(DWORD* type, DWORD* dwMsg);
	bool PopMessage(FlyCameraControl::SMsg* sMsg);
	void MsgProcedures(void);

	void AddReserveAction(DWORD type);
	void AddAfterAction(DWORD type);

public:
	bool IsPlayVideo(void);
	int GetKeyFrameSize(void) const;
	int GetAtIndex(int nAt) const;
	int GetCurrentFrame(void) const;
	
	const FlyCameraControl::FlyCameraKey& GetKeyData(int nKey) const;

private:
	void InterpolateValueOperation( IN const float fElapsedTime
		, IN const float fInterpTime
		, IN const D3DXVECTOR3& vInterpNormal
		, IN OUT D3DXVECTOR3& vInterpValue);

public :
	void SetFPS( int nFPS );
	int GetFPS();

	void SetCurrentFrame( int nKey );

	void SetTotalFrame( int nTotal );
	int GetTotalFrame();

	void AddKeyFrame( int nKey );
	void DeleteKeyFrame( int nKey );

	void Play();
	void Stop();
	void Rewind();
	void SaveKeyFrame();

	LuaTable GetKeyFramePos( int nIndex );
	LuaTable GetMoveToTypeString();
	void ReqKeyFrameInfo();
};