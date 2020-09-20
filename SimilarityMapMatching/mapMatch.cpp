#include "stdafx.h"
#include "mapMatch.h"

void mapMatch::getAllFile()
{
	fileList.clear();
	HANDLE fh;
	WIN32_FIND_DATAA fd;
	std::set<std::string> set;
	std::vector<std::string> dirList = this->dirList;
	for (int i = 0; i < dirList.size(); i++)
	{
		fh = FindFirstFileA((dirList[i]+std::string("\\*.*")).c_str(), &fd);
		do
		{
			if (strcmp(".", fd.cFileName) == 0 || strcmp("..", fd.cFileName) == 0)
				continue;
			if (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				if (ifFindSubDir)
					dirList.push_back(dirList[i] + std::string("\\") + std::string(fd.cFileName));
				continue;
			}
			//set去重！！！！！！！！！！
			std::string fullPath = dirList[i] + std::string("\\") + std::string(fd.cFileName);
			if (set.find(fullPath) == set.end())
			{
				//类型匹配
				CString fileName = CString(fd.cFileName);
				CString type = fileName.Right(fileName.GetLength() - fileName.ReverseFind('.'));
				bool isInList = false;
				for (int i = 0; i < typeList.size(); i++)
				{
					if (CString(typeList[i].c_str()) == type)
						isInList = true;
				}
				if (isInList)
				{
					set.insert(fullPath);
					fileList.insert(fileList.begin(), fullPath);
				}
			}
		} while (FindNextFileA(fh, &fd));
	}
}

std::string mapMatch::averageHash(cv::Mat img)
{
	std::string out;
	//转换为灰度图
	if (img.channels() == 3)
		cv::cvtColor(img, img, CV_BGR2GRAY);
	else if (img.channels() != 1)//不是灰度图也不是三通道
		return std::string("");
	//改变图片大小
	cv::resize(img, img, cv::Size(8, 8));
	//转换为64级灰度图 255->64
	for (int i = 0; i < img.rows; ++i)
		for (int j = 0; j < img.cols; ++j)
			img.ptr(i)[j] /= 4;
	//获取第一个通道的平均值
	int avg = cv::mean(img).val[0];
	//平均值与像素点比较
	for (int i = 0; i < img.rows; ++i)
		for (int j = 0; j < img.cols; ++j)
			out.push_back(img.ptr(i)[j] > avg ? 1 : 0 + '0');
	return out;
}

std::string mapMatch::perceptualHash(cv::Mat img)
{
	std::string out;
	//转换为灰度图
	if (img.channels() == 3)
		cv::cvtColor(img, img, CV_BGR2GRAY);
	else if (img.channels() != 1)//不是灰度图也不是三通道
		return std::string("");
	//改变图片大小
	cv::resize(img, img, cv::Size(32, 32));
	img.convertTo(img, CV_32F);
	//离散余弦变换
	cv::dct(img,img);
	//求均值
	double avg = 0;	//8*8平均值
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			avg += img.ptr(i)[j];
	avg /= 64;
	//取hash
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			out.push_back(img.ptr(i)[j] > avg ? 1 : 0 + '0');
	return out;
}

std::string mapMatch::differentHash(cv::Mat img)
{
	std::string out;
	//转换为灰度图
	if (img.channels() == 3)
		cv::cvtColor(img, img, CV_BGR2GRAY);
	else if (img.channels() != 1)//不是灰度图也不是三通道
		return std::string("");
	//改变图片大小
	cv::resize(img, img, cv::Size(9, 8));
	//转换为64级灰度图 255->64
	for (int i = 0; i < img.rows; ++i)
		for (int j = 0; j < img.cols; ++j)
			img.ptr(i)[j] /= 4;
	//平均值与像素点比较 左 >= 右 1
	for (int i = 0; i < 8; ++i)
		for (int j = 0; j < 8; ++j)
			out.push_back(img.ptr(i)[j] >= img.ptr(i)[j + 1] ? 1 : 0 + '0');
	return out;
}

