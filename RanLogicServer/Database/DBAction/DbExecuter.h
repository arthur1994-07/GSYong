#ifndef _DB_EXECUTER_H_
#define _DB_EXECUTER_H_

#pragma once

#include <queue>
#include <boost/noncopyable.hpp>
#include "../../../SigmaCore/Util/Lock.h"
#include "../../../SigmaCore/Thread/EventThread.h"
#include "../DbDefine.h"
#include "./DbAction.h"

#ifdef _USE_TBB
#include "../../../=TBB/include/tbb/concurrent_queue.h"
#endif

class NetServer;

namespace db
{

//! 2011-03-21 Jgkim EventThread 를 이용하도록 수정
class DbExecuter : public sc::EventThread, private boost::noncopyable
{
public:
	DbExecuter(NetServer* pServer);
	virtual ~DbExecuter();

protected:

#ifdef _USE_TBB
	tbb::concurrent_bounded_queue<DbActionPtr> m_cqJob;
#else
	std::queue<DbActionPtr> m_vJob;
#endif

	NetServer* m_pServer;

	int m_nError;

public:	
    //! -----------------------------------------------------------------------
    //! Virtual Function	
    virtual void process()	override;
	virtual void start()	override;
	virtual void end()		override;
	virtual void endHook()	override;
    
	void AddJob(DbActionPtr spJob);	
	void ExecuteJobLast();
    bool IsJobEmpty() const
	{
#ifdef _USE_TBB
		return m_cqJob.empty();
#else
		return m_vJob.empty(); 
#endif
	}
};

//! 2011-03-21 Jgkim
class DbExecuterMan sealed : private boost::noncopyable
{
public:
    DbExecuterMan(NetServer* pServer, SERVER_TYPE Type);
    ~DbExecuterMan();    

private:
    NetServer* m_pServer;

    DbExecuter* m_pGame;
    DbExecuter* m_pLog;
    DbExecuter* m_pUser;
    DbExecuter* m_pWeb;
    DbExecuter* m_pShop;
    DbExecuter* m_pClubLog;
	DbExecuter* m_pAuth;

public:
    void AddJob(DbActionPtr spJob, DB_TYPE Type);
    void AddGameDbJob(DbActionPtr spJob);
    void AddUserDbJob(DbActionPtr spJob);
    void AddLogDbJob(DbActionPtr spJob);
    void AddWebDbJob(DbActionPtr spJob);
    void AddShopDbJob(DbActionPtr spJob);
    void AddClubLogDbJob(DbActionPtr spJob);
	void AddAuthDbJob(DbActionPtr spJob);
    void ExecuteLastJob();
    bool IsAllDbJobCompleted();
};

} // namespace db

#endif // _DB_EXECUTER_H_