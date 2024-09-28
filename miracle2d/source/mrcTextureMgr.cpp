
#include "mrcPrecompiled.h"
#include "mrcTextureMgr.h"
#include "mrcTextureData.h"
#include "mrcTiXml.h"
#include "mrcUtil/mrcSingleton.h"
#include "mrcRenderer.h"


namespace mrc
{	
	static const int32 VERSION = 1;


	TextureDataMgr& TextureDataMgr::getInstance()
	{
		typedef Loki::SingletonHolder<TextureDataMgr,Loki::CreateUsingNew,
			Loki::LongevityLifetime::DieIndependencyMgr > TextureMgrS;
		
		return TextureMgrS::Instance();
	}
	
	void TextureDataMgr::saveXml( const String& fullPath ) const
	{
		tixml::SP_TiXmlDocument spDoc;
		TiXmlElement* pTextureMgrEle = tixml::createDocument(_mrcT("TextureMgr"),VERSION,spDoc);
		
		tixml::insertElement(pTextureMgrEle,_mrcT("TextureDataMap"),mTextureDataMap);
		
		tixml::saveDocument(spDoc,fullPath);		
	}

	void TextureDataMgr::loadXml( const String& path)
	{
		clear();

		tixml::SP_TiXmlDocument spDoc;
		int32 version;
		TiXmlElement* pTextureMgrEle = tixml::loadDocument(path,_mrcT("TextureMgr"),version,spDoc);
		mrcASSERT( version == VERSION );

		tixml::getElement(pTextureMgrEle,_mrcT("TextureDataMap"), mTextureDataMap );

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		TextureFileMap   noExistMap;
		TextureFileMap   checkModifyMap;

		// ���� �ε�� �����Ϳ��� , �̹� �ε�� ���� �ؽ��� 
		for (TextureDataMap::iterator it = mTextureDataMap.begin(); 
			it != mTextureDataMap.end(); ++it)
		{
			const TextureData& data = refPtr(it->second);
			for (int16 i=0; i < data.getFileCount(); ++i)
			{
				const filesystem::Path&	path = data.getFilePath(i);
				TextureFileMap::iterator fileIt = mTextureFileMap.find( path );
				if( fileIt != mTextureFileMap.end() )
				{
					checkModifyMap.insert( *fileIt );
				}
				else
				{
					noExistMap.insert( std::make_pair(path,TextureFile()) );
				}
			}
		}

		// �Ⱦ��°�  ��ε�
		for (TextureFileMap::iterator it = mTextureFileMap.begin(); it != mTextureFileMap.end(); ++it)
		{
			const filesystem::Path&    path =  it->first;
			const TextureFile& file =  it->second;
			if( checkModifyMap.find(path) == checkModifyMap.end() )
				IRenderer::getInstance().releaseTexture( file.getHandle() );
		}

		// ���� ������ ���� �ٽ� ä����
		mTextureFileMap.clear();

		// �����ϴ� �ؽ��� ����
		for (TextureFileMap::iterator it = checkModifyMap.begin(); it != checkModifyMap.end(); ++it)
		{
			const filesystem::Path& path    =  it->first;
			TextureFile& file       =  it->second;
 			const filesystem::Path fullPath =  getRootPath() / path;
			time_t modifiedTime     =  filesystem::last_write_time(fullPath);
			time_t originFileTime   =  file.getTime();

			// ���ٸ� �н�
			if( modifiedTime == originFileTime )
				continue;
		
			IRenderer::getInstance().releaseTexture( file.getHandle() );

			HTexture hTexture = NULL;
			LoadedTextureInfo info;
			bool ret = IRenderer::getInstance().loadTexture(fullPath,&hTexture,&info);
			mrcASSERT_MSG(ret, "texture load failed : " + toNString(fullPath));
			file.setHandle(hTexture);
			file.setTime(modifiedTime);
			file.setInfo(info);
		}


		// �������� �ε� 
		for ( TextureFileMap::iterator it = noExistMap.begin(); it != noExistMap.end(); ++it )
		{
			const filesystem::Path& path    =  it->first;
			TextureFile& file       =  it->second;
			const filesystem::Path fullPath =  getRootPath() / path;
			HTexture hTexture = NULL;
			LoadedTextureInfo info;
			bool ret = IRenderer::getInstance().loadTexture(fullPath,&hTexture,&info);
			mrcASSERT_MSG(ret, "texture load failed : " + toNString(fullPath));
			time_t modifiedTime     =  filesystem::last_write_time(fullPath);

			file.setHandle(hTexture);	
			file.setTime(modifiedTime);
			file.setInfo(info);
		}


		// ����
		mTextureFileMap.insert( checkModifyMap.begin(),checkModifyMap.end() );
		mTextureFileMap.insert( noExistMap.begin(),noExistMap.end() );

		//////////////////////////////////////////////////////////////////////////
		// ������  
		// �ؽ��İ� ������ 2�� n�¹�� �þ���� üũ�ϰ� �ٽ� ���ٲ�
		for( TextureDataMap::iterator it = mTextureDataMap.begin(); it != mTextureDataMap.end(); ++it)
		{
			TextureData& data = refPtr(it->second);
			const filesystem::Path&	path = data.getFilePath(0);

			TextureFileMap::iterator fileIt = mTextureFileMap.find(path);
			mrcASSERT(fileIt != mTextureFileMap.end());

			const LoadedTextureInfo& info = fileIt->second.getInfo();
			data.setSurfaceRect(info.surfaceWidth,info.surfaceHeight);
			if( !data.isSettingRegionRect() )
				data.setRegionRect(info.fileWidth,info.fileHeight);
			
			data.setVertexForTextureUV(
				(info.fileWidth != info.surfaceWidth && info.fileHeight != info.surfaceHeight) ||
				data.isSettingRegionRect() );
		}

		//////////////////////////////////////////////////////////////////////////
		// �������� ĳ�� �۾�
		for (TextureDataMap::iterator it = mTextureDataMap.begin(); it != mTextureDataMap.end(); ++it)
		{
			TextureData& data = refPtr( it->second );
			data.makeCacheTexture(mTextureFileMap);
		}

		mLoadedPath = filesystem::Path(path.c_str());
	}

	void TextureDataMgr::clear()
	{		
		mTextureDataMap.clear();

		for ( TextureFileMap::iterator it = mTextureFileMap.begin(); it != mTextureFileMap.end(); ++it )
		{
			TextureFile& file = it->second;
			IRenderer::getInstance().releaseTexture( file.getHandle() );
		}

		mTextureFileMap.clear();
	}

	TextureDataMgr::~TextureDataMgr()
	{
		clear();
	}

	WP_TextureData TextureDataMgr::getTextureData( const TextureDataID& id )
	{
		TextureDataMap::iterator it = mTextureDataMap.find(id);
		if( it == mTextureDataMap.end() )
			return WP_TextureData();

		return it->second;
	}
}