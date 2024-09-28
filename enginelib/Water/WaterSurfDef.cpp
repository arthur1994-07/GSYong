#include "pch.h"

#include "../../SigmaCore/File/SerialFile.h"

#include "WaterSurfDef.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


void WATERSURF_PROPERTY::Save( sc::SerialFile &SFile )
{
	SFile << m_bPlaneInverse;			// 
	SFile << m_fPlaneWidth;				//	
	SFile << m_fPlaneHeight;			//	
	SFile << m_fSurfVelocityINIT;		// 초기 물살 이동 스피드
	SFile << m_fEdit_SurfAcceleration;	// [Edit] m_fSurfVelocity 줄어드는 수치
	SFile << m_fSurfSleepTimeINIT;		// 물살 잠복 시간
	SFile << m_fAlphaAddition;			// 알파 추가 스피드
	SFile << m_fAlphaSubtractionTime;	// 알파 삭제 시간
	SFile << m_fAlphaSubtraction;		// 알파 삭제 스피드
	SFile << m_fSideLineInterval;		// 파도의 옆라인이 나오는 시간간격
}

void WATERSURF_PROPERTY::Load( sc::SerialFile &SFile )
{
	SFile >> m_bPlaneInverse;			// 
	SFile >> m_fPlaneWidth;				//	
	SFile >> m_fPlaneHeight;			//	
	SFile >> m_fSurfVelocityINIT;		// 초기 물살 이동 스피드
	SFile >> m_fEdit_SurfAcceleration;	// [Edit] m_fSurfVelocity 줄어드는 수치
	SFile >> m_fSurfSleepTimeINIT;		// 물살 잠복 시간
	SFile >> m_fAlphaAddition;			// 알파 추가 스피드
	SFile >> m_fAlphaSubtractionTime;	// 알파 삭제 시간
	SFile >> m_fAlphaSubtraction;		// 알파 삭제 스피드
	SFile >> m_fSideLineInterval;		// 파도의 옆라인이 나오는 시간간격
}