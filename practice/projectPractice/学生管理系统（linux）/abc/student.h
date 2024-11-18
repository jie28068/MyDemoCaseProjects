#include <stdio.h>
extern void muen();
extern void lookall();
extern void cmback();
extern void add_p();
extern void delete_p();
extern void look_p();
extern void changed_p();
extern void search_p();
extern int get_once(char* );
extern int get_len(FILE* );
typedef struct student
{
	char name[20];
	int id;
	int scores;
	char sex[10];
}stu;


