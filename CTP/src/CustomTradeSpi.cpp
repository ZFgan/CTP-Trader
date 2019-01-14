#include <iostream>
#include <string>
#include <time.h>
#include <thread>
#include <chrono>
#include "CustomTradeSpi.h"
#include "StrategyTrade.h"
using namespace std;

// ---- 全局参数声明 ---- //
extern TThostFtdcBrokerIDType gBrokerID;                      // 模拟经纪商代码
extern TThostFtdcInvestorIDType gInvesterID;                  // 投资者账户名
extern TThostFtdcPasswordType gInvesterPassword;              // 投资者密码
extern CThostFtdcTraderApi *g_pTradeUserApi;                  // 交易指针
// ------会话参数--------//
TThostFtdcOrderRefType	order_ref;	//报单引用

void CustomTradeSpi::OnFrontConnected()
{
	std::cout << "=====建立网络连接成功=====" << std::endl;
	// 开始登录
	reqUserLogin();
}

void CustomTradeSpi::OnFrontDisconnected(int nReason)
{
	std::cerr << "=====网络连接断开=====" << std::endl;
	std::cerr << "错误码： " << nReason << std::endl;
}

void CustomTradeSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cerr << "=====网络心跳超时=====" << std::endl;
	std::cerr << "距上次连接时间： " << nTimeLapse << std::endl;
}

void CustomTradeSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	isErrorRspInfo(pRspInfo);
}

void CustomTradeSpi::OnRspUserLogin(
	CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	if (!isErrorRspInfo(pRspInfo))
	{
		std::cout << "=====账户登录成功=====" << std::endl;
		std::cout << "交易日： " << pRspUserLogin->TradingDay << std::endl;
		std::cout << "登录时间： " << pRspUserLogin->LoginTime << std::endl;
		std::cout << "帐户名： " << pRspUserLogin->UserID << std::endl;
		// 保存会话参数
		std::cout << "开启交易信号监控" << std::endl;
		// 投资者结算结果确认
		//reqSettlementInfoConfirm();
	}
}

void CustomTradeSpi::reqUserLogin()
{
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq));
	strcpy(loginReq.BrokerID, gBrokerID);
	strcpy(loginReq.UserID, gInvesterID);
	strcpy(loginReq.Password, gInvesterPassword);
	static int requestID = 0; //请求编号
	int rt = g_pTradeUserApi->ReqUserLogin(&loginReq, requestID);
	if (!rt)
		cout << "====发送登陆请求成功====" << endl;
	else
		cerr << "发送登陆请求失败,错误编码：" << rt << endl;
}

void CustomTradeSpi::OnRtnOrder(CThostFtdcOrderField * pOrder)
{
	//委托函数的具体细节此处不做输出，因为具体可以直接看客户端
	//这里仅仅输出合约代码和委托号
	//cout << pOrder->InstrumentID << "  "<<"委托号：" << pOrder->OrderSysID << endl;
}

void CustomTradeSpi::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
	cout << pTrade->InstrumentID << "  " << "已成交" << endl;
}
void CustomTradeSpi::OnRspOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}
void CustomTradeSpi::OnRspOrderAction(CThostFtdcInputOrderActionField * pInputOrderAction, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}

bool CustomTradeSpi::isErrorRspInfo(CThostFtdcRspInfoField * pRspInfo)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (bResult)
		cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

//下单核心
void CustomTradeSpi::reqOrderInsert( TThostFtdcInstrumentIDType instrumentID, TThostFtdcPriceType price, TThostFtdcVolumeType volume, string direction)
{
	CThostFtdcInputOrderField orderInsertReq;//下单结构

	//账户设置
	memset(&orderInsertReq, 0, sizeof(orderInsertReq));
	///经纪公司代码
	strcpy(orderInsertReq.BrokerID, gBrokerID);
	///投资者代码
	strcpy(orderInsertReq.InvestorID, gInvesterID);
	///合约代码
	strcpy(orderInsertReq.InstrumentID, instrumentID);
	///报单引用
	strcpy(orderInsertReq.OrderRef, order_ref);

	//价格和交易手数
	///数量：1
	orderInsertReq.VolumeTotalOriginal = volume;
	///价格
	orderInsertReq.LimitPrice = price;
	///报单价格条件: 限价
	orderInsertReq.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///有效期类型: 当日有效
	orderInsertReq.TimeCondition = THOST_FTDC_TC_GFD;


	//开平设置
	if (direction == "buy")
	{
		cout << "可以接收到buy" << endl;
		orderInsertReq.Direction = THOST_FTDC_D_Buy;       //方向
		orderInsertReq.TimeCondition = THOST_FTDC_OF_Open; //开仓
	}
	if (direction == "sell" )
	{	
		cout << "可以接收到sell" << endl;
		orderInsertReq.Direction = THOST_FTDC_D_Sell;
		orderInsertReq.TimeCondition = THOST_FTDC_OF_CloseToday;//平今，上期所特殊
	}
	if (direction == "short")
	{
		cout << "可以接收到short" << endl;
		orderInsertReq.Direction = THOST_FTDC_D_Sell;
		orderInsertReq.CombOffsetFlag[0] = THOST_FTDC_OF_Open; 
	}
	if (direction == "cover")
	{
		cout << "可以接收到cover" << endl;
		orderInsertReq.Direction = THOST_FTDC_D_Buy;
		orderInsertReq.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
	}


	//BPK SPK需要特殊定义，模拟交易不支持市价单
	if (price == 0)
	{
		///报单价格条件: 限价
		orderInsertReq.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
		//市价立即撤销
		orderInsertReq.TimeCondition = THOST_FTDC_TC_IOC;
	}


	//其他设置，理论上基本无需改变，默认即可
	///组合投机套保标志
	orderInsertReq.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///成交量类型: 任何数量
	orderInsertReq.VolumeCondition = THOST_FTDC_VC_AV;
	///最小成交量: 1
	orderInsertReq.MinVolume = 1;
	///触发条件: 立即
	orderInsertReq.ContingentCondition = THOST_FTDC_CC_Immediately;
	///强平原因: 非强平
	orderInsertReq.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///自动挂起标志: 否
	orderInsertReq.IsAutoSuspend = 0;
	///用户强评标志: 否
	orderInsertReq.UserForceClose = 0;


	//下单
	static int requestID = 0;//请求编号
	int rt = g_pTradeUserApi->ReqOrderInsert(&orderInsertReq, ++requestID);
	if (rt == -1)
		cout << "====网络连接失败====" << endl;
	else if (rt == -2)
		cout << "====未处理请求超过许可数====" << endl;
	else if (rt == -3)
		cout << "====每秒发送请求数超过许可数目====" << endl;
	else if (rt == 0)
		cout << "已发送订单" << endl;
}








