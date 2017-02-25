#pragma once

/************************************************************************/
/* Tcp事务类                                                          */
/*	JqTcpServer: 用于处理客户端回话										*/
/* 作者： 张凯    时间：2017.02.22										*/
/************************************************************************/

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <vector>
#include <deque>
#include <iostream>
#include "JqTcpServerDelegate.hpp"

namespace JQ{
	class JqTcpSession{
	public:
		JqTcpSession(boost::asio::io_service& ios, JqTcpServerDelegate* deleg, size_t t = 5);
		~JqTcpSession();
		boost::asio::ip::tcp::socket& getSocket();
		void startSession();
		void wrtData(char* data, size_t dataLen, void* userData);
	private:
		void beginRead();
		void startTimer(size_t t);
		void readHandle(const boost::system::error_code& ec, size_t byteRcv);
		void wrtHandle(const boost::system::error_code& ec, void* userData);
		void timeoutHandle(const boost::system::error_code& ec);
	private:
		boost::asio::ip::tcp::socket _socket;
		boost::asio::deadline_timer _timer;
		std::vector<char> _sndBuf;
		std::vector<char> _rcvBuf;
		char _onceRcv[1024];
		boost::mutex  _lock;
		bool _commStatus;
		JqTcpServerDelegate* _delegate;
		size_t _t; /*超时时间*/
	};

	JqTcpSession::JqTcpSession(boost::asio::io_service& ios, JqTcpServerDelegate* deleg, size_t t) :
		_socket(ios),_timer(ios), _delegate(deleg), _t(t){
		std::cout << "create session" << std::endl;
	}

	JqTcpSession::~JqTcpSession(){
		boost::system::error_code ec;
		if (_socket.is_open()){
			_socket.shutdown(boost::asio::socket_base::shutdown_both);
			_socket.close(ec);
		}
		_timer.cancel(ec);
		std::cout << "delete session" << std::endl;
		if (_delegate){
			delete _delegate;
			_delegate = NULL;
		}
	}

	boost::asio::ip::tcp::socket& JqTcpSession::getSocket(){
		return _socket;
	}

	void JqTcpSession::startSession(){
		beginRead();
		startTimer(_t);
	}

	void JqTcpSession::wrtData(char* data, size_t dataLen, void* userData){
		_sndBuf.clear();
		_sndBuf.insert(_sndBuf.begin(), data, data + dataLen);
		boost::asio::async_write(_socket,
			boost::asio::buffer(_sndBuf),
			boost::bind(&JqTcpSession::wrtHandle, this, boost::asio::placeholders::error, userData));
	}

	void JqTcpSession::beginRead(){
		_socket.async_read_some(boost::asio::buffer(_onceRcv, sizeof(_onceRcv)),
			boost::bind(&JqTcpSession::readHandle, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	void JqTcpSession::startTimer(size_t t){
		_lock.lock();
		_commStatus = false;
		_lock.unlock();
		boost::system::error_code ec;
		_timer.cancel(ec);
		_timer.expires_from_now(boost::posix_time::seconds((long)t), ec);
		_timer.async_wait(boost::bind(&JqTcpSession::timeoutHandle, this, boost::asio::placeholders::error));
	}

	void JqTcpSession::readHandle(const boost::system::error_code& ec, size_t byteRcv){
		if (!ec && byteRcv > 0){
			_lock.lock();
			_commStatus = true;
			_lock.unlock();
			_onceRcv[byteRcv] = 0;
			//std::cout << _onceRcv << std::endl;
			_rcvBuf.insert(_rcvBuf.end(), _onceRcv, _onceRcv + byteRcv);
			if (_delegate){
				size_t startIndex = 0, dataLen = 0;
				while (_delegate->fetchOneFrame(_rcvBuf, startIndex, dataLen)){
					//获取一帧数据成功后，开始处理该帧数据
					_delegate->handleOneFrame(std::vector<char>(_rcvBuf.begin(),
						_rcvBuf.begin() + startIndex + dataLen), this);
					//处理完该帧数据后删除该帧数据
					_rcvBuf.erase(_rcvBuf.begin(), _rcvBuf.begin() + startIndex + dataLen);
				}
			}
			beginRead();
		}
	
	}

	void JqTcpSession::wrtHandle(const boost::system::error_code& ec, void* userData){
		if (!ec){
			_lock.lock();
			_commStatus = true;
			_lock.unlock();
			if (_delegate){
				_delegate->handleAfterWrt(this, userData);
			}
		}
	}

	void JqTcpSession::timeoutHandle(const boost::system::error_code& ec){
		_lock.lock();
		bool commStatus = _commStatus;
		_lock.unlock();
		if (!commStatus){
			delete this;
		}
		else{
			startTimer(_t);
		}
		
	}

}
