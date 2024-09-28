


#ifndef _mrcTextureMgr_h_
#define _mrcTextureMgr_h_


#include "mrcConfig.h"
#include "mrcStdContainer.h"
#include "mrcAllocatedObject.h"
#include "mrcUtil/mrcFileSystem.h"
#include "mrcRenderer.h"

namespace mrc
{

	class TextureData;

	typedef mrcSharedPtr< TextureData > SP_TextureData;
	typedef mrcWeakenPtr< const TextureData > WP_TextureData;

	class TextureFile 
	{
	public:
		TextureFile() { mModifiedTime = 0;  makeInvalidHandle(mTextureHandle); }
		TextureFile(const HTexture& hTexture,const time_t& modifiedTime) 
		{
			mModifiedTime = modifiedTime;
			mTextureHandle = hTexture;
		}

		void setHandle(const HTexture& hTexture) { mTextureHandle = hTexture; }
		void setTime(const time_t& time) { mModifiedTime = time; }
		void setInfo(const LoadedTextureInfo& info) { mInfo = info; }

		const HTexture& getHandle() const { return mTextureHandle; }
		const time_t&   getTime() const  { return mModifiedTime; }
		const LoadedTextureInfo& getInfo() { return mInfo; }
	private:
		HTexture mTextureHandle;
		time_t   mModifiedTime;
		LoadedTextureInfo mInfo;
	};

	class TextureDataMgr 
	{
		MIRACLE_FRIEND_CLASS_SINGLETON(TextureDataMgr);
		TextureDataMgr(){}

	public:
		~TextureDataMgr();

		static TextureDataMgr& getInstance();

		void setRootPath(const filesystem::Path& path) { mPath = path; }
		const filesystem::Path getRootPath() const { return filesystem::initial_path() / mPath; }

		WP_TextureData getTextureData(const TextureDataID& id);

		void saveXml(const String& path) const;
		void loadXml(const String& path);

		void clear();

		void reload() { loadXml( toString(mLoadedPath) ); }

	private:
		filesystem::Path mPath;
		filesystem::Path mLoadedPath;


		typedef hash_map<TextureDataID,SP_TextureData> TextureDataMap;
		TextureDataMap mTextureDataMap;

		typedef map<filesystem::Path,TextureFile> TextureFileMap;
		TextureFileMap mTextureFileMap;
	};


}


#endif

