
// FiberStretcherDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FiberStretcher.h"
#include "FiberStretcherDlg.h"
#include "afxdialogex.h"
#include "ncpod.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HINSTANCE hDllInst;     
 
#define    XAXIS 0
#define  YAXIS   1
#define  ZAXIS   2
#define   AAXIS  3
#define  BAXIS   4
#define  CAXIS   5
#define  AutoMove   3
#define  SigHmMove  1
#define  SigPbMove  2
#define   Idle 4
#define OK   0
#define DEVICEMV  1
#define DEVICEBUSY  2
#define LMTLOCK   3
#define DEVICEIDLE   4
#define PARAERR  5
#define COMERR   6
#define NOUSB   -1
#define MOVLEN  10
#define  STEPPERX   800		//���ϸ�֣�Ĭ��800pulse/circle
#define  STEPPERY   800		//���ϸ�֣�Ĭ��800pulse/circle
#define  DEFAULTPER   800  //�������嵱��,����Ĭ��Ϊ100     
#define  PMOVELENGTH (STEPPERX/5)		//0.2 circle, 1mm
#define  FIFOMVONCELNETH  24

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFiberStretcherDlg �Ի���
CFiberStretcherDlg::CFiberStretcherDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFiberStretcherDlg::IDD, pParent)
	, m_distance_l(0)
	, m_speed_l(1)
	, m_dir_l(FALSE)
	, m_dir_r(FALSE)
	, m_speed_r(1)
	, m_distance_r(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFiberStretcherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_L, m_distance_l);
	DDX_Text(pDX, IDC_EDIT_SPEED_L, m_speed_l);
	DDX_Radio(pDX, IDC_RADIO_DIRUP_L, m_dir_l);
	DDX_Radio(pDX, IDC_RADIO_DIRUP_R, m_dir_r);
	DDX_Text(pDX, IDC_EDIT_SPEED_R, m_speed_r);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_R, m_distance_r);
	DDV_MinMaxDouble(pDX, m_speed_l, 0, 100);
	DDV_MinMaxDouble(pDX, m_speed_r, 0, 100);
	DDV_MinMaxDouble(pDX, m_distance_l, 0, 300);
	DDV_MinMaxDouble(pDX, m_distance_r, 0, 300);
}

BEGIN_MESSAGE_MAP(CFiberStretcherDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_MOVE, &CFiberStretcherDlg::OnBnClickedButtonMove)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CFiberStretcherDlg::OnBnClickedButtonStop)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_EN_KILLFOCUS(IDC_EDIT_SPEED_L, &CFiberStretcherDlg::OnEnKillfocusEditSpeedL)
	ON_EN_KILLFOCUS(IDC_EDIT_SPEED_R, &CFiberStretcherDlg::OnEnKillfocusEditSpeedR)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CFiberStretcherDlg::OnBnClickedButtonReset)
END_MESSAGE_MAP()


// CFiberStretcherDlg ��Ϣ�������

BOOL CFiberStretcherDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	Init_Controller();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFiberStretcherDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFiberStretcherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFiberStretcherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFiberStretcherDlg::OnBnClickedButtonMove()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int poscur_l,poscur_r;
	UpdateData(1);

	reslt = Get_Command_Po(0, XAXIS, &poscur_l);
	if (m_dir_l)
		reslt = PMov(0, XAXIS, poscur_l +(int)(PMOVELENGTH*m_distance_l));
	else
		reslt = PMov(0, XAXIS, poscur_l -(int)(PMOVELENGTH*m_distance_l));

	reslt = Get_Command_Po(0, YAXIS, &poscur_r);
	if (m_dir_r)
		reslt = PMov(0, YAXIS, poscur_r +(int)(PMOVELENGTH*m_distance_r));
	else
		reslt = PMov(0, YAXIS, poscur_r -(int)(PMOVELENGTH*m_distance_r));
}

void CFiberStretcherDlg::OnBnClickedButtonStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	gstate=Idle;
    Stop_FifoMov(0);
}


