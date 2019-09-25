#include "Comm.h"


#include "Comm.h"

/*默认构造函数*/
CComm::CComm()
{
}

/*默认析构函数*/
CComm::~CComm()
{
}

/*打开串口函数 参数为“COM1”、“COM2”之类的串口名字符串*/
BOOL CComm::OpenComm(CString sCommName)
{
	sCommName = "//./" + sCommName;          /*利用字符串拼接*/

	hComm = CreateFile(sCommName,            /* 合法的标准设备名,//./COM1,//./COM2等 */
		GENERIC_READ | GENERIC_WRITE,        /* 访问模式,可同时读写 */
		0,                                   /* 共享模式,0表示不共享 */
		0,                                   /*安全性设置,一般使用NULL */
		OPEN_EXISTING,                       /* 该参数表示设备必须存在,否则创建失败 */
		0,
		0);
	if (hComm == INVALID_HANDLE_VALUE) /*判断打开COM操作是否成功*/
	{
		//MessageBox("Cannot open Communication Port.Please\nQuit the program and Re-start your PC.","Com Port Error",MB_OK+MB_ICONERROR);
		return false;
	}
	else
		return true;

}

/*通过设置串口控制参数控制串口*/
BOOL CComm::ConfigureComm(DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits)
{
	char szDCBparam[50];

	sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", BaudRate, Parity, ByteSize, StopBits);
	if (GetCommState(hComm, &m_dcb) && BuildCommDCB(szDCBparam, &m_dcb))
	{
		return true;
	}
	CloseHandle(hComm);
	return false;


	//    SetupComm(hComm, 1024, 1024);
	if ((m_bCommReady = GetCommState(hComm, &m_dcb)) == 0) /*判断串口信息是否获取情况*/
	{

		CloseHandle(hComm);
		return false;
	}
	m_bCommReady = 0;
	m_dcb.BaudRate = BaudRate;/*波特率*/
	m_dcb.ByteSize = ByteSize;/*通信字节位数*/
	m_dcb.Parity = Parity;    /*指定奇偶校验位*/
	m_dcb.StopBits = StopBits;/*指定停止位的位数*/
	//m_dcb.fParity = fParity;  /*指定奇偶校验位*/
#if 0
	m_dcb.fBinary = TRUE;
	m_dcb.fDsrSensitivity = false;
	m_dcb.fOutX = false;
	m_dcb.fInX = false;
	m_dcb.fNull = false;
	m_dcb.fAbortOnError = TRUE;
	m_dcb.fOutxCtsFlow = FALSE;
	m_dcb.fOutxDsrFlow = false;
	m_dcb.fDtrControl = DTR_CONTROL_DISABLE;
	m_dcb.fDsrSensitivity = false;
	m_dcb.fRtsControl = RTS_CONTROL_DISABLE;
	m_dcb.fOutxCtsFlow = false;
	m_dcb.fOutxCtsFlow = false;
#endif
	m_bCommReady = SetCommState(hComm, &m_dcb);/*将hComm的控制参数设置成m_dcb的参数*/
	if (m_bCommReady == 0) /*判断设置是否成功*/
	{
		//MessageBox("SetCommState Error", "Error", MB_OK + MB_ICONERROR);
		CloseHandle(hComm);
		//return false;
	}
	return true;
}

/*设置串口超时参数*/
BOOL CComm::SetCommunicationTimeouts(DWORD ReadIntervalTimeout, DWORD ReadTotalTimeoutMultiplier, DWORD ReadTotalTimeoutConstant, DWORD WriteTotalTimeoutMultiplier, DWORD WriteTotalTimeoutConstant)
{
	if ((m_bCommReady = GetCommTimeouts(hComm, &m_CommTimeouts)) == 0)
		return false;
	m_CommTimeouts.ReadIntervalTimeout = ReadIntervalTimeout;                /*读间隔超时时间*/
	m_CommTimeouts.ReadTotalTimeoutConstant = ReadTotalTimeoutConstant;      /*读时间系数*/
	m_CommTimeouts.ReadTotalTimeoutMultiplier = ReadTotalTimeoutMultiplier;  /*读时间常量*/
	m_CommTimeouts.WriteTotalTimeoutConstant = WriteTotalTimeoutConstant;    /*写时间系数*/
	m_CommTimeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier;/*写时间常量*/
	m_bCommReady = SetCommTimeouts(hComm, &m_CommTimeouts);/*将串口hCom超时参数设置成传入的超时参数*/
	if (m_bCommReady == 0)    /*判断设置是否成功*/
	{
		//MessageBox("StCommTimeouts function failed", "Com Port Error", MB_OK + MB_ICONERROR);
		CloseHandle(hComm);
		return false;
	}
	return true;
}

