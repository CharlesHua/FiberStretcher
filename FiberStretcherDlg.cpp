
// FiberStretcherDlg.cpp : 实现文件
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
#define  STEPPERX   800		//电机细分，默认800pulse/circle
#define  STEPPERY   800		//电机细分，默认800pulse/circle
#define  DEFAULTPER   800  //不设脉冲当量,卡会默认为100     
#define  PMOVELENGTH (STEPPERX/5)		//0.2 circle, 1mm
#define  FIFOMVONCELNETH  24

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CFiberStretcherDlg 对话框
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


// CFiberStretcherDlg 消息处理程序

BOOL CFiberStretcherDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	Init_Controller();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFiberStretcherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFiberStretcherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFiberStretcherDlg::OnBnClickedButtonMove()
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
	gstate=Idle;
    Stop_FifoMov(0);
}


void CFiberStretcherDlg::Init_Controller(void)
{
	hDllInst=NULL;

	// TODO: 在此添加额外的初始化代码
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

	//首先要初始化卡并连接卡成功
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
 
	//初始化电机参数
	reslt = Set_Steppe(0, 0, STEPPERX) ;//设X脉冲当量
	reslt = Set_MoveSpee(0, 0, STEPPERX); //60rpm,转为脉冲/秒
	reslt = Set_Ac(0, 0, 16000);	//设X加速度 PPS,1秒内加速到1200rpm
 
	reslt = Set_Steppe(0, 1, STEPPERY); //设Y脉冲当量
	reslt = Set_MoveSpee(0, 1, STEPPERY/2); //60rpm,转为脉冲/秒
	reslt = Set_Ac(0, 1, 16000) ;    //设Y加速度 PPS,1秒内加速到1200rpm
 
	//初始化IO口,准备 对刀,回原点 等
	//pin0-pin4 通用IO,可作为原点,对刀信号.
	reslt = Set_IoIn_Mod(0, 0, 0, 0, 1); //In0
	reslt = Set_IoIn_Mod(0, 1, 0, 0, 1); //In1
	reslt = Set_IoIn_Mod(0, 2, 0, 0, 1); //In2
	reslt = Set_IoIn_Mod(0, 3, 0, 0, 1); //In3
	reslt = Set_IoIn_Mod(0, 4, 0, 0, 1);//In4
	//pin5 急停低电平输入
	reslt = Set_IoIn_Mod(0, 5, 16, 0, 1); //In5 estop
	//pin6,8 X轴,y轴正限位 低电平输入
	reslt = Set_IoIn_Mod(0, 6, 1, 0, 1); //x++
	reslt = Set_IoIn_Mod(0, 8, 2, 0, 1);   //y++
	//pin7,9 X轴,y轴负限位 低电平输入
	reslt = Set_IoIn_Mod(0, 7, -1, 0, 1) ;//x--
	reslt = Set_IoIn_Mod(0, 9, -2, 0, 1); //y--
	//设软限位
	reslt = SetSoftLm(0, 0, 60*STEPPERX, -60*STEPPERX); //200 * STEPPERX
	reslt = SetSoftLm(0, 1, 60*STEPPERY, -60*STEPPERY)  ;    //200 * STEPPERY
	reslt = Set_SoftLmtEnalb(0, 0);
	//设电机DIR电平,改此电平会改电机运行方向.
	reslt = Set_MotorDirLeve(0, XAXIS, 0);
	reslt = Set_MotorDirLeve(0, YAXIS, 0);

	//初始化结束
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

	// TODO: 在此处添加消息处理程序代码
	if(hDllInst==NULL)	
		return;
	CloseMotoionDevci(0);
}

void CFiberStretcherDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(hDllInst!=NULL)	
		 StopPlu(0);

	CDialogEx::OnClose();
}

void CFiberStretcherDlg::OnEnKillfocusEditSpeedL()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(1);

	reslt = Set_MoveSpee(0, 0, (m_speed_l/5.0)*STEPPERX); //1605丝杠,每圈移动5mm,转为脉冲/秒
}


void CFiberStretcherDlg::OnEnKillfocusEditSpeedR()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(1);
	reslt = Set_MoveSpee(0, 1, (m_speed_r/5.0)*STEPPERY); //1605丝杠,每圈移动5mm,转为脉冲/秒
}


void CFiberStretcherDlg::OnBnClickedButtonReset()
{
	// TODO: 在此添加控件通知处理程序代码
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
