#pragma once

/************************************************************************/
/* Tcp服务器代理类                                                      */
/*	JqTcpServerDelgate: 用于与客户端交互								*/
/* 作者： 张凯    时间：2017.02.25										*/
/************************************************************************/

#include <vector>
#include "JqTcpSession.hpp"
namespace JQ{
	class JqTcpServerDelegate{
	public:
		virtual bool fetchOneFrame(std::vector<char>& buf, size_t& startIndex, size_t& dataLen){
			return false;
		}
		virtual void handleOneFrame(std::vector<char> buf, void* session){
			;
		}
		virtual void handleAfterWrt(void* session, void* userData){
			;
		}
	};
}