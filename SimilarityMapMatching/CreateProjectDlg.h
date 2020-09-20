#pragma once


// CreateProjectDlg 对话框

class CreateProjectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CreateProjectDlg)

public:
	CreateProjectDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CreateProjectDlg();
	CString path;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NEW_PROJECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonGetFolder();
	afx_msg void OnBnClickedButtonCheck();
	afx_msg void OnBnClickedOk();
};
