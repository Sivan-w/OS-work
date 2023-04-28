#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_USER_NUM 4
#define MAX_NAME_LENGTH 20
//记录结构体
typedef struct record
{
	char name[MAX_NAME_LENGTH];
	char sex[7];
	struct record* next;
} Record;

//用户文件
typedef struct user_file
{
	char name[MAX_NAME_LENGTH];
	int length;
	Record* r_head;			//指向该文件的记录链表的头
	struct user_file* next; //指向下一个文件
} User_file;

//主文件
typedef struct master_file
{
	char username[MAX_NAME_LENGTH];
	User_file* ufd_head;
} Master_file;

//活跃文件
typedef struct active_file
{
	char name[MAX_NAME_LENGTH];
	int length;
	bool flag;				  //修改位
	Record* r_head;			  //指向该打开的文件的目录的链表的头
	struct active_file* next; //指向下一个打开的文件
} Active_file;

void init();										//初始化
void new_file();									//创建文件
void delete_file();									//删除文件
void open_file();									//打开文件
void close_file();									//关闭文件
void close_file1(char* filename);					//辅助关闭文件
void read_file();									//读取记录
void write_file();									//写入记录
void display_root();								//显示主目录
void display_directory();							//显示用户目录
void display_file();								//显示文件全部内容
void close_user();									//关闭用户
void show_menu();									//显示菜单
void create_user(int n);							//创建用户，并检测用户名重复，n是创建的总个数
void choose_user();									//选择用户，并检测用户名是否存在
void add_user_file(User_file* file);				//新增用户文件
void add_active_file(Active_file* file);			//新增打开文件
void add_record(Active_file* file, Record* record); //新增记录
User_file* find_user_file(char* filename);			//查找文件是否存在
Active_file* find_active_file(char* filename);		//查找文件是否打开
void rename_file();

//全局变量
Master_file mfd[MAX_USER_NUM]; //主文件目录
Active_file* afd = NULL;	   //打开的文件列表
int now_user = 0;			   //选中的用户
int user_num = 0;			   //现有用户数量

int main()
{
	int cmd;
	init();
	while (1)
	{
		show_menu();
		printf("\n选择功能: ");
		scanf("%d", &cmd);
		getchar();				//去除回车干扰 
		switch (cmd)
		{
		case 1:
			new_file();
			break;
		case 2:
			open_file();
			break;
		case 3:
			delete_file();
			break;
		case 4:
			close_file();
			break;
		case 5:
			read_file();
			break;
		case 6:
			write_file();
			break;
		case 7:
			display_directory();
			break;
		case 8:
			display_file();
			break;
		case 9:
			close_user();
			break;
		case 10:
			rename_file();
			break;
		
		case 0:
			exit(EXIT_SUCCESS);
			break;
		default:
			printf("\n-- 无效输入! --\n");
		}
		system("pause");
		system("cls");
	}
}

void init()
{
	//创建用户
	int n;
	printf("----------------------------------------------\n");
	printf("------------- 欢迎来到文件管理系统------------\n");
	printf("----------------------------------------------\n");
	while (1)
	{
		printf("\n请输入要创建的用户数量(0<n<=%d): ", MAX_USER_NUM);
		scanf("%d", &n);
		user_num = n;
		if (n > 0 && n < MAX_USER_NUM)
			break;
		printf("\n-- 无效输入! --\n");
	}
	create_user(n);
}

void create_user(int n)
{
	int i;
	char name[MAX_NAME_LENGTH];
	for (i = 0; i < n; i++)
	{
		printf("\n请输入第%d个用户的用户名: ", i + 1);
		do
		{ //防止输入空白名
			gets(name);
		} while (!name[0]);
		for (int j = 0; j < i; j++)
		{ //遍历寻找该用户名
			if (!strcmp(mfd[j].username, name))
			{ //防止名称重复
				printf("\n-- 用户名已存在! --\n");
				i--;
			}
		}
		strcpy(mfd[i].username, name); //存入姓名
		mfd[i].ufd_head = NULL;
	}
	printf("\n-- %d个用户创建成功 --\n", i);
	choose_user();
}

