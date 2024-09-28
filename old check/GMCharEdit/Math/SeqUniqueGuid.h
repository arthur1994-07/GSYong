#pragma once

#include "../Util/GLSingletone.h"

namespace sc
{
    //! class SeqUniqueGUID
    //!
    //! \History
    //! 2007.05.28 Jgkim Create
    //!
    //! Copyright (c) Min Communications. All rights reserved.
    //! \Note
    //! DB ���� ����� GUID�� �����Ѵ�.
    //! DB Index ȿ������ ���ؼ� �����ϸ鼭 ������ ���� �����
    //! ���� ���� ��ǥ�� �Ѵ�.
    class SeqUniqueGUID  : public CSingletone< SeqUniqueGUID >
    {
	protected:
		friend CSingletone;

    private:
        SeqUniqueGUID();
        virtual ~SeqUniqueGUID();

    protected:
        LONG m_Data3;	
        unsigned char m_Data4[6];
        unsigned char m_ucServerGroup;
        
    public:
        void setServerGroup( unsigned char ServerGroup ) { m_ucServerGroup = ( unsigned char ) ServerGroup; }
        DWORD getRandomDword();
        void reset();
        void getGUID( GUID& guid );
    };

}; // namespace CORE_COMMON
