
// Async.Socket.Example.MFCDlg.h : header file
//

#pragma once

#include <vu>
#include <thread>

// CAsyncSocketExampleMFCDlg dialog
class CAsyncSocketExampleMFCDlg : public CDialogEx
{
// Construction
public:
	CAsyncSocketExampleMFCDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ASYNCSOCKETEXAMPLEMFC_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnClose();

	afx_msg void OnBnClickedClear();
  afx_msg void OnBnClickedStart();
  afx_msg void OnBnClickedStop();

	DECLARE_MESSAGE_MAP()

private:
	void Log(const std::string& text);

private:
	DWORD m_IP;
	int   m_Port;
	CListBox m_Log;
	HANDLE m_Worker;
	vu::CAsyncSocket* m_pServer;
};
