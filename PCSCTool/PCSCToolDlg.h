
// PCSCToolDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CPCSCToolDlg �Ի���
class CPCSCToolDlg : public CDialogEx
{
// ����
public:
	CPCSCToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PCSCTOOL_DIALOG };

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
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnCbnSelchangeComboReaders();
	afx_msg void OnCbnSelchangeComboMode();
	afx_msg void OnCbnSelchangeComboProtocol();
	CComboBox m_ComboReaders;
	CButton m_BtnRefresh;
	CComboBox m_ComboMode;
	CComboBox m_ComboProtocol;
	CButton m_BtnConnect;
	CEdit m_EditCMD;
	CButton m_BtnSend;
	CButton m_CheckEscape;
	CStatic m_StaticView;
	afx_msg void OnBnClickedCheckEscape();
	CListBox m_ListView;
	afx_msg void OnLbnSelchangeListView();
};
