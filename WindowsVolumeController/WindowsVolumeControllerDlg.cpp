
// WindowsVolumeControllerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WindowsVolumeController.h"
#include "WindowsVolumeControllerDlg.h"
#include "afxdialogex.h"

#include <atomic>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWindowsVolumeControllerDlg dialog


CWindowsVolumeControllerDlg::CWindowsVolumeControllerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WINDOWSVOLUMECONTROLLER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWindowsVolumeControllerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SLDR_VOLUME, m_slider);
}

BEGIN_MESSAGE_MAP(CWindowsVolumeControllerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CWindowsVolumeControllerDlg message handlers

BOOL CWindowsVolumeControllerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_slider.SetRange(0, 100);
	m_controller.Initialize(CPGVolumeController::DeviceType::Speaker, &VolumeCallback, &m_slider);
	float volume = 0.0f;
	m_controller.GetMasterVolume(&volume);
	m_slider.SetPos(UINT(volume * 100 + 0.5f));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWindowsVolumeControllerDlg::OnPaint()
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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWindowsVolumeControllerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWindowsVolumeControllerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar != nullptr)
	{
		if (pScrollBar == (CScrollBar*)&m_slider)
		{
			UINT volume = static_cast<UINT>(m_slider.GetPos());
			m_controller.SetMasterVolume(volume);
		}
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

HRESULT CWindowsVolumeControllerDlg::VolumeCallback(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify, void* param)
{
	if (pNotify != nullptr)
	{
		const std::size_t size = 64;
		wchar_t text[size] = { 0, };
		UINT volume = 0;
		CPGVolumeController::CalculateIntegerVolume(pNotify->fMasterVolume, volume);
		swprintf_s(text, size, L"[DEBUG] Volume: %d, Mute: %d\n", volume, pNotify->bMuted);
		::OutputDebugStringW(text);
		reinterpret_cast<CSliderCtrl*>(param)->SetPos(volume);
	}
	return S_OK;
}
