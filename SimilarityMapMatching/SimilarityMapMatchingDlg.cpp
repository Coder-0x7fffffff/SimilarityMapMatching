
// SimilarityMapMatchingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimilarityMapMatching.h"
#include "SimilarityMapMatchingDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSimilarityMapMatchingDlg 对话框

UINT matchThread(LPVOID ptr)
{
	CSimilarityMapMatchingDlg *dlg = (CSimilarityMapMatchingDlg*)ptr;
	std::vector<FindResult> result = dlg->match.matching(&dlg->processContorl,&dlg->stateLableControl);
	dlg->stateLableControl.SetWindowTextW(TEXT("输出结果......"));
	dlg->listResult.DeleteAllItems();
	for (int i = 0; i < result.size(); ++i)
	{
		CString temp;
		USES_CONVERSION;
		CString parentPath = CString(A2W(result[i].parentFile.c_str())),subPath,fileName;
		//插入第一条
		fileName = parentPath.Right(parentPath.GetLength() - parentPath.ReverseFind('\\') - 1);
		dlg->listResult.InsertItem(dlg->listResult.GetItemCount(), fileName);
		dlg->listResult.SetItemText(dlg->listResult.GetItemCount() - 1, 1, parentPath);
		float ti = 100;
		temp.Format(TEXT("%.2f"), ti);
		dlg->listResult.SetItemText(dlg->listResult.GetItemCount() - 1, 2, temp);
		temp.Format(TEXT("%d"), i + 1);
		dlg->listResult.SetItemText(dlg->listResult.GetItemCount() - 1, 3, temp);
		//插入子数据
		std::vector<std::string> &subStr = result[i].subFile;
		std::vector<double> &subPercent = result[i].percent;
		for (int j = 0; j < subStr.size(); ++j)
		{
			subPath = CString(A2W(subStr[j].c_str()));
			fileName = subPath.Right(subPath.GetLength()-subPath.ReverseFind('\\')-1);
			dlg->listResult.InsertItem(dlg->listResult.GetItemCount(),fileName);
			temp.Format(TEXT("%.2f"), subPercent[j]*100);
			dlg->listResult.SetItemText(dlg->listResult.GetItemCount()-1, 1, subPath);
			dlg->listResult.SetItemText(dlg->listResult.GetItemCount() - 1, 2, temp);
			temp.Format(TEXT("%d"), i + 1);
			dlg->listResult.SetItemText(dlg->listResult.GetItemCount() - 1, 3, temp);
		}
	}

	dlg->comboPattern.EnableWindow(true);
	dlg->editPercent.EnableWindow(true);
	dlg->checkBox.EnableWindow(true);
	dlg->isMatching = false;
	dlg->stateLableControl.SetWindowTextW(TEXT("匹配完成"));
	return 0;
}

CSimilarityMapMatchingDlg::CSimilarityMapMatchingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIMILARITYMAPMATCHING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimilarityMapMatchingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUT_NEW, newBut);
	DDX_Control(pDX, IDC_COMBO_PATTERN, comboPattern);
	DDX_Control(pDX, IDC_EDIT_PERCENT, editPercent);
	DDX_Control(pDX, IDC_LIST_RESULT, listResult);
	DDX_Control(pDX, IDC_CHECK_SUBDIR, checkBox);
	DDX_Control(pDX, IDC_LIST_FOLDER, listFolder);
	DDX_Control(pDX, IDC_STATI_STATE, stateLableControl);
	DDX_Control(pDX, IDC_PROGRESS_STATE, processContorl);
}

