1、编译：
	1) 执行./bulid/zbwosbuild.sh 2440
	2) 会在./bulid下生成 zbwos.bin, 可在2440里运行

2、平台扩展(假设扩展Plat平台)：
	1) 在./soc中新增名为Plat的平台文件夹，并在里面实现所有./initsoc.c调用接口
	2) 在./bulid中新增并适配makefile_Plat