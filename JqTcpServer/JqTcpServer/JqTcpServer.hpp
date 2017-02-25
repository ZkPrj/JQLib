#pragma once

/************************************************************************/
/* Tcp服务器类                                                          */
/*	JqTcpServer: 用于处理客户端连接										*/
/* 作者： 张凯    时间：2017.02.22										*/
/************************************************************************/


#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <list>
#include <boost/bind.hpp>
#include <iostream>
#include "JqTcpSession.hpp"
#include "JqTcpServerDelegate.hpp"
namespace JQ{
	template<class T>
	class JqTcpServer
	{
	public:
		JqTcpServer(boost::asio::io_service& ios, unsigned short port, size_t t = 5);
		~JqTcpServer();
		void startServer();
		void stopServer();
		void reStartServer(unsigned short port);
	private:
		bool startListen(unsigned short port);
		void beginAccept();
		void acceptHandle(const boost::system::error_code& ec);
	private:
		unsigned short _port;
		boost::asio::ip::tcp::acceptor _acceptor;
		std::list<JqTcpSession*> _clients;
		boost::asio::io_service& _ios;
		size_t _t;
	};


	template<class T>
	JqTcpServer<T>::JqTcpServer(boost::asio::io_service& ios, unsigned short port, size_t t) :
		_ios(ios), _acceptor(ios), _port(port), _t(t)
	{
	}

	template<class T>
	JqTcpServer<T>::~JqTcpServer()
	{
		stopServer();
	}

	template<class T>
	void JqTcpServer<T>::startServer(){
		startListen(_port);
	}

	template<class T>
	void JqTcpServer<T>::stopServer(){
		boost::system::error_code ec;
		_acceptor.close(ec);
	}

	template<class T>
	void JqTcpServer<T>::reStartServer(unsigned short port){
		stopServer();
		startServer();
	}

	template<class T>
	bool JqTcpServer<T>::startListen(unsigned short port){
		boost::system::error_code ec;
		boost::asio::ip::tcp::endpoint edp(boost::asio::ip::tcp::v4(), port);
		_acceptor.open(edp.protocol(), ec);
		if (!ec){
			_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
			if (!ec){
				_acceptor.bind(edp, ec);
				if (!ec){
					_acceptor.listen(boost::asio::socket_base::max_connections, ec);
					if (!ec){
						beginAccept();
						return true;
					}
				}
			}
		}
		return false;
	}

	template<class T>
	void JqTcpServer<T>::beginAccept(){
		JqTcpSession* session = new JqTcpSession(_ios, new T, _t);
		_clients.push_back(session);
		_acceptor.async_accept(session->getSocket(), boost::bind(&JqTcpServer::acceptHandle, this, boost::asio::placeholders::error));
	}

	template<class T>
	void JqTcpServer<T>::acceptHandle(const boost::system::error_code& ec){
		JqTcpSession* client = _clients.back();
		_clients.pop_back();
		if (!ec){
			std::cout << client->getSocket().remote_endpoint().address().to_string() << std::endl;
			client->startSession();
			beginAccept();
		}
		else{	
			delete client;
		}

	}

}





