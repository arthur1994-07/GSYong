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
    //! DB 에서 사용할 GUID를 생성한다.
    //! DB Index 효율성을 위해서 증가하면서 고유한 값을 만들어
    //! 내는 것을 목표로 한다.
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
