#pragma once
// ---- 派生的交易类 ---- //
#include "CTP_API/ThostFtdcTraderApi.h"
#include <string>

using namespace std;

class CustomTradeSpi : public CThostFtdcTraderSpi
{
	// ---- ctp_api部分回调接口 ---- //
public:
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	void OnFrontConnected();
	///登录请求响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///错误应答
	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	void OnFrontDisconnected(int nReason);
	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	void OnHeartBeatWarning(int nTimeLapse);

	//------------------------------------下单撤单回调函数------------------------------------------//
	//报单：
	//正确的报单撤单 回报都在OnRtnOrder响应
	//成交后OnRtnTrade响应。 
	//报单错误在OnRspOrderInsert中响应，撤单错误在OnRspOrderAction
	///【报单通知】,，此处于命令行中打印下单合约和委托号，方便撤单使用
	void OnRtnOrder(CThostFtdcOrderField *pOrder);
	///【成交通知】
	void OnRtnTrade(CThostFtdcTradeField *pTrade);
	///报单录入请求响应
	void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///报单操作请求响应
	void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	// ---- 自定义函数 ---- //
public:
	bool isErrorRspInfo(CThostFtdcRspInfoField *pRspInfo); // 是否收到错误信息
	//【个性化下单】：投资者代码（多账户），合约代码，价格，下单量，方向
	//之后可能需要加入策略ID，以达到具体具体账户具体策略下单
	void reqOrderInsert(TThostFtdcInstrumentIDType instrumentID, TThostFtdcPriceType price, TThostFtdcVolumeType volume,string direction);
	//【撤单函数】，自动低频自动化交易似乎并不需要，可以手工撤单
	//void reqOrderAction()
private:
	void reqUserLogin(); // 登录请求
	//在写一个下单函数测试用
};