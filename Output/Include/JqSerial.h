/************************************************************************/
/* ����ͨ����															*/
/* ��ע��������boost��asio�⣬���еײ㴮�ڶ�д                          */
/* by:�ſ�   2017-02-06                                                 */
/************************************************************************/
#pragma once
#include <deque>
#include <list>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace JQ{
	class JqSerial;

	/*���ڶ�д�Ĵ���ӿڣ������ṩ��д����߼�����*/
	class JqSerialDelegate{
	public:
		/*��ȡһ֡����*/
		virtual bool fetchOneFrame(std::deque<char> src, size_t& startIndex, size_t& dataLen) = 0;
		/*����һ֡����*/
		virtual bool handleFrameData(const std::deque<char> data) = 0;
	};

	class JqSerial{
	public:
		JqSerial(std::string serialPort = "COM1", size_t baudRate = 115200);
		~JqSerial();
		bool startMonitor(); /*������ش���*/
		bool stopMonitor();	/*ֹͣ��ش���*/
		void addDelegate(JqSerialDelegate* deleg); /*��Ӵ���, ע�⣡�������д����ڻ�ȡһ֡����ʱ���߼�������ͬ*/
		void removeDelegate(JqSerialDelegate* deleg); /*ɾ��ĳ������*/
		void removeAllDelegate();	/*ɾ�����д���*/
		bool writeSerial(char* data, size_t dataLen);	/*д����*/
	private:
		void oneKeyRun();	/*һ����������*/
		bool openSerial(std::string serialPort);
		bool closeSerial();
		bool initSerial(size_t baudRate);
		void readyRead();
		void onReadSerialData(char buf[], boost::system::error_code ec, size_t bytesTransferred);

	private:
		std::list<JqSerialDelegate*> _delegates; /*���еĴ���*/
		std::deque<char> _buf;		/*������ݵĻ�����*/
		char _c[1];

		boost::asio::io_service _ios;
		boost::asio::serial_port* _sp;
		boost::system::error_code _ecd;
		boost::thread* _thread;

		std::string _serialPort; /*���ں�*/
		size_t _baudRate;	/*������*/

	};
}

