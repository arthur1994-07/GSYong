#pragma once

/** 
 * ���̺��� X,Y ������� ���� ��(ž��);
*/

class SDICE;
class DiceLogic{
public:
	void Grab(const float _fPositionX, const float _fPositionY, const float _fPositionZ = -5.0f); // ���;
	void HoldOut(const unsigned int* const _iDetNum); // ����, ���ڴ� ������ �ֻ��� ��ȣ �迭;
	const bool Update(void);

	void Grab(const int _iPositionX, const int _iPositionY, const int _iPositionZ = -5);	

	const unsigned int GetNDice(void);
	const D3DXMATRIXA16& GetMatrix(const unsigned int _iIndex);

private:
	void CheckWall(void);
	void CheckCrash(void);	

public:
	DiceLogic(const unsigned int _nDice, const unsigned int _iStageSizeX, const unsigned int _iStageSizeY);
	~DiceLogic(void);

private:
	SDICE* pDice;		
	const unsigned int nDice;
	const unsigned int iWidth, iHeight;
	bool bMoving, bGrab;	
};

