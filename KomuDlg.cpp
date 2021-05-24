
// KomuDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Komu.h"
#include "KomuDlg.h"
#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h> // memset()
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKomuDlg dialog


IMPLEMENT_DYNAMIC(CKomuDlg, CDialogEx)

CKomuDlg::CKomuDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KOMU_DIALOG, pParent)
{		
	m_sServer = NULL;	
	m_strHistoryMsg = "";
	m_nServerPort = -1;
	DWORD m_ThreadId = NULL;
	bSocketLive = TRUE;
}
CKomuDlg::~CKomuDlg()
{	
	delete[] m_pict;	
	CloseHandle(hTread);
}
void CKomuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HISTORY, m_editHistory);
	DDX_Control(pDX, IDC_EDIT_SEND, m_editSend);	
	DDX_Control(pDX, IDC_TREE_SOCKET, m_tree_ctrl);
}

BEGIN_MESSAGE_MAP(CKomuDlg, CDialogEx)
	ON_WM_PAINT()
	ON_MESSAGE(WM_PB_FIND_CLIENT,  OnFindClients)
	ON_MESSAGE(WM_PB_SHOWDATAPORT, OnShowPort_Name)
	ON_MESSAGE(WM_PB_ADD_NEW_USER, OnAddNewUser)
	ON_MESSAGE(WM_PB_USER_BITMAP,  OnUserBitMap)
	ON_MESSAGE(WM_PB_RECEIVED,     OnPacketReceived)
	ON_MESSAGE(WM_PB_REMOVE,       OnRemoveClient)
	ON_BN_CLICKED(ID_BUTTON_SEND, &CKomuDlg::OnBnClickedSend)
	ON_WM_CLOSE() 

END_MESSAGE_MAP()



// CKomuDlg message handlers

BOOL CKomuDlg::OnInitDialog()
{

	CDialogEx::OnInitDialog();
	SetWindowText(L"Messanger          Login : "+m_strLogin);
	hTread = CreateThread(NULL, 0, StartServerUDP, (LPVOID)this, 0, &m_ThreadId);
	m_imageList.Create(26,26, ILC_COLOR24, 1, 0);
	m_tree_ctrl.SetImageList(&m_imageList, 0);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKomuDlg::CreateAvatarImage(CPixelCOLORREF cColor, CImage &image)
{
	
	image.Create(cColor.nWidth, cColor.nHeight, cColor.nBPP);

	int bajtsPerPixel = 4;
	UINT size = cColor.nHeight * cColor.nWidth * bajtsPerPixel;
	int pos = 0;
	int sizepx = cColor.nHeight * cColor.nWidth;
	m_pict = new COLORREF[sizepx];
	int max = 0;
	for (int x = 0; x < cColor.nWidth; x++)
	{
		for (int y = 0; y < cColor.nHeight; y++)
		{			
			image.SetPixel(x, y, cColor.bData[x * cColor.nHeight + y]);
			m_pict[x * cColor.nHeight + y] = cColor.bData[x * cColor.nHeight + y];
		}
	}
	delete[] cColor.bData;
}


void CKomuDlg::SetAvatar_Login(CPixelCOLORREF cColor, CString strLogin)
{	
	m_cAvatar = cColor;
	CreateAvatarImage(m_cAvatar,m_image);	
	m_strLogin = strLogin;
}
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKomuDlg::OnPaint()
{	

	CPaintDC dc(this); // device context for painting
	CWnd* d = GetDlgItem(IDC_STATIC_AVATAR2);
	CDC *cdc = d->GetDC();
	
	if (m_image != NULL)
	{
		CRect cr;
		d->GetClientRect(cr);
		int h = m_image.GetHeight();
		int u = m_image.GetHeight();
		cdc->SetStretchBltMode(STRETCH_HALFTONE);
		m_image.Draw(cdc->m_hDC, 0, 0, cr.Width(), cr.Height(), 0, 0,
			m_image.GetWidth(), m_image.GetHeight());
		
	
	}
	CDialogEx::OnPaint();	
}


void CKomuDlg::ShowMessage(CString str)
{
	MessageBox(str);
}

DWORD CKomuDlg::StartClientUDP()
{
	
	strcpy_s(m_szServerName, "127.0.0.1");
	m_sServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sServer == INVALID_SOCKET)
	{
		CString sError;
		DWORD error = GetLastError();
		sError.Format(L"Błąd  socket -> %d", GetLastError());
		ShowMessage(sError);
		return error;
	}

	char szMessage[1024];
	CString strMess;
	strMess.Format(L"SeRveR_P0RT?=%d", m_nServerPort);
	int nlength = strMess.GetLength();
	strcpy_s(szMessage, CT2A(strMess));

	m_serveraddres.sin_family = AF_INET;

	for (int nPort = 6000; nPort < 10000; nPort++)
	{
		if (nPort == m_nServerPort)
			continue;
		m_serveraddres.sin_port = htons(nPort);
		int u = inet_pton(AF_INET, m_szServerName, &m_serveraddres.sin_addr.s_addr);
		
		if (u < 0)
		{			
			CString sError;			
			sError.Format(L"Błąd  socket -> %d", GetLastError());
			ShowMessage(sError);			
		}
		
		socklen_t len = sizeof(m_serveraddres);
		if (sendto(m_sServer, szMessage, nlength, 0, (struct sockaddr*)&m_serveraddres, len) < 0)
		{			
			CString sError;
			DWORD error = GetLastError();
			sError.Format(L"Błąd  send -> %d", GetLastError());
			ShowMessage(sError);
			return error;
		}				
	}	
	return 1;		
}

