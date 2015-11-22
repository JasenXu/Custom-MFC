// CDGramSocket.cpp : 实现文件
//


#include "StdAfx.h"
#include "DGramSocket.h"


CDGramSocket::CDGramSocket(void)
	: m_strTargetIP(_T(""))
	, m_nTargetPort(0)
	, m_nLocalPort(0)
	, m_bIsReceiving(FALSE)
	, m_hRecviveThread(NULL)
{
}


CDGramSocket::~CDGramSocket(void)
{
	// 销毁所有使用过的Socket
	CloseSender();
	CloseReceiver();
	StopReceiveThread();
}

//------------------------------------------------------------------------
// 设置远程端IP
void CDGramSocket::SetTargetIP(CString strTargetIP)
{
	m_strTargetIP = strTargetIP;
}

//------------------------------------------------------------------------
// 获取远程端IP
CString CDGramSocket::GetTargetIP(void)
{
	return CString(m_strTargetIP);
}

//------------------------------------------------------------------------
// 设置远程端口号
void CDGramSocket::SetTargetPort(UINT nTargetPort)
{
	m_nTargetPort = nTargetPort;
}

//------------------------------------------------------------------------
// 获取远程端口号
UINT CDGramSocket::GetTargetPort(void)
{
	return m_nTargetPort;
}

//------------------------------------------------------------------------
// 设置本地端口号
void CDGramSocket::SetLocalPort(UINT nLocalPort)
{
	m_nLocalPort = nLocalPort;
}

//------------------------------------------------------------------------
// 获取本地端口号
UINT CDGramSocket::GetLocalPort(void)
{
	return m_nLocalPort;
}

