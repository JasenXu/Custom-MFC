#include "StdAfx.h"
#include "StreamSocket.h"


CStreamSocket::CStreamSocket(void)
	: m_hSocket(NULL)
{
}


CStreamSocket::~CStreamSocket(void)
{
	Close();
}


//	-------------------------------------------------------------------------------------------------------
//	在构造函数之后，创建套接字
//	-------------------------------------------------------------------------------------------------------
BOOL CStreamSocket::Create(UINT nSocketPort, LPCTSTR lpszSocketAddress)
{
	m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_hSocket == INVALID_SOCKET)
	{
		return FALSE;
	}

	// 设置参数
	int nOptVal(128*1024);
	SetSockOpt(SO_SNDBUF, &nOptVal, sizeof(int));
	SetSockOpt(SO_RCVBUF, &nOptVal, sizeof(int));

	BOOL bOptVal(TRUE);
	SetSockOpt(TCP_NODELAY, &bOptVal, sizeof(BOOL), IPPROTO_TCP);
	SetSockOpt(SO_REUSEADDR, &bOptVal, sizeof(BOOL));
	SetSockOpt(SO_DONTLINGER, &bOptVal, sizeof(BOOL));

	if (nSocketPort == 0 && lpszSocketAddress == NULL)
		return TRUE;

	// 服务器端绑定socket
	SOCKADDR_IN addr;
	addr.sin_family				= AF_INET;
	addr.sin_port				= htons(nSocketPort);

	if (lpszSocketAddress == NULL)
	{
		addr.sin_addr.S_un.S_addr	= htonl(INADDR_ANY);
	} 
	else
	{	
		char chIP[MAX_PATH] = {0};
		CStringToPSZ((CString)lpszSocketAddress, chIP);
		addr.sin_addr.S_un.S_addr	= inet_addr(chIP);
	}

	// 绑定套接收字
	int iRet = bind(m_hSocket, (PSOCKADDR)&addr, sizeof(SOCKADDR));
	return (iRet == 0);
}

//	-------------------------------------------------------------------------------------------------------
//	应答连接请求
//	-------------------------------------------------------------------------------------------------------
BOOL CStreamSocket::Accept(CStreamSocket& rConnectedSocket, SOCKADDR* lpSockAddr, int* lpSockAddrLen)
{
	SOCKADDR_IN addrClient = {0};
	int nAddrLen(sizeof(SOCKADDR));

	if (lpSockAddr == NULL)
	{
		lpSockAddr = (SOCKADDR*)&addrClient;
	}

	if (lpSockAddrLen == NULL)
	{
		lpSockAddrLen = &nAddrLen;
	}

	rConnectedSocket.m_hSocket = accept(m_hSocket, lpSockAddr, lpSockAddrLen);

	return (rConnectedSocket.m_hSocket != INVALID_SOCKET);
}

//	-------------------------------------------------------------------------------------------------------
//	绑定套接字
//	-------------------------------------------------------------------------------------------------------
BOOL CStreamSocket::Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress)
{
	SOCKADDR_IN addr;
	addr.sin_family				= AF_INET;
	addr.sin_port				= htons(nSocketPort);

	if (lpszSocketAddress == NULL)
	{
		addr.sin_addr.S_un.S_addr	= htonl(INADDR_ANY);
	} 
	else
	{	
		char chIP[MAX_PATH] = {0};
		CStringToPSZ((CString)lpszSocketAddress, chIP);
		addr.sin_addr.S_un.S_addr	= inet_addr(chIP);
	}

	// 绑定套接收字
	int iRet = bind(m_hSocket, (PSOCKADDR)&addr, sizeof(SOCKADDR));
	return (iRet == 0);
}

//	-------------------------------------------------------------------------------------------------------
//	绑定套接字
//	-------------------------------------------------------------------------------------------------------
BOOL CStreamSocket::Bind(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
	int iRet = bind(m_hSocket, (PSOCKADDR)&lpSockAddr, nSockAddrLen);
	return (iRet == 0);
}

//	-------------------------------------------------------------------------------------------------------
//	关闭套接字
//	-------------------------------------------------------------------------------------------------------
BOOL CStreamSocket::Close(void)
{
	shutdown(m_hSocket, 2);
	int iRet = closesocket(m_hSocket);
	if (iRet == SOCKET_ERROR)
	{
		WSAGetLastError();
		return FALSE;
	}

	return TRUE;
}

//	-------------------------------------------------------------------------------------------------------
//	非阻塞发送连接请求
//	-------------------------------------------------------------------------------------------------------
BOOL CStreamSocket::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
	timeval timeout = {0};
	fd_set read = {0};
	
	// 设为非阻塞模式
	int nFlag(1);
	if (ioctlsocket(m_hSocket, FIONBIO, (ULONG *)&nFlag) != 0)
	{
		closesocket(m_hSocket);
		return FALSE;
	}

	// connect
	char chIP[MAX_PATH] = {0};
	CStringToPSZ((CString)lpszHostAddress, chIP);

	SOCKADDR_IN addr;
	addr.sin_addr.S_un.S_addr	= inet_addr(chIP);
	addr.sin_family				= AF_INET;
	addr.sin_port				= htons(nHostPort);

	BOOL bRet(FALSE);
	int iRet = connect(m_hSocket, (PSOCKADDR)&addr, sizeof(SOCKADDR));
	if (iRet == SOCKET_ERROR)
	{
		FD_ZERO(&read);
		FD_SET(m_hSocket, &read);

		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		int nError(0);
		int nLen = sizeof(int);
		iRet = select(0, 0, &read, 0, &timeout);
		if (iRet > 0)
		{
			getsockopt(m_hSocket, SOL_SOCKET, SO_ERROR, (FAR char *)&nError, &nLen);
			if (nError == 0)
			{
				bRet = TRUE;
			}
		}
	}
	else
	{
		bRet = TRUE;
	}

	if (!bRet)
	{
		closesocket(m_hSocket);
		bRet = FALSE;
	}

	// 转为阻塞模式
	nFlag = 0;
	if (ioctlsocket(m_hSocket, FIONBIO, (ULONG *)&nFlag) == SOCKET_ERROR)
	{
		closesocket(m_hSocket);
		bRet = FALSE;
	}

	return bRet;
}