BOOL CKomuDlg::SendMessageToClientUDP(int nPort, CString strMessage, CString strLogin)
{
	char szMessage[1024];
		
	CString strUserMessage = (strLogin.IsEmpty() ? L"" :strLogin + L" : ") + strMessage;
	int nlength = strUserMessage.GetLength();
	strcpy_s(szMessage, CT2A(strUserMessage));

	m_serveraddres.sin_port = htons(nPort);
	int u = inet_pton(AF_INET, m_szServerName, &m_serveraddres.sin_addr.s_addr);

	if (u < 0)
	{
		CString sError;		
		sError.Format(L"Błąd  socket -> %d", GetLastError());
		ShowMessage(sError);
	}

	socklen_t len = sizeof(m_serveraddres);
	if (sendto(m_sServer, szMessage, nlength, 0, (struct sockaddr*)&m_serveraddres, len) < 0)
	{
		CString sError;
		DWORD error = GetLastError();
		sError.Format(L"Błąd  send -> %d", GetLastError());
		ShowMessage(sError);
		return error;
	}
	else if(strLogin.IsEmpty() == FALSE)
	{		
		AddToHistory(L"Ty : "+strMessage);
	}
	return TRUE;
}

BOOL CKomuDlg::SendMessageToClientUDP(int nPort, char *strMessage, int nlength)
{
		
	//int nlength = sizeof(strMessage);
	
	m_serveraddres.sin_port = htons(nPort);
	int u = inet_pton(AF_INET, m_szServerName, &m_serveraddres.sin_addr.s_addr);

	if (u < 0)
	{
		CString sError;
		sError.Format(L"Błąd  socket -> %d", GetLastError());
		ShowMessage(sError);
	}

	socklen_t len = sizeof(m_serveraddres);
	if (sendto(m_sServer, strMessage, nlength, 0, (struct sockaddr*)&m_serveraddres, len) < 0)
	{
		CString sError;
		DWORD error = GetLastError();
		sError.Format(L"Błąd  send -> %d", GetLastError());
		ShowMessage(sError);
		return error;
	}
	
	return TRUE;
}

