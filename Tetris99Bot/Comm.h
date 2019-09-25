#pragma once

#include  <iostream>
#include  <Windows.h>
#include  <string>
#include  <atlstr.h>
using namespace std;

class CComm
{
public:
	CComm();
	~CComm();
public:
	void CloseComm();            /**串口关闭函数*/
	BOOL ReadByte(BYTE &byByte); /*读取数据函数*/
	BOOL WriteByte(BYTE bybyte);/*写入数据函数*/
	BOOL OpenComm(CString sCommName);   /*串口打开函数*/
	BOOL CleanComm();                   /*清理串口缓存区*/

    /*对串口超时参数进行设置*/                                    
	BOOL SetCommunicationTimeouts(DWORD ReadIntervalTimeout,  /*读间隔超时时间*/
		DWORD ReadTotalTimeoutMultiplier,  /*读时间系数*/
		DWORD ReadTotalTimeoutConstant,    /*读时间常量*/
		DWORD WriteTotalTimeoutMultiplier, /*写时间系数*/
		DWORD WriteTotalTimeoutConstant);  /*写时间常量*/

     /*对串口超时参数进行设置*/                                      
	BOOL ConfigureComm(
		DWORD BaudRate,  /*波特率*/
		BYTE ByteSize,   /*通信字节位数*/    
		BYTE  Parity,    /*指定奇偶校验位*/
		BYTE StopBits);  /*指定停止位的位数*/
	HANDLE hComm;               /*串口COM句柄*/
	DCB    m_dcb;               /*串口状态信息*/
	COMMTIMEOUTS m_CommTimeouts;
	BOOL  m_bCommReady;
	BOOL  bWriteRC;
	BOOL  bReadRC;
	DWORD  iBytesWritten;
	DWORD  iBytesRead;
	DWORD  dwBytesRead;
};
