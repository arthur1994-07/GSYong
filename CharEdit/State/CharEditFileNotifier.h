#ifndef CharEditFileNotifier_H
#define CharEditFileNotifier_H

#include "FileNotifier.h"

struct  DxSkinPiece;
struct  SMATERIAL_PIECE;
struct  DxSkinAnimation;
struct  DXMATERIALEX;
class   DxSkinAniControl;

/**
	조각, 캐릭터 구분 없이 사용되어 지는 텍스처 파일(dds)의
	변경을 처리 한다.

 */
class CTextureFileNotifier
	: public CFileNotifier
{
public:
		bool			Init();
		int				OnNotify();
		void			OnChangeFile(CParam& kParam);

static  TSTRING			GetFile(SMATERIAL_PIECE* pMtlPiece, DXMATERIALEX* pDxMtl, bool bPath = true);
};

/**
	조각(piece)에 사용되어 지는 본 파일(.x)의
	변경을 처리 한다.

 */
class CSkeletonFileNotifier
	: public CFileNotifier
{
public:
virtual	bool			Init();
		int				OnNotify();

static  TSTRING			GetFile(DxSkinPiece* pSkinPiece, bool bPath = true);
static  TSTRING			GetFile(DxSkinAniControl* pSkinAni, bool bPath = true);
};

/**
	조각(piece)에 사용되어 지는 메쉬 파일(.x)의
	변경을 처리 한다.

 */
class CMeshFileNotifier
	: public CSkeletonFileNotifier
{
public:
		bool			Init();

static  TSTRING			GetFile(DxSkinPiece* pSkinPiece, bool bPath = true);
};

/**
	캐릭터의 파츠에 사용되어 지는 본 파일(.x)의
	변경을 처리 한다.

 */
class CPartSkeletonFileNotifier
	: public CFileNotifier
{
public:
virtual	bool			Init();
		int				OnNotify();
};

/**
	캐릭터의 파츠에 사용되어 지는 메쉬 파일(.x)의
	변경을 처리 한다.

 */
class CPartMeshFileNotifier
	: public CPartSkeletonFileNotifier
{
public:
		bool			Init();
};

/**
	캐릭터, 조각 구분 없이 애니 파일(.x)의
	변경을 처리 한다.

 */
class CAnimationFileNotifier
	: public CFileNotifier
{
public:
		bool			Init();
		int				OnNotify();

static  TSTRING			GetFile(DxSkinAnimation* pSkinAnimation, bool bPath = true);
static  TSTRING			GetCFGFile(DxSkinAnimation* pSkinAnimation, bool bPath = true);
};

#endif