DWORD WINAPI CKomuDlg::StartServerUDP(LPVOID pDlg)
{
	CKomuDlg* dlg = reinterpret_cast<CKomuDlg*>(pDlg);
	SOCKET sServerListen;
	sockaddr_in localaddres, clientaddres;
	
	sServerListen = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sServerListen == INVALID_SOCKET)
	{
		CString sError;
		DWORD error = GetLastError();
		sError.Format(L"Błąd  socket -> %d", GetLastError());
		//dlg->ShowMessage(sError);
		return error;
	}
		
	localaddres.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddres.sin_family = AF_INET;
	int nPort = 6000;
	for (; nPort < 10000; nPort++)
	{
		localaddres.sin_port = htons(nPort);
		if (bind(sServerListen, (struct sockaddr*)&localaddres, sizeof(localaddres)) == SOCKET_ERROR)
		{
			continue;
		}
		else
		{			
			::PostMessage(dlg->GetSafeHwnd(), WM_PB_FIND_CLIENT, nPort, 0);
			break;
		}
	}
	
	char buf[1024];
	char pom;
	char* graph = &pom;
	
	std::vector<BitMapData> vecBitmap;
	
	socklen_t len = sizeof(clientaddres);
		
	while (1)
	{
		int ret = recvfrom(sServerListen, buf, 1024, 0, (struct sockaddr*)&clientaddres, &len);
		if(ret < 1024)
			buf[ret] = 0;
		
		CString str = CA2T(buf);
		
		if (str.Find(L"SeRveR") > -1)
		{
			int posSer = str.Find(L"SeRveR_P0RT?=");
			if (posSer == 0)
			{
				CString strNum = str.Mid(13, 4);
				int nPortRec = _wtoi(strNum);
				::PostMessage(dlg->GetSafeHwnd(), WM_PB_SHOWDATAPORT, nPortRec, 0);				
			}
			posSer = str.Find(L"SeRveR_DaTa:");
			if (posSer == 0)
			{
				CString strNum = str.Mid(12, 4);
				int nPortRec = _wtoi(strNum);
				CString strName = str.Mid(17, ret - 17);
				TCHAR* chName = _tcsdup(strName);
				::PostMessage(dlg->GetSafeHwnd(), WM_PB_ADD_NEW_USER, nPortRec, (LPARAM)chName);				
			}
			posSer = str.Find(L"SeRveR_RemOve:");
			if (posSer == 0)
			{
				
				CString strNum = str.Mid(14, 4);
				int nPortRec = _wtoi(strNum);
				if (nPortRec == nPort)
				{
					closesocket(sServerListen);
					return 0;
				}
				::PostMessage(dlg->GetSafeHwnd(), WM_PB_REMOVE, nPortRec, 0);			
			}
			posSer = str.Find(L"SeRveR_BID:");
			if (posSer == 0)
			{
				
				CString strNum = str.Mid(11, 4);
				int nPortRec = _wtoi(strNum);
								
				byte byt[21];
				int a = 0;
				
				memcpy(byt, (buf+15) ,21);
				int nPakietMax = BufferToInt(byt,0);
				
				int nMaxSize =  BufferToInt(byt, 4);
												
				portData pData;
				pData.nAllPackiet = nPakietMax;
				pData.port = nPortRec;
				pData.nCurrSize = 0;
				pData.nMaxSize = nMaxSize;	
				pData.nRecvPakiet = 0;
				pData.data = new byte[nMaxSize];
								
				pData.nHeight = BufferToInt(byt, 8);					
				
				pData.nWidth = BufferToInt(byt, 12);
								
				pData.nBPP = byt[16];
				
				pData.nPitch = BufferToInt(byt, 17);

				vecBitmap.push_back(pData);
			}
			posSer = str.Find(L"SeRveR_BMP:");
			if (posSer == 0)
			{
				CString strNum = str.Mid(11, 4);
				int nPortRec = _wtoi(strNum);
				
				byte byt[1009];				
				memcpy(byt, (buf + 15) , ret - 15);
				int nPakiet = BufferToInt(byt, 0);

				for (UINT nCurr = 0; nCurr <  vecBitmap.size(); nCurr++)
				{
					if (vecBitmap[nCurr].port == nPortRec)
					{
						
						if (vecBitmap[nCurr].nRecvPakiet + 1 < nPakiet)
						{
							dlg->AddToHistory(L"Zgubiono pakiet");
							break;						
							
						}
						int sizePacket = ret - 19;
										

						memcpy(vecBitmap[nCurr].data+ vecBitmap[nCurr].nCurrSize, byt+4, sizePacket);
												
						vecBitmap[nCurr].nRecvPakiet++;
						vecBitmap[nCurr].nCurrSize += sizePacket;
												
						if (vecBitmap[nCurr].nCurrSize == vecBitmap[nCurr].nMaxSize)
						{						
							::PostMessage(dlg->GetSafeHwnd(), WM_PB_USER_BITMAP, nPortRec, (LPARAM)&vecBitmap[nCurr]);													
						}
						// TODO Dorobić funkcjonalność wysyłania potwierdzeń przyjęcia!!!
						break;
					}					
				}								
			}
			posSer = str.Find(L"SeRveR_LoadImg:");
			if (posSer == 0)
			{
				CString strNum = str.Mid(15, 4);
				int nPortRec = _wtoi(strNum);
				for (UINT nCurr = 0; nCurr < vecBitmap.size(); nCurr++)
				{
					if (vecBitmap[nCurr].port == nPortRec)
					{
						delete[] vecBitmap[nCurr].data;
						vecBitmap.erase(vecBitmap.begin() + nCurr);
						break;
					}
				}				
			}
		}
		else
		{
			dlg->AddToHistory(str);			
		}		
	}
	closesocket(sServerListen);
}

