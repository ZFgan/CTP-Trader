#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <thread>
#include <mutex>
#include "StrategyTrade.h"
#include "CustomTradeSpi.h"



////可以采用从数据库读取数据
//extern std::unordered_map<std::string, TickToKlineHelper> g_KlineHash;
//
//// 线程互斥量
//std::mutex marketDataMutex;
//
//void StrategyCheckAndTrade(TThostFtdcInstrumentIDType instrumentID, CustomTradeSpi *customTradeSpi)
//{
//	// 加锁
//	std::lock_guard<std::mutex> lk(marketDataMutex);
//	TickToKlineHelper tickToKlineObject = g_KlineHash.at(std::string(instrumentID));
//	// 策略
//	std::vector<double> priceVec = tickToKlineObject.m_priceVec;
//	//std::cout << priceVec.size() << std::endl;
//	if (priceVec.size() >= 3)
//	{
//		int len = priceVec.size();
//		//std::cout << priceVec[len - 1] << std::endl;
//		//最后连 就买开仓,反之就卖开仓,这里暂时用最后一个价格下单
//		if (priceVec[len - 1] > priceVec[len - 2] && priceVec[len - 2] > priceVec[len - 3])
//			customTradeSpi->reqOrderInsert(instrumentID, priceVec[len - 1], 1, THOST_FTDC_D_Buy);
//		else if (priceVec[len - 1] < priceVec[len - 2] && priceVec[len - 2] < priceVec[len - 3])
//			customTradeSpi->reqOrderInsert(instrumentID, priceVec[len - 1], 1, THOST_FTDC_D_Buy);
//	}
//}
