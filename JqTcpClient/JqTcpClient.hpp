#pragma once

/************************************************************************/
/* Tcp客户端处理类                                                      */
/*	JqTcpCliente: 用于与服务器通信										*/
/* 作者： 张凯    时间：2017.02.25										*/
/************************************************************************/

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <vector>
#include <string>
#include <iostream>

namespace JQ{
	template<class T>
	class JqTcpClient{
	public:
		JqTcpClient(boost::asio::io_service& ios, size_t timeoutTime = 5, bool reConn = true);
		~JqTcpClient();
		void wrtData(char* data, size_t dataLen, void* userData);
		void startComm(std::string host, std::string server);
		void closeComm();
	private:
		void reConn();
		void resolveHost();
		void beginRead();
		void resolveHandle(const boost::system::error_code& ec,
			boost::asio::ip::tcp::resolver::iterator ept);
		void connHandle(const boost::system::error_code& ec);
		void readHandle(const boost::system::error_code& ec, size_t bytesTransferred);
		void wrtHandle(const boost::system::error_code& ec, void* userData);
		void startTimer(size_t t);
		void timeoutHandle(const boost::system::error_code& ec);
	private:
		boost::asio::ip::tcp::socket _socket; /*socket*/
		boost::asio::ip::tcp::resolver _resolver; /*域名解析*/
		boost::asio::deadline_timer _timer;	/*定时器*/
		std::string _host;
		std::string _server;
		bool _commStatus; /*连接状态*/
		std::vector<char> _rcvBuf; /*接收缓冲区*/
		std::vector<char> _sndBuf; /*发送缓冲区*/
		char _onceRcv[1024]; /*临时存放一次接收到的数据*/
		T* _delegate;	/*代理*/
		boost::mutex _lock; /*同步信号*/
		bool _reConn;
		size_t _t;
	};

	template<class T>
	JqTcpClient<T>::JqTcpClient(boost::asio::io_service& ios, size_t timeoutTime, bool reConn) :
		_socket(ios), _resolver(ios), _timer(ios), _commStatus(false), _reConn(reConn),_t(timeoutTime){
		_rcvBuf.clear();
		_sndBuf.clear();
		_delegate = new T;
	}

	template<class T>
	JqTcpClient<T>::~JqTcpClient(){
		closeComm();
		if (_delegate){
			delete _delegate;
		}
	}

	template<class T>
	void JqTcpClient<T>::wrtData(char* data, size_t dataLen, void* userData){
		if (!_commStatus) return;
		_sndBuf.clear();
		_sndBuf.insert(_sndBuf.end(), data, data + dataLen);
		boost::asio::async_write(_socket, boost::asio::buffer(_sndBuf),
			boost::bind(&JqTcpClient::wrtHandle, this, boost::asio::placeholders::error, userData));
	}

	template<class T>
	void JqTcpClient<T>::startComm(std::string host, std::string server){
		_host = host;
		_server = server;
		resolveHost();
		startTimer(_t);
	}

	template<class T>
	void JqTcpClient<T>::closeComm(){
		boost::system::error_code ec;
		if (_socket.is_open()){
			
			_socket.shutdown(boost::asio::socket_base::shutdown_both, ec);
			_socket.close(ec);
		}
		_timer.cancel(ec);
		_resolver.cancel();
		_commStatus = false;
	}

	template<class T>
	void JqTcpClient<T>::reConn(){
		closeComm();
		if (_reConn){
			
			startComm(_host, _server);
			std::cout << "Reconnect to server" << std::endl;
		}
	}

	template<class T>
	void JqTcpClient<T>::resolveHost(){
		boost::asio::ip::tcp::resolver::query query(_host, _server);
		_resolver.async_resolve(query, boost::bind(&JqTcpClient::resolveHandle, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
	}

	template<class T>
	void JqTcpClient<T>::resolveHandle(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator ept){
		if (!ec){
			boost::asio::async_connect(_socket, ept, boost::bind(&JqTcpClient::connHandle, this, boost::asio::placeholders::error));
		}
	}

	template<class T>
	void JqTcpClient<T>::connHandle(const boost::system::error_code& ec){
		if (!ec){
			_lock.lock();
			_commStatus = true;
			_lock.unlock();
			/*调用代理程序*/
			if (_delegate){
				_delegate->handleConnected(this);
			}
			beginRead();/*从服务器中获取数据*/
		}
		else{
			/*连接失败*/
		}

	}

	template<class T>
	void JqTcpClient<T>::beginRead(){
		_socket.async_read_some(boost::asio::buffer(_onceRcv, sizeof(_onceRcv)),
			boost::bind(&JqTcpClient::readHandle, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}

	template<class T>
	void JqTcpClient<T>::readHandle(const boost::system::error_code& ec, size_t bytesTransferred){
		if (!ec){
			if (bytesTransferred > 0){
				_lock.lock();
				_commStatus = true;
				_lock.unlock();
				_rcvBuf.insert(_rcvBuf.end(), _onceRcv, _onceRcv + bytesTransferred);
				/*调用代理程序*/
				if (_delegate){
					size_t startIndex = 0, dataLen = 0;
					while (_delegate->fetchOneFrame(_rcvBuf, startIndex, dataLen))
					{/*获取数据并处理，然后删除该数据*/
						_delegate->handleOneFrame(std::vector<char>(_rcvBuf.begin(),
							_rcvBuf.begin() + startIndex + dataLen), this);
						_rcvBuf.erase(_rcvBuf.begin(), _rcvBuf.begin() + startIndex + dataLen);
					}
				}
				beginRead();
			}
		}
		else{
			/*读错误*/
		}
	}

	template<class T>
	void JqTcpClient<T>::wrtHandle(const boost::system::error_code& ec, void* userData){
		if (!ec){
			_lock.lock();
			_commStatus = true;
			_lock.unlock();
			/*写成功，则调用代理程序*/
			if (_delegate){
				_delegate->handleAfterWrt(this, userData);
			}
		}
	}

	template<class T>
	void JqTcpClient<T>::startTimer(size_t t){
		if (t > 0){
			_lock.lock();
			_commStatus = false;
			_lock.unlock();
			_timer.expires_from_now(boost::posix_time::seconds((long)t));
			_timer.async_wait(boost::bind(&JqTcpClient::timeoutHandle, this, boost::asio::placeholders::error));
		}
	}

	template<class T>
	void JqTcpClient<T>::timeoutHandle(const boost::system::error_code& ec){
		_lock.lock();
		bool commStatus = _commStatus;
		_lock.unlock();
		if (!commStatus){
			/*通信超时*/
			reConn();
		}
		else{
			startTimer(_t);
		}
	}
}