void CKomuDlg::OnBnClickedSend()
{	
	CString str;
	m_editSend.GetWindowText(str);
	
	HTREEITEM hSel = m_tree_ctrl.GetSelectedItem();

	if (hSel == NULL)
	{
		MessageBox(L"Zaznacz do kogo chcesz wysłać");
		return;
	}
	int port = m_tree_ctrl.GetItemData(hSel);
	SendMessageToClientUDP(port, str, m_strLogin);		
}
LRESULT CKomuDlg::OnUserBitMap(WPARAM wParam, LPARAM lParam)
{
	int nParam = wParam;
	BitMapData vecBitmap = *(BitMapData*)lParam;
	CImage imgUser;
	imgUser.Create(vecBitmap.nWidth, vecBitmap.nHeight, vecBitmap.nBPP);
	
	COLORREF* color = (COLORREF*)vecBitmap.data;
	for (int x = 0; x < vecBitmap.nWidth; x++)
	{
		for (int y = 0; y < vecBitmap.nHeight; y++)
		{		
			imgUser.SetPixel(x, y, *(color + (x * vecBitmap.nHeight + y)));						
		}
	}
	
	CBitmap* Bmp = CBitmap::FromHandle(imgUser);
	int nImage = m_imageList.Add(Bmp, RGB(0, 0, 0));
		
	HTREEITEM hTree = m_tree_ctrl.GetChildItem(TVI_ROOT);

	while (hTree != NULL)
	{
		if (nParam == m_tree_ctrl.GetItemData(hTree))
		{
			BOOL is = m_tree_ctrl.SetItemImage(hTree, nImage, nImage);
			RedrawWindow();
			break;
		}
		hTree = m_tree_ctrl.GetNextItem(hTree, TVGN_NEXT);
	}
	CString message;
	message.Format(L"SeRveR_LoadImg:%d", nParam);
	SendMessageToClientUDP(m_nServerPort, message, L"");
		
	return 1;
}
LRESULT CKomuDlg::OnAddNewUser(WPARAM wParam, LPARAM lParam)
{
	TCHAR* pName = (TCHAR*)lParam;
	int nParam = wParam;
	CString strName = pName;

	HTREEITEM hTree = m_tree_ctrl.GetChildItem(TVI_ROOT);
	BOOL isFound = FALSE;
	while (hTree != NULL)
	{		
		if (strName == m_tree_ctrl.GetItemText(hTree))
		{
			isFound = TRUE;
			break;
		}
		hTree = m_tree_ctrl.GetNextItem(hTree, TVGN_NEXT);
	}
		
	if (isFound == FALSE)
	{
		HTREEITEM hNew = m_tree_ctrl.InsertItem(strName, TVI_ROOT);
		m_tree_ctrl.SetItemData(hNew, nParam);
		
		CString message;
		message.Format(L"SeRveR_DaTa:%d,%s", m_nServerPort, m_strLogin);
		SendMessageToClientUDP(nParam, message,L"");
		
		char cr[1024];
		message.Format(L"SeRveR_BID:%d", m_nServerPort);
		strcpy_s(cr, CT2A(message));

		int maxDataSend = 1005;
		int nHeight = m_cAvatar.nHeight;
		int nWidth = m_cAvatar.nWidth;
		int nBPP = m_cAvatar.nBPP;
		int nPitch = m_cAvatar.nPitch;
		int size = nHeight * nWidth * 4;
		int portion = (int)ceil(float(size) / maxDataSend);

		IntToString(cr, portion,15);
		IntToString(cr, size,19);
		IntToString(cr, nHeight,23);
		IntToString(cr, nWidth,27);
		cr[31] =  nBPP;
		IntToString(cr, nPitch,32);
		cr[37] = '\0';
		SendMessageToClientUDP(nParam, cr, 37);
				
		message.Format(L"SeRveR_BMP:%d", m_nServerPort);
		strcpy_s(cr, CT2A(message));
		int sendByte = 0;
		
		for (int p = 1; p <= portion; p++)
		{
			IntToString(cr,p,15);

			if(p  == portion)
			{
				maxDataSend = size % maxDataSend;
			}

			memcpy((cr+19), ((byte*)m_pict)+ sendByte, maxDataSend);
			SendMessageToClientUDP(nParam,cr, maxDataSend + 19);
			sendByte += maxDataSend;
			
			Sleep(5);//TODO   Zmienić na sprawdzanie potwierdzenia przyjęcia!!!!
		}			
		
	}
	delete[] pName;
	return 0;
}
LRESULT CKomuDlg::OnShowPort_Name(WPARAM wParam, LPARAM lParam)
{
	 int nParam = wParam;
	 CString message;
	 message.Format(L"SeRveR_DaTa:%d,%s", m_nServerPort, m_strLogin);
	 SendMessageToClientUDP(nParam,message,L"");
	 return 0;
}