//	-------------------------------------------------------------------------------------------------------
//	发送连接请求
//	-------------------------------------------------------------------------------------------------------
BOOL CStreamSocket::Connect(const SOCKADDR* lpSockAddr, int nSocketAddrLen)
{
	int iRet = connect(m_hSocket, (PSOCKADDR)&lpSockAddr, nSocketAddrLen);
	return (iRet == 0);
}

//	-------------------------------------------------------------------------------------------------------
//	监听连接请求
//	-------------------------------------------------------------------------------------------------------
BOOL CStreamSocket::Listen(int nConnectionBacklog)
{
	int iRet = listen(m_hSocket, nConnectionBacklog);
	return (iRet == 0);
}

//	-------------------------------------------------------------------------------------------------------
//	接收数据
//	-------------------------------------------------------------------------------------------------------
int CStreamSocket::Receive(LPVOID lpBuf, int nBufLen, int nFlags)
{
	int iRetLen = recv(m_hSocket, (PSZ)lpBuf, nBufLen, nFlags);
	return iRetLen;
}

//	-------------------------------------------------------------------------------------------------------
//	发送数据
//	-------------------------------------------------------------------------------------------------------
int CStreamSocket::Send(const LPVOID lpBuf, int nBufLen, int nFlags)
{
	int iRetLen = send(m_hSocket, (PSZ)lpBuf, nBufLen, nFlags);
	return iRetLen;
}

//	-------------------------------------------------------------------------------------------------------
//	关闭套接字的发送端、接收端或收发端
//	-------------------------------------------------------------------------------------------------------
BOOL CStreamSocket::ShutDown(int nHow)
{
	int iRet = shutdown(m_hSocket, nHow);
	return (iRet == 0);
}

//	-------------------------------------------------------------------------------------------------------
//	获取最近产生的错误
//	-------------------------------------------------------------------------------------------------------
int PASCAL CStreamSocket::GetLastError(void)
{
	return int(WSAGetLastError());
}

//	-------------------------------------------------------------------------------------------------------
//	获取套接字属性
//	-------------------------------------------------------------------------------------------------------
BOOL CStreamSocket::GetSockOpt(int nOptionName, LPVOID lpOptionValue, int* lpOptionLen, int nLevel)
{
	int iRet = getsockopt(m_hSocket, nLevel, nOptionName, (char FAR*)lpOptionValue, (int*)lpOptionLen);
	return (iRet == 0);
}

//	-------------------------------------------------------------------------------------------------------
//	设置套接字属性
//	-------------------------------------------------------------------------------------------------------
BOOL CStreamSocket::SetSockOpt(int nOptionName, const LPVOID lpOptionValue, int nOptionLen, int nLevel)
{
	int iRet = setsockopt(m_hSocket, nLevel, nOptionName, (char FAR*)lpOptionValue, nOptionLen);
	return (iRet == 0);
}

//	-------------------------------------------------------------------------------------------------------
//	CString转换为PSZ
//	-------------------------------------------------------------------------------------------------------
int CStreamSocket::CStringToPSZ(CString str, char* ptr)
{
#ifdef _UNICODE
	int nLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, str, -1, ptr, nLen, NULL, NULL);
#endif
	return 0;
}

//	-------------------------------------------------------------------------------------------------------
//	获取主机地址
//	-------------------------------------------------------------------------------------------------------
void CStreamSocket::GetHostAddr(CString& strIP)
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

//	-------------------------------------------------------------------------------------------------------
//	关联SOCKET句柄
//	-------------------------------------------------------------------------------------------------------
BOOL CStreamSocket::Attach(SOCKET hSocket)
{
	if (m_hSocket == NULL)
	{
		m_hSocket = hSocket;
		return TRUE;
	}

	return FALSE;
}

//	-------------------------------------------------------------------------------------------------------
//	分离SOCKET句柄
//	-------------------------------------------------------------------------------------------------------
SOCKET CStreamSocket::Detach(void)
{
	SOCKET hSocket = m_hSocket;
	m_hSocket = NULL;

	return SOCKET(hSocket);
}

//	-------------------------------------------------------------------------------------------------------
//	CString类型的IP转换为DWORD类型的IP
//	-------------------------------------------------------------------------------------------------------
DWORD CStreamSocket::CStringToDWordIP(CString strIP)
{
	char szIP[MAX_PATH] = {0};
	CStringToPSZ(strIP, szIP);
	return DWORD(inet_addr(szIP));
}

//	-------------------------------------------------------------------------------------------------------
//	DWORD类型的IP转换为CString类型的IP
//	-------------------------------------------------------------------------------------------------------
CString CStreamSocket::DWordToCStringIP(DWORD dwIP)
{
	BYTE *p = (BYTE *)&dwIP;
	CString str(_T(""));
	str.Format(_T("%d.%d.%d.%d"), *p, *(p+1), *(p+2), *(p+3));
	return str;
}