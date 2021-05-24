#pragma once
#include "CPixelCOLORREF.h"

// CLogDlg dialog

class CLogDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLogDlg)

public:
	CLogDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLogDlg();
	
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOG };
#endif

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()

	CImage m_image;
	CImage m_imageSmall;
	CPixelCOLORREF m_pixels;
	
	CString m_strLogin;
public:
		
	afx_msg void OnBnClickedButtonGetFile();
	CStatic m_Icon_Avatar;
	afx_msg void OnBnClickedOk();
	CPixelCOLORREF GetAvatar_Login(CString& strLogin);
	bool copyImg(const CImage& src, CImage& dst);
};
