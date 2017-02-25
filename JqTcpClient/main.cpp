#include <iostream>
#include "JqTcpClient.hpp"
#include "JqTcpClientDelegate.hpp"

using namespace JQ;

class MyDelegate : public JqTcpClientDelegate{
public:
	void handleConnected(void* client){
		sprintf_s(_buf, "Connected Ok!");
		_index = 0;
		JqTcpClient<MyDelegate>* tcpClient = (JqTcpClient<MyDelegate>*)client;
		tcpClient->wrtData(_buf, strlen(_buf) + 1, &_index);
	}

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

	void handleOneFrame(std::vector<char> buf, void* client){
		for (std::vector<char>::iterator iter = buf.begin(); iter != buf.end(); ++iter){
			std::cout << *iter;
		}
		std::cout << std::endl;
		JqTcpClient<MyDelegate>* tcpClient = (JqTcpClient<MyDelegate>*)client;
		sprintf_s(_buf, "This is the %d msg", ++_index);
		tcpClient->wrtData(_buf, strlen(_buf) + 1, &_index);
	}

	void handleAfterWrt(void* client, void* userData){
		std::cout << "The " << *((int*)userData) << " msg snd ok" << std::endl;
	}
private: 
	char _buf[1024];
	int _index;
};

int main(){
	boost::asio::io_service ios;
	JqTcpClient<MyDelegate> client(ios, 3);
	client.startComm("localhost", "12123");
	ios.run();
	return 0;
}