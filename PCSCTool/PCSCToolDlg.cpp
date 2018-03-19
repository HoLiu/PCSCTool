
// PCSCToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PCSCTool.h"
#include "PCSCToolDlg.h"
#include "afxdialogex.h"
#include "easylogging++.h"
#include "reader.h"
#include <atlconv.h>

INITIALIZE_EASYLOGGINGPP

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPCSCToolDlg *pThis = NULL;



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


// CPCSCToolDlg �Ի���



CPCSCToolDlg::CPCSCToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPCSCToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPCSCToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_READERS, m_ComboReaders);
	DDX_Control(pDX, IDC_BUTTON_REFRESH, m_BtnRefresh);
	DDX_Control(pDX, IDC_COMBO_MODE, m_ComboMode);
	DDX_Control(pDX, IDC_COMBO_PROTOCOL, m_ComboProtocol);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_BtnConnect);
	DDX_Control(pDX, IDC_EDIT_CMD, m_EditCMD);
	DDX_Control(pDX, IDC_BUTTON_SEND, m_BtnSend);
	DDX_Control(pDX, IDC_CHECK_ESCAPE, m_CheckEscape);
	DDX_Control(pDX, IDC_LIST_VIEW, m_ListView);
}

BEGIN_MESSAGE_MAP(CPCSCToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CPCSCToolDlg::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CPCSCToolDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CPCSCToolDlg::OnBnClickedButtonSend)
	ON_CBN_SELCHANGE(IDC_COMBO_READERS, &CPCSCToolDlg::OnCbnSelchangeComboReaders)
	ON_CBN_SELCHANGE(IDC_COMBO_MODE, &CPCSCToolDlg::OnCbnSelchangeComboMode)
	ON_CBN_SELCHANGE(IDC_COMBO_PROTOCOL, &CPCSCToolDlg::OnCbnSelchangeComboProtocol)
	ON_BN_CLICKED(IDC_CHECK_ESCAPE, &CPCSCToolDlg::OnBnClickedCheckEscape)
	ON_LBN_SELCHANGE(IDC_LIST_VIEW, &CPCSCToolDlg::OnLbnSelchangeListView)
END_MESSAGE_MAP()


static void showHex(const char *str, unsigned char *text, unsigned int length)
{
	int i;
	char buffer[3];
	CString showStr(str);

	for (i = 0; i < length; i++)
	{
		sprintf(buffer, "%02X", text[i] & 0x00FF);
		showStr += buffer;
		showStr += ' ';
	}
	pThis->m_ListView.AddString(showStr);
}

static void showErr(const char *str, int code)
{
	CString showStr(str);
	if (code != 0) {
		USES_CONVERSION;
		showStr += A2T(GetScardErrMsg(code));
	}
	pThis->m_ListView.AddString(showStr);
}

// CPCSCToolDlg ��Ϣ�������

BOOL CPCSCToolDlg::OnInitDialog()
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	pThis = this;
	char *list;
	int ret;
	ret = RD_find(NULL, &list);
	if (ret < 0) {
		LOG(ERROR) << "RD_find :" << GetScardErrMsg(ret);
		showErr("RD_find : ", ret);
	}
	else if (ret == 0) {
		LOG(INFO) << "No Reader.";
		showErr("RD_find : No Reader", 0);
	}
	else {
		USES_CONVERSION;
		char *p = list;
		while (*p)
		{
			int i;
			for (i = 0; p[i]; i++);
			i++;
			if (*p != 0)
			{
				m_ComboReaders.AddString(A2T(p));
			}
			p = &p[i];
		}
		m_ComboReaders.SetCurSel(0);
	}

	m_ComboMode.ResetContent();
	m_ComboMode.AddString(_T("SCARD_SHARE_EXCLUSIVE"));
	m_ComboMode.AddString(_T("SCARD_SHARE_SHARED"));
	m_ComboMode.AddString(_T("SCARD_SHARE_DIRECT"));
	m_ComboMode.SetCurSel(1);

	m_ComboProtocol.ResetContent();
	m_ComboProtocol.AddString(_T("SCARD_PROTOCOL_UNDEFINED"));
	m_ComboProtocol.AddString(_T("SCARD_PROTOCOL_T0"));
	m_ComboProtocol.AddString(_T("SCARD_PROTOCOL_T1"));
	m_ComboProtocol.AddString(_T("SCARD_PROTOCOL_Tx"));
	m_ComboProtocol.AddString(_T("SCARD_PROTOCOL_RAW"));
	m_ComboProtocol.SetCurSel(3);
	LOG(INFO) << "Init done";
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CPCSCToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPCSCToolDlg::OnPaint()
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
HCURSOR CPCSCToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPCSCToolDlg::OnBnClickedButtonRefresh()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	char *list;
	int ret;
	m_ComboReaders.ResetContent();
	ret = RD_find(NULL, &list);
	if (ret < 0) {
		LOG(ERROR) << "RD_find :" << GetScardErrMsg(ret);
		showErr("RD_find : ", ret);
	}
	else if (ret == 0) {
		LOG(INFO) << "No Reader.";
		showErr("RD_find : No Reader.", 0);
	}
	else {
		USES_CONVERSION;
		char *p = list;
		while (*p)
		{
			int i;
			for (i = 0; p[i]; i++);
			i++;
			if (*p != 0)
			{
				m_ComboReaders.AddString(A2T(p));
			}
			p = &p[i];
		}
		m_ComboReaders.SetCurSel(0);
	}
}


