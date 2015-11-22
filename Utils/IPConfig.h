#pragma once

//未公开函数DhcpNotifyConfigChange位于 dhcpcsvc.dll中.  
//原型声明 
typedef BOOL ( CALLBACK* DHCPNOTIFYPROC) ( 
	LPWSTR lpwszServerName,			// 本地机器为NULL 
	LPWSTR lpwszAdapterName,		// 适配器名称 
	BOOL bNewIpAddress,				// TRUE表示更改IP 
	DWORD dwIpIndex,				// 指明第几个IP地址，如果只有该接口只有一个IP地址则为0 
	DWORD dwIpAddress,				// IP地址 
	DWORD dwSubNetMask,				// 子网掩码 
	int nDhcpAction					// 对DHCP的操作 0:不修改, 1:启用 DHCP，2:禁用 DHCP 
	);  

class CIPConfig
{
public:
	CIPConfig(void);
	virtual ~CIPConfig(void);

	// 读取注册表取得适配器名称
	void GetLanAdapterName(char * pszAdapterName);

	// 注册IP地址
	BOOL RegisterIPAddress(LPCSTR pszAdapterName, LPCSTR pszIPAddress, LPCSTR pszNetMask, LPCSTR pszGateway = NULL, LPCSTR pszDNSServer1 = NULL, LPCSTR pszDNSServer2 = NULL);
	
	BOOL NotifyIPChange(LPCSTR pszAdapterName, int nIndex, LPCSTR pszIPAddress, LPCSTR pszNetMask);
};

