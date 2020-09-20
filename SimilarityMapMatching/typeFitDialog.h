#pragma once
#include "mapMatch.h"
#include "inputType.h"

// typeFitDialog 对话框

class typeFitDialog : public CDialogEx
{
//自定义
private:
	mapMatch *matchPtr;
	std::set<std::string> set;
public:
	void setMatch(mapMatch *ptr);
	std::set<std::string> getSet();
	DECLARE_DYNAMIC(typeFitDialog)

public:
	typeFitDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~typeFitDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FIT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox typeListControl;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
};