void choose_user()
{
	char name[MAX_NAME_LENGTH];
	int i;
	printf("\n选择一个你想要操作的用户名：");
	do
	{ //防止输入空白名
		gets(name);
	} while (!name[0]);
	for (i = 0; i < user_num; i++)
	{ //遍历寻找该用户
		if (!strcmp(name, mfd[i].username))
		{
			system("cls");
			printf("\n-- 已选中用户%s --\n", name);
			now_user = i;
			return;
		}
	}
	printf("\n-- 用户%s不存在,请重新输入！ --", name);
	choose_user();
}

void show_menu()
{
	printf("\n\
=============================================\n\
= 1--新建文件\t2--打开文件\t3--删除文件 =\n\
= 4--关闭文件\t5--读取文件\t6--写入文件 =\n\
= 7--显示目录\t8--显示文件\t9--关闭用户 =\n\
= 10-重命名   =\n\
= \t\t 0--退出\t\t    =\n\
=============================================\n");
}

void new_file()
{
	char name[MAX_NAME_LENGTH];
	User_file* file;

	printf("\n输入你要创建的文件名(输入数字0返回): ");
	do
	{ //防止输入空白名
		gets(name);
	} while (!name[0]);
	if (!strcmp(name, "0"))
		return;

	//检测文件名是否存在
	if (find_user_file(name) != NULL)
	{
		printf("\n-- 文件名已存在! --\n");
		new_file();
		return; //存在则重新输入
	}

	//创建文件信息 
	file = (User_file*)malloc(sizeof(User_file));
	strcpy(file->name, name);
	file->length = 0;
	file->r_head = NULL;
	file->next = NULL;
	add_user_file(file);//加入到ufd中 
	printf("\n-- 创建成功！ --\n");
}

void add_user_file(User_file* file)
{
	User_file* p = mfd[now_user].ufd_head;
	if (p == NULL) //用户文件目录为空，直接插入
	{
		mfd[now_user].ufd_head = file;
		return;
	}
	while (p->next != NULL)  //  链表遍历
	{
		p = p->next;
	}
	p->next = file;
}

void open_file()
{
	char name[MAX_NAME_LENGTH];
	Active_file* active_file;
	User_file* user_file;

	printf("\n输入你要打开的文件名(输入数字0返回): ");
	do
	{ //防止输入空白名
		gets(name);
	} while (!name[0]);
	if (!strcmp(name, "0"))   //  判断是否输入为0，为0回退
		return;

	//检测文件是否存在
	if ((user_file = find_user_file(name)) == NULL)
	{
		printf("\n-- 文件未找到! --\n");
		open_file();
		return; //存在则重新输入
	}

	//检测文件是否已打开
	if (find_active_file(name) != NULL)
	{
		printf("\n-- 文件已经打开! --\n");
		open_file();
		return; //存在则重新输入
	}

	//新键表目
	active_file = (Active_file*)malloc(sizeof(Active_file));
	strcpy(active_file->name, user_file->name);
	active_file->length = user_file->length;
	active_file->r_head = user_file->r_head;
	active_file->next = NULL;
	active_file->flag = 0;
	add_active_file(active_file);
	printf("\n-- 打开成功！ --\n");
}

void add_active_file(Active_file* file)
{
	Active_file* p = afd;
	if (p == NULL) //用户文件目录为空，直接插入
	{
		afd = file;
		return;
	}
	while (p->next != NULL)
	{
		p = p->next;
	}
	p->next = file;
}

User_file* find_user_file(char* filename)
{
	User_file* p = mfd[now_user].ufd_head;
	if (p == NULL)		//ufd为空 
	{
		return NULL;
	}
	do
	{
		if (!strcmp(p->name, filename))
		{
			return p;	//找到文件 
		}
		p = p->next;
	} while (p != NULL);
	return NULL;		//没有找到文件 
}

