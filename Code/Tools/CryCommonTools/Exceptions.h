// Copyright 2007-2021 Crytek GmbH / Crytek Group. All rights reserved.

#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include <stdexcept>
#include <string>

class BaseException : public std::exception
{
public:
	explicit BaseException(const string& msg): msg(msg) {}
	virtual const char* what() const noexcept {return msg.c_str();}

private:
	string msg;
};

template <typename Tag> class Exception : public BaseException
{
public:
	explicit Exception(const string& msg): BaseException(msg) {}
};

#endif //__EXCEPTIONS_H__
