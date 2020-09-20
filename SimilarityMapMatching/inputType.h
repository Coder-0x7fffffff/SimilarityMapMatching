#pragma once


// inputType 对话框

class inputType : public CDialogEx
{
	DECLARE_DYNAMIC(inputType)

public:
	inputType(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~inputType();
	CString type;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit();
};