BEGIN_MESSAGE_MAP(CSimilarityMapMatchingDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_STN_CLICKED(IDC_BUT_START, &CSimilarityMapMatchingDlg::OnStnClickedButStart)
	ON_STN_CLICKED(IDC_BUT_END, &CSimilarityMapMatchingDlg::OnStnClickedButEnd)
	ON_STN_CLICKED(IDC_BUT_ADD, &CSimilarityMapMatchingDlg::OnStnClickedButAdd)
	ON_STN_CLICKED(IDC_BUT_REMOVE, &CSimilarityMapMatchingDlg::OnStnClickedButRemove)
	ON_STN_CLICKED(IDC_BUT_NEW, &CSimilarityMapMatchingDlg::OnStnClickedButNew)
	ON_STN_CLICKED(IDC_BUT_OPEN, &CSimilarityMapMatchingDlg::OnStnClickedButOpen)
	ON_STN_CLICKED(IDC_BUT_SAVE, &CSimilarityMapMatchingDlg::OnStnClickedButSave)
	ON_STN_CLICKED(IDC_BUT_CLOSE, &CSimilarityMapMatchingDlg::OnStnClickedButClose)
	ON_EN_CHANGE(IDC_EDIT_PERCENT, &CSimilarityMapMatchingDlg::OnEnChangeEditPercent)
	ON_CBN_SELCHANGE(IDC_COMBO_PATTERN, &CSimilarityMapMatchingDlg::OnCbnSelchangeComboPattern)
	ON_BN_CLICKED(IDC_CHECK_SUBDIR, &CSimilarityMapMatchingDlg::OnBnClickedCheckSubdir)
//	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_RESULT, &CSimilarityMapMatchingDlg::OnNMCustomdrawListResult)
//ON_NOTIFY(HDN_ITEMCLICK, 0, &CSimilarityMapMatchingDlg::OnHdnItemclickListResult)
//ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LIST_RESULT, &CSimilarityMapMatchingDlg::OnLvnItemActivateListResult)
//ON_NOTIFY(NM_SETFOCUS, IDC_LIST_RESULT, &CSimilarityMapMatchingDlg::OnNMSetfocusListResult)
ON_NOTIFY(NM_CLICK, IDC_LIST_RESULT, &CSimilarityMapMatchingDlg::OnNMClickListResult)
ON_STN_CLICKED(IDC_BUT_END2, &CSimilarityMapMatchingDlg::OnStnClickedButEnd2)
END_MESSAGE_MAP()


// CSimilarityMapMatchingDlg 消息处理程序

BOOL CSimilarityMapMatchingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//初始化GUI
	comboPattern.AddString(TEXT("均值哈希"));
	comboPattern.AddString(TEXT("感知哈希"));
	comboPattern.AddString(TEXT("差异哈希"));
	comboPattern.AddString(TEXT("颜色分布"));
	comboPattern.AddString(TEXT("内容特征"));
	comboPattern.SetCurSel(0);
	editPercent.SetWindowTextW(TEXT("70"));
	listResult.InsertColumn(0, TEXT("文件名"),LVCFMT_LEFT, 100, -1);
	listResult.InsertColumn(1, TEXT("路径"), LVCFMT_LEFT, 450, -1);
	listResult.InsertColumn(2, TEXT("相似度"), LVCFMT_LEFT, 100, -1);
	listResult.InsertColumn(3, TEXT("分组"), LVCFMT_LEFT, 100, -1);
	listResult.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	checkBox.SetCheck(true);
	//初始化match
	match.setMode(PATTERN_AVERAGE_HASH);
	match.setRate(0.70);
	match.findSubDir();
	//初始化变量
	ifOpen = isMatching = false;
	projectPath = CString("");
	//绑定绘制窗口
	CWnd *first = GetDlgItem(IDC_PIC_FISRST);
	CWnd *second = GetDlgItem(IDC_PIC_SECOND);
	cv::namedWindow("first", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("second", cv::WINDOW_AUTOSIZE);
	HWND fHwnd = (HWND)cvGetWindowHandle("first");
	HWND sHwnd = (HWND)cvGetWindowHandle("second");
	HWND fParent = ::GetParent(fHwnd);
	HWND sParent = ::GetParent(sHwnd);
	::SetParent(fHwnd, GetDlgItem(IDC_PIC_FISRST)->m_hWnd);
	::SetParent(sHwnd, GetDlgItem(IDC_PIC_SECOND)->m_hWnd);
	::ShowWindow(fParent, SW_HIDE);
	::ShowWindow(sParent, SW_HIDE);
	stateLableControl.SetWindowTextW(TEXT("空闲"));
	//初始化类型
	match.addType(".jpg");
	match.addType(".png");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSimilarityMapMatchingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSimilarityMapMatchingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSimilarityMapMatchingDlg::OnStnClickedButStart()
{
	// TODO: 在此添加控件通知处理程序代码
	if (ifOpen == false)
	{
		MessageBox(TEXT("未打开工程"), TEXT("提示"), MB_OK | MB_ICONASTERISK);
		return;
	}
	if (isMatching == true)
	{
		MessageBox(TEXT("正在匹配,请稍等......"), TEXT("提示"), MB_OK);
		return;
	}
	//线程查找
	comboPattern.EnableWindow(false);
	editPercent.EnableWindow(false);
	checkBox.EnableWindow(false);
	isMatching = true;
	matchThreadPtr = AfxBeginThread(matchThread, this);
}


void CSimilarityMapMatchingDlg::OnStnClickedButEnd()
{
	// TODO: 在此添加控件通知处理程序代码
	if (ifOpen == false)
	{
		MessageBox(TEXT("未打开工程"), TEXT("提示"), MB_OK | MB_ICONASTERISK);
		return;
	}
	if (isMatching == true)
	{
		MessageBox(TEXT("正在匹配,请稍等......"), TEXT("提示"), MB_OK);
		return;
	}
	listResult.DeleteAllItems();
}


void CSimilarityMapMatchingDlg::OnStnClickedButAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	if (ifOpen == false)
	{
		MessageBox(TEXT("未打开工程"), TEXT("提示"), MB_OK | MB_ICONASTERISK);
		return;
	}
	if (isMatching == true)
	{
		MessageBox(TEXT("正在匹配,请稍等......"), TEXT("提示"), MB_OK);
		return;
	}
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.lpszTitle = TEXT("选择文件夹");
	LPITEMIDLIST targetLocation = SHBrowseForFolder(&bi);
	if (targetLocation)
	{
		WCHAR targetPath[MAX_PATH];
		SHGetPathFromIDList(targetLocation, targetPath);
		USES_CONVERSION;
		std::vector<std::string> &vec = match.getDir();
		for (int i = 0; i < vec.size(); ++i)
		{
			if (vec[i] == std::string(W2A(targetPath)))
			{
				MessageBox(TEXT("目录已经存在"), TEXT("提示"), MB_OK | MB_ICONASTERISK);
				return;
			}
		}
		match.addDir(W2A(targetPath));
		listFolder.AddString(targetPath);
	}
}


void CSimilarityMapMatchingDlg::OnStnClickedButRemove()
{
	// TODO: 在此添加控件通知处理程序代码
	if (ifOpen == false)
	{
		MessageBox(TEXT("未打开工程"), TEXT("提示"), MB_OK | MB_ICONASTERISK);
		return;
	}
	if (isMatching == true)
	{
		MessageBox(TEXT("正在匹配,请稍等......"), TEXT("提示"), MB_OK);
		return;
	}
	if (listFolder.GetCurSel() == -1)
	{
		MessageBox(TEXT("请先选择要删除的目录"), TEXT("提示"), MB_OK | MB_ICONASTERISK);
		return;
	}
	CString str;
	listFolder.GetText(listFolder.GetCurSel(),str);
	USES_CONVERSION;
	match.removeDir(W2A(str));
	listFolder.DeleteString(listFolder.GetCurSel());
}


void CSimilarityMapMatchingDlg::OnStnClickedButNew()
{
	// TODO: 在此添加控件通知处理程序代码
	if (isMatching == true)
	{
		MessageBox(TEXT("正在匹配,请稍等......"), TEXT("提示"), MB_OK);
		return;
	}
	CreateProjectDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		projectPath = dlg.path;
		CFile file(projectPath,CFile::modeCreate);
		file.Close();
		ifOpen = true;
		//初始化
		comboPattern.SetCurSel(0); 
		editPercent.SetWindowTextW(TEXT("70"));
		checkBox.SetCheck(true);
		//初始化match
		match.setMode(PATTERN_AVERAGE_HASH);
		match.setRate(0.70);
		match.findSubDir();
		match.resetDir();
		match.clearType();
		//加入默认type
		match.addType(".jpg");
		match.addType(".png");
		match.addType(".bmp");
		//新建工程
		CString str;
		str.Format(TEXT("%d"), comboPattern.GetCurSel());
		WritePrivateProfileString(TEXT("config"), TEXT("mode"),str , projectPath);
		editPercent.GetWindowTextW(str);
		WritePrivateProfileString(TEXT("config"), TEXT("percent"), str, projectPath);
		str.Format(TEXT("%d"), checkBox.GetCheck());
		WritePrivateProfileString(TEXT("config"), TEXT("subFile"), str, projectPath);
		WritePrivateProfileString(TEXT("config"), TEXT("pathCount"), TEXT("0"), projectPath);
		str.Format(TEXT("%d"), match.getType().size());
		WritePrivateProfileString(TEXT("config"), TEXT("typeCount"), str, projectPath);
		std::vector<std::string> typeSet = match.getType();
		for (int i = 0; i < typeSet.size(); i++)
		{
			str.Format(TEXT("type%d"), i);
			USES_CONVERSION;
			WritePrivateProfileString(TEXT("type"), str, A2W(typeSet[i].c_str()), projectPath);
		}
		listFolder.ResetContent();
		listResult.DeleteAllItems();
		stateLableControl.SetWindowTextW(TEXT("空闲"));
		processContorl.SetPos(0);
	}
}


