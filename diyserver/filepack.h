#pragma once

#include <vector>

#define BLOCK_SIZE   4096
class filepack
{
public:
	filepack();
	virtual ~filepack();

	bool resize(int nsize);
	int  size();
	bool set_data(const char* data, int block, int len = BLOCK_SIZE);

protected:
	int               m_ctime;  /// create time,the buffer will alive sometime,if timeout,it will be deleted automatic
	std::vector<char> m_buffer;
	std::vector<bool> m_valid;
};

