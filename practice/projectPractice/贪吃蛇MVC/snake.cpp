#include "snake.h"
#include <Windows.h>

/*��궨λ*/
void gotoxy1(HANDLE hOut1, int x, int y)
{
	COORD pos;
	pos.X = x;  //������
	pos.Y = y;   //������
	SetConsoleCursorPosition(hOut1, pos);
}
//������ʾ���������
HANDLE hOut1 = GetStdHandle(STD_OUTPUT_HANDLE);

Snake::Snake(Wall& tempWall, Food& tempFood) :wall(tempWall),food(tempFood)
{
	head = NULL;
	isend = false;
}

//��ʼ��
void Snake::initSnake()
{
	deleteSnake();
	addPoint(5, 3);
	addPoint(5, 4);
	addPoint(5, 5);
	
}

//����
void Snake::deleteSnake()
{
	Point* pcur = head;
	while (head!=NULL)
	{
		pcur = head->next;
		delete head;
		head = pcur;
	}
}

//���
void Snake::addPoint(int x, int y)
{
	//�����½ڵ�
	Point* newp = new Point;
	newp->x = x;
	newp->y = y;
	newp->next = NULL;
	
	//���ͷ��Ϊ�վ͸�Ϊ����
	if (head != NULL)
	{
		wall.setWall(head->x, head->y, '=');
		gotoxy1(hOut1, head->y * 2, head->x);
		cout << "=";

	}
	//����ͷ
	newp->next = head;
	head = newp;
	wall.setWall(head->x, head->y, '@');  //��ͷ
	gotoxy1(hOut1, head->y * 2, head->x);
	cout << "@";
}

//ɾ��,���һ��
void Snake::delPoint()
{
	//�����ڵ����ϲ�ɾ��
	if (head == NULL || head->next == NULL)
	{
		return;
	}
	Point* pcur = head->next;
	Point* per = head;
	while (pcur->next != NULL)
	{
		per = per->next;
		pcur = pcur->next;
	}

	//�ҵ����ɾ���ڵ�
	wall.setWall(pcur->x, pcur->y, ' ');
	gotoxy1(hOut1, pcur->y * 2, pcur->x);
	cout << " ";
	delete pcur;
	pcur = NULL;
	per->next = NULL;

}

//�ƶ�
bool Snake::move(char k)
{
	int x = head->x;
	int y = head->y;

	switch (k)
	{
	case U:
		x--;
		break;
	case D:
		x++;
		break;
	case L:
		y--;
		break;
	case R:
		y++;
		break;
	default:
		break;
	}

	//�ж�Ҫ�Ե��ǲ���β��,������
	Point* pcur = head->next;
	Point* per = head;
	while (pcur->next != NULL)
	{
		per = per->next;
		pcur = pcur->next;
	}
	if (pcur->x == x && pcur->y == y)
	{
		//����β��
		isend = true;
	}
	else
	{
		//�ж��Ƿ�Ϊ��ȷλ��
		if (wall.getWall(x, y) == '*' || wall.getWall(x, y) == '=')
		{
			addPoint(x, y);
			delPoint();
			system("cls");
			wall.drawWali();
			cout << "�÷֣�" << getscore() << endl;
			cout << "�ģ�����" << endl;
			return false;
		}
	}
	
	//�ƶ��ɹ����Ե�ʳ���û�Ե�
	if (wall.getWall(x, y) == '#')
	{
		addPoint(x, y);

		//��������ʳ��
		food.setFood();
	}
	else
	{
		addPoint(x, y);
		delPoint();

		//Ҫ�Ե�β��ʱ��ͷ��ʾ����
		if (isend == true)
		{
			wall.setWall(x, y, '@');
			gotoxy1(hOut1, head->y * 2, head->x);
			cout << "@";
		}
	}
	return true;
}

//�߳�
int Snake::count()
{
	int size = 0;
	Point* cp = head;
	while (cp != NULL)
	{
		size++;
		cp = cp->next;
	}
	return size;
}

//���ٶ�
int Snake::sleep()
{
	int stime = 0;
	int size = count();
	if (size < 5)
	{
		stime = 300;
	}
	else if(size>5 && size <10)
	{
		stime = 200;
	}
	else
	{
		stime = 100;
	}
	return stime;
}

//�÷�
int Snake::getscore()
{
	int size = count();
	int score = (size-3) * 10;
	return score;
}

