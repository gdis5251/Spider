#include "Spider.hpp"

void Welcome()
{
	std::cout << "********************************" << std::endl;
	std::cout << "*  欢迎使用 Gerald's 爬虫程序  *" << std::endl;
	std::cout << "********************************" << std::endl;
	std::cout << "请输入想要抓取的 URL：";

}

int main(void)
{
	Welcome();
	
	std::string url;
	std::cin >> url;

	// 创建文件夹
	CreateDirectory("./images", NULL);

	// 开始抓取
	StartCatch(url);


	return 0;
}

