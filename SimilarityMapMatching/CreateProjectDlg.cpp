// CreateProjectDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimilarityMapMatching.h"
#include "CreateProjectDlg.h"
#include "afxdialogex.h"


// CreateProjectDlg 对话框

IMPLEMENT_DYNAMIC(CreateProjectDlg, CDialogEx)

CreateProjectDlg::CreateProjectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NEW_PROJECT, pParent)
{

}

CreateProjectDlg::~CreateProjectDlg()
{
}

void CreateProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CreateProjectDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_GET_FOLDER, &CreateProjectDlg::OnBnClickedButtonGetFolder)
	ON_BN_CLICKED(IDC_BUTTON_CHECK, &CreateProjectDlg::OnBnClickedButtonCheck)
	ON_BN_CLICKED(IDOK, &CreateProjectDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CreateProjectDlg 消息处理程序


void CreateProjectDlg::OnBnClickedButtonGetFolder()
{
	// TODO: 在此添加控件通知处理程序代码
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.lpszTitle = TEXT("选择工程目录");
	LPITEMIDLIST targetLocation = SHBrowseForFolder(&bi);
	if (targetLocation)
	{
		WCHAR targetPath[MAX_PATH];
		SHGetPathFromIDList(targetLocation, targetPath);
		GetDlgItem(IDC_EDIT_PATH)->SetWindowTextW(targetPath);
	}
}


void CreateProjectDlg::OnBnClickedButtonCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	CString path;
	GetDlgItem(IDC_EDIT_PATH)->GetWindowTextW(path);
	if (!PathFileExists(path))
	{
		MessageBox(TEXT("目录不存在"), TEXT("提示"), MB_OK|MB_ICONERROR);
		return;
	}
	CString fileName;
	GetDlgItem(IDC_EDIT_FILENAME)->GetWindowTextW(fileName);
	if (fileName == CString(""))
	{
		MessageBox(TEXT("工程名不能为空"), TEXT("提示"), MB_OK | MB_ICONERROR);
		return;
	}
	if (path.Right(1) != CString("\\"))
		path.Append(CString("\\"));
	if (fileName.Right(CString(".imatch").GetLength()) != CString(".imatch"))
		fileName.Append(CString(".imatch"));
	path += fileName;
	if (PathFileExists(path))
	{
		MessageBox(TEXT("同名工程已经存在"), TEXT("提示"), MB_OK | MB_ICONERROR);
		return;
	}
	MessageBox(TEXT("可创建工程"), TEXT("提示"), MB_OK);
}


void CreateProjectDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString path;
	GetDlgItem(IDC_EDIT_PATH)->GetWindowTextW(path);
	if (!PathFileExists(path))
	{
		MessageBox(TEXT("目录不存在"), TEXT("提示"), MB_OK | MB_ICONERROR);
		return;
	}
	CString fileName;
	GetDlgItem(IDC_EDIT_FILENAME)->GetWindowTextW(fileName);
	if (fileName == CString(""))
	{
		MessageBox(TEXT("工程名不能为空"), TEXT("提示"), MB_OK | MB_ICONERROR);
		return;
	}
	if (path.Right(1) != CString("\\"))
		path.Append(CString("\\"));
	if (fileName.Right(CString(".imatch").GetLength()) != CString(".imatch"))
		fileName.Append(CString(".imatch"));
	path += fileName;
	if (PathFileExists(path))
	{
		MessageBox(TEXT("同名工程已经存在"), TEXT("提示"), MB_OK | MB_ICONERROR);
		return;
	}
	this->path = path;
	CDialogEx::OnOK();
}
