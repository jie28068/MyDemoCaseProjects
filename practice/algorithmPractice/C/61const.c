#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
#if 0
 //const���α���
void test01() {
	//1. const��������
	const int i = 0;
	//i = 100; //����ֻ��������ʼ��֮�����޸�

	//2. ����const������ó�ʼ��
	const int j;
	//j = 100; //���󣬲����ٴθ�ֵ

	//3. c���Ե�const��һ��ֻ��������������һ����������ͨ��ָ�����޸�
	const int k = 10;
	//k = 100; //���󣬲���ֱ���޸ģ����ǿ�ͨ��ָ�����޸�
	printf("k:%d\n", k);
	int* p = &k;
	*p = 100;
	printf("k:%d\n", k);
}

//const ����ָ��
void test02() {

	int a = 10;
	int b = 20;
	//const����*����� ����p_aָ��ָ����ڴ�ռ䲻���޸�,�����޸�ָ���ָ��
	const int* p_a = &a;
	//*p_a = 100; //�����޸�ָ��ָ����ڴ�ռ�
	p_a = &b; //���޸�ָ���ָ��

	//const����*�ŵ��Ҳ࣬ ����ָ���ָ�����޸ģ����ǿ��޸�ָ��ָ����ڴ�ռ�
	int* const p_b = &a;
	//p_b = &b; //�����޸�ָ���ָ��
	*p_b = 100; //���޸�ָ��ָ����ڴ�ռ�

	//ָ���ָ���ָ��ָ����ڴ�ռ䶼�����޸�
	const int* const p_c = &a;
}
//constָ���÷�
struct Person {
	char name[64];
	int id;
	int age;
	int score;
};

//ÿ�ζ��Զ�����п�����Ч�ʵͣ�Ӧ����ָ��
void printPersonByValue(struct Person person) {
	printf("Name:%s\n", person.name);
	printf("Name:%d\n", person.id);
	printf("Name:%d\n", person.age);
	printf("Name:%d\n", person.score);
}

//������ָ����и����ã����ܻ᲻С���޸�ԭ����
void printPersonByPointer(const struct Person* person) {
	printf("Name:%s\n", person->name);
	printf("Name:%d\n", person->id);
	printf("Name:%d\n", person->age);
	printf("Name:%d\n", person->score);
}
void test03() {
	struct Person p = { "Obama", 1101, 23, 87 };
	//printPersonByValue(p);
	printPersonByPointer(&p);
}
int main()
{
	
	system("pause");
	return 0;
}
#endif