std::vector<int> mapMatch::colorHistogram(cv::Mat img)
{
	std::vector<int> out;
	out.resize(64);
	//大小
	cv::resize(img, img, cv::Size(32, 32));
	//统计颜色
	int nRow = img.rows, nCol = img.cols*img.channels();//一层一层往下存的
	uchar *p;
	for (int i = 0; i < nRow; ++i)
	{
		p = img.ptr(i);
		for (int j = 0; j < nCol; j+=3)
		{
			out[(p[j + 0] / 64) * 4 * 4 + (p[j + 1] / 64) * 4 + (p[j + 2] / 64) * 1]++;//r为层 g为线 b为点
		}
	}
	return out;
}

std::vector<int> mapMatch::characteristic(cv::Mat img)
{
	const int sizeLen = 50;
	std::vector<int> out;
	std::vector<int> histogram;
	//求阈值
	int threshold = -1;	//阈值
	int sumb = 0, sumf = 0, sumw = 0, sumPix = sizeLen * sizeLen;
	float wf = 0, wb = 0, ub = 0, uf = 0, curVal = -1, maxVal = -1;
	histogram.resize(256);
	out.resize(sumPix);
	//转换为灰度图
	if (img.channels() == 3)
		cv::cvtColor(img, img, CV_BGR2GRAY);
	else if (img.channels() != 1)//不是灰度图也不是三通道
		return out;
	//改变图片大小
	cv::resize(img, img, cv::Size(sizeLen, sizeLen));
	//统计直方图
	for (int i = 0; i < sizeLen; ++i)
		for (int j = 0; j < sizeLen; ++j)
			histogram[img.ptr(i)[j]]++;
	for (int i = 0; i < 256; i++)
		sumw += i * histogram[i];
	//找阈值
	for (int i = 0; i < 256; ++i)
	{
		//概率密度
		wb += histogram[i];
		wf = sumPix - wb;
		if (wb == 0 || wf == 0)
			continue;
		//均值
		sumb += i * histogram[i];
		sumf = sumw - sumb;
		ub = sumb / wb;
		uf = sumf / wf;
		//方差
		curVal = wb * wf*(ub - uf)*(ub - uf);
		if (curVal > maxVal)
		{
			threshold = i;
			maxVal = curVal;
		}
	}
	//转换 真·黑白图
	for (int i = 0; i < sizeLen; ++i)
		for (int j = 0; j < sizeLen; ++j)
			if (histogram[img.ptr(i)[j]] >= threshold)
				out[i * sizeLen + j] = 1;
	return out;
}

cv::Mat mapMatch::sift(cv::Mat img)
{
	const int minHessian = 100;
	std::vector<cv::KeyPoint> out;
	cv::Mat description;
	cv::Ptr<cv::Feature2D> dector = cv::xfeatures2d::SIFT::create(minHessian);
	dector->detectAndCompute(img, cv::Mat(), out, description);
	return description;
}

double mapMatch::siftMatch(cv::Mat v1, cv::Mat v2)
{
	double res;
	cv::BFMatcher bfmacher(cv::NORM_L2, true);
	std::vector<cv::DMatch> match;
	bfmacher.match(v1, v2, match);
	//统计最值
	double distMax, distMin;
	distMax = distMin = match[0].distance;
	for (int i = 0; i < match.size(); i++)
	{
		if (match[i].distance > distMax)
			distMax = match[i].distance;
		if (match[i].distance > distMax)
			distMax = match[i].distance;
	}
	//筛选匹配
	std::vector<cv::DMatch> resMatch;
	for (int i = 0; i < match.size(); i++)
	{
		if (match[i].distance < distMin * 2)
			resMatch.push_back(match[i]);
	}

	res = double(resMatch.size()) / double(match.size());
	return  res;
}

double mapMatch::notOr(std::vector<int> v1, std::vector<int> v2)
{
	double res = 0;
	if (v1.size() != v2.size())
		return -1;
	int len = v1.size();
	for (int i = 0; i < len; ++i)
		res += v1[i] ^ v2[i];
	return 1 - (res / len);
}

