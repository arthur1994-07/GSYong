
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
	// Mode���� ���ŵ� �Ʒ��� �� ������ �� Control���� ����ؾ��Ѵ�; 
	// ����μ��� ����ȭ�� �ȸ��� �� ������, �Ŀ� �ȸ°� �Ǹ� �ȵȴ�;
	// �Ŀ� ���� �ȸ°ԵǸ�, �̰͵� �޼���������� �ؾ��ϳ�..;
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