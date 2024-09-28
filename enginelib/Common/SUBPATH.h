#pragma once

#include <string>
#include "../ServiceProviderDefine.h"

namespace SubPath
{
	const static std::string CACHE						= _T("\\cache\\");

	const static std::string OBJ_FILE_ROOT				= _T("\\Data\\");
	const static std::string OBJ_FILE_EDIT				= _T("\\Data\\Editor\\");
	const static std::string OBJ_FILE_PIECE				= _T("\\Data\\Piece\\");
	const static std::string OBJ_FILE_PIECEEDIT			= _T("\\Data\\Piece\\Edit\\");
	const static std::string OBJ_FILE_SKIN				= _T("\\Data\\Skin\\");
	const static std::string OBJ_FILE_SKELETON			= _T("\\Data\\Skeleton\\");
	const static std::string OBJ_FILE_ANIMATION			= _T("\\Data\\Animation\\");
	const static std::string OBJ_FILE_SKINOBJECT		= _T("\\Data\\SkinObject\\");
	const static std::string OBJ_FILE_OBJECT			= _T("\\Data\\Object\\");
	const static std::string OBJ_FILE_PREFAB			= _T("\\data\\prefab\\");
	const static std::string OBJ_FILE_STATICMESH		= _T("\\Data\\StaticMesh\\");
	const static std::string OBJ_FILE_VISUAL_MATERIAL	= _T("\\Data\\VisualMaterial\\");
	const static std::string OBJ_FILE_BILLBOARD			= _T("\\Data\\billboard\\");

	const static std::string MAP_FILE					= _T("\\Data\\Map\\");
	const static std::string GLMAP_FILE					= _T("\\Data\\GLMap\\");
	const static std::string GLOGIC_FILE				= _T("\\Data\\GLogic\\");
	const static std::string GLOGIC_LEVEL_FILE			= _T("\\Data\\GLogic\\Level\\");
	const static std::string GLOGIC_SCRIPTS_FILE		= _T("\\Data\\GLogic\\Scripts\\");
	const static std::string GLOGIC_SERVER_FILE			= _T("\\Data\\GLogicServer\\");
	const static std::string GLOGIC_SERVER_SCRIPTS_FILE	= _T("\\Data\\GLogicServer\\Scripts\\");

	const static std::string TILE_FILE_DATA				= _T("\\Data\\Tiles\\");	
	const static std::string TILE_FILE_TEXTURE			= _T("\\Textures\\Tiles\\");

	const static std::string TEXTURE_FILE_ROOT			= _T("\\Textures");
	const static std::string TEXTURE_FILE_SHADOW		= _T("\\Textures\\Shadow\\");
	const static std::string TEXTURE_FILE_CLUB			= _T("\\Textures\\Club\\");
	const static std::string TEXTURE_FILE_EFF			= _T("\\Textures\\Effect\\");
	const static std::string TEXTURE_FILE_BILLBOARD		= _T("\\Textures\\billboard\\");
	const static std::string MATERIALS_FILE_ROOT		= _T("\\Data\\Materials\\");
	const static std::string SHADER_FILE_ROOT			= _T("\\Data\\Shaders\\");

	const static std::string EFF_FILE_SINGLE			= _T("\\Data\\Effect\\");
	const static std::string EFF_FILE_CHAR				= _T("\\Data\\Effect\\Char\\");

	const static std::string SOUND_FILE_ROOT			= _T("\\Sounds\\Sfx\\");			
	const static std::string BGM_FILE_ROOT				= _T("\\Sounds\\BGM\\");

	const static std::string GUI_FILE_ROOT				= _T("\\Data\\GUI\\");
	const static std::string NTK_FILE_ROOT				= _T("\\Data\\GLogic\\NPCTALK\\");
	const static std::string QST_FILE_ROOT				= _T("\\Data\\GLogic\\Quest\\");
	const static std::string ACTV_FILE_ROOT				= _T("\\Data\\GLogic\\Activity\\");
	const static std::string GFX_FILE_ROOT				= _T( "\\data\\gfx\\" );
	const static std::string WIDGET_SCRIPT_ROOT			= _T( "\\data\\GLogic\\WidgetScript\\" );
}



class SUBPATH
{
public:
	SUBPATH(EMSERVICE_PROVIDER ServiceProvider);
	~SUBPATH();

protected:
	EMSERVICE_PROVIDER m_ServiceProvider;

	std::string APP_ROOT;

	std::string SAVE_ROOT;

	std::string PLAYINFO_ROOT;
	std::string UIINFO_ROOT;
	std::string DEBUGINFO_ROOT;
	std::string CAPTURE;
	std::string CHATLOG;

	std::string FULL_PATH_VISUAL_MATERIAL;

public:
	const std::string& AppRoot() const { return APP_ROOT; }
	void        AppRootSet(const std::string& Path) { APP_ROOT=Path; }

