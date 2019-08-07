# C++ 爬虫程序

## 项目描述

使用 C++ 编程语言，实现一个 爬虫程序，来对图片进行下载。

## 开发平台

Windows 10

## 流程介绍

![在这里插入图片描述](https://img-blog.csdnimg.cn/2019080714230368.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjY3ODUwNw==,size_16,color_FFFFFF,t_70)

1. 先输入一个起始 URL 进入 URL 队列。

2. 在当前路径下创建一个文件夹。

   ```cpp
   #include <Windows.h>
   
   CreateDirectory("./images", NULL);
   ```

3. 获取到当前队列中首部的 URL。

4. 解析 URL，主要作用是判断是否是正确的 URL，并且获取到服务器域名和资源路径。

5. 连接服务器。

   ```cpp
   gethostbyname(std::string); // 通过这个函数获取到服务器的 IP 地址 
   ```

6. 获取网页源码。

   向服务器发送 HTTP 请求。获取到网页源码。

7. 使用正则表达式解析网页源码。

   - 如果是图片 URL 就放进先前创建好的数组中。
   - 如果是其他 URL 就放进队列中，让其后续循环处理。

8. 下载图片。

   循环遍历刚才存放图片的数组，重新对图片的 URL 建立连接和获取源码。

   去除服务器返回的 HTTP 头部信息，将正文信息写到一个新创建的文件里，放到之前创建的文件夹。