
#include <iostream>
#include "JqTcpServer.hpp"

using namespace JQ;

class MyDelegate :public JqTcpServerDelegate{
public:
	bool fetchOneFrame(std::vector<char>& buf, size_t& startIndex, size_t& dataLen){
		startIndex = 0;
		dataLen = 0;
		if (buf.size() > 0){
			for (dataLen = 0; dataLen < buf.size(); ++dataLen){
				if (buf.at(startIndex + dataLen) == 0){
					dataLen += 1;
					return true;
				}
			}
		}
		return false;
	}

	void handleOneFrame(std::vector<char> buf, void* session){
		int i = 0;
		for (std::vector<char>::iterator iter = buf.begin(); iter != buf.end(); ++iter){
			std::cout << *iter;
			_buf[i++] = *iter;
		}
		std::cout << std::endl;
		
		JqTcpSession* tcpSession = (JqTcpSession*)session;
		tcpSession->wrtData(_buf, buf.size(), NULL);
		
	}
private:
	char _buf[1024];
};

int main(){
	boost::asio::io_service ios;
	JqTcpServer<MyDelegate> server(ios, 12123, 2);
	server.startServer();
	ios.run();
	return 0;
}