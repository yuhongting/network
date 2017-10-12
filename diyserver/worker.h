//worker.h
//created by yht 2017-08-27
//actually the worker is a thread class with two message queue

#pragma once
#include <list>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

class worker
{
public:
	worker(void);
	virtual ~worker(void);

public:
	virtual void start();
	virtual	void stop();
	virtual	void join();
	virtual void sleep(int interval);

	virtual void push(void* msg);
	virtual void swap();
	virtual void* pop();
	virtual void run();
	virtual	bool done();
	virtual void handle_msg(void* msg) = 0;
private:
	void thread_fun();

protected:
	boost::shared_ptr<boost::thread> m_pthread;
	std::list<void*>     m_msgqueue[2];
	std::list<void*>*    m_precv;
	std::list<void*>*    m_pdoing;
	boost::mutex         m_mutex;
	bool				 m_stop;
};