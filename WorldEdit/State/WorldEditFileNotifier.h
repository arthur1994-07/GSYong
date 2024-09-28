#ifndef CWorldEditFileNotifier_H
#define CWorldEditFileNotifier_H

#include "FileNotifier.h"

class   DxSetLandMan;
struct  SMATERIAL_PIECE;
struct  DXMATERIALEX;

/**
	프레임 메쉬 X 파일
	월드 툴의 프레임 에디트 탭 페이지에서
	사용되는 메인 x 파일을 의미 한다.

 */
class CFrameMeshXFileNotifier
	: public CFileNotifier
{
public:
		bool				Init();
		int					OnNotify();

static  TSTRING             GetFile(DxSetLandMan* pSetLandMan);
};

/**
	월드 에디트 툴에서 사용되는 모든
	텍스처의 갱신을 처리 한다.
	
 */
class CTextureFileNotifier
	: public CFileNotifier
{
public:
		bool				Init();
		int					OnNotify();

static  bool                GetFile(TSTRING& strFile);
static  TSTRING			    GetFile(SMATERIAL_PIECE* pMtlPiece, DXMATERIALEX* pDxMtl);
};

/**
	월드 조각(pie) 파일
	월드 에디트 툴에서는 pef 와 pie 라는 2가지 조각 파일을 사용한다.
	원래 pis 라는 것도 사용했지만 현재는 쓰지 않는다.

	pef 는 그래픽팀에서 관리하는 편집용 확장자 이며
	pie 는 이 편집용 확장자를 최종 클라이언트에서 사용하기 위한 변환 파일이다.

 */
class CPieceFileNotifier
	: public CFileNotifier
{
public:
virtual	bool				Init();
virtual	int					OnNotify();
		LPDIRECT3DDEVICEQ	GetDevice();

static  TSTRING             GetFile(const TCHAR* pPisName);
};

/**
	월드 조각(pie) 파일 - wld 에서 사용되는 pie 파일.
	pie 는 이 편집용 확장자를 최종 클라이언트에서 사용하기 위한 변환 파일이다.
 */
class CPieceWldFileNotifier
	: public CFileNotifier
{
public:
virtual	bool				Init();
virtual	int					OnNotify();
		LPDIRECT3DDEVICEQ	GetDevice();

static  TSTRING             GetFile(const TCHAR* pPisName);
};

#endif