//------------------------------------------------------------------------
// 创建用于接收的Socket
BOOL CDGramSocket::CreateReceiver(UINT nLocalPort, LPCTSTR lpszSocketAddress)
{
	CloseReceiver();

	// 创建一个UDP类型的Socket
	m_sockReceiver = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_sockReceiver != INVALID_SOCKET)
	{
		// 在Socket上设置参数，允许地址复用
		BOOL bOptionValue(TRUE);
		int nRet = setsockopt(m_sockReceiver, SOL_SOCKET, SO_REUSEADDR, (char FAR*)&bOptionValue, sizeof(bOptionValue));
		if (nRet == SOCKET_ERROR)
		{
			CloseReceiver();
			return FALSE;
		}

		m_nLocalPort = nLocalPort;

		// 将Socket绑定到本地端口号上
		SOCKADDR_IN addr = {0};
		addr.sin_family			= AF_INET;
		addr.sin_addr.s_addr	= htonl(INADDR_ANY);
		addr.sin_port			= htons(m_nLocalPort);

		nRet = bind(m_sockReceiver, (SOCKADDR*)&addr, sizeof(addr));
		if (nRet == SOCKET_ERROR)
		{
			CloseReceiver();
			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------
// 关闭用于接收的Socket
void CDGramSocket::CloseReceiver(void)
{
	if (m_sockReceiver != INVALID_SOCKET)
	{
		closesocket(m_sockReceiver);
		m_sockReceiver = INVALID_SOCKET;
	}
}

//------------------------------------------------------------------------
// 创建用于发送的Socket
BOOL CDGramSocket::CreateSender(UINT nTargetPort, LPCTSTR lpszTargetAddress)
{
	CloseSender();

	m_nTargetPort = nTargetPort;
	m_strTargetIP = lpszTargetAddress;

	// 创建一个UDP类型的Socket
	m_sockSender = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_sockSender != INVALID_SOCKET)
	{
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------
// 关闭用于发送的Socket
void CDGramSocket::CloseSender(void)
{
	if (m_sockSender != INVALID_SOCKET)
	{
		closesocket(m_sockSender);
		m_sockSender = INVALID_SOCKET;
	}
}

//------------------------------------------------------------------------
// 接收数据
int CDGramSocket::ReceiveFrom(LPVOID lpBuf, int nBufLen, CString& rSocketAddress, UINT& rSocketPort, int nFlags)
{
	SOCKADDR_IN addr = {0};
	int nAddrLen(sizeof(addr));

	int nRet = recvfrom(m_sockReceiver, (char*)lpBuf, nBufLen, 0, (SOCKADDR*)&addr, &nAddrLen);

	try
	{
		char* pszAddr(inet_ntoa(addr.sin_addr));
		rSocketAddress = pszAddr;
		rSocketPort = ntohs(addr.sin_port);
	}
	catch (...)
	{
		rSocketAddress = _T("");
		rSocketPort = 0;
	}

	return nRet;
}


int CDGramSocket::ReceiveFrom(LPVOID lpBuf, int nBufLen, SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags)
{
	int nRet = recvfrom(m_sockReceiver, (char*)lpBuf, nBufLen, 0, (SOCKADDR*)lpSockAddr, lpSockAddrLen);
	return nRet;
}


int CDGramSocket::ReceiveFrom(LPVOID lpBuf, int nBufLen)
{
	int nRet = recvfrom(m_sockReceiver, (char*)lpBuf, nBufLen, 0, NULL, NULL);
	return nRet;
}


//------------------------------------------------------------------------
// 发送数据
int CDGramSocket::SendTo(const LPVOID lpBuf, int nBufLen, UINT nTargetPort, LPCTSTR lpszTargetAddress, int nFlags)
{
	char szIP[MAX_PATH] = {0};
	CString str(lpszTargetAddress);
	CStringToPSZ(str, szIP);

	SOCKADDR_IN addr = {0};
	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= inet_addr(szIP);
	addr.sin_port			= htons((USHORT)nTargetPort);

	int nRet = sendto(m_sockSender, (char*)lpBuf, nBufLen, 0, (SOCKADDR*)&addr, sizeof(addr));
	return nRet;
}


int CDGramSocket::SendTo(const LPVOID lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags)
{
	int nRet = sendto(m_sockSender, (char*)lpBuf, nBufLen, 0, (SOCKADDR*)lpSockAddr, nSockAddrLen);
	return nRet;
}


int CDGramSocket::SendTo(const LPVOID lpBuf, int nBufLen)
{
	char szIP[MAX_PATH] = {0};
	CString str(m_strTargetIP);
	CStringToPSZ(str, szIP);

	SOCKADDR_IN addr = {0};
	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= inet_addr(szIP);
	addr.sin_port			= htons((USHORT)m_nTargetPort);

	int nRet = sendto(m_sockSender, (char*)lpBuf, nBufLen, 0, (SOCKADDR*)&addr, sizeof(addr));
	return nRet;
}


//------------------------------------------------------------------------
// 启动数据接收线程
BOOL CDGramSocket::StartReceiveThread(void)
{
	if (m_sockReceiver == INVALID_SOCKET)
	{
		CreateReceiver(m_nLocalPort);
	}

	if (m_sockReceiver != INVALID_SOCKET)
	{
		if (m_bIsReceiving)
		{
			return TRUE;
		}

		m_hRecviveThread = AfxBeginThread((AFX_THREADPROC)ReceiveProc, (LPVOID)this);
		return (m_hRecviveThread != NULL);
	}

	return FALSE;
}

//------------------------------------------------------------------------
// 停止数据接收线程
void CDGramSocket::StopReceiveThread(void)
{
	if (m_bIsReceiving)
	{
		// 销毁Socket以使接收函数失败或脱离阻塞
		CloseReceiver();

		// 等待接收线程完全退出
		if (m_hRecviveThread)
		{
			if (WaitForSingleObject(m_hRecviveThread->m_hThread, 5000) == WAIT_TIMEOUT)
			{
				::TerminateThread(m_hRecviveThread->m_hThread, 1);
			}
			m_hRecviveThread = NULL;
		}
	}
}

//------------------------------------------------------------------------
// 数据接收线程
UINT CDGramSocket::ReceiveProc(LPVOID lpParam)
{
	ASSERT(lpParam);
	CDGramSocket* ptr = (CDGramSocket*)lpParam;
	ptr->ReceiveLoop();
	return 0;
}


void CDGramSocket::ReceiveLoop(void)
{
	char szBuffer[1024] = {0};
	SOCKADDR_IN addrClient = {0};
	int nLenClient(sizeof(addrClient));
	int nRetLen(0);

	m_bIsReceiving = TRUE;
	while (m_bIsReceiving)
	{
		// 等待接收数据
		nRetLen = recvfrom(m_sockReceiver, szBuffer, 1024, 0, (SOCKADDR*)&addrClient, (int*)&nLenClient);
		if (nRetLen == SOCKET_ERROR || nRetLen == 0)
		{
			// 如果Socket发送错误或者Socket断开，则跳出循环
			m_bIsReceiving = FALSE;
		}
		else
		{
			// TODO: 在此添加数据预处理代码
			// 连接控制
			// 向远程端发送反馈消息
		}
	}
}

//------------------------------------------------------------------------
// CString转换为char*
int CDGramSocket::CStringToPSZ(CString str, char* ptr)
{
#ifdef _UNICODE
	int nLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, str, -1, ptr, nLen, NULL, NULL);
#endif
	return 0;
}

//------------------------------------------------------------------------
// CString类型的IP转换为DWORD类型的IP
DWORD CDGramSocket::CStringToDWordIP(CString strIP)
{
	char szIP[MAX_PATH] = {0};
	CStringToPSZ(strIP, szIP);
	return DWORD(inet_addr(szIP));
}

//------------------------------------------------------------------------
// DWORD类型的IP转换为CString类型的IP
CString CDGramSocket::DWordToCStringIP(DWORD dwIP)
{
	BYTE *p = (BYTE *)&dwIP;
	CString str(_T(""));
	str.Format(_T("%d.%d.%d.%d"), *p, *(p+1), *(p+2), *(p+3));
	return str;
}

//------------------------------------------------------------------------
//	获取主机地址
void CDGramSocket::GetHostAddr(CString& strIP)
{
	// 获取本地主机名
	char chName[256] = {0x00};
	if (::gethostname(chName, 256))
	{
		strIP = _T("127.0.0.1");
		return;
	}

	// 获取本地主机地址信息
	HOSTENT* pHost(::gethostbyname(chName));
	if (!pHost)
	{
		strIP = _T("127.0.0.1");
		return;
	}

	try
	{
		if (pHost->h_addrtype == AF_INET)
		{
			// IPv4
			in_addr addr = {0};
			if ((u_long*)pHost->h_addr_list == NULL)
			{
				throw addr;
			}
			addr.S_un.S_addr = *(u_long*)pHost->h_addr_list[0];
			strIP = ::inet_ntoa(addr);
		}
		else
		{
			// IPv6
			strIP = _T("127.0.0.1");
			return;
		}
	}
	catch (...)
	{
		strIP = _T("127.0.0.1");
		return;
	}
}