// typeFitDialog.cpp: 实现文件
//

#include "stdafx.h"
#include "SimilarityMapMatching.h"
#include "typeFitDialog.h"
#include "afxdialogex.h"


// typeFitDialog 对话框

IMPLEMENT_DYNAMIC(typeFitDialog, CDialogEx)

void typeFitDialog::setMatch(mapMatch * ptr)
{
	matchPtr = ptr;
}

std::set<std::string> typeFitDialog::getSet()
{
	return set;
}

typeFitDialog::typeFitDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FIT, pParent)
{

}

typeFitDialog::~typeFitDialog()
{
}

void typeFitDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, typeListControl);
}


BEGIN_MESSAGE_MAP(typeFitDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &typeFitDialog::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &typeFitDialog::OnBnClickedButtonDel)
END_MESSAGE_MAP()


// typeFitDialog 消息处理程序


BOOL typeFitDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	std::vector<std::string> &typeList = matchPtr->getType();
	set.insert(typeList.begin(),typeList.end());
	// TODO:  在此添加额外的初始化
	for (std::set<std::string>::iterator it = set.begin(); it != set.end(); it++)
	{
		USES_CONVERSION;
		typeListControl.AddString(A2W(it->c_str()));
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void typeFitDialog::OnBnClickedButtonAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	inputType dlg;
	if (dlg.DoModal() == IDOK)
	{
		if (dlg.type.GetLength())
		{
			//类型前缀
			if (dlg.type[0] != '.')
				dlg.type = CString(".") + dlg.type;
			//加入
			USES_CONVERSION;
			if (set.find(W2A(dlg.type)) == set.end())
			{
				set.insert(W2A(dlg.type));
				typeListControl.AddString(dlg.type);
			}
		}
	}
}


void typeFitDialog::OnBnClickedButtonDel()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	int item = typeListControl.GetCurSel();
	typeListControl.GetText(item, str);
	USES_CONVERSION;
	set.erase(W2A(str));
	typeListControl.DeleteString(item);
}
