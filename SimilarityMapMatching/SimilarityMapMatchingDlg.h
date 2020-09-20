
// SimilarityMapMatchingDlg.h: 头文件
//

#pragma once
#include <opencv2/opencv.hpp>
#include "CreateProjectDlg.h"
#include "mapMatch.h"
#include "typeFitDialog.h"

// CSimilarityMapMatchingDlg 对话框
class CSimilarityMapMatchingDlg : public CDialogEx
{
//自定义
public:
	bool ifOpen;			//是否打开工程
	bool isMatching;		//是否在匹配
	CString projectPath;	//工程路径
	mapMatch match;			//匹配类
	CWinThread *matchThreadPtr;

// 构造
public:
	CSimilarityMapMatchingDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMILARITYMAPMATCHING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CStatic newBut;
	CComboBox comboPattern;
	CEdit editPercent;
	afx_msg void OnStnClickedButStart();
	afx_msg void OnStnClickedButEnd();
	afx_msg void OnStnClickedButAdd();
	afx_msg void OnStnClickedButRemove();
	afx_msg void OnStnClickedButNew();
	afx_msg void OnStnClickedButOpen();
	afx_msg void OnStnClickedButSave();
	afx_msg void OnStnClickedButClose();
	CListCtrl listResult;
	CButton checkBox;
	afx_msg void OnEnChangeEditPercent();
	afx_msg void OnCbnSelchangeComboPattern();
	afx_msg void OnBnClickedCheckSubdir();
	CListBox listFolder;
	afx_msg void OnNMClickListResult(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic stateLableControl;
	CProgressCtrl processContorl;
	afx_msg void OnStnClickedButEnd2();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
