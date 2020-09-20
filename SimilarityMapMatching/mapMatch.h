#pragma once

#include <vector>
#include <set>
#include <string>
#include <Windows.h>
#include <opencv2\opencv.hpp>
#include <opencv2\xfeatures2d.hpp>

#define PATTERN_AVERAGE_HASH 0
#define PATTERN_PERCEPTUAL_HASH 1
#define PATTERN_DIFFERENT_HASH 2
#define PATTERN_COLOR_HISTOGRAM 3
#define PATTERN_IMG_CHARACTERISTIC 4
#define PATTERN_SIFT 5

struct FindResult
{
	std::string parentFile;
	std::vector<std::string> subFile;
	std::vector<double> percent;
};

struct HashPath
{
	std::string hash;
	std::string path;
	HashPath(std::string hash, std::string path)
	{
		this->hash = hash;
		this->path = path;
	}
};

struct HistoramPath
{
	std::vector<int> historam;
	std::string path;
	HistoramPath(std::vector<int> historam, std::string path)
	{
		this->historam = historam;
		this->path = path;
	}
};

struct CharacteristicPath
{
	std::vector<int> characteristic;
	std::string path;
	CharacteristicPath(std::vector<int> characteristic, std::string path)
	{
		this->characteristic = characteristic;
		this->path = path;
	}
};

struct SiftPath
{
	cv::Mat img;
	std::string path;
	SiftPath(cv::Mat img, std::string path)
	{
		this->img = img;
		this->path = path;
	}
};

class mapMatch
{
private:
	//文件夹
	std::vector<std::string> dirList;
	std::vector<std::string> fileList;
	bool ifFindSubDir;	//是否查找子目录
	//目录列表
	std::vector<std::string> typeList;
	//匹配
	int pattern;
	double rate;
	//内部操作
	void getAllFile();
	//特征提取
	std::string averageHash(cv::Mat img);//图片hash 返回空字符串即失败
	std::string perceptualHash(cv::Mat img);
	std::string differentHash(cv::Mat img);
	std::vector<int> colorHistogram(cv::Mat img);//颜色直方图
	std::vector<int> characteristic(cv::Mat img);//内容特征
	cv::Mat sift(cv::Mat img);//sift
	//数学计算
	double siftMatch(cv::Mat v1, cv::Mat v2);
	double notOr(std::vector<int> v1, std::vector<int> v2);//异或比较
	double correlation(std::vector<int> v1, std::vector<int> v2);//比较向量
	int hanmingDistance(std::string hash1, std::string hash2);//汉明距离
public:
	mapMatch();
	void addType(std::string type);
	void resetTyep(std::set<std::string> set);
	void clearType();
	std::vector<std::string>& getType();
	void findSubDir(bool ifFindSubDir = true);
	void setMode(int mode);
	void setRate(double x);
	void addDir(std::string path);
	std::vector<std::string> getDir();
	void removeDir(std::string path);
	void resetDir();
	std::vector<FindResult> matching(CProgressCtrl* ptr, CStatic *ptr1);
};