void CPCSCToolDlg::OnBnClickedButtonConnect()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString btnCString;
	m_BtnConnect.GetWindowText(btnCString);
	if (btnCString == _T("Disconnect"))
	{
		RD_close(0);
		m_BtnConnect.SetWindowTextW(_T("Connect"));
		return;
	}
	CString csName;
	m_ComboReaders.GetLBText(m_ComboReaders.GetCurSel(), csName);
	int mode = m_ComboMode.GetCurSel() + 1;
	int protocol = m_ComboProtocol.GetCurSel();
	if (protocol == 4)
		protocol = 0x10000;
	LOG(INFO) << "mode :" << mode;
	LOG(INFO) << "protocol :" << protocol;
	unsigned char atr[40];
	unsigned long atr_len = sizeof(atr);
	USES_CONVERSION;
	int ret = RD_open(T2A(csName), mode, protocol, atr, &atr_len);
	if (ret < 0) {
		showErr("RD_open : ", ret);
		return;
	}
	m_BtnConnect.SetWindowText(_T("Disconnect"));

	showHex("ATR : ", atr, atr_len);

	return;
}

void CPCSCToolDlg::OnBnClickedButtonSend()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int ret;
	unsigned char tx[257], rx[262];
	char buffer[257];
	unsigned int tx_len;
	unsigned long rx_len = sizeof(rx);
	CString cStr;
	unsigned char tmp[257];
	int len;

	m_EditCMD.GetWindowText(cStr);
	cStr.Remove(' ');
	if (cStr.IsEmpty())
	{
		m_ListView.AddString(_T("No input data"));
		return;
	}
	len = cStr.GetLength();
	if (len % 2 != 0)
	{
		m_ListView.AddString(_T("Invalid input data, uneven number of characters"));
		return;
	}
	if (len / 2 < 5){
		m_ListView.AddString(_T("Insufficient input data"));
		return;
	}
	USES_CONVERSION;
	char *p = T2A(cStr);
	int i, j;
	if (len / 2 < 6) {
		for (j = i = 0; i < 5; i++){
			sscanf(&p[j], "%02X", &tx[i]);
			j += 2;
		}
		tx_len = 5;
	}
	else {
		for (j = i = 0; i < 5; i++){
			sscanf(&p[j], "%02X", &tx[i]);
			j += 2;
		}
		tx_len = tx[4] + 5;
		if (len / 2 < tx_len){
			m_ListView.AddString(_T("Insufficient input data"));
			return;
		}
		for (j = i = 0; i < tx[4]; i++){
			sscanf(&p[j + 10], "%02X", &tx[i + 5]);
			j += 2;
		}
	}

	showHex("TX : ", tx, tx_len);
	ret = RD_transceiver(0, tx, tx_len, rx, &rx_len);
	if (ret < 0) {
		LOG(ERROR) << "RD_transceiver :" << GetScardErrMsg(ret);
		showErr("RD_transceiver : ", ret);
		return;
	}

	showHex("RX : ", rx, rx_len);
}


void CPCSCToolDlg::OnCbnSelchangeComboReaders()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CPCSCToolDlg::OnCbnSelchangeComboMode()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (m_ComboMode.GetCurSel() == 2 && m_ComboProtocol.GetCurSel() == 0)
	{
		m_CheckEscape.SetCheck(1);
	} 
	else
	{
		m_CheckEscape.SetCheck(0);
	}
}


void CPCSCToolDlg::OnCbnSelchangeComboProtocol()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (m_ComboMode.GetCurSel() == 2 && m_ComboProtocol.GetCurSel() == 0)
	{
		m_CheckEscape.SetCheck(1);
	}
	else
	{
		m_CheckEscape.SetCheck(0);
	}
}


void CPCSCToolDlg::OnBnClickedCheckEscape()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (m_CheckEscape.GetCheck())
	{
		m_ComboMode.SetCurSel(2);
		m_ComboProtocol.SetCurSel(0);
	} 
	else
	{
		m_ComboMode.SetCurSel(1);
		m_ComboProtocol.SetCurSel(2);
	}
}


void CPCSCToolDlg::OnLbnSelchangeListView()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
