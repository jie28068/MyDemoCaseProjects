#include "snake.h"
#include <Windows.h>

/*光标定位*/
void gotoxy1(HANDLE hOut1, int x, int y)
{
	COORD pos;
	pos.X = x;  //横坐标
	pos.Y = y;   //纵坐标
	SetConsoleCursorPosition(hOut1, pos);
}
//定义显示器句柄变量
HANDLE hOut1 = GetStdHandle(STD_OUTPUT_HANDLE);

Snake::Snake(Wall& tempWall, Food& tempFood) :wall(tempWall),food(tempFood)
{
	head = NULL;
	isend = false;
}

//初始化
void Snake::initSnake()
{
	deleteSnake();
	addPoint(5, 3);
	addPoint(5, 4);
	addPoint(5, 5);
	
}

//销毁
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

//添加
void Snake::addPoint(int x, int y)
{
	//创建新节点
	Point* newp = new Point;
	newp->x = x;
	newp->y = y;
	newp->next = NULL;
	
	//如果头不为空就改为身子
	if (head != NULL)
	{
		wall.setWall(head->x, head->y, '=');
		gotoxy1(hOut1, head->y * 2, head->x);
		cout << "=";

	}
	//更新头
	newp->next = head;
	head = newp;
	wall.setWall(head->x, head->y, '@');  //画头
	gotoxy1(hOut1, head->y * 2, head->x);
	cout << "@";
}

//删除,最后一个
void Snake::delPoint()
{
	//俩个节点以上才删除
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

	//找到最后，删除节点
	wall.setWall(pcur->x, pcur->y, ' ');
	gotoxy1(hOut1, pcur->y * 2, pcur->x);
	cout << " ";
	delete pcur;
	pcur = NULL;
	per->next = NULL;

}

//移动
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

	//判断要吃到是不是尾巴,不结束
	Point* pcur = head->next;
	Point* per = head;
	while (pcur->next != NULL)
	{
		per = per->next;
		pcur = pcur->next;
	}
	if (pcur->x == x && pcur->y == y)
	{
		//碰到尾巴
		isend = true;
	}
	else
	{
		//判断是否为正确位置
		if (wall.getWall(x, y) == '*' || wall.getWall(x, y) == '=')
		{
			addPoint(x, y);
			delPoint();
			system("cls");
			wall.drawWali();
			cout << "得分：" << getscore() << endl;
			cout << "寄！！！" << endl;
			return false;
		}
	}
	
	//移动成功，吃到食物和没吃到
	if (wall.getWall(x, y) == '#')
	{
		addPoint(x, y);

		//重新生成食物
		food.setFood();
	}
	else
	{
		addPoint(x, y);
		delPoint();

		//要吃到尾巴时把头显示出来
		if (isend == true)
		{
			wall.setWall(x, y, '@');
			gotoxy1(hOut1, head->y * 2, head->x);
			cout << "@";
		}
	}
	return true;
}

//蛇长
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

//蛇速度
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

//得分
int Snake::getscore()
{
	int size = count();
	int score = (size-3) * 10;
	return score;
}

