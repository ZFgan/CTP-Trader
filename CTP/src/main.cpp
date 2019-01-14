#include <iostream>
#include <stdio.h>
#include <string>
#include <ctime>
#include <thread>
//#include <windows.h>
#include "CustomTradeSpi.h"
#include "Port.h"
using namespace std;
//链接库
#pragma comment (lib, "thosttraderapi.lib")
#pragma comment (lib, "thostmduserapi.lib")

//------------------------------- 全局变量 -------------------------------------------- //
//使用全局变量违背了面向对象的初衷，此处仅仅是为了方便
//公共参数
TThostFtdcBrokerIDType gBrokerID = "9999";                          // 模拟经纪商代码
TThostFtdcInvestorIDType gInvesterID = "";                    // 输入自己的投资者账户名（sinmow模拟账户即可）
TThostFtdcPasswordType gInvesterPassword = "";                // 投资者密码
//交易参数
CustomTradeSpi *pTradeSpi = new CustomTradeSpi;					   // 创建交易回调实例
CThostFtdcTraderApi *g_pTradeUserApi = nullptr;                    // 交易指针
char gTradeFrontAddr[] = "tcp://180.168.146.187:10001";            // 模拟交易前置地址
/*
	1、多账户可使用XMLD读写  https://www.cnblogs.com/lyggqm/p/7204472.html
	2、多账户单策略（需要先登陆多个账户，每个账户新建一个交易指针即可）
*/

//无人值守参数
//业务上的需求不需要夜盘下单,具体个自己修改
int DAY_START_HOUR=8, DAY_START_MIN=45, DAY_END_HOUR=15, DAY_END_MIN=30;

void thread_server()
{
	cout << "启动服务器" << endl;
	Port *port = new Port();
	delete port;
};

void trader_server()
{

}
//----------------------------交易主机----------------------------------------------
int main()
{
	//无人值守,一种方式是使用条件变量终止线程，主线程使用notify_all启动即可
	//这里仅使用while方式终止
	//启动一个服务器线程（一直运行），实际上只需要定时取消交易线程即可
	bool recording = false;
	bool thread_ = false;

	thread server(thread_server);  	       //独立开一个线程，使得服务器一直运行
	while (true)
	{
		time_t now = time(0);
		tm *ltm = localtime(&now);
		cout << ltm->tm_min <<":"<<ltm->tm_sec << "是否交易时间："<<" "<<recording<<"线程是否启动："<<thread_<<endl;
		Sleep(1000);
		recording = false;
		//判断当前所属时间段(time>=8:30 and time<=15:30)
		if ((ltm->tm_hour == DAY_START_HOUR && ltm->tm_min > DAY_START_MIN)
			|| (ltm->tm_hour > DAY_START_HOUR	&& ltm->tm_hour < DAY_END_HOUR)
			|| (ltm->tm_hour == DAY_END_HOUR && ltm->tm_min < DAY_END_MIN))
			recording = true;
		else
			recording = false;

		if (recording  && !thread_)
		{
			cout << "启动交易主机" << endl;
			thread_ = true;
			g_pTradeUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(); // 创建交易实例
			g_pTradeUserApi->RegisterSpi(pTradeSpi);                      // 注册事件类
			g_pTradeUserApi->SubscribePublicTopic(THOST_TERT_RESTART);    // 订阅公共流
			g_pTradeUserApi->SubscribePrivateTopic(THOST_TERT_RESTART);   // 订阅私有流
			g_pTradeUserApi->RegisterFront(gTradeFrontAddr);              // 设置交易前置地址
			g_pTradeUserApi->Init();     //连接
			//g_pTraderUserApi->Join();  //线程挂载
		}
		if (!recording && thread_)
		{
			cout << "关闭交易主机" << endl;
			g_pTradeUserApi->Release();
			thread_ = false;
			cout << "交易主机关闭成功" << endl;
		}	
	}
	delete pTradeSpi;
	server.join();
	return 0;
}
