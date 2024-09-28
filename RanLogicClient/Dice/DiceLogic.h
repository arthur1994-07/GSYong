#pragma once

/** 
 * 테이블은 X,Y 평면으로 생성 됨(탑뷰);
*/

class SDICE;
class DiceLogic{
public:
	void Grab(const float _fPositionX, const float _fPositionY, const float _fPositionZ = -5.0f); // 잡고;
	void HoldOut(const unsigned int* const _iDetNum); // 놓고, 인자는 결정될 주사위 번호 배열;
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

