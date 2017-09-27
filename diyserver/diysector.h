#pragma once
#include "worker.h"
#include "singleton.hpp"

class diysector :
	public worker,public singleton<diysector>
{
public:
	diysector(void);
	virtual ~diysector(void);

public:
	virtual void handle_msg(void* msg);
};