double mapMatch::correlation(std::vector<int> a, std::vector<int> b)
{
	double AB = 0, lenA2 = 0, lenB2 = 0;
	if (a.size() != b.size())
		return -1;
	int len = a.size();
	for (int i = 0; i < len; ++i)
	{
		AB += a[i] * b[i];
		lenA2 += a[i] * a[i];
		lenB2 += b[i] * b[i];
	}
	return AB / (sqrt(lenA2) * sqrt(lenB2));
}

int mapMatch::hanmingDistance(std::string hash1, std::string hash2)
{
	int dis = 0;
	if (hash1.size() != hash2.size())
		return -1;
	for (int i = 0; i < hash1.size(); ++i)
		dis += hash1[i] != hash2[i];
	return dis;
}

mapMatch::mapMatch()
{
	ifFindSubDir = false;
	pattern = PATTERN_COLOR_HISTOGRAM;
	rate = 1;
}

void mapMatch::addType(std::string type)
{
	typeList.push_back(type);
}

void mapMatch::resetTyep(std::set<std::string> set)
{
	typeList.clear();
	for (std::set<std::string>::iterator it = set.begin(); it != set.end(); it++)
	{
		typeList.push_back(*it);
	}
}

void mapMatch::clearType()
{
	typeList.clear();
}

std::vector<std::string>& mapMatch::getType()
{
	return typeList;
}

void mapMatch::findSubDir(bool ifFindSubDir)
{
	this->ifFindSubDir = ifFindSubDir;
}

void mapMatch::setMode(int mode)
{
	pattern = mode;
}

void mapMatch::setRate(double x)
{
	rate = x;
}

void mapMatch::addDir(std::string path)
{
	dirList.push_back(path);
}

std::vector<std::string> mapMatch::getDir()
{
	return dirList;
}

void mapMatch::removeDir(std::string path)
{
	for (int i = 0; i < dirList.size(); i++)
	{
		if (dirList[i] == path)
		{
			dirList.erase(dirList.begin() + i);
			break;
		}
	}
}

void mapMatch::resetDir()
{
	dirList.clear();
	fileList.clear();
}

