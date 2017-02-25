#pragma once

/************************************************************************/
/* Tcp客户端代理类                                                     */
/*	JqTcpCliente: 用于与解析并处理与服务器的交互数据					*/
/* 作者： 张凯    时间：2017.02.25										*/
/************************************************************************/
#include <vector>

namespace JQ{
	class JqTcpClientDelegate{
	public:
		virtual void handleConnected(void* client){

		}
		virtual bool fetchOneFrame(std::vector<char>& buf, size_t& startIndex, size_t& dataLen){
			return false;
		}
		virtual void handleOneFrame(std::vector<char> buf, void* client){
			;
		}
		virtual void handleAfterWrt(void* client, void* userData){
			;
		}
	};
}