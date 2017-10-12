//netpack.hpp
//created by yht 2017-08-27
//simple net package impl

#ifndef NETPACK_HPP
#define NETPACK_HPP
#include <string>
#include <vector>
#include "logger.h"

#define PACK_SIZE_DEF     8192
typedef    std::vector<char>      tbuffer;
struct netpack
{
	netpack(){
		m_data.resize(PACK_SIZE_DEF,0);
	}
	netpack(int nsize){
		if(nsize < 0) nsize = PACK_SIZE_DEF;
		m_data.resize(nsize,0);
	}
	virtual ~netpack()
	{
	}
	char* data(){
		if(m_data.size()){
			return &m_data[0];
		}
		return NULL;
	}
	bool data(std::string& v)
	{
		if (m_data.size() > v.length())
		{
			memcpy(&m_data[0], v.c_str(), v.length());
			m_size = v.length();
			return true;
		}
		return false;
	}
	bool data(tbuffer& buf)
	{
		if (m_data.size() > buf.size())
		{
			memcpy(&m_data[0], &buf[0], buf.size());
			m_size = buf.size();
			return true;
		}
		return false;
	}
	int capacity(){
		return m_data.size();
	}
	int size(){
		return m_size;
	}
	tbuffer           m_data;
	int               m_size;
};
#endif