std::vector<FindResult> mapMatch::matching(CProgressCtrl* ptr, CStatic *ptr1)
{
	getAllFile();
	std::vector<FindResult> res;
	switch (pattern)
	{
	case PATTERN_AVERAGE_HASH:
	case PATTERN_PERCEPTUAL_HASH:
	case PATTERN_DIFFERENT_HASH:
		{
			ptr1->SetWindowTextW(TEXT("正在读取文件......"));
			ptr->SetRange32(0,fileList.size()-1);
			ptr->SetPos(0);
			cv::Mat img;
			std::vector<HashPath> cmp;
			for (int i = 0; i < fileList.size(); ++i)
			{
				img = cv::imread(fileList[i]);
				ptr->SetPos(i);
				switch (pattern)
				{
					case PATTERN_AVERAGE_HASH:
						cmp.push_back(HashPath(averageHash(img), fileList[i]));
						break;
					case PATTERN_PERCEPTUAL_HASH:
						cmp.push_back(HashPath(perceptualHash(img), fileList[i]));
						break;
					case PATTERN_DIFFERENT_HASH:
						cmp.push_back(HashPath(differentHash(img), fileList[i]));
						break;
				}
			}
			ptr1->SetWindowTextW(TEXT("正在匹配图像......"));
			ptr->SetRange32(0, fileList.size() - 1);
			ptr->SetPos(0);
			while (!cmp.empty())
			{
				HashPath cmpHash = cmp.back();//取出一张图用于比较
				cmp.pop_back();
				ptr->SetPos(fileList.size()-cmp.size());
				FindResult subRes;//结果存储结构体
				subRes.parentFile = cmpHash.path;
				for (int it = 0; it < cmp.size();)
				{
					int dis = hanmingDistance(cmpHash.hash, cmp[it].hash);
					double percent = 1 - (dis*1.0) / (cmpHash.hash.size()*1.0);
					if (percent >= rate)
					{
						//存储相同的图
						subRes.subFile.push_back(cmp[it].path);
						subRes.percent.push_back(percent);
						cmp.erase(cmp.begin() + it);
						ptr->SetPos(fileList.size() - cmp.size());
					}
					else it++;
				}
				res.push_back(subRes);
			}
			break;
		}
	case PATTERN_COLOR_HISTOGRAM:
		{
			ptr1->SetWindowTextW(TEXT("正在读取文件......"));
			ptr->SetRange32(0, fileList.size() - 1);
			ptr->SetPos(0);
			cv::Mat img;
			std::vector<HistoramPath> cmp;
			for (int i = 0; i < fileList.size(); ++i)
			{
				img = cv::imread(fileList[i]);
				ptr->SetPos(i);
				cmp.push_back(HistoramPath(colorHistogram(img),fileList[i]));
			}
			ptr1->SetWindowTextW(TEXT("正在匹配图像......"));
			ptr->SetRange32(0, fileList.size() - 1);
			ptr->SetPos(0);
			while (!cmp.empty())
			{
				HistoramPath cmpHistoram = cmp.back();
				cmp.pop_back();
				ptr->SetPos(fileList.size() - cmp.size());
				FindResult subRes;//结果存储结构体
				subRes.parentFile = cmpHistoram.path;
				for (int it = 0; it < cmp.size();)
				{
					double percent = abs(correlation(cmpHistoram.historam, cmp[it].historam));
					if (percent >= rate)
					{
						//存储相同的图
						subRes.subFile.push_back(cmp[it].path);
						subRes.percent.push_back(percent);
						cmp.erase(cmp.begin() + it);
						ptr->SetPos(fileList.size() - cmp.size());
					}
					else it++;
				}
				res.push_back(subRes);
			}
			break;
		}
	case PATTERN_IMG_CHARACTERISTIC:
		{
			ptr1->SetWindowTextW(TEXT("正在读取文件......"));
			ptr->SetRange32(0, fileList.size() - 1);
			ptr->SetPos(0);
			cv::Mat img;
			std::vector<CharacteristicPath> cmp;
			for (int i = 0; i < fileList.size(); ++i)
			{
				img = cv::imread(fileList[i]);
				ptr->SetPos(i);
				cmp.push_back(CharacteristicPath(characteristic(img), fileList[i]));
			}
			ptr1->SetWindowTextW(TEXT("正在匹配图像......"));
			ptr->SetRange32(0, fileList.size() - 1);
			ptr->SetPos(0);
			while (!cmp.empty())
			{
				CharacteristicPath cmpHistoram = cmp.back();
				cmp.pop_back();
				ptr->SetPos(fileList.size() - cmp.size());
				FindResult subRes;//结果存储结构体
				subRes.parentFile = cmpHistoram.path;
				for (int it = 0; it < cmp.size();)
				{
					double percent = notOr(cmpHistoram.characteristic, cmp[it].characteristic);
					if (percent >= rate)
					{
						//存储相同的图
						subRes.subFile.push_back(cmp[it].path);
						subRes.percent.push_back(percent);
						cmp.erase(cmp.begin() + it);
						ptr->SetPos(fileList.size() - cmp.size());
					}
					else it++;
				}
				res.push_back(subRes);
			}
			break;
		}
	case PATTERN_SIFT://never use
		{
			cv::Mat img;
			std::vector<SiftPath> cmp;
			for (int i = 0; i < fileList.size(); ++i)
			{
				img = cv::imread(fileList[i]);
				cmp.push_back(SiftPath(sift(img), fileList[i]));
			}
			while (!cmp.empty())
			{
				SiftPath cmpSift = cmp.back();
				cmp.pop_back();
				FindResult subRes;//结果存储结构体
				subRes.parentFile = cmpSift.path;
				for (int it = 0; it < cmp.size();)
				{
					double percent = siftMatch(cmpSift.img, cmp[it].img);
					if (percent >= rate)
					{
						//存储相同的图
						subRes.subFile.push_back(cmp[it].path);
						subRes.percent.push_back(percent);
						cmp.erase(cmp.begin() + it);
					}
					else it++;
				}
				res.push_back(subRes);
			}
			break;
		}
	}
	return res;
}
