#include "Comm.h"


#include "Comm.h"

/*Ĭ�Ϲ��캯��*/
CComm::CComm()
{
}

/*Ĭ����������*/
CComm::~CComm()
{
}

/*�򿪴��ں��� ����Ϊ��COM1������COM2��֮��Ĵ������ַ���*/
BOOL CComm::OpenComm(CString sCommName)
{
	sCommName = "//./" + sCommName;          /*�����ַ���ƴ��*/

	hComm = CreateFile(sCommName,            /* �Ϸ��ı�׼�豸��,//./COM1,//./COM2�� */
		GENERIC_READ | GENERIC_WRITE,        /* ����ģʽ,��ͬʱ��д */
		0,                                   /* ����ģʽ,0��ʾ������ */
		0,                                   /*��ȫ������,һ��ʹ��NULL */
		OPEN_EXISTING,                       /* �ò�����ʾ�豸�������,���򴴽�ʧ�� */
		0,
		0);
	if (hComm == INVALID_HANDLE_VALUE) /*�жϴ�COM�����Ƿ�ɹ�*/
	{
		//MessageBox("Cannot open Communication Port.Please\nQuit the program and Re-start your PC.","Com Port Error",MB_OK+MB_ICONERROR);
		return false;
	}
	else
		return true;

}

/*ͨ�����ô��ڿ��Ʋ������ƴ���*/
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
	if ((m_bCommReady = GetCommState(hComm, &m_dcb)) == 0) /*�жϴ�����Ϣ�Ƿ��ȡ���*/
	{

		CloseHandle(hComm);
		return false;
	}
	m_bCommReady = 0;
	m_dcb.BaudRate = BaudRate;/*������*/
	m_dcb.ByteSize = ByteSize;/*ͨ���ֽ�λ��*/
	m_dcb.Parity = Parity;    /*ָ����żУ��λ*/
	m_dcb.StopBits = StopBits;/*ָ��ֹͣλ��λ��*/
	//m_dcb.fParity = fParity;  /*ָ����żУ��λ*/
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
	m_bCommReady = SetCommState(hComm, &m_dcb);/*��hComm�Ŀ��Ʋ������ó�m_dcb�Ĳ���*/
	if (m_bCommReady == 0) /*�ж������Ƿ�ɹ�*/
	{
		//MessageBox("SetCommState Error", "Error", MB_OK + MB_ICONERROR);
		CloseHandle(hComm);
		//return false;
	}
	return true;
}

/*���ô��ڳ�ʱ����*/
BOOL CComm::SetCommunicationTimeouts(DWORD ReadIntervalTimeout, DWORD ReadTotalTimeoutMultiplier, DWORD ReadTotalTimeoutConstant, DWORD WriteTotalTimeoutMultiplier, DWORD WriteTotalTimeoutConstant)
{
	if ((m_bCommReady = GetCommTimeouts(hComm, &m_CommTimeouts)) == 0)
		return false;
	m_CommTimeouts.ReadIntervalTimeout = ReadIntervalTimeout;                /*�������ʱʱ��*/
	m_CommTimeouts.ReadTotalTimeoutConstant = ReadTotalTimeoutConstant;      /*��ʱ��ϵ��*/
	m_CommTimeouts.ReadTotalTimeoutMultiplier = ReadTotalTimeoutMultiplier;  /*��ʱ�䳣��*/
	m_CommTimeouts.WriteTotalTimeoutConstant = WriteTotalTimeoutConstant;    /*дʱ��ϵ��*/
	m_CommTimeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier;/*дʱ�䳣��*/
	m_bCommReady = SetCommTimeouts(hComm, &m_CommTimeouts);/*������hCom��ʱ�������óɴ���ĳ�ʱ����*/
	if (m_bCommReady == 0)    /*�ж������Ƿ�ɹ�*/
	{
		//MessageBox("StCommTimeouts function failed", "Com Port Error", MB_OK + MB_ICONERROR);
		CloseHandle(hComm);
		return false;
	}
	return true;
}

/*�򴮿�д��1�ֽڵ�����*/
BOOL CComm::WriteByte(BYTE byByte)
{
	if (WriteFile(hComm, &byByte, 1, &iBytesWritten, NULL) == 0)/*д��ָ���ֽڸ��������ݲ��ж�д���Ƿ�ɹ�*/
		return false;
	else return true;
}
BOOL CComm::CleanComm()
{
	return PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
}
/*�Ӵ��ڶ�ȡָ���ֽڸ���������*/
BOOL CComm::ReadByte(BYTE &byByte)
{
	BYTE m_brx;
	DWORD dwBytesTransferred = 0;/*����ʵ�ʶ�ȡ�ֽ����ı���*/

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

/*���ڹرպ���*/
void CComm::CloseComm()
{
	if (hComm == INVALID_HANDLE_VALUE)
		return;
	CloseHandle(hComm);/*�رմ���*/
	return;
}


// CommComunication_Send.cpp : �������̨Ӧ�ó������ڵ㡣
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
		//GetCommState(cComm.hComm, &dcb);//��ȡ�˿�״̬��Ϣ
								  //�����ʼ��dcb,��dcb�Ĳ����Դ��ڵĲ�����������
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

		        /*����һ��COMMTIMEOUTES�ṹ����ʼ�����ò���*/
		COMMTIMEOUTS  ctTimSet;
		ctTimSet.ReadIntervalTimeout = 0;
		ctTimSet.ReadTotalTimeoutMultiplier = 0;
		ctTimSet.ReadTotalTimeoutConstant = 0;
		ctTimSet.WriteTotalTimeoutMultiplier = 0;
		ctTimSet.WriteTotalTimeoutConstant = 0;

		        /*�ýӹ����еĲ������ô��ڳ�ʱ����*/
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
					BYTE  byData[9] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h','\t' };//����Ҫд�������                    
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
	PurgeComm(cComm.hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);//���COM1���뻺�������
	cComm.CloseComm();
	system("pause");
	return 0;
}