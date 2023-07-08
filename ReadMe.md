# VulnInfoSearch

- [ ] VulnInfoSearch
	- [ ] 函数名  
		- [ ] Joern: 完整的函数体
	- [ ] 原始 commit - file 对应
	- [ ] CPP/C 分类问题
		- [ ] Github 查询语句解决
	- ENV:
		- clang
		- javalang

## Todo-List
- [ ] Download commit files
	- https://github.com/{owner}/{repo}/blob/{commit-hashcode}/{file_path_and_file_name}
	- https://github.com/krb5/krb5/blob/ea92d2f0fcceb54a70910fa32e9a0d7a5afc3583/src/lib/krb5/krb/pac.c
	- likes:
		-	commit:	https://github.com/krb5/krb5/commit/ea92d2f0fcceb54a70910fa32e9a0d7a5afc3583
		-   parent:	https://github.com/krb5/krb5/commit/fa62bd33a0c0889c083999c0289ffa81a5d51e7b
		- commit_file:
			- commit: https://github.com/krb5/krb5/blob/ea92d2f0fcceb54a70910fa32e9a0d7a5afc3583/src/lib/krb5/krb/pac.c
			- parent: https://github.com/krb5/krb5/blob/fa62bd33a0c0889c083999c0289ffa81a5d51e7b/src/lib/krb5/krb/pac.c
- [ ] Test TianXiao's Programs
- [ ] Get the whole Code. 


# 设计思路

目前的需求如下(按照需求和优先级排序):
- 测试师姐的脚本
- 能否通过正则匹配的方式，去匹配函数体
- 需要下载这些文件