	const std::string& SaveRoot() const { return SAVE_ROOT; }

	const std::string& PlayInfoRoot() const { return PLAYINFO_ROOT; }
	const std::string& UiInfoRoot() const { return UIINFO_ROOT; }
	const std::string& DebugInfoRoot() const { return DEBUGINFO_ROOT; }
	const std::string& Capture() const { return CAPTURE; }
	const std::string& ChatLog() const { return CHATLOG; }

	const std::string& FullPathVisualMaterial() const { return FULL_PATH_VISUAL_MATERIAL; }
	void        FullPathVisualMaterialSet(const std::string& Path) { FULL_PATH_VISUAL_MATERIAL=Path; }

public:
	const std::string& Cache() const					{ return SubPath::CACHE; }

	const std::string& ObjFileRoot() const				{ return SubPath::OBJ_FILE_ROOT; }    
	const std::string& ObjFileEdit() const				{ return SubPath::OBJ_FILE_EDIT; }
	const std::string& ObjFilePiece() const				{ return SubPath::OBJ_FILE_PIECE; }
	const std::string& ObjFilePieceEdit() const			{ return SubPath::OBJ_FILE_PIECEEDIT; }
	const std::string& ObjFileSkin() const				{ return SubPath::OBJ_FILE_SKIN; }
	const std::string& ObjFileSkeleton() const			{ return SubPath::OBJ_FILE_SKELETON; }
	const std::string& ObjFileAnimation() const			{ return SubPath::OBJ_FILE_ANIMATION; }
	const std::string& ObjFileSkinObject() const		{ return SubPath::OBJ_FILE_SKINOBJECT; }
	const std::string& ObjFileObject() const			{ return SubPath::OBJ_FILE_OBJECT; }
	const std::string& ObjFilePrefab() const			{ return SubPath::OBJ_FILE_PREFAB; }
	const std::string& ObjFileStaticMesh() const		{ return SubPath::OBJ_FILE_STATICMESH; }
	const std::string& ObjFileVisualMaterial() const	{ return SubPath::OBJ_FILE_VISUAL_MATERIAL; }
	const std::string& ObjFileBillboard() const			{ return SubPath::OBJ_FILE_BILLBOARD; }

	const std::string& EffFileSingle() const			{ return SubPath::EFF_FILE_SINGLE; }
	const std::string& EffFileChar() const				{ return SubPath::EFF_FILE_CHAR; }

	const std::string& TextureFileRoot() const			{ return SubPath::TEXTURE_FILE_ROOT; }
	const std::string& TextureFileShadow() const		{ return SubPath::TEXTURE_FILE_SHADOW; }
	const std::string& TextureFileClub() const			{ return SubPath::TEXTURE_FILE_CLUB; }        
	const std::string& TextureFileEff() const			{ return SubPath::TEXTURE_FILE_EFF; }
	const std::string& TextureFileBillboard() const		{ return SubPath::TEXTURE_FILE_BILLBOARD; }
	const std::string& MaterialsFileRoot() const		{ return SubPath::MATERIALS_FILE_ROOT; }

	const std::string& ShaderFileRoot() const			{ return SubPath::SHADER_FILE_ROOT; }

	const std::string& MapFile() const					{ return SubPath::MAP_FILE; }
	const std::string& GLMapFile() const				{ return SubPath::GLMAP_FILE; }

	const std::string& GLogicPath() const				{ return SubPath::GLOGIC_FILE; }
	const std::string& GLogicLevel() const				{ return SubPath::GLOGIC_LEVEL_FILE; }
	const std::string& GLogicScriptsPath() const		{ return SubPath::GLOGIC_SCRIPTS_FILE; }
	const std::string& GLogicServerPath() const			{ return SubPath::GLOGIC_SERVER_FILE; }
	const std::string& GLogicServerScriptsPath() const	{ return SubPath::GLOGIC_SERVER_SCRIPTS_FILE; }

	const std::string& GuiFileRoot() const				{ return SubPath::GUI_FILE_ROOT; }
	const std::string& NtkFileRoot() const				{ return SubPath::NTK_FILE_ROOT; }
	const std::string& QuestFileRoot() const			{ return SubPath::QST_FILE_ROOT; }

	const std::string& ActivityFileRoot() const			{ return SubPath::ACTV_FILE_ROOT; }

	const std::string& SoundFileRoot() const			{ return SubPath::SOUND_FILE_ROOT; }
	const std::string& BgmFileRoot() const				{ return SubPath::BGM_FILE_ROOT; }

	const std::string& TileFileData() const				{ return SubPath::TILE_FILE_DATA; }
	const std::string& TileFileTexture() const			{ return SubPath::TILE_FILE_TEXTURE; }

	const std::string& GfxPath() const					{ return SubPath::GFX_FILE_ROOT; }
	const std::string& WidgetScriptPath() const			{ return SubPath::WIDGET_SCRIPT_ROOT; }
};
