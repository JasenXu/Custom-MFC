// CDGramSocket.h : 头文件
//


#pragma once
#include "afxwin.h"



class CDGramSocket
{
public:
	CDGramSocket(void);
	virtual ~CDGramSocket(void);

private:
	SOCKET		m_sockReceiver;		// 用于接收的Socket
	SOCKET		m_sockSender;		// 用于发送的Socket
	CString		m_strTargetIP;		// 远程端IP
	UINT		m_nTargetPort;		// 远程端口号
	UINT		m_nLocalPort;		// 本地端口号
	BOOL		m_bIsReceiving;		// 正在接收数据标记
	CWinThread* m_hRecviveThread;	// 接收线程句柄

public:
	// 设置/获取远程端IP
	void SetTargetIP(CString strTargetIP);
	CString GetTargetIP(void);

	// 设置/获取远程端口号
	void SetTargetPort(UINT nTargetPort);
	UINT GetTargetPort(void);
	
	// 设置/获取本地端口号
	void SetLocalPort(UINT nLocalPort);
	UINT GetLocalPort(void);
	
	// 创建/关闭用于接收的Socket
	BOOL CreateReceiver(UINT nLocalPort, LPCTSTR lpszSocketAddress = NULL);
	void CloseReceiver(void);
	
	// 创建/关闭用于发送的Socket
	BOOL CreateSender(UINT nTargetPort = 0, LPCTSTR lpszTargetAddress = NULL);
	void CloseSender(void);

	// 接收数据
	int ReceiveFrom(LPVOID lpBuf, int nBufLen, CString& rSocketAddress, UINT& rSocketPort, int nFlags = 0);
	int ReceiveFrom(LPVOID lpBuf, int nBufLen, SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags = 0);
	int	ReceiveFrom(LPVOID lpBuf, int nBufLen);

	// 发送数据
	int SendTo(const LPVOID lpBuf, int nBufLen, UINT nTargetPort, LPCTSTR lpszTargetAddress, int nFlags = 0);
	int SendTo(const LPVOID lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags = 0);
	int SendTo(const LPVOID lpBuf, int nBufLen);

	// 启动/停止数据接收线程
	BOOL StartReceiveThread(void);
	void StopReceiveThread(void);

protected:
	// 数据接收线程
	static UINT ReceiveProc(LPVOID lpParam);
	void ReceiveLoop(void);	

	// CString转换为char*
	int CStringToPSZ(CString str, char* ptr);

public:
	// CString类型的IP转换为DWORD类型的IP
	DWORD CStringToDWordIP(CString strIP);

	// DWORD类型的IP转换为CString类型的IP
	CString DWordToCStringIP(DWORD dwIP);

	// 获取主机地址
	void GetHostAddr(CString& strIP);
};

