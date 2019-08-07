#define _CRT_SECURE_NO_WARNINGS
#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <string>
#include <Windows.h>
#include <queue>
#include <cstring>
#include <regex>
#include <cstdio>

class CHttp
{
public:
	CHttp()
	{}

	~CHttp()
	{}

	bool Init()
	{
		WSADATA wd;
		if (0 != WSAStartup(MAKEWORD(2, 2), &wd))
		{
			return false;
		}

		if (LOBYTE(wd.wVersion) != 2 || HIBYTE(wd.wVersion) != 2)
		{
			return false;
		}

		socket_ = socket(AF_INET, SOCK_STREAM, 0);

		if (socket_ == INVALID_SOCKET)
		{
			return false;
		}

		return true;
	}

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
			obj_ = url.substr(pos);
		}

		// 判断是否截取到主机
		if (host_.empty())
		{
			return false;
		}

		// TODO
		std::cout << "Host: " << host_.c_str() << std::endl;
		std::cout << "Path: " << obj_.c_str() << std::endl;

		return true;
	}


	bool Connect()						// 连接服务器
	{
		Init();

		// 域名到ip地址的转换
		hostent* p = gethostbyname(host_.c_str());
		if (NULL == p)
		{
			return false;
		}


		// 连接服务器
		sockaddr_in sa;
		memcpy(&sa.sin_addr, p->h_addr, 4);
		sa.sin_family = AF_INET;
		sa.sin_port = htons(80);

		if (SOCKET_ERROR == connect(socket_, (sockaddr*)& sa, sizeof(sa)))
		{
			return false;
		}

		return true;
	}


	bool FetchHtml(std::string& html)	// 获取网页
	{
		// 给服务器发送 GET 请求
		std::string info;
		info += "GET " + obj_ + " HTTP/1.1\r\n";
		info += "Host: " + host_ + "\r\n";
		info += "Connection: Close\r\n\r\n";

		// 发送给服务器
		if (SOCKET_ERROR == send(socket_, info.c_str(), info.size(), 0))
		{
			return false;
		}

		char ch = 0;
		while (recv(socket_, &ch, sizeof(ch), 0) > 0)
		{
			html += ch;
		}

		return true;
	}


	bool AnalyseHtml(const std::string& html, std::vector<std::string>& vecImage, std::queue<std::string>& q)					// 解析网页
	{
		std::smatch mat;
		// 正则表达式
		std::regex rex("http://[^\\s'\"<>()]+");

		std::string::const_iterator start = html.begin();
		std::string::const_iterator end = html.end();

		while (regex_search(start, end, mat, rex))
		{
			std::string per(mat[0].first, mat[0].second);

			if (per.find(".jpg") != std::string::npos ||
				per.find(".png") != std::string::npos ||
				per.find(".jpeg") != std::string::npos)
			{
				vecImage.push_back(per);
			}
			else
			{
				// 去除 w3c
				if (per.find("http://www.w3.org/") == std::string::npos)
				{
					q.push(per);
				}
			}

			start = mat[0].second;
		}

		return true;
	}


	bool Download(std::string& url, std::string& filename)	// 下载资源
	{
		if (!AnalyseUrl(url))
		{
			return false;
		}
		if (!Connect())
		{
			return false;
		}

		// 给服务器发送 GET 请求
		std::string info;
		info += "GET " + obj_ + " HTTP/1.1\r\n";
		info += "Host: " + host_ + "\r\n";
		info += "Connection: Close\r\n\r\n";

		// 发送给服务器
		if (SOCKET_ERROR == send(socket_, info.c_str(), info.size(), 0))
		{
			return false;
		}

		// 打开文件
		FILE* fp = fopen(filename.c_str(), "wb");
		if (fp == nullptr)
		{
			return false;
		}

		// 接收头信息
		char ch = 0;
		while (recv(socket_, &ch, 1, 0))
		{
			// 连续换了两行表示接收完了 \r\n\r\n
			if (ch == '\r')
			{
				recv(socket_, &ch, 1, 0);
				if (ch == '\n')
				{
					recv(socket_, &ch, 1, 0);
					if (ch == '\r')
					{
						recv(socket_, &ch, 1, 0);
						if (ch == '\n')
						{
							break;
						}
					}
				}
			}
		}
		// 接收数据
		while (recv(socket_, &ch, sizeof(ch), 0) > 0)
		{
			fwrite(&ch, 1, 1, fp);
		}

		// 关闭文件
		fclose(fp);

		return true;
	}

	bool Close()
	{
		closesocket(socket_);
		host_.clear();
		obj_.clear();
		socket_ = 0;

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
	SOCKET socket_;	   // 客户端套接字
};


// 开始抓取
bool StartCatch(const std::string& url)
{
	// URL 队列
	std::queue<std::string> q;
	q.push(url);
	CHttp http;

	while (!q.empty())
	{
		// 抓取
		// 1. 取出当前队首的 URL
		std::string c_url = q.front();
		q.pop();

		// 2. 解析 URL
		http.AnalyseUrl(c_url);

		// 3. 连接服务器
		if (http.Connect())
		{
			std::cout << "连接成功！" << std::endl;
		}
		else
		{
			std::cout << "连接失败！" << std::endl;
			return false;
		}

		// 4. 获取网页
		std::string html;
		http.FetchHtml(html);

		// 存放图片链接
		std::vector<std::string> vecImage;

		// 5. 解析网页内容
		http.AnalyseHtml(html, vecImage, q);
		http.Close();

		// 6. 下载图片
		CHttp download;
		for (int i = 0; i < vecImage.size(); i++)
		{
			std::string filename = "D:/学习/项目/Spider/Spider/images/" + vecImage[i].substr(vecImage[i].find_last_of('/') + 1);

			if (!download.Download(vecImage[i], filename))
			{
				std::cout << "下载失败！" << std::endl;
			}
			else
			{
				std::cout << "下载成功！" << std::endl;
			}
		}
	}


	return true;
}