void CFiberStretcherDlg::Init_Controller(void)
{
	hDllInst=NULL;

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	if (NULL == hDllInst)
		hDllInst= LoadLibrary(_T("NcPod.dll"));    
	if(hDllInst==NULL)		   
	{
		MessageBox(_T("load DLL Error!"),0,0);
	}
	else
	{  	 
		XHCmcc_Initia = (XHCmcc_Initial)GetProcAddress(hDllInst, "XHCmcc_Initial");
		Get_Lib_Versio =  (Get_Lib_Version)GetProcAddress(hDllInst, "Get_Lib_Version");
		Set_Pulse_Mod  =  (Set_Pulse_Mode)GetProcAddress(hDllInst, "Set_Pulse_Mode"); 
		Set_Ac        =   (Set_Acc) GetProcAddress(hDllInst, "Set_Acc"); 
		Set_Start      =   (Set_StartV)GetProcAddress(hDllInst, "Set_StartV");
		Set_MoveSpee    =  (Set_MoveSpeed) GetProcAddress(hDllInst, "Set_MoveSpeed");
		PMov             = (PMove) GetProcAddress(hDllInst, "PMove");
		StopPMov        =  (StopPMove) GetProcAddress(hDllInst, "StopPMove");
		Sudden_Sto     =  (Sudden_Stop) GetProcAddress(hDllInst, "Sudden_Stop");
		SignalMov       =  (SignalMove ) GetProcAddress(hDllInst, "SignalMove");
		Set_Command_Po      =  (Set_Command_Pos) GetProcAddress(hDllInst, "Set_Command_Pos");
		Get_Command_Po  =  (Get_Command_Pos)GetProcAddress(hDllInst, "Get_Command_Pos");
		Get_Statu       =   (Get_Status)  GetProcAddress(hDllInst, "Get_Status");
		Start_FifoMov   =   (Start_FifoMove ) GetProcAddress(hDllInst, "Start_FifoMove");
		Stop_FifoMov     =  (Stop_FifoMove) GetProcAddress(hDllInst, "Stop_FifoMove");
		Write_Bi        =   (Write_Bit) GetProcAddress(hDllInst, "Write_Bit");
		Get_CodeLin     =   (Get_CodeLine)  GetProcAddress(hDllInst, "Get_CodeLine");
		Read_Bi         =    (Read_Bit)  GetProcAddress(hDllInst, "Read_Bit");
		Set_IoIn_Mod    =    (Set_IoIn_Mode) GetProcAddress(hDllInst, "Set_IoIn_Mode");
		UnlimtSigna     =   (UnlimtSignal)  GetProcAddress(hDllInst, "UnlimtSignal");
		Set_Spindle_Spee =  (Set_Spindle_Speed) GetProcAddress(hDllInst, "Set_Spindle_Speed");
		Fifo_InP_Comman =    (Fifo_InP_Command) GetProcAddress(hDllInst, "Fifo_InP_Command");
		Read_Fifo_Stat =    (Read_Fifo_State)  GetProcAddress(hDllInst, "Read_Fifo_State");
		Reset_Fif       =    (Reset_Fifo) GetProcAddress(hDllInst, "Reset_Fifo");
		StopPlu         =    (StopPlug)GetProcAddress(hDllInst, "StopPlug");
		SetSoftLm       =    (SetSoftLmt)GetProcAddress(hDllInst, "SetSoftLmt");
		OpenMotoionDevci   =  (OpenMotoionDevcie) GetProcAddress(hDllInst, "OpenMotoionDevcie");
		CloseMotoionDevci =  (CloseMotoionDevcie) GetProcAddress(hDllInst, "CloseMotoionDevcie");
		Set_Steppe         =  (Set_Stepper)GetProcAddress(hDllInst, "Set_Stepper");
		Set_SoftLmtEnalb  =  (Set_SoftLmtEnalbe) GetProcAddress(hDllInst, "Set_SoftLmtEnalbe");
		CloseSpindl        =  (CloseSpindle) GetProcAddress(hDllInst, "CloseSpindle");
		OpenSpindl        =    (OpenSpindle)  GetProcAddress(hDllInst, "OpenSpindle");
		Set_MotorDirLeve   =  (Set_MotorDirLevel) GetProcAddress(hDllInst, "Set_MotorDirLevel");
		Set_MotorPulsLeve  =   (Set_MotorPulsLevel) GetProcAddress(hDllInst, "Set_MotorPulsLevel");
		Set_SpindPulsPerRotat = (Set_SpindPulsPerRotate) GetProcAddress(hDllInst, "Set_SpindPulsPerRotate");
	}

	//����Ҫ��ʼ���������ӿ��ɹ�
	if(XHCmcc_Initia(0)!=OK) 
	{
		if(OpenMotoionDevci(0)==OK)
		{
			m_strpod1="Motion Card Is Ok!"  ;
		}
		else
		{
			m_strpod1="Please insert card!"  ;
		}
	}
	else
	{
		m_strpod1="Motion Card Is Ok!"  ;
	}
 
	//��ʼ���������
	reslt = Set_Steppe(0, 0, STEPPERX) ;//��X���嵱��
	reslt = Set_MoveSpee(0, 0, STEPPERX); //60rpm,תΪ����/��
	reslt = Set_Ac(0, 0, 16000);	//��X���ٶ� PPS,1���ڼ��ٵ�1200rpm
 
	reslt = Set_Steppe(0, 1, STEPPERY); //��Y���嵱��
	reslt = Set_MoveSpee(0, 1, STEPPERY/2); //60rpm,תΪ����/��
	reslt = Set_Ac(0, 1, 16000) ;    //��Y���ٶ� PPS,1���ڼ��ٵ�1200rpm
 
	//��ʼ��IO��,׼�� �Ե�,��ԭ�� ��
	//pin0-pin4 ͨ��IO,����Ϊԭ��,�Ե��ź�.
	reslt = Set_IoIn_Mod(0, 0, 0, 0, 1); //In0
	reslt = Set_IoIn_Mod(0, 1, 0, 0, 1); //In1
	reslt = Set_IoIn_Mod(0, 2, 0, 0, 1); //In2
	reslt = Set_IoIn_Mod(0, 3, 0, 0, 1); //In3
	reslt = Set_IoIn_Mod(0, 4, 0, 0, 1);//In4
	//pin5 ��ͣ�͵�ƽ����
	reslt = Set_IoIn_Mod(0, 5, 16, 0, 1); //In5 estop
	//pin6,8 X��,y������λ �͵�ƽ����
	reslt = Set_IoIn_Mod(0, 6, 1, 0, 1); //x++
	reslt = Set_IoIn_Mod(0, 8, 2, 0, 1);   //y++
	//pin7,9 X��,y�Ḻ��λ �͵�ƽ����
	reslt = Set_IoIn_Mod(0, 7, -1, 0, 1) ;//x--
	reslt = Set_IoIn_Mod(0, 9, -2, 0, 1); //y--
	//������λ
	reslt = SetSoftLm(0, 0, 60*STEPPERX, -60*STEPPERX); //200 * STEPPERX
	reslt = SetSoftLm(0, 1, 60*STEPPERY, -60*STEPPERY)  ;    //200 * STEPPERY
	reslt = Set_SoftLmtEnalb(0, 0);
	//����DIR��ƽ,�Ĵ˵�ƽ��ĵ�����з���.
	reslt = Set_MotorDirLeve(0, XAXIS, 0);
	reslt = Set_MotorDirLeve(0, YAXIS, 0);

	//��ʼ������
	Sign = 0;
	Startline = 0;
	gstate = Idle;
	Stag = 0 ;
	reslt = Get_Lib_Versio(0);
	UpdateData(FALSE); 
}


void CFiberStretcherDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	if(hDllInst==NULL)	
		return;
	CloseMotoionDevci(0);
}

void CFiberStretcherDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(hDllInst!=NULL)	
		 StopPlu(0);

	CDialogEx::OnClose();
}

void CFiberStretcherDlg::OnEnKillfocusEditSpeedL()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(1);

	reslt = Set_MoveSpee(0, 0, (m_speed_l/5.0)*STEPPERX); //1605˿��,ÿȦ�ƶ�5mm,תΪ����/��
}


void CFiberStretcherDlg::OnEnKillfocusEditSpeedR()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(1);
	reslt = Set_MoveSpee(0, 1, (m_speed_r/5.0)*STEPPERY); //1605˿��,ÿȦ�ƶ�5mm,תΪ����/��
}


void CFiberStretcherDlg::OnBnClickedButtonReset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
 int state;
	if(hDllInst==NULL)	
		return;

reslt = Get_Statu(0, 0, &state);
if (reslt == 0 || state == LMTLOCK) 
{	
  reslt = Sudden_Sto(0, 0);
  reslt = Reset_Fif(0);
  reslt = UnlimtSigna(0, 0);
}
else
{
  reslt = OpenMotoionDevci(0);
  if (reslt == 0) 
  { 	
        m_strpod1 = "Usb Card Is Ok!";
        reslt = Get_Statu(0, 0, &state);
        if(state == LMTLOCK)
        {	
          reslt = Sudden_Sto(0, 0);
          reslt = Reset_Fif(0);
          reslt = UnlimtSigna(0, 0);
        }
  }      	
  else
 	{
        m_strpod1 = "Please Insert Usb Card!";
  }
}
reslt = Get_CodeLin(0);
gstate=Idle;
   UpdateData(FALSE);  
}
