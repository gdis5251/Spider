#pragma once
#include <iostream>
#include <string>
#include <Windows.h>
#include <queue>
#include <cstring>

// 开始抓取
bool StartCatch(const std::string& url);


class CHttp
{
public:
	CHttp();
	~CHttp();

	bool AnalyseUrl(const std::string& url)					// 解析 URL
	{
		// 字符串大小写转换，全部转换为小写
		// 将 http/https://www.baidu.com/ 这些字符全部转成小写

		int loc = 0;
		if (std::string::npos != url.find("http://"))
		{
			loc = 7;
		}
		else if (std::string::npos != url.find("https://"))
		{
			loc = 8;
		}
		else
		{
			return false;
		}

		// double check 如果长度小于 7  那是不可能找到 http://
		if (url.length() <= 7)
		{
			return false;
		}

		// 截取网址
		int pos = url.find('/', loc);
		if (pos == std::string::npos) //  http://www.baidu.com
		{
			host_ = url.substr(7);
			obj_ = "/";
		}
		else
		{
			host_ = url.substr(loc, pos - loc);
			obj_ = url.substr(pos + 1);
		}

		// 判断是否截取到主机
		if (host_.empty())
		{
			return false;
		}

		std::cout << "Host: " << host_.c_str() << std::endl;
		std::cout << "Path: " << obj_.c_str() << std::endl;

		return true;
	}


	bool Connect()						// 连接网页
	{
		return true;
	}


	bool FetchHtml(std::string& html)	// 获取网页
	{
		return true;
	}


	bool AnalyseHtml()					// 解析网页
	{

		return true;
	}


	bool Download(std::string& url, std::string& filename)	// 下载资源
	{
		
		return true;
	}

private:
	std::string& ToLower(std::string& url)
	{
		// 找到 http/https://www.xxx.com/ 这段字符串
		int pos = url.find('/', 8);
		int sz = 0;

		if (pos == std::string::npos)
		{
			sz = url.size();
		}
		else
		{
			sz = pos;
		}

		for (int i = 0; i < sz; i++)
		{
			url[i] = tolower(url[i]);
		}

		return url;
	}

private:
	std::string host_; // 主机名
	std::string obj_;  // 资源路径
};