LRESULT CKomuDlg::OnFindClients(WPARAM wParam, LPARAM lParam)
{
	m_nServerPort = wParam;
	StartClientUDP();
	return 1;
}

LRESULT CKomuDlg::OnPacketReceived(WPARAM wParam, LPARAM lParam)
{	
	AddToHistory(L"Przyszedł");
	return 1;
}

void CKomuDlg::AddToHistory(CString tekst)
{	
	m_strHistoryMsg += tekst;
	m_strHistoryMsg += L"\r\n";
	m_editHistory.SetWindowText(m_strHistoryMsg);
}


LRESULT CKomuDlg::OnRemoveClient(WPARAM wParam, LPARAM lParam)
{
	int NSocketDel = wParam;

	HTREEITEM hTree = m_tree_ctrl.GetChildItem(TVI_ROOT);

	while (hTree != NULL)
	{
		if (NSocketDel == m_tree_ctrl.GetItemData(hTree))
		{
			m_tree_ctrl.DeleteItem(hTree);
			return 1;
		}
		hTree = m_tree_ctrl.GetNextItem(hTree, TVGN_NEXT);
	}
	
	return 0;
}

void CKomuDlg::OnClose()
{
	CString message;
	message.Format(L"SeRveR_RemOve:%d", m_nServerPort);

	HTREEITEM hTree = m_tree_ctrl.GetChildItem(TVI_ROOT);
	
	while (hTree != NULL)
	{
		SendMessageToClientUDP(m_tree_ctrl.GetItemData(hTree), message, L"");		
		hTree = m_tree_ctrl.GetNextItem(hTree, TVGN_NEXT);
	}
		
	SendMessageToClientUDP(m_nServerPort, message, L"");
	shutdown(m_sServer, 2);
	closesocket(m_sServer);
	CDialogEx::OnClose();
}





















