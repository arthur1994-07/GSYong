#pragma	once

// UI Modal 효과를 위해 존재 ( 모달 )
namespace NSGUI
{
	// 마우스 만을 막음 ( 약식 모달 )
    BOOL IsCharMoveBlock();
	void SetCharMoveBlock();
	void ResetCharMoveBlock();

	// 키보드, 마우스 모두 막음 ( 모달 )
	BOOL IsCharMoveAllBlock();
	void SetCharMoveAllBlock();
	void ResetCharMoveAllBlock();
}