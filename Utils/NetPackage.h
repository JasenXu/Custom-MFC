#pragma once
#include "NetBuffer.h"
#include "afxwin.h"
#include "afxmt.h"

#define WM_NET_RESOLVE	 (WM_USER+3000)
// CNetPackage

class CNetPackage
{

public:
	CNetPackage();
	virtual ~CNetPackage();

public:
	void Init(void);
	int CopyData(char* pBuf, UINT nLen);
	void SetDataType(USHORT usType);
	USHORT GetDataType(void);
	void SetDataChannel(USHORT usChannel);
	USHORT GetDataChannel(void);
	void SetWorkFlag(BOOL bWork);
	void SetCallBack(int nCallBack);

protected:
	void FindHeader(void);
	BOOL IsHeader(void);
	BOOL IsHeader(UINT nStartPos);

	static UINT PackageProc(LPVOID lpParam);
	void PackageLoop(void);

	void LockWrite(void);
	void UnLockWrite(void);
	void LockRead(void);
	void UnLockRead(void);
	BOOL WriteBuffer(UINT nStartPos, UINT nLen);

public:
	void ReadBuffer(__out PUCHAR& pBuf, __out UINT& nLen);
	void ReleaseBuffer(void);

protected:
	CCriticalSection	m_ccs;

	USHORT			m_usDataType;
	USHORT			m_usDataChannel;

	CNetBuffer*		m_pNetBuf;
	BYTE*			m_pBuf;
	UINT			m_nBufSize;
	UINT			m_nLen;

	COleDateTime	m_oleLastRecvTime;
	char			m_chFrameFlag[FRAME_FLAG_LEN];
	UINT			m_nWritePos;
	UINT			m_nReadPos;

	BOOL			m_bFindHeader;
	UINT			m_nFrameLen;

	HANDLE			m_hCLose;
	CWinThread*		m_hPackageThread;
	BOOL			m_bWorkFlag;
	int				m_nCallBack;
};


