/**********************************************************
 * Name: DBDefine.h
 * Author: zhouxiaochuan@gmail.com
 * Date: 2006-10-09
 **********************************************************/

#pragma once

#include <exception>

//---------------------------------------------------------------------------------------
// 数据库异常类
class AFX_EXT_CLASS DBException : public std::exception
{
public:
	DBException(std::string ex):error(ex){};
	virtual ~DBException() throw(){};
	const std::string& what(){return error;};
private:
	const std::string error;
};

#define START_DBPROCESS				try{
#define END_DBPROCESS				}catch(DBException& ex){printf("[DBERROR] %s\n",ex.what().c_str());}
#define END_DBPROCESS_RETURN(x)		}catch(DBException& ex){printf("[DBERROR] %s\n",ex.what().c_str());return x;}