#pragma once
#include "afxwin.h"


#define SOCK_BUF_SIZE	(16*1024)
// CMulticastSocket

class CMulticastSocket
{
//	DECLARE_DYNAMIC(CMulticastSocket)

public:
	CMulticastSocket();
	virtual ~CMulticastSocket();

protected:
	SOCKET		m_sockRecv;
	SOCKET		m_sockSend;

	CString		m_strGroupIP;
	UINT		m_uGroupPort;

	CString		m_strLocalIP;
	UINT		m_uLocalPort;

	IP_MREQ		m_mreq;
	
	BOOL		m_bRecv;
	CWinThread*	m_hRecvThread;

	char*		m_pchBuf;
	UINT		m_uBufLen;

public:
	// ===========================================================================================
	// 设置组播IP
	// ===========================================================================================
	void SetGroupIP(CString strGroupIP);
	CString GetGroupIP(void);

	// ===========================================================================================
	// 设置组播端口
	// ===========================================================================================
	void SetGroupPort(UINT uGroupPort);
	UINT GetGroupPort(void);

	// ===========================================================================================
	// 加入或离开组播组
	// ===========================================================================================
	BOOL JoinGroup(CString strGroupIP, UINT uGroupPort, UINT uTTL, BOOL bLookback);
	BOOL JoinGroup(CString strGroupIP, UINT uGroupPort);
	BOOL LeaveGroup(void);

	// ===========================================================================================
	// 创建发送或接收socket
	// ===========================================================================================
	BOOL CreateSendSocket(UINT uTTL = 15, BOOL bLookback = 0);
	BOOL CreateRecvSocket(CString strGroupIP, UINT uGroupIP);

	// ===========================================================================================
	// 设置回送与生存时间参数
	// ===========================================================================================
	void SetLookBack(BOOL bLoop = 0);
	BOOL SetTTL(UINT uTTL = 15);

	// ===========================================================================================
	// 发送函数
	// ===========================================================================================
	int SendTo(const void* pBuf, int nBufLen, UINT uPort);
	int SendTo(const void* pBuf, int nBufLen, UINT uHostPort, LPCTSTR lpszHostAddress);

	// ===========================================================================================
	// 接收函数
	// ===========================================================================================
	int RecvFrom(LPVOID lpBuf, int nBufLen);
	int RecvFrom(LPVOID lpBuf, int nBufLen, CString& rSocketAddress, UINT& rSocketPort);

	// ===========================================================================================
	// 内部接收控制函数
	// ===========================================================================================
	BOOL StartRecv(LPVOID pBuf, UINT uBufLen);
	void StopRecv(void);

	// ===========================================================================================
	// 兼容接口函数
	// ===========================================================================================
	BOOL Create(CString strGroupIP, UINT uGroupPort, UINT uTTL = 15, BOOL bLoopback = 0);
	void Close(void);

protected:
	// ===========================================================================================
	// 内部接收线程
	// ===========================================================================================
	static UINT RecvProc(LPVOID lpParam);
	void RecvLoop(void);

	// ===========================================================================================
	// CString转换为PSZ
	// ===========================================================================================
	int CStringToPSZ(CString str, char* ptr);	
};


