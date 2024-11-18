/*
 * @Author: your name
 * @Date: 2021-10-27 20:01:49
 * @LastEditTime: 2021-10-28 15:58:47
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /桌面/2电子相册/src/pack.cpp
 */
#include "pack.h"
#include "touch.h"
#include "Led.h"

void pack()
{
   Led l;
    l.P_rintf(l.T_raverse(l.get_T()));

    list<string>::iterator p;
    list<string>::reverse_iterator per;

   A: 
    p=l.get_T().begin();
    BMP(*p,0,0);
    while(1)
    {
      switch (get_xy())
      {
      case 1:
         cout<<"left"<<endl;      
        if(*p == l.get_T().front())
         {
            BMP(*p,0,0);
           p=l.get_T().end();
         }
         else
         {
          p--;
          BMP(*p,0,0);        
         } 
        break;

      case 2:       
        if(*p == l.get_T().back())
         {
             BMP(*p,0,0);
            p=l.get_T().begin();
         }
         else
         {
           p++;
           BMP(*p,0,0);          
         }     
         cout<<"right"<<endl;
        break;
      
      case 3:
        if(*p == l.get_T().front())
         {
            BMP(*p,0,0);
           p=l.get_T().end();

         }
         else
         {
          p--;
          BMP(*p,0,0);        
         }        
         cout<<"up"<<endl;
         break;

      case 4:
        if(*p == l.get_T().back())
         {
             BMP(*p,0,0);
            p=l.get_T().begin();
         }
         else
         {
           p++;
           BMP(*p,0,0);          
         }
         cout<<"down"<<endl;
         break;

      case 5:
        while(1)
        {
          for(p=l.get_T().begin();p!=l.get_T().end();p++)
          {
            sleep(1);
            BMP(*p,0,0);
          }
          for(per=l.get_T().rbegin();per!=l.get_T().rend();per++)
          {
            sleep(1);
            BMP(*per,0,0);
          }
          goto A;
        }
         cout<<"pause"<<endl;
        break;
        
      default:
        break;
      }
    }
}