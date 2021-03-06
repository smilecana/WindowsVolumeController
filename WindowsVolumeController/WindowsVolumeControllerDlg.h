
// WindowsVolumeControllerDlg.h : header file
//

#pragma once

#include "PGVolumeController.h"

// CWindowsVolumeControllerDlg dialog
class CWindowsVolumeControllerDlg : public CDialogEx
{
// Construction
public:
	CWindowsVolumeControllerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINDOWSVOLUMECONTROLLER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
	static HRESULT VolumeCallback(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify, void* param);

// Implementation
protected:
	HICON m_hIcon;
	CSliderCtrl m_slider;

	CPGVolumeController m_controller;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()
};
