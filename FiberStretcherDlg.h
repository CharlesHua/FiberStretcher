
// FiberStretcherDlg.h : 头文件
//

#pragma once


// CFiberStretcherDlg 对话框
class CFiberStretcherDlg : public CDialogEx
{
// 构造
public:
	CFiberStretcherDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FIBERSTRETCHER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonMove();
	afx_msg void OnBnClickedButtonStop();
	void Init_Controller(void);


	CString m_strpod1;
	CString str_Xpos;
	CString str_Ypos;
	CString str_Zpos;
	CString str_Apos;
	CString str_Bpos;
	CString str_Cpos;
	CString m_strSpind;
	BOOL spindleon;
	int  reslt;
	int  gstate;
	int  hmstate;
	int  Startline;
	int  Stag;
	int  Sign;
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	double m_distance_l;
	double m_speed_l;
	afx_msg void OnEnKillfocusEditSpeedL();
	BOOL m_dir_l;
	BOOL m_dir_r;
	double m_speed_r;
	double m_distance_r;
	afx_msg void OnEnKillfocusEditSpeedR();
	afx_msg void OnBnClickedButtonReset();
};
