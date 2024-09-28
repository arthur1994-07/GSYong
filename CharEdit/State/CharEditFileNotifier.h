#ifndef CharEditFileNotifier_H
#define CharEditFileNotifier_H

#include "FileNotifier.h"

struct  DxSkinPiece;
struct  SMATERIAL_PIECE;
struct  DxSkinAnimation;
struct  DXMATERIALEX;
class   DxSkinAniControl;

/**
	����, ĳ���� ���� ���� ���Ǿ� ���� �ؽ�ó ����(dds)��
	������ ó�� �Ѵ�.

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
	����(piece)�� ���Ǿ� ���� �� ����(.x)��
	������ ó�� �Ѵ�.

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
	����(piece)�� ���Ǿ� ���� �޽� ����(.x)��
	������ ó�� �Ѵ�.

 */
class CMeshFileNotifier
	: public CSkeletonFileNotifier
{
public:
		bool			Init();

static  TSTRING			GetFile(DxSkinPiece* pSkinPiece, bool bPath = true);
};

/**
	ĳ������ ������ ���Ǿ� ���� �� ����(.x)��
	������ ó�� �Ѵ�.

 */
class CPartSkeletonFileNotifier
	: public CFileNotifier
{
public:
virtual	bool			Init();
		int				OnNotify();
};

/**
	ĳ������ ������ ���Ǿ� ���� �޽� ����(.x)��
	������ ó�� �Ѵ�.

 */
class CPartMeshFileNotifier
	: public CPartSkeletonFileNotifier
{
public:
		bool			Init();
};

/**
	ĳ����, ���� ���� ���� �ִ� ����(.x)��
	������ ó�� �Ѵ�.

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