Active_file* find_active_file(char* filename)
{
	Active_file* p = afd;
	if (p == NULL)
	{
		return NULL;	//afd为空 
	}
	do
	{
		if (!strcmp(p->name, filename))
		{
			return p;	//找到文件 
		}
		p = p->next;
	} while (p != NULL);
	return NULL;		//没有找到文件 
}

void delete_file()
{
	char name[MAX_NAME_LENGTH];
	User_file* user_file, * cur, * prev;

	printf("\n输入你要删除的文件名(输入数字0返回): ");
	do
	{ //防止输入空白名
		gets(name);
	} while (!name[0]);
	if (!strcmp(name, "0"))
		return;

	//检测文件是否已打开
	if (find_active_file(name) != NULL)
	{
		close_file1(name); //关闭文件
	}

	//检测文件名是否存在
	if (find_user_file(name) == NULL)
	{
		printf("\n-- 文件不存在! --\n");
		delete_file();
		return; //存在则重新输入
	}
	//从ufd中删除
	if (mfd[now_user].ufd_head->next == NULL) //只有一个用户文件
	{
		user_file = mfd[now_user].ufd_head;
		mfd[now_user].ufd_head = NULL;
	}
	else
	{ //不止一个文件
		//找到节点位置
		for (prev = cur = mfd[now_user].ufd_head; cur != NULL && strcmp(cur->name, name) != 0; prev = cur, cur = cur->next)
			;
		user_file = cur;
		prev->next = cur->next;
	}
	free(user_file);
	printf("\n-- 删除成功！ --\n");
}

void close_file()
{
	char name[MAX_NAME_LENGTH];
	printf("\n输入你要关闭的文件名(输入数字0返回): ");
	do
	{ //防止输入空白名
		gets(name);
	} while (!name[0]);
	if (!strcmp(name, "0"))
		return;
	close_file1(name);
}

void close_file1(char* filename)
{
	Active_file* active_file, * cur, * prev;
	User_file* user_file;

	//检测文件是否已打开
	if (find_active_file(filename) == NULL)
	{
		printf("\n-- 文件未找到! --\n");
		close_file();
		return; //存在则重新输入
	}

	//从afd中删除
	if (afd->next == NULL) //只有一个文件被打开
	{
		active_file = afd;
		afd = NULL;
	}
	else
	{ //不止一个文件
		for (prev = cur = afd; cur != NULL && strcmp(cur->name, filename) != 0; prev = cur, cur = cur->next);
		active_file = cur;
		prev->next = cur->next;
	}

	//判断标志位
	if (active_file->flag == true)
	{
		user_file = find_user_file(filename);
		user_file->length = active_file->length;
		user_file->r_head = active_file->r_head;
	}
	free(active_file);
	printf("\n-- 关闭成功! --\n");
}

void read_file()
{
	char name[MAX_NAME_LENGTH];
	Active_file* active_file;
	Record* record, * p;
	int num;

	printf("\n输入你要读取的文件名(输入数字0返回): ");
	do
	{ //防止输入空白名
		gets(name);
	} while (!name[0]);
	if (!strcmp(name, "0"))
		return;

	//检测文件是否已打开
	if ((active_file = find_active_file(name)) == NULL)
	{
		printf("\n-- 文件未打开! --\n");
		read_file();
		return; //存在则重新输入
	}
	printf("\n输入记录号: ");
	scanf("%d", &num);

	//查找记录号
	p = active_file->r_head;
	for (int i = 0; i < num; i++)
	{
		if (p == NULL)
		{
			printf("\n-- 没有找到记录 --\n");
			read_file();
			return;
		}
		p = p->next;
	}
	printf("\n\
\t记录号  \t姓名\t性别\n\
\t-----------------------------\n\
\t%d\t\t%s\t%s\n\
\t-----------------------------\n",
num, p->name, p->sex);
}

