1.
在当前目录打开Windows powershell（管理员）
注册虚拟摄像头
regsvr32 ./VirtualCamera.dll
卸载虚拟摄像头
regsvr32 /u ./VirtualCamera.dll
2.
注册成功或者卸载成功都会有对话框弹出提示。
3.
注册后即可在设备列表中出现Nama Virtual Camera。
运行FULive.exe，打开虚拟化会自动传输数据到虚拟摄像头。