/*向串口写入1字节的数据*/
BOOL CComm::WriteByte(BYTE byByte)
{
	if (WriteFile(hComm, &byByte, 1, &iBytesWritten, NULL) == 0)/*写入指定字节个数的数据并判断写入是否成功*/
		return false;
	else return true;
}
BOOL CComm::CleanComm()
{
	return PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
}
/*从串口读取指定字节个数的数据*/
BOOL CComm::ReadByte(BYTE &byByte)
{
	BYTE m_brx;
	DWORD dwBytesTransferred = 0;/*定义实际读取字节数的变量*/

	if (ReadFile(hComm, &m_brx, 1, &dwBytesTransferred, 0))
	{
		if (dwBytesTransferred == 1)
		{
			byByte = m_brx;
			return true;
		}
	}

	return false;
}

/*串口关闭函数*/
void CComm::CloseComm()
{
	if (hComm == INVALID_HANDLE_VALUE)
		return;
	CloseHandle(hComm);/*关闭串口*/
	return;
}


// CommComunication_Send.cpp : 定义控制台应用程序的入口点。
//

int mainhj()
{
	CComm cComm;
	char szCommName[] = "COM1";
	//{ 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07 };
	if (cComm.OpenComm(szCommName))
	{

#if 0
		DCB dcb;
		//GetCommState(cComm.hComm, &dcb);//获取端口状态信息
								  //定义初始化dcb,用dcb的参数对串口的参数进行设置
		dcb.BaudRate = 9600;
		dcb.ByteSize = 8;
		dcb.Parity = 0;
		dcb.StopBits = 1;
		//if (SetCommState(cComm.hComm, &dcb))
		char szDCBparam[50];
		sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", CBR_9600, 'N', 8, 1);
		if (GetCommState(cComm.hComm, &dcb) && BuildCommDCB(szDCBparam, &dcb))
		{
			cout << "SetCommState OK." << endl;

		}
		system("pause");
#endif

		        /*定义一个COMMTIMEOUTES结构并初始化设置参数*/
		COMMTIMEOUTS  ctTimSet;
		ctTimSet.ReadIntervalTimeout = 0;
		ctTimSet.ReadTotalTimeoutMultiplier = 0;
		ctTimSet.ReadTotalTimeoutConstant = 0;
		ctTimSet.WriteTotalTimeoutMultiplier = 0;
		ctTimSet.WriteTotalTimeoutConstant = 0;

		        /*用接构当中的参数设置串口超时参数*/
		if (cComm.SetCommunicationTimeouts(ctTimSet.ReadIntervalTimeout, ctTimSet.ReadTotalTimeoutMultiplier, ctTimSet.ReadTotalTimeoutConstant, ctTimSet.WriteTotalTimeoutMultiplier, ctTimSet.WriteTotalTimeoutConstant))
		{

			DWORD BaudRate = CBR_9600;
			BYTE ByteSize = 8;
			DWORD fParity = 1;
			BYTE  Parity = 'N';
			BYTE  StopBits = 1;
			//if (cComm.ConfigureComm(BaudRate, ByteSize, fParity, Parity, StopBits))
			if (cComm.ConfigureComm(BaudRate, ByteSize, Parity, StopBits))
			{

				while (1)
				{
					int byDataNum = 0;
					BYTE  byData[9] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h','\t' };//定义要写入的数据                    
					int iByteCount = sizeof(byData) / sizeof(byData[0]);
					for (int i = 0; i != iByteCount; )
					{
						if (cComm.WriteByte(byData[i]))
						{
							i++;
							continue;

						}
						else
						{
							cout << "dog";
							Sleep(4000);

						}
						cout << endl;

					}

				}
				cout << "dog1" << endl;
			}
			cout << "dog2" << endl;

		}

		cout << "dog3" << endl;
	}
	else
	{
		return  false;
	}
	PurgeComm(cComm.hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);//清空COM1输入缓存的数据
	cComm.CloseComm();
	system("pause");
	return 0;
}