void write_file()
{
	char name[MAX_NAME_LENGTH];
	Active_file* active_file;
	User_file* user_file;
	Record* record;
	int gender;

	printf("\n输入你要写入的文件名(输入数字0返回): ");
	do
	{ //防止输入空白名
		gets(name);
	} while (!name[0]);
	if (!strcmp(name, "0"))
		return;

	//检测文件是否已打开
	if ((active_file = find_active_file(name)) == NULL)
	{
		printf("\n-- 文件未打开! --\n");
		write_file();
		return; //存在则重新输入
	}

	record = (Record*)malloc(sizeof(Record));
	printf("\n输入姓名: ");
	do
	{ //防止输入空白名
		gets(record->name);
	} while (!record->name[0]);

	while (1)
	{
		printf("\n输入性别(1-男/2-女): ");
		scanf("%d", &gender);
		if (gender == 1)
		{
			strcpy(record->sex, "男");
			break;
		}
		else if (gender == 2)
		{
			strcpy(record->sex, "女");
			break;
		}
		else
		{
			printf("\n-- 输入格式有误！ --\n");
		}
	}
	record->next = NULL;
	add_record(active_file, record);
	active_file->length++;  //
	active_file->flag = true;
	printf("\n-- 写入成功 --\n");
}

void add_record(Active_file* active_file, Record* record)
{
	Record* p = active_file->r_head;
	if (p == NULL) //用户文件目录为空，直接插入
	{
		active_file->r_head = record;
		return;
	}
	while (p->next != NULL)
	{
		p = p->next;
	}
	p->next = record;
}

void display_directory()
{
	//udf为空
	if (mfd[now_user].ufd_head == NULL)
	{
		printf("\n-- 用户文件为空！ --\n");
		return;
	}
	printf("\n\
\t用户名: %s\n\
\t-------------------------\n\
\t文件名  \t文件长度\n",
mfd[now_user].username);
	for (User_file* p = mfd[now_user].ufd_head; p != NULL; p = p->next)
	{
		printf("\t  %s\t\t%4d\n", p->name, p->length);
	}
	printf("\t-------------------------\n");
}

void display_file()
{
	char name[MAX_NAME_LENGTH];
	Active_file* active_file;
	Record* p;

	printf("\n输入你要显示的文件名(输入数字0返回): ");
	do
	{ //防止输入空白名
		gets(name);
	} while (!name[0]);
	if (!strcmp(name, "0"))
		return;

	//检测文件是否已打开
	if ((active_file = find_active_file(name)) == NULL)
	{
		printf("\n-- 文件未打开! --\n");
		write_file();
		return; //存在则重新输入
	}
	if (active_file->r_head == NULL)
	{
		printf("\n-- 记录为空！ --\n");
		return;
	}
	printf("\n\
\t记录号  \t姓名\t性别\n\
\t-----------------------------\n");
	p = active_file->r_head;
	for (int i = 0; p != NULL; p = p->next, i++)
	{
		printf("\t%3d\t\t%s\t %s\n", i, p->name, p->sex);
	}
	printf("\t-----------------------------\n");
}

void close_user()
{
	Active_file* p = afd;
	//关闭所有打开文件
	while (p != NULL)
	{
		close_file1(p->name);
		p = p->next;
	}
	display_root(); //显示主目录
	choose_user();	//选择用户
}

void display_root()
{
	printf("\n\
\t 用户列表\n\
\t----------\n");
	for (int i = 0; i < user_num; i++)
	{
		printf("\t   %s\n", mfd[i].username);
	}
	printf("\t----------\n");
}

void rename_file() {
	char username[MAX_NAME_LENGTH], filename[MAX_NAME_LENGTH];
	User_file* user_file;
	printf("\n输入要重命名的文件名(输入数字0返回): ");
	do
	{ //防止输入空白名
		gets(filename);
	} while (!filename[0]);
	if (!strcmp(filename, "0"))
		return;
	user_file = find_user_file(filename);
	printf("请输入修改后的文件名");
	do
	{ //防止输入空白名
		gets(filename);
	} while (!filename[0]);
	strcpy(user_file->name, filename);
	printf("\n-- 修改完成！ --\n");
}