/*

DWORD WINAPI CKomuDlg::StartServerUDP2(LPVOID pDlg)
{
	CKomuDlg* dlg = reinterpret_cast<CKomuDlg*>(pDlg);
	SOCKET sServerListen;
	SOCKET ClientSocket[10];
	int TotalSocket = 0;

	sockaddr_in localaddres, clientaddres;
	int size;

	sServerListen = socket(AF_INET, SOCK_STREAM, 0);
	if (sServerListen == SOCKET_ERROR)
	{
		dlg->ShowMessage(L"Brak biblioteki WinSock");
		return FALSE;
	}
	ULONG ulBlock = 1;

	if (ioctlsocket(sServerListen, FIONBIO, &ulBlock) == SOCKET_ERROR)
	{
		return false;
	}
	localaddres.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddres.sin_family = AF_INET;
	localaddres.sin_port = htons(6000);
	if (bind(sServerListen, (struct sockaddr*)&localaddres, sizeof(localaddres)) == SOCKET_ERROR)
	{
		dlg->ShowMessage(L"Czyżby server juz istniał");
		return FALSE;
	}
	else
	{
		listen(sServerListen, 4);
	}
	FD_SET ReadSet;
	int ReadySocket;

	enum stat {
		RECV_COMM = 1,
		SEND_DATA = 2,

	};

	while (1)
	{


		FD_ZERO(&ReadSet);
		FD_SET(sServerListen, &ReadSet);
		for (int i = 0; i < TotalSocket; i++)
		{
			if (ClientSocket[i] != INVALID_SOCKET)
			{
				FD_SET(ClientSocket[i], &ReadSet);
			}
		}
		if ((ReadySocket = select(0, &ReadSet, NULL, NULL, NULL)) == SOCKET_ERROR)
		{
			dlg->ShowMessage(L"Error select");
		}

		//Nowe połączeenie
		if (FD_ISSET(sServerListen, &ReadSet))
		{
			size = sizeof(clientaddres);
			ClientSocket[TotalSocket] =
				accept(sServerListen, (struct sockaddr*)&clientaddres, &size);
			if (ClientSocket[TotalSocket] == INVALID_SOCKET)
			{
				break;
			}
			TotalSocket++;
		}

		//Przyszły dane

		for (int i = 0; i < TotalSocket; i++)
		{
			if (ClientSocket[TotalSocket] == INVALID_SOCKET)
			{
				continue;
			}
			//Nowe połączeenie
			if (FD_ISSET(ClientSocket[i], &ReadSet))
			{
				char szRecvBuff[1024];
				char szSendBuff[1024];


				int ret = recv(ClientSocket[i], szRecvBuff, 1024, 0);
				if (ret == 0)
				{
					closesocket(ClientSocket[i]);
					ClientSocket[i] = INVALID_SOCKET;
					break;
				}
				else if (ret == SOCKET_ERROR)
				{
					dlg->ShowMessage(L"Błąd odbioru danych");
					break;
				}
				else
				{
					CString str = CString(szRecvBuff);
					dlg->AddToHistory(str);
				}

				if (dlg->IsSomeToSend())
				{
					CString strMessage = dlg->GetStringToSend();
					BYTE* buffer = (BYTE*)strMessage.GetBuffer();
					memcpy(szSendBuff, buffer, strMessage.GetLength());

					ret = send(ClientSocket[i], szSendBuff, sizeof(szSendBuff), 0);
					dlg->SetSomeToSend(FALSE);
					if (ret == SOCKET_ERROR)
					{
						dlg->AddToHistory(L"Error");
						break;
					}
					dlg->AddToHistory(strMessage);
				}

			}
		}
	}
	closesocket(sServerListen);
	return TRUE;
}*/


