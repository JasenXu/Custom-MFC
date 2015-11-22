#pragma once
#include "afxtempl.h"
#include "NetMediaDef.h"

#define BUF_COUNT		6
#define DATA_TYPE		NETMEDIA_PACK
#define DATA_TYPE_PTR	DATA_TYPE*

class CDualListBuffer
{
public:
	CDualListBuffer(void);
	virtual ~CDualListBuffer(void);

public:
	HANDLE		m_hBufEnough;

protected:
	CMutex*		m_pMutex;
	BOOL		m_bFlushing;

	CTypedPtrList<CPtrList, DATA_TYPE_PTR>		m_listData;
	CTypedPtrList<CPtrList, DATA_TYPE_PTR>		m_listPool;

public:
	void SetFlushing(BOOL bFlush = TRUE);
	BOOL IsFlushing(void);

	int ResetList(void);
	int GetListSize(void);

	DATA_TYPE_PTR GetDataBuf(void);
	int ReleaseDataBuf(DATA_TYPE_PTR pDataBuf);

	DATA_TYPE_PTR GetWriteBuf(void);
	int ReleaseWriteBuf(DATA_TYPE_PTR pDataBuf);

	DATA_TYPE_PTR PointToDataHead(void);
	int RollBackDataHead(DATA_TYPE_PTR pDataBuf);

	DWORD WaitForNext(DWORD dwTimeOut);
};

