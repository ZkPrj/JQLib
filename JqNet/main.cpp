#include "JqTcpClient.h"
#include <iostream>
<<<<<<< HEAD

int main(){
	JQ::JqTcpClient client("www.baidu.com", "80");
	client.startComm();
	char c;
	std::cin >> c;
=======
#include <string>

class MyDelegate :public JQ::JqTcpClientDelegate{
public:
	MyDelegate(JQ::JqTcpClient* tcpClient) :_tcpClient(tcpClient){}
	bool fetchOneFrame(std::deque<unsigned char> buf, size_t& startIndex, size_t& dataLen){
		
		startIndex = 0;
		dataLen = buf.size();
		return true;
	}
	void praseFrameData(std::deque<unsigned char> data){
		for each (unsigned char c in data)
		{
			std::cout << (char)c;
		}
		std::cout << std::endl;
		std::string wrtStr;
		std::cin >> wrtStr;
		_tcpClient->sndData(std::vector<unsigned char>(wrtStr.begin(), wrtStr.end()));
	}
	void handleErr(const boost::system::error_code& ec){
		std::cout << ec.message() << std::endl;
		_tcpClient->startComm("127.0.0.1", "80");
		std::cout << "Communicate with Self"<<std::endl;
	}

	void writeCallback(){
		std::cout << "Write Ok" << std::endl;;
		
	}
private:
	JQ::JqTcpClient* _tcpClient;
};


int main(){
	JQ::JqTcpClient client;
	MyDelegate* myDele = new MyDelegate(&client);
	client.setDelegate(myDele);
	client.startComm("127.0.0.1", "80");
	Sleep(5000);
	char* buf = "GET /";
	client.sndData(std::vector<unsigned char>((unsigned char*)buf, (unsigned char*)buf + 5));
	Sleep(10000000);
>>>>>>> 2ae37f6d33191c2c2e1bade52f8a08437fd68c5e
	return 0;
}