void CSimilarityMapMatchingDlg::OnStnClickedButOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	if (ifOpen == true)
	{
		MessageBox(TEXT("已打开工程"), TEXT("提示"), MB_OK | MB_ICONASTERISK);
		return;
	}
	if (isMatching == true)
	{
		MessageBox(TEXT("正在匹配,请稍等......"), TEXT("提示"), MB_OK);
		return;
	}
	//打开工程！！！！！！！！！！
	CFileDialog dlg(true, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, TEXT("图匹配文件(.imatch)|*.imatch||"));
	dlg.m_ofn.lpstrTitle = TEXT("打开工程");
	if (dlg.DoModal() == IDOK)
	{
		projectPath = dlg.GetPathName();
		ifOpen = true;
		//读取配置
		int temp;
		stateLableControl.SetWindowTextW(TEXT("读取配置......"));
		temp = GetPrivateProfileInt(TEXT("config"), TEXT("mode"), 0, projectPath);
		match.setMode(temp);
		comboPattern.SetCurSel(temp);
		temp = GetPrivateProfileInt(TEXT("config"), TEXT("percent"), 0, projectPath);
		match.setRate((temp*1.0) / 100.0);
		CString str;
		str.Format(TEXT("%d"), temp);
		editPercent.SetWindowTextW(str);
		temp = GetPrivateProfileInt(TEXT("config"), TEXT("subFile"), 0, projectPath);
		match.findSubDir(temp);
		checkBox.SetCheck(temp);
		//读取路径
		temp = GetPrivateProfileInt(TEXT("config"), TEXT("pathCount"), 0, projectPath);
		int noUsePathCount = 0;
		WCHAR path[MAX_PATH];
		stateLableControl.SetWindowTextW(TEXT("读取路径......"));
		processContorl.SetRange32(0, temp - 1);
		for (int i = 0; i < temp; i++)
		{
			str.Format(TEXT("path%d"), i);
			GetPrivateProfileString(TEXT("path"), str, CString(""), path, MAX_PATH, projectPath);
			processContorl.SetPos(i);
			if (PathFileExists(path))
			{
				USES_CONVERSION;
				match.addDir(W2A(path));
				listFolder.AddString(path);
			}
			else noUsePathCount++;
		}
		//读取类型过滤器
		WCHAR type[64];
		temp = GetPrivateProfileInt(TEXT("config"), TEXT("typeCount"), 0, projectPath);
		for (int i = 0; i < temp; i++)
		{
			str.Format(TEXT("type%d"), i);
			GetPrivateProfileString(TEXT("type"), str, CString(""), type, 64, projectPath);
			USES_CONVERSION;
			match.addType(W2A(type));
		}
		stateLableControl.SetWindowTextW(TEXT("读取完毕"));
	}
}


