
// Async.Socket.Example.MFCDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Async.Socket.Example.MFC.h"
#include "Async.Socket.Example.MFCDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
  CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_ABOUTBOX };
#endif

  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CAsyncSocketExampleMFCDlg dialog

static DWORD WINAPI fnthread(LPVOID lpParam);

CAsyncSocketExampleMFCDlg::CAsyncSocketExampleMFCDlg(CWnd* pParent /*=nullptr*/)
  : CDialogEx(IDD_ASYNCSOCKETEXAMPLEMFC_DIALOG, pParent)
  , m_IP(0), m_Port(0)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAsyncSocketExampleMFCDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_IPAddress(pDX, IDC_IP, m_IP);
  DDX_Text(pDX, IDC_PORT, m_Port);
  DDX_Control(pDX, IDC_LOG, m_Log);
}

BEGIN_MESSAGE_MAP(CAsyncSocketExampleMFCDlg, CDialogEx)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()

  ON_BN_CLICKED(IDC_START, OnBnClickedStart)
  ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
  ON_BN_CLICKED(IDC_CLEAR, &CAsyncSocketExampleMFCDlg::OnBnClickedClear)
  ON_WM_CLOSE()
END_MESSAGE_MAP()

// CAsyncSocketExampleMFCDlg message handlers

BOOL CAsyncSocketExampleMFCDlg::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // Add "About..." menu item to system menu.

  // IDM_ABOUTBOX must be in the system command range.
  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != nullptr)
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

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);      // Set big icon
  SetIcon(m_hIcon, FALSE);    // Set small icon

  // TODO: Add extra initialization here

  m_IP = INADDR_LOOPBACK;
  m_Port = 1609;

  UpdateData(FALSE);

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAsyncSocketExampleMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAsyncSocketExampleMFCDlg::OnPaint()
{
  if (IsIconic())
  {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CDialogEx::OnPaint();
  }
}

HCURSOR CAsyncSocketExampleMFCDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

BOOL CAsyncSocketExampleMFCDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
  auto result = __super::OnNotify(wParam, lParam, pResult);

  GetDlgItem(IDC_IP)->EnableWindow(m_pServer == nullptr || !m_pServer->Running());
  GetDlgItem(IDC_PORT)->EnableWindow(m_pServer == nullptr || !m_pServer->Running());
  GetDlgItem(IDC_START)->EnableWindow(m_pServer == nullptr || !m_pServer->Running());
  GetDlgItem(IDC_STOP)->EnableWindow(m_pServer != nullptr && m_pServer->Running());

  return result;
}

void CAsyncSocketExampleMFCDlg::OnClose()
{
  OnBnClickedStop();
  __super::OnClose();
}

void CAsyncSocketExampleMFCDlg::OnBnClickedStart()
{
  if (m_pServer != nullptr)
  {
    return;
  }

  UpdateData(TRUE);

  struct sockaddr_in sai = { 0 };
  sai.sin_addr.s_addr = htonl(m_IP);
  const auto IP = inet_ntoa(sai.sin_addr);
  const vu::CSocket::TEndPoint EndPoint(IP, m_Port);

  m_pServer = new vu::CAsyncSocket;
  Log("Created");

  m_pServer->On(vu::CAsyncSocket::OPEN, [&](vu::CSocket& client) -> void
  {
    auto s = vu::FormatA("client %d opened\n", client.GetRemoteSAI().sin_port);
    Log(s);
  });

  m_pServer->On(vu::CAsyncSocket::CLOSE, [&](vu::CSocket& client) -> void
  {
    auto s = vu::FormatA("client %d closed\n", client.GetRemoteSAI().sin_port);
    Log(s);
  });

  m_pServer->On(vu::CAsyncSocket::SEND, [&](vu::CSocket& client) -> void
  {
    std::string s = "hello from server";
    client.Send(s.data(), int(s.size()));

    s = vu::FormatA("client %d send `%s`\n", client.GetRemoteSAI().sin_port, s.c_str());
    Log(s);
  });

  m_pServer->On(vu::CAsyncSocket::RECV, [&](vu::CSocket& client) -> void
  {
    vu::CBuffer data(KiB);
    client.Recv(data);
    std::string s(reinterpret_cast<char*>(data.GetpBytes()));

    s = vu::FormatA("client %d recv `%s`\n", client.GetRemoteSAI().sin_port, s.c_str());
    Log(s);
  });

  m_pServer->Bind(EndPoint);
  Log("Binded");

  m_pServer->Listen();
  Log("Listening...");

  DWORD tid = 0;
  m_Worker = CreateThread(nullptr, 0, fnthread, m_pServer, 0, &tid);
  Log("Running...");
}

void CAsyncSocketExampleMFCDlg::OnBnClickedStop()
{
  if (m_pServer == nullptr)
  {
    return;
  }

  CloseHandle(m_Worker);
  m_pServer->Stop();
  Log("Stopped");

  m_pServer->Close();
  delete m_pServer;
  m_pServer = nullptr;
  Log("Destroy");
}

void CAsyncSocketExampleMFCDlg::OnBnClickedClear()
{
  m_Log.ResetContent();
}

void CAsyncSocketExampleMFCDlg::Log(const std::string& text)
{
  CString s(text.c_str());
  m_Log.InsertString(-1, s);
  m_Log.SetTopIndex(m_Log.GetCount() - 1);
}

DWORD WINAPI fnthread(LPVOID lpParam)
{
  auto m_pServer = reinterpret_cast<vu::CAsyncSocket*>(lpParam);
  ASSERT(m_pServer != nullptr);

  m_pServer->Run();

  return 0;
}