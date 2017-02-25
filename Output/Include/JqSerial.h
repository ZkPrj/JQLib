/************************************************************************/
/* 串口通信类															*/
/* 备注：引用了boost的asio库，进行底层串口读写                          */
/* by:张凯   2017-02-06                                                 */
/************************************************************************/
#pragma once
#include <deque>
#include <list>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace JQ{
	class JqSerial;

	/*串口读写的代理接口，用于提供读写后的逻辑处理*/
	class JqSerialDelegate{
	public:
		/*获取一帧数据*/
		virtual bool fetchOneFrame(std::deque<char> src, size_t& startIndex, size_t& dataLen) = 0;
		/*处理一帧数据*/
		virtual bool handleFrameData(const std::deque<char> data) = 0;
	};

	class JqSerial{
	public:
		JqSerial(std::string serialPort = "COM1", size_t baudRate = 115200);
		~JqSerial();
		bool startMonitor(); /*开启监控串口*/
		bool stopMonitor();	/*停止监控串口*/
		void addDelegate(JqSerialDelegate* deleg); /*添加代理, 注意！！！所有代理在获取一帧数据时的逻辑必须相同*/
		void removeDelegate(JqSerialDelegate* deleg); /*删除某个代理*/
		void removeAllDelegate();	/*删除所有代理*/
		bool writeSerial(char* data, size_t dataLen);	/*写串口*/
	private:
		void oneKeyRun();	/*一键启动串口*/
		bool openSerial(std::string serialPort);
		bool closeSerial();
		bool initSerial(size_t baudRate);
		void readyRead();
		void onReadSerialData(char buf[], boost::system::error_code ec, size_t bytesTransferred);

	private:
		std::list<JqSerialDelegate*> _delegates; /*所有的代理*/
		std::deque<char> _buf;		/*存放数据的缓冲区*/
		char _c[1];

		boost::asio::io_service _ios;
		boost::asio::serial_port* _sp;
		boost::system::error_code _ecd;
		boost::thread* _thread;

		std::string _serialPort; /*串口号*/
		size_t _baudRate;	/*波特率*/

	};
}

