
// FiberStretcherDlg.h : ͷ�ļ�
//

#pragma once


// CFiberStretcherDlg �Ի���
class CFiberStretcherDlg : public CDialogEx
{
// ����
public:
	CFiberStretcherDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FIBERSTRETCHER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
