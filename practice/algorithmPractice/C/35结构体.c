#include <stdio.h>

struct students   //�ṹ����
{
	char name[20];
	int age;
	char tel[15];
	char sex;
	double scores[3];

}stu, stu1, stu2 = {"����",155,"111225",'n',10,20,32};      //����ֱ�������ﶨ������������Զ��������ҿ���ֱ�Ӹ�ֵ

int main35()
{
	//struct students stu;
	stu.age = 15;
	strcpy(stu.name, "����");
	stu.sex = 'M';
	stu.scores[2] = 10.0;
	printf("%s\n", stu.name);
	printf("%d\n", stu.age);
	printf("%lf\n", stu.scores[2]);
	printf("%s\n", stu.sex=='M'?"��":"Ů");
	char a[10] = "wangwu";
	printf("%s\n", a);
	system("pause");
	return 0;
}