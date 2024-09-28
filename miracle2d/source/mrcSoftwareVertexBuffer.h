
#ifndef _mrcSoftwareVertexBuffer_h_
#define _mrcSoftwareVertexBuffer_h_

#include "mrcConfig.h"
#include "mrcRenderer.h"


namespace mrc
{
	class SoftwareVertexBuffer 
	{
		friend class ScopeLock;
		void lock(Vertex** pVertex)  { *pVertex = &mVB[0]; }
		void unlock()  {}
	public:
		SoftwareVertexBuffer(int32 size ) { mVB.resize(size); }
		class ScopeLock
		{
		public:
			ScopeLock(SoftwareVertexBuffer& vertexBuffer,Vertex** pVertex) : mVertexBuffer(vertexBuffer) 
			{ mVertexBuffer.lock(pVertex); }
			~ScopeLock() { mVertexBuffer.unlock(); }

		private:
			SoftwareVertexBuffer& mVertexBuffer;
		};

		
		int32  size() const  { return (int32)mVB.size(); }
		int32  capacity() const  { return (int32)mVB.capacity(); }

	private:
		vector<Vertex> mVB;
	};


	class SoftwareVertexQuad 
	{
		friend class ScopeLock;
		void lock(Vertex** pVertex) { *pVertex = &mVB[0]; }
		void unlock() {}
	public:
		class ScopeLock
		{
		public:
			ScopeLock(SoftwareVertexQuad& vertexBuffer,Vertex** pVertex) : mVertexBuffer(vertexBuffer) 
			{ mVertexBuffer.lock(pVertex); }
			~ScopeLock() { mVertexBuffer.unlock(); }

		private:
			SoftwareVertexQuad& mVertexBuffer;
		};

		int32  size() const { return 4; }
		int32  capacity() const { return 4; }
		
		const Vertex* getRawBuffer() const { return &mVB[0]; }
	private:
		Vertex mVB[4];
	};

	
	
	
}


#endif