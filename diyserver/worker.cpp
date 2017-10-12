//worker.cpp
//created by yht 2017-08-27
//impl the worker class

#include "worker.h"
#include "msgpack.hpp"

worker::worker(void)
{
	m_pdoing = &m_msgqueue[0];
	m_precv  = &m_msgqueue[1];
	m_stop = false;
}

worker::~worker(void)
{
}

void worker::start()
{
	if(!m_pthread)
	{
		m_pthread = boost::shared_ptr<boost::thread> (new boost::thread(boost::bind(&worker::thread_fun,this)));
	}
}

void worker::stop()
{
	m_stop = true;
}

void worker::join()
{
	if(m_pthread)
	{
		m_pthread->join();
	}
}

void worker::sleep( int interval )
{
	boost::this_thread::sleep(boost::posix_time::millisec(interval));
}

void worker::push( void* msg )
{
	boost::mutex::scoped_lock l(m_mutex);
	m_precv->push_back(msg);
}

void worker::swap()
{
	boost::mutex::scoped_lock l(m_mutex);
	std::list<void*>* tmp = m_pdoing;
	m_pdoing = m_precv;
	m_precv = tmp;
}

void* worker::pop()
{
	void* pack = NULL;
	if(m_pdoing->empty())
	{
		swap();
	}

	if(m_pdoing->empty())
	{
		return pack;
	}

	pack = m_pdoing->front();
	m_pdoing->pop_front();
	return pack;
}

void worker::run()
{
	while(!done())
	{
		void* msg = pop();
		if(!msg)
		{
			sleep(1);
		}
		else
		{
			handle_msg(msg);
			msgpack* pack = (msgpack*)msg;
			delete pack;
			pack = NULL;
		}
	}
}

bool worker::done()
{
	return m_stop;
}

void worker::thread_fun()
{
	run();
}
