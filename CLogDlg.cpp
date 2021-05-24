// CLogDlg.cpp : implementation file
//

#include "pch.h"
#include "Komu.h"
#include "CLogDlg.h"
#include "afxdialogex.h"


// CLogDlg dialog

IMPLEMENT_DYNAMIC(CLogDlg, CDialogEx)

CLogDlg::CLogDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LOG, pParent)
{
	
}

CLogDlg::~CLogDlg()
{
	
}

void CLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE_CTRL, m_Icon_Avatar);

}


BEGIN_MESSAGE_MAP(CLogDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_GET_FILE, &CLogDlg::OnBnClickedButtonGetFile)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CLogDlg::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL CLogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLogDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	if (m_image != NULL)
	{			
		CRect rc;
		m_Icon_Avatar.GetClientRect(&rc);		
		CDC* pDC = m_Icon_Avatar.GetWindowDC();

		
		// The next two lines test the image on a picture control.
		// The output has wrong color palette, like my initial version
		//HDC hdcpic = ::GetDC(m_Icon_Avatar.m_hWnd);

		//img.StretchBlt(hdcpic, 0, 0, rc.Width(), rc.Height(), SRCCOPY);

		//sm_img.Save(L"C:\\Users\\IreneuszJe\\Documents\\NOWYYY2.bmp");
		pDC->SetStretchBltMode(STRETCH_HALFTONE);
		m_image.Draw(pDC->m_hDC, 0, 0, rc.Width(), rc.Height(), 0, 0,
			m_image.GetWidth(), m_image.GetHeight());
		
			CImage sm_img;
			sm_img.Create(rc.Width(), rc.Height(), m_image.GetBPP());
			HDC newDc = CreateCompatibleDC(pDC->m_hDC);
			HBITMAP h = sm_img.Detach();			
			HBITMAP hold = (HBITMAP)SelectObject(newDc, h);	
			BitBlt(newDc, 0, 0, rc.Width(), rc.Height(), pDC->m_hDC, 0, 0, SRCCOPY);
			sm_img.Attach((HBITMAP)SelectObject(newDc, hold));
			sm_img.Detach();
			sm_img.Destroy();
			DeleteDC(newDc);


			GetDlgItem(IDC_STATIC_AVATAR)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_AVATAR)->GetClientRect(&rc);
			 pDC = GetDlgItem(IDC_STATIC_AVATAR)->GetWindowDC();
			 pDC->SetStretchBltMode(STRETCH_HALFTONE);
		m_image.Draw(pDC->m_hDC, 0, 0, rc.Width(), rc.Height(), 0, 0,
			m_image.GetWidth(), m_image.GetHeight());
			CImage sm_img2;
			sm_img2.Create(rc.Width(), rc.Height(), m_image.GetBPP());
			newDc = CreateCompatibleDC(pDC->m_hDC);
			h = sm_img2.Detach();
			hold = (HBITMAP)SelectObject(newDc, h);
			BitBlt(newDc, 0, 0, rc.Width(), rc.Height(), pDC->m_hDC, 0, 0, SRCCOPY);
			sm_img2.Attach((HBITMAP)SelectObject(newDc, hold));

			m_pixels.nHeight = sm_img2.GetHeight();
			m_pixels.nWidth = sm_img2.GetWidth();
			m_pixels.nBPP = sm_img2.GetBPP();
			m_pixels.nPitch = sm_img2.GetPitch();
			if(m_pixels.bData != NULL)
				delete[]  m_pixels.bData;
			m_pixels.bData = new COLORREF[m_pixels.nHeight * m_pixels.nWidth];
			for (int x = 0; x < m_pixels.nWidth; x++)
			{
				for (int y = 0; y < m_pixels.nHeight; y++)
				{
					COLORREF r = sm_img2.GetPixel(x, y);
					m_pixels.bData[x* m_pixels.nHeight + y] = r;
				}

			}
			GetDlgItem(IDC_STATIC_AVATAR)->ShowWindow(FALSE);
			sm_img2.Detach();
			sm_img2.Destroy();
			DeleteDC(newDc);
			
	}
	CDialogEx::OnPaint();
}

void CLogDlg::OnBnClickedButtonGetFile()
{
	OPENFILENAME ofn;
	WCHAR sNazwaPliku[MAX_PATH] = L"";

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = L"Pliki graficzne (*.png)\0*.png\0Pliki graficzne (*.bmp)\0*.bmp\0Pliki graficzne (*.jpg)\0*.jpg\0";
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFile = sNazwaPliku;
	ofn.lpstrDefExt = L"txt";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	if (GetOpenFileName(&ofn))
	{
		if (m_image != NULL)
		{
			m_image.Detach();
		}
		HRESULT res  = m_image.Load(ofn.lpstrFile);
		
		if (res == S_OK)
		{			
			RedrawWindow();
			
		}		
		else
		{
			::AfxMessageBox(L"Nie mo¿na za³adowaæ Avatara");
			return;
		}
	}
}

CPixelCOLORREF CLogDlg::GetAvatar_Login(CString& strLogin)
{			
	strLogin = m_strLogin;
	return m_pixels;
}

void CLogDlg::OnBnClickedOk()
{
	CString strLog;
    GetDlgItemText(IDC_EDIT_LOGIN, strLog);
	if (strLog.IsEmpty())
	{
		::AfxMessageBox(L"Prosze podaæ login");
		return;
	}
	if (m_image == NULL)
	{
		::AfxMessageBox(L"Prosze za³adowaæ Avatar");
		return;
	}
	m_strLogin = strLog;
	
	m_image.Destroy();
	CDialogEx::OnOK();
}
bool CLogDlg::copyImg(const CImage& src, CImage& dst)
{
	if (src.IsNull())
		return FALSE;
	//Source image parameters
	BYTE* srcPtr = (BYTE*)src.GetBits();
	int srcBitsCount = src.GetBPP();
	int srcWidth = src.GetWidth();
	int srcHeight = src.GetHeight();
	int srcPitch = src.GetPitch();
	//Destroy the original image
	if (!dst.IsNull())
	{
		dst.Destroy();
	}
	//Create a new image
	if (srcBitsCount == 32)   //Support alpha channel
	{
		dst.Create(srcWidth, srcHeight, srcBitsCount, 1);
	}
	else
	{
		dst.Create(srcWidth, srcHeight, srcBitsCount, 0);
	}
	//Load palette, for 8 bit bitmap
	if (srcBitsCount <= 8 && src.IsIndexed())// need palette
	{
		RGBQUAD pal[256];
		int nColors = src.GetMaxColorTableEntries();
		if (nColors > 0)
		{
			src.GetColorTable(0, nColors, pal);
			dst.SetColorTable(0, nColors, pal);//Copy palette program
		}
	}
	//Target image parameters
	BYTE* destPtr = (BYTE*)dst.GetBits();
	int destPitch = dst.GetPitch();
	//Copy image data, the original codes is a redundant 
	/*for (int i = 0; i < srcHeight; i++)
	{
	memcpy(destPtr + i*destPitch, srcPtr + i*srcPitch, abs(srcPitch));
	}*/
	// the new codes by sund 
	if (srcPitch < 0) {
		memcpy(destPtr + destPitch * (srcHeight - 1), srcPtr + srcPitch * (srcHeight - 1), srcHeight * abs(srcPitch));
	}
	else {
		memcpy(destPtr, srcPtr, srcHeight * abs(srcPitch));

	}
	return true;
}