void CSimilarityMapMatchingDlg::OnStnClickedButSave()
{
	// TODO: 在此添加控件通知处理程序代码
	if (ifOpen == false)
	{
		MessageBox(TEXT("未打开工程"), TEXT("提示"), MB_OK | MB_ICONASTERISK);
		return;
	}
	if (isMatching == true)
	{
		MessageBox(TEXT("正在匹配,请稍等......"), TEXT("提示"), MB_OK);
		return;
	}
	//保存工程！！！！！！！！！！！！
	//重建配置文件
	DeleteFile(projectPath);
	CFile file(projectPath, CFile::modeCreate);
	file.Close();
	//写入配置
	stateLableControl.SetWindowTextW(TEXT("写入配置......"));
	processContorl.SetPos(0);
	CString str;
	str.Format(TEXT("%d"), comboPattern.GetCurSel());
	WritePrivateProfileString(TEXT("config"), TEXT("mode"), str, projectPath);
	editPercent.GetWindowTextW(str);
	WritePrivateProfileString(TEXT("config"), TEXT("percent"), str, projectPath);
	str.Format(TEXT("%d"), checkBox.GetCheck());
	WritePrivateProfileString(TEXT("config"), TEXT("subFile"), str, projectPath);
	str.Format(TEXT("%d"), match.getDir().size());
	WritePrivateProfileString(TEXT("config"), TEXT("pathCount"), str, projectPath);
	//写入路径
	CString path;
	stateLableControl.SetWindowTextW(TEXT("写入路径......"));
	processContorl.SetRange32(0, match.getDir().size() - 1);
	for (int i = 0; i < match.getDir().size(); ++i)
	{
		listFolder.GetText(i, path);
		str.Format(TEXT("path%d"), i);
		WritePrivateProfileString(TEXT("path"), str, path, projectPath);
		processContorl.SetPos(i);
	}
	//写入类型过滤器
	str.Format(TEXT("%d"), match.getType().size());
	WritePrivateProfileString(TEXT("config"), TEXT("typeCount"), str, projectPath);
	std::vector<std::string> typeSet = match.getType();
	for (int i = 0; i < typeSet.size(); i++)
	{
		str.Format(TEXT("type%d"), i);
		USES_CONVERSION;
		WritePrivateProfileString(TEXT("type"), str, A2W(typeSet[i].c_str()), projectPath);
	}
	stateLableControl.SetWindowTextW(TEXT("保存完毕"));
}


