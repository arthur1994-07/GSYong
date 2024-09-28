#pragma once

#include "./GfxScene.h"

class GfxLoadingScene : public GfxScene
{
public :
	GfxLoadingScene();
	virtual ~GfxLoadingScene();

private :
	enum ELoadingTextID
	{
		MapName = 0,
		Tip,
	};

private :
	// �� ID
	SNATIVEID m_sMapID;

	// �ε��� �� �̹���
	std::string m_strLoadingImgFile;

	// ��� ǥ��
	bool m_bShowGrade;

public :
	virtual HRESULT OnInit( LPDIRECT3DDEVICE9 pDevice );
	
public :
	void SetData( const SNATIVEID& sMapID, const std::string& strLoadingImgFile, bool bShowGrade );

	void OnBegin( ELoadingSceneType eType );
	void OnEnd();

private :
	void SetDisplay( ELoadingSceneType eType );
};