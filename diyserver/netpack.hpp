//netpack.hpp
//created by yht 2017-09-26
//simple net package impl

#ifndef NETPACK_HPP
#define NETPACK_HPP
#include <string>
#include <vector>

#define PACK_SIZE_DEF     8192
struct netpack
{
	netpack(){
		m_data.resize(PACK_SIZE_DEF,0);
	}
	netpack(int nsize){
		if(nsize < 0) nsize = PACK_SIZE_DEF;
		m_data.resize(nsize,0);
	}
	char* data(){
		if(m_data.size()){
			return &m_data[0];
		}
		return NULL;
	}
	int capacity(){
		return m_data.size();
	}
	int size(){
		return m_size;
	}
	std::vector<char> m_data;
	int               m_size;
};
#endif