void CSimilarityMapMatchingDlg::OnStnClickedButClose()
{
	// TODO: 在此添加控件通知处理程序代码
	if (ifOpen == false)
	{
		MessageBox(TEXT("未打开工程"), TEXT("提示"), MB_OK | MB_ICONASTERISK);
		return;
	}
	if (isMatching == true)
	{
		MessageBox(TEXT("正在匹配,请稍等......"), TEXT("提示"), MB_OK);
		return;
	}
	//关闭工程！！！！！！！！！！！
	ifOpen = false;
	//初始化
	comboPattern.SetCurSel(0);
	editPercent.SetWindowTextW(TEXT("70"));
	checkBox.SetCheck(true);
	//初始化match
	match.setMode(PATTERN_AVERAGE_HASH);
	match.setRate(0.70);
	match.findSubDir();
	match.resetDir();
	match.clearType();
	listFolder.ResetContent();
	listResult.DeleteAllItems();
	stateLableControl.SetWindowTextW(TEXT("关闭工程"));
	processContorl.SetPos(0);
}


void CSimilarityMapMatchingDlg::OnEnChangeEditPercent()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString str;
	editPercent.GetWindowText(str);
	double percent = _ttof(str) / 100;
	if (percent > 1)
		match.setRate(1);
	else if (percent < 0)
		match.setRate(0);
	else match.setRate(percent);
}


void CSimilarityMapMatchingDlg::OnCbnSelchangeComboPattern()
{
	// TODO: 在此添加控件通知处理程序代码
	match.setMode(comboPattern.GetCurSel());
}


void CSimilarityMapMatchingDlg::OnBnClickedCheckSubdir()
{
	// TODO: 在此添加控件通知处理程序代码
	match.findSubDir(checkBox.GetCheck());
}


void CSimilarityMapMatchingDlg::OnNMClickListResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	POSITION pos = listResult.GetFirstSelectedItemPosition();
	if(pos)
	{
		int sel = listResult.GetNextSelectedItem(pos);
		CString groupStr = listResult.GetItemText(sel, 3);
		int group = _ttoi(groupStr);
		//获取分组第一个
		int parent = sel;
		while (parent >= 0 && group == _ttoi(listResult.GetItemText(parent - 1, 3)))
			parent--;
		CString parentPath = listResult.GetItemText(parent, 1);
		CString selPath = listResult.GetItemText(sel, 1);
		//绘制
		CWnd *first = GetDlgItem(IDC_PIC_FISRST);
		CWnd *second = GetDlgItem(IDC_PIC_SECOND);
		RECT fRect, sRect;
		first->GetClientRect(&fRect);
		second->GetClientRect(&sRect);
		USES_CONVERSION;
		cv::Mat imgf, imgs;
		imgf = cv::imread(W2A(parentPath));
		imgs = cv::imread(W2A(selPath));
		cv::resize(imgf, imgf, cv::Size(fRect.right - fRect.left, fRect.bottom - fRect.top));
		cv::resize(imgs, imgs, cv::Size(sRect.right - sRect.left, sRect.bottom - sRect.top));
		cv::imshow("first", imgf);
		cv::imshow("second", imgs);
	}
}


void CSimilarityMapMatchingDlg::OnStnClickedButEnd2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (ifOpen == false)
	{
		MessageBox(TEXT("未打开工程"), TEXT("提示"), MB_OK | MB_ICONASTERISK);
		return;
	}
	if (isMatching == true)
	{
		MessageBox(TEXT("正在匹配,请稍等......"), TEXT("提示"), MB_OK);
		return;
	}
	typeFitDialog dlg(nullptr);
	dlg.setMatch(&match);
	if (dlg.DoModal() == IDOK)
	{
		std::set<std::string> typeSet = dlg.getSet();
		match.resetTyep(typeSet);
	}
}


BOOL CSimilarityMapMatchingDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return true;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
