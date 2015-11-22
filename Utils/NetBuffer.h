#pragma once

// 数据存储格式
typedef struct tagNetBuffer
{
	BOOL	bValid;			// 数据有效位
// 	USHORT	usType;			// 数据类别
// 	USHORT	usChannel;		// 数据通道
	UINT	nDataLen;		// 数据长度
	UCHAR	pBuf;			// 数据内容
}NETBUFFER, *PNETBUFFER;

// CNetBuffer

class CNetBuffer
{

public:
	CNetBuffer();
	virtual ~CNetBuffer();

public:
	// ===========================================================================================
	// Purpose:数组初始化
	// Para1:缓冲区大小
	// Para2:数组成员数
	// ===========================================================================================
	void Init(UINT nBufSize, UINT nBufCount);

	// ===========================================================================================
	// Purpose:向数组添加数据
	// Para1:数据首地址
	// Para2:数据长度
	// Return:成功，返回数据成员下标；失败，返回-1表示添加的数据过长，返回-2表示数组成员无空闲
	// ===========================================================================================
	int AddData(PUCHAR pBuf, UINT nLen);

	// ===========================================================================================
	// Purpose:在数组的某个成员尾部添加数据
	// Para1:数据首地址
	// Para2:数据长度
	// Para3:数组成员下标
	// Return:成功，返回数据成员下标；返回-1表示添加的数据过长，失败，返回-2表示不存在指定下标的数组成员
	// ===========================================================================================
	int AddDataAt(PUCHAR pBuf, UINT nLen, int nIndex);

	// ===========================================================================================
	// Purpose:获取数组成员数
	// ===========================================================================================
	UINT GetCount(void) const;

	// ===========================================================================================
	// Purpose:读取数据
	// Para1:数据的缓冲首地址，失败时，为nullptr
	// Para2:数据的数据长度，失败时，为0
	// ===========================================================================================
	void GetData(__out PUCHAR& pBuf, __out UINT& nLen);

	// ===========================================================================================
	// Purpose:删除当前读取的数据
	// ===========================================================================================
	void DeleteData(void);

protected:
	CArray<PUCHAR>	m_arBuf;		// 数据缓冲数组，采用NETBUFFER格式存储数据
	UINT			m_nCount;		// 数组成员数
	UINT			m_nBufSize;		// 缓冲区大小
	int				m_nWriteIndex;	// 当前写入的数据成员下标
	int				m_nReadIndex;	// 当前读取的数组成员下标
};


