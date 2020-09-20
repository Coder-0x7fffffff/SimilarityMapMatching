// inputType.cpp: 实现文件
//

#include "stdafx.h"
#include "SimilarityMapMatching.h"
#include "inputType.h"
#include "afxdialogex.h"


// inputType 对话框

IMPLEMENT_DYNAMIC(inputType, CDialogEx)

inputType::inputType(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

inputType::~inputType()
{
}

void inputType::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(inputType, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT, &inputType::OnEnChangeEdit)
END_MESSAGE_MAP()


// inputType 消息处理程序


void inputType::OnEnChangeEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	GetDlgItem(IDC_EDIT)->GetWindowTextW(type);
}
