#include <stdio.h>

struct students   //结构体名
{
	char name[20];
	int age;
	char tel[15];
	char sex;
	double scores[3];

}stu, stu1, stu2 = {"王五",155,"111225",'n',10,20,32};      //可以直接在这里定义变量名，可以定义多个，且可以直接赋值

int main35()
{
	//struct students stu;
	stu.age = 15;
	strcpy(stu.name, "张三");
	stu.sex = 'M';
	stu.scores[2] = 10.0;
	printf("%s\n", stu.name);
	printf("%d\n", stu.age);
	printf("%lf\n", stu.scores[2]);
	printf("%s\n", stu.sex=='M'?"男":"女");
	char a[10] = "wangwu";
	printf("%s\n", a);
	system("pause");
	return 0;
}