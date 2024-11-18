# libzmq编译
## 编译环境:
+ MSVC2017
+ CMAKE 3.23.0
## 下载源码
https://github.com/zeromq/libzmq/archive/refs/tags/v4.3.4.zip
解压
## 使用cmake进行configure操作
1. 选择好源码目录和输出目录
2. 点击Generate，选择Visual Studio 15 2017
3. 根据需求选择x64或者x86
4. 点击finish
5. 完成后打开输出目录
6. 找到ZeroMQ.sln，用VS打开
7. 生成解决方案
8. 库文件位于：输出目录\bin\Debug\libzmq-v141-mt-gd-4_3_4.dll
			输出目录\lib\Debug\libzmq-v142-mt-gd-4_3_4.lib
			输出目录\bin\Debug\libzmq-v142-mt-sgd-4_3_4.lib
9. 头文件位于：源码目录\include
