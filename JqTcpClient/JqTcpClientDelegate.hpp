#pragma once

/************************************************************************/
/* Tcp�ͻ��˴�����                                                     */
/*	JqTcpCliente: �����������������������Ľ�������					*/
/* ���ߣ� �ſ�    ʱ�䣺2017.02.25										*/
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