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
	void CloseComm();            /**���ڹرպ���*/
	BOOL ReadByte(BYTE &byByte); /*��ȡ���ݺ���*/
	BOOL WriteByte(BYTE bybyte);/*д�����ݺ���*/
	BOOL OpenComm(CString sCommName);   /*���ڴ򿪺���*/
	BOOL CleanComm();                   /*�����ڻ�����*/

    /*�Դ��ڳ�ʱ������������*/                                    
	BOOL SetCommunicationTimeouts(DWORD ReadIntervalTimeout,  /*�������ʱʱ��*/
		DWORD ReadTotalTimeoutMultiplier,  /*��ʱ��ϵ��*/
		DWORD ReadTotalTimeoutConstant,    /*��ʱ�䳣��*/
		DWORD WriteTotalTimeoutMultiplier, /*дʱ��ϵ��*/
		DWORD WriteTotalTimeoutConstant);  /*дʱ�䳣��*/

     /*�Դ��ڳ�ʱ������������*/                                      
	BOOL ConfigureComm(
		DWORD BaudRate,  /*������*/
		BYTE ByteSize,   /*ͨ���ֽ�λ��*/    
		BYTE  Parity,    /*ָ����żУ��λ*/
		BYTE StopBits);  /*ָ��ֹͣλ��λ��*/
	HANDLE hComm;               /*����COM���*/
	DCB    m_dcb;               /*����״̬��Ϣ*/
	COMMTIMEOUTS m_CommTimeouts;
	BOOL  m_bCommReady;
	BOOL  bWriteRC;
	BOOL  bReadRC;
	DWORD  iBytesWritten;
	DWORD  iBytesRead;
	DWORD  dwBytesRead;
};
