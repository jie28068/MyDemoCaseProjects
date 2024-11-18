/*
 * @Author: your name
 * @Date: 2021-10-25 17:05:21
 * @LastEditTime: 2021-10-27 15:42:25
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /桌面/2电子相册/src/Led.cpp
 */
#include "Led.h"

#if 0
l_list* Led::Traverse()
{
    /*打开图片文件夹*/
    DIR* ptDir = opendir("../image");
    struct dirent* ptDirdent = NULL;
    /*没啥用*/
    char* temp = NULL;
    /*定义哑结点*/
    l_list* head = new l_list(0);
    /*遍历指针*/
    l_list* p = head;
    while ((ptDirdent = readdir(ptDir)) != NULL)
    {
        temp = ptDirdent->d_name;
        for (int i = 0; i < strlen(temp); i++)
        {
            if ((temp[i] == '.' && temp[i + 1] == 'j') || (temp[i] == '.' && temp[i + 1] == 'p'))
            {
                /*建立新节点*/
                l_list* node = new l_list(ptDirdent->d_name);
                /*尾插*/
                p->next = node;
                node->per = p;
                p = node;
            }
        }
    }
    closedir(ptDir);
    return head->next;

}

/*打印链表信息*/
void Led::Printf(l_list* p)
{
    for (; p != NULL; p = p->next)
    {
        cout << p->data << endl;
    }
}
#else
list<string>& Led::T_raverse(list<string>& num)
{
    /*打开图片文件夹*/
    DIR* ptDir = opendir("../image");
    struct dirent* ptDirdent = NULL;

    /*遍历当前目录文件信息*/
    while ((ptDirdent = readdir(ptDir)) != NULL)
    {
        for (int i = 0; i < strlen(ptDirdent->d_name); i++)
        {
            /*存放到list中*/
            if ((ptDirdent->d_name[i] == '.' && ptDirdent->d_name[i + 1] == 'j') || (ptDirdent->d_name[i] == '.' && ptDirdent->d_name[i + 1] == 'b'))
            
            {
            //拼接字符串
            const char* d="../image/";
            char* p=ptDirdent->d_name;
            char* sum=new char;
            sprintf(sum,"%s%s",d,p);
            string g=sum;
            num.insert(num.end(), g);
            } 
        }
    }
    closedir(ptDir);
    return num;
}

/*打印链表*/
void  Led::P_rintf(list<string>& num)
{
    /*定义迭代器*/
    list<string>::iterator numiterator;
    /*打印容器内节点数量*/
    cout << "number:" << num.size() << endl;
    /*遍历容器，并打节点*/
    for (numiterator = num.begin(); numiterator != num.end(); ++numiterator)
    {
        cout << *numiterator << endl;
    }
}

#endif