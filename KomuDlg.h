
// KomuDlg.h : header file
//
#include "CPixelCOLORREF.h"
#pragma once

#define WM_PB_FIND_CLIENT	WM_USER + 1
#define WM_PB_EXCEPTION		WM_USER + 2
#define WM_PB_LOGRECVPACKET WM_USER + 3
#define WM_PB_LOGSENDPACKET WM_USER + 4
#define WM_PB_SHOWDATAPORT  WM_USER + 5
#define WM_PB_ADD_NEW_USER  WM_USER + 6
#define WM_PB_RECEIVED		WM_USER + 7
#define WM_PB_REMOVE		WM_USER + 8
#define WM_PB_USER_BITMAP   WM_USER + 9
// CKomuDlg dialog
class CKomuDlg : public CDialogEx 
{
	DECLARE_DYNAMIC(CKomuDlg)
// Construction
public:
	CKomuDlg(CWnd* pParent = nullptr);	// standard constructor
	virtual ~CKomuDlg();
	void SetAvatar_Login(CPixelCOLORREF cColor, CString strLogin);
	static CString m_strin;

static DWORD WINAPI StartServerUDP(LPVOID pDlg);
DWORD StartClientUDP();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KOMU_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
			
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
	
	BOOL bSocketLive;
	BOOL SendMessageToClientUDP(int nPort, CString strMessage, CString strLogin);
	BOOL SendMessageToClientUDP(int nPort, char* strMessage, int nLength);
	
	
private:
	sockaddr_in m_serveraddres;
	char m_szServerName[512] = { 0 };
	SOCKET m_sServer;
	CEdit m_editHistory;
	CEdit m_editSend;
	CString m_strHistoryMsg;
	int m_nServerPort;
	DWORD m_ThreadId;

	COLORREF* m_pict;
	HANDLE hTread;
	CImage m_image;

	CString m_strLogin;
	CImageList m_imageList;
	CPixelCOLORREF m_cAvatar;

	afx_msg LRESULT OnPacketReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowPort_Name(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddNewUser(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFindClients(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRemoveClient(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserBitMap(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedSend();

	void CreateAvatarImage(CPixelCOLORREF cColorCreate, CImage& image);
	void AddToHistory(CString teks);
	void ShowMessage(CString str);

	static int BufferToInt(BYTE * strInt, int nStart)
	{	

		return  (*(strInt + nStart) << 24) + 
			(*(strInt + nStart +1) << 16) + 
			(*(strInt + nStart+2) << 8) + 
			*(strInt + nStart+3);
	}
	static void IntToString(char *chadd, int nString, int nPosition)
	{	
		*(chadd+ nPosition) = (nString>>24) & 0xFF;
		*(chadd + nPosition + 1) =  (nString >>16) &0xFF;
		*(chadd + nPosition + 2) =  (nString >> 8) & 0xFF;
		*(chadd + nPosition +3) = nString &0xFF;
		
		//CString sInt = CString(a);	
		
		//sInt.Format(L"%s", a);
		return ;
	}

	typedef struct portData
	{
		int port;
		byte* data;
		int nHeight, nWidth;
		int nBPP;
		int nPitch;		
		int nRecvPakiet;
		int nAllPackiet;
		int nCurrSize;
		int nMaxSize;
	} BitMapData;

public:
	
	CTreeCtrl m_tree_ctrl;

};
