/************************************************************************/
/* TCP客户端处理类                                                      */
/* 作者： 张凯    创建时间： 2017.02.14									*/
/************************************************************************/

#pragma once
#include <deque>
#include <string>
#include <boost/asio.hpp>
#include <vector>
#include <boost/array.hpp>
namespace JQ{
	class JqTcpClientDelegate{
	public:
		virtual bool fetchOneFrame(std::deque<unsigned char> buf, size_t& startIndex, size_t& dataLen) = 0;
		virtual void praseFrameData(std::deque<unsigned char> data){}
		virtual void handleErr(const boost::system::error_code& ec){}
		virtual void writeCallback(){}/*写数据结束后的处理*/
	};
	class JqTcpClient{
	public:
<<<<<<< HEAD:JqNet/JqTcpClient.h
		JqTcpClient(std::string host, std::string port);
=======
		JqTcpClient();
>>>>>>> 2ae37f6d33191c2c2e1bade52f8a08437fd68c5e:Output/Include/JqTcpClient.h
		~JqTcpClient();
		void sndData(std::vector<unsigned char> buf);
		void startComm(std::string host, std::string port); /*开启通信*/
		void closeComm();	/*关闭通信*/
		void setDelegate(JqTcpClientDelegate* deleg);
	private:
		void conn2Host();
		void startReadData();
		void startTimer();
		void handleResolve(const boost::system::error_code& ec,
			boost::asio::ip::tcp::resolver::iterator endPnt);
		void handleConnect(const boost::system::error_code& ec);
		void handleReadSome(const boost::system::error_code& ec, size_t bytesTransferred);
		void handleWrite(const boost::system::error_code& ec);
		void handleTimeOut(const boost::system::error_code& ec);
	private:
		JqTcpClientDelegate* _delegate; /*代理*/
		std::string _host;	/*主机*/
		std::string _port;	/*端口*/
		boost::asio::io_service _ios;
		boost::asio::ip::tcp::socket _socket;
		boost::asio::ip::tcp::resolver _resolver;
		bool _status;	/*连接状态,用于超时判断*/
		std::deque<unsigned char> _buf; /*读缓冲区*/
<<<<<<< HEAD:JqNet/JqTcpClient.h
=======
		std::vector<unsigned char> _sndBuf;
>>>>>>> 2ae37f6d33191c2c2e1bade52f8a08437fd68c5e:Output/Include/JqTcpClient.h
		unsigned char _tempBuf[8000];
		boost::asio::deadline_timer _timer;	/*定时器用于判断超时*/
		boost::asio::io_service::work _work;
	};
}