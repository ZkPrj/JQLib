/************************************************************************/
/* TCP客户端处理类                                                      */
/* 作者： 张凯    创建时间： 2017.02.14									*/
/************************************************************************/

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include "JqTcpClient.h"

using namespace JQ;
using namespace std;

<<<<<<< HEAD
JqTcpClient::JqTcpClient(std::string host, std::string port) :_delegate(NULL),
_host(host), _port(port), _status(false),_ios(boost::asio::io_service()),
_socket(_ios), _resolver(_ios),_timer(_ios),_work(_ios){

}

JqTcpClient::~JqTcpClient(){
	closeComm();
}

void JqTcpClient::sndData(std::vector<unsigned char> buf){
	boost::asio::async_write(_socket,
		boost::asio::buffer(buf, buf.size()),
		boost::bind(&JqTcpClient::handleWrite, this, boost::asio::placeholders::error));
	startTimer();
}

void JqTcpClient::startComm(){
	boost::thread  t(boost::bind(&boost::asio::io_service::run, &_ios));
	conn2Host();
}

void JqTcpClient::closeComm(){
	_socket.close();
	_ios.stop();
=======
JqTcpClient::JqTcpClient() :_delegate(NULL),
 _status(false),_ios(boost::asio::io_service()),
_socket(_ios), _resolver(_ios),_timer(_ios),_work(_ios){

}

JqTcpClient::~JqTcpClient(){
	closeComm();
	_ios.stop();
}

void JqTcpClient::sndData(std::vector<unsigned char> buf){

	_sndBuf.clear();
	_sndBuf = buf;
	boost::asio::async_write(_socket,
		boost::asio::buffer(_sndBuf),
		boost::bind(&JqTcpClient::handleWrite, this, boost::asio::placeholders::error));
	startTimer();
}

void JqTcpClient::startComm(std::string host, std::string port){
	_host = host;
	_port = port;
	closeComm();

	boost::thread  t(boost::bind(&boost::asio::io_service::run, &_ios));
	conn2Host();
}

void JqTcpClient::closeComm(){
	_socket.close();
>>>>>>> 2ae37f6d33191c2c2e1bade52f8a08437fd68c5e
}

void JqTcpClient::setDelegate(JqTcpClientDelegate* deleg){
	_delegate = deleg;
}

void JqTcpClient::conn2Host(){
	boost::asio::ip::tcp::resolver::query  query(_host, _port);
	_resolver.async_resolve(query, boost::bind(&JqTcpClient::handleResolve, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::iterator));
	startTimer();
}

void JqTcpClient::startTimer(){
	_status = false;
	_timer.cancel();
<<<<<<< HEAD
	_timer.expires_from_now(boost::posix_time::seconds(3));
=======
	_timer.expires_from_now(boost::posix_time::seconds(5));
>>>>>>> 2ae37f6d33191c2c2e1bade52f8a08437fd68c5e
	_timer.async_wait(boost::bind(&JqTcpClient::handleTimeOut, this, boost::asio::placeholders::error));
}

void JqTcpClient::startReadData(){
	_socket.async_read_some(boost::asio::buffer(_tempBuf, sizeof(_tempBuf)),
		boost::bind(&JqTcpClient::handleReadSome, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void JqTcpClient::handleResolve(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator endPnt){
	if (!ec){

		boost::asio::async_connect(_socket,
			endPnt, boost::bind(&JqTcpClient::handleConnect,
			this, boost::asio::placeholders::error));
		startTimer();
	}
	else{
		if (_delegate){
<<<<<<< HEAD
=======

>>>>>>> 2ae37f6d33191c2c2e1bade52f8a08437fd68c5e
			_delegate->handleErr(ec);
		}
	}
}

void JqTcpClient::handleConnect(const boost::system::error_code& ec){
	if (!ec){
<<<<<<< HEAD
		_status = true;
		startReadData();
	}
	else{
		if (_delegate){
=======
		startReadData();
		startTimer();
	}
	else{
		if (_delegate){

>>>>>>> 2ae37f6d33191c2c2e1bade52f8a08437fd68c5e
			_delegate->handleErr(ec);
		}
	}
}

void JqTcpClient::handleReadSome(const boost::system::error_code& ec, size_t bytesTransferred){
	if (!ec){
<<<<<<< HEAD
=======
		_status = true;
>>>>>>> 2ae37f6d33191c2c2e1bade52f8a08437fd68c5e
		_buf.insert(_buf.end(), _tempBuf, _tempBuf + bytesTransferred);/*先把数据插入到缓冲区末尾*/
		size_t start = 0, dataLen = 0;
		if (_delegate){
			if (_delegate->fetchOneFrame(_buf, start, dataLen)){ /*获取一帧数据*/
				_delegate->praseFrameData(deque<unsigned char>(_buf.begin() + start, _buf.begin() + start + dataLen));
				_buf.erase(_buf.begin(), _buf.begin() + start + dataLen); /*处理该帧数据，并从缓冲区中删除该数据*/
			}
		}
		startReadData();
	}
	else{
		if (_delegate){

			_delegate->handleErr(ec);
		}
	}
}

void JqTcpClient::handleWrite(const boost::system::error_code& ec){
	if (!ec){ //写数据成功
		_status = true;
		if (_delegate){
			_delegate->writeCallback();
		}
	}
	else{
		if (_delegate){
<<<<<<< HEAD
=======

>>>>>>> 2ae37f6d33191c2c2e1bade52f8a08437fd68c5e
			_delegate->handleErr(ec);
		}
	}
}

void JqTcpClient::handleTimeOut(const boost::system::error_code& ec){
<<<<<<< HEAD
	if (ec.value() == boost::system::errc::timed_out){
=======
	if (!ec){
>>>>>>> 2ae37f6d33191c2c2e1bade52f8a08437fd68c5e
		//超时
		if (_status){//已经收到数据
			
		}
		else {
			//网络超时
<<<<<<< HEAD
		}
	}
	std::cout << ec.message() << std::endl;
=======
			if (_delegate){
				closeComm();
			}
		}
	}
>>>>>>> 2ae37f6d33191c2c2e1bade52f8a08437fd68c5e
}

