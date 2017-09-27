//singleton.hpp
//created by yht 2017-09-26
//simple single instance template class impl

#ifndef SINGLETON_H_
#define SINGLETON_H_

template<class T>
class singleton
{
private:
	singleton(const singleton&);
	singleton& operator = (const singleton&);

public:
	singleton()
	{

	}
	~singleton()
	{

	}

	static T& instance()
	{
		static T t;
		return t;
	}
};

#endif