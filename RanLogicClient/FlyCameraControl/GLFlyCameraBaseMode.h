
#pragma once

class GLGaeaClient;
class GLFlyCameraControl;

class GLFlyCameraBaseMode
{
protected:
	GLGaeaClient* m_pGaeaClient;
	GLFlyCameraControl* m_pParentCtrl;
	DWORD m_dwModeType;

	//  [10/13/2014 gbgim];
	// Mode에서 갱신된 아래의 두 정보는 꼭 Control에서 사용해야한다; 
	// 현재로서는 동기화가 안맞을 순 없지만, 후에 안맞게 되면 안된다;
	// 후에 만약 안맞게되면, 이것도 메세지방식으로 해야하나..;
	D3DXVECTOR3 m_vReservMove;
	D3DXVECTOR3 m_vReservRotate;

public:
	GLFlyCameraBaseMode( GLGaeaClient* pGaeaClient, GLFlyCameraControl* pParentCtrl, DWORD dwModeType );
	virtual ~GLFlyCameraBaseMode(void);

public:
	virtual void Init(void) {}
	virtual void MsgProcedures ( DWORD dwType, DWORD dwMsg ) {}
	virtual void Update( float fTime, float fElapsedTime ) {}
	virtual DWORD GetModeType(void) { return m_dwModeType; }

	virtual D3DXVECTOR3 GetReserveMove(void);
	virtual D3DXVECTOR3 GetReserveRotation(void);
};