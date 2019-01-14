# -*- coding: utf-8 -*-
#导入模块
import socket
from time import *
import pandas as pd 
#from WindPy import *

class Trader:
	def __init__(self, data_path):
		print("connect")
		self.client=socket.socket()
		ip_port=("127.0.0.1",8888)
		self.client.connect(ip_port)
		self.data_path=data_path
		self.start()

	def load_data(self):
		#w.start()
		#data=w.wsq("A1905.DCE", "rt_ask1,rt_bid1")
		#print(data.Data)
		pass
    	#self.data=w_wsq_data = vba_wsq("A1905.DCE","rt_ask1,rt_bid1")


	def load_singal(self):
		self.df=pd.read_csv(self.data_path)
		print(self.df)
	


	def trader(self):
		instrument_list=[];
		for index in self.df.index:
			strs=''
			for columns in self.df.columns:
				strs=strs+str(self.df.loc[index,columns])+' '
			sleep(1)
			print(strs)      
			self.client.send(bytes(strs,encoding="ascii"))   
			 
			x=self.client.recv(1024)  #接收信息
			print(x);
			#str_singal=[instrument_id,' ',price,' ',lots,' ',diretcion,' '];
			#instrument_list.append(lists);

	def start(self):
		self.load_singal()
		self.trader()

if __name__ == '__main__':
	#创建实例
	data_paths = 'predict.csv'
	Trader(data_paths)

