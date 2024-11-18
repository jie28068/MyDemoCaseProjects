import requests
from lxml import etree
import re
import time
from datetime import datetime, timedelta
from bs4 import BeautifulSoup
import json

def calculate_days_to_holidays():
    urlmtself = 'https://blog.csdn.net/weixin_49065061'
    headers = {
        'cookie' : 'uuid_tt_dd=10_37405698930-1680225117508-356392; __bid_n=1873539657233b05ea4207; FPTOKEN=R8ipn7MZiqZ3C8p7U2POhzlTFORSp2xwnNLasGcaI4NayFDrcWHuhRXo4mphJPbUZevDKlLqgaoUNvfwUyDKSHJ3906kDNLogX85XPqb/YmfKNqGiJRkeajUs8IKeueHFHxnBrlfWAB8regLRdRt8Zj+moSGTlQolTupZu9rKFJWcji7Ah9L97qbmPOJRu7BeSNVFPcwpmNLXKiqUBj7ZLIX5N32/y4vPrR+tod1ZxABwt30VH0i4PecX7WbGnE7+v8/p7AmDTiQpB0SCq8S3nCh3JEq3JGGY4XEkbqMH+jvkLQf9logmZTKBU1SJ1ujnhgupEZ8CYSLPs0Y6eRmmLzcspngL+7Ut+cvmfYh/ZzBo/iguO5DzQtcNQ+5HYjKYIaTPmX7dYCOpuSFiRBBBA==|V03GGoPMaQepDb3oWVddiSNzaH5M6DBKduMH5PaOxmM=|10|b53af8cd6f43b8386d7d9d238beca048; ins_first_time=1692610128427; p_uid=U010000; log_Id_click=1603; log_Id_pv=1496; log_Id_view=10661; c_dl_prid=1707037408879_530733; c_dl_rid=1707187439505_304243; c_dl_fref=https://blog.csdn.net/weixin_49065061/article/details/129584649; c_dl_fpage=/download/weixin_43871665/86515510; c_dl_um=distribute.pc_search_result.none-task-blog-2%7Eall%7Esobaiduend%7Edefault-1-85280823-null-null.142%5Ev99%5Econtrol; weixin_49065061comment_new=1708915702250; creative_btn_mp=3; c_segment=8; Hm_lvt_6bcd52f51e9b3dce32bec4a3997715ac=1708650593,1708909495,1708995939,1708997067; Hm_lvt_e5ef47b9f471504959267fd614d579cd=1708999338; Hm_lpvt_e5ef47b9f471504959267fd614d579cd=1708999338; ssxmod_itna=YqRxuD2DB7D=VDBaqPrGkD0AY1wgS40KpYtdD/iDfo4iNDnD8x7YDv+mkWR+KRKj8uPW4=BkpAQfzjeuDefiQAhfqA8Sd4GLDmKDyQYgHeDxOq0rD74irDDxD3DbSdDSDWKD9D0+kSBuqtDm4GWCqGfDDoDYR=nDitD4qDBGEdDKqGgCdmQ+dqD0dwruAxbR00PMS2MUjtqFexeCqDMReGXFgFNveH1uAXPhaF4rx=DzM7DtdXMlMLdx0pB/TN4ZE3f+Avx3OPabGwKR0Dr80vxKimR=S+YDh+QYAkPe2+62NqDGbhbSxxxD; ssxmod_itna2=YqRxuD2DB7D=VDBaqPrGkD0AY1wgS40KpYtG9tPPxBLgfx7PwxmBaFMhOjlBj7PCZdFX1K3b7A6eqr8m7NkDrn8v=ADV0SO2d/A/Z8rLPZR4i=IME7Kghh5zZL8j6mhalSXRIu8CaV65Hq3qapgyPxZFHZYRGcZKxdtGxjAA=+QDdTfpGWOgWXoS4ezAEXxeD60Wqjj3FAhK4jASt228WspMEf+yO/fk6K/If3+kvDsQ0oyqLTfSwYGF6Yj3zMFaQfFH9YOQ0Yl=z608kr5tn7UMDN4Uu6X=IpNG8jbpb98uZPTu98N68aw6cKB6foPYuZXlYi7Zmj1te7vfoq/gQoU=eUdP2fo4tdUL3+DVmLCgj5hm9PRAWt2665F=Ah0F464I=t8=G=iFo6FK24fG8OGaaX+hDr/mF3bCarj=QggpTDG2RDdD08DijQYD; SESSION=61c03aeb-efd0-4ade-990f-88b7c3b3c987; hide_login=1; loginbox_strategy=%7B%22taskId%22%3A317%2C%22abCheckTime%22%3A1708999660770%2C%22version%22%3A%22ExpA%22%2C%22nickName%22%3A%22%E5%A5%A5%E7%89%B9%E6%9B%BC%E7%8B%82%E6%89%81%E5%B0%8F%E6%80%AA%E5%85%BD%22%7D; tfstk=eXrkxbsdjzuS8DK_H7n5DQJSyg_YPQiI1WKK9DhFujlj9TG-wJP3KRuU2bCQ-rV0sHhrwzd3-XPCFDFdRDc3a0uQyXh-tD2QstCOXGe7FDgN61I90EYb0DogwWE0F8iBhwtwQteWxEE9xMukk1HcKoWomfvMjavZJ_X-s8lDj-ZmUWY7ejxeYo4mrfoniH-UmYjrIE8ambtI39E2RegrhxcTOZNXEyEYmcBcnFeIzxMs6tXDRngrhx0lntY1T4kjF71..; UserName=weixin_49065061; UserInfo=46eb35a12b37485b8c59843d8fdc9340; UserToken=46eb35a12b37485b8c59843d8fdc9340; UserNick=%E5%A5%A5%E7%89%B9%E6%9B%BC%E7%8B%82%E6%89%81%E5%B0%8F%E6%80%AA%E5%85%BD; AU=ECD; UN=weixin_49065061; BT=1709000306131; Hm_up_6bcd52f51e9b3dce32bec4a3997715ac=%7B%22islogin%22%3A%7B%22value%22%3A%221%22%2C%22scope%22%3A1%7D%2C%22isonline%22%3A%7B%22value%22%3A%221%22%2C%22scope%22%3A1%7D%2C%22isvip%22%3A%7B%22value%22%3A%220%22%2C%22scope%22%3A1%7D%2C%22uid_%22%3A%7B%22value%22%3A%22weixin_49065061%22%2C%22scope%22%3A1%7D%7D; c_utm_source=vip_chatgpt_common_pc_toolbar; fe_request_id=1709002195292_3516_2752991; csrfToken=Bexiyixp7EypdAEH1EK9GC5z; dc_session_id=10_1709010626594.487419; SidecHatdocDescBoxNum=true; dc_sid=46407ebe960f8e788ef46c2c9676a57d; log_Id_pv=1497; log_Id_view=10662; log_Id_click=1604; dc_tos=s9i693; c_utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ERate-4-114632808-blog-113869295.235%5Ev43%5Epc_blog_bottom_relevance_base7; c_utm_relevant_index=9; firstDie=1; c_pref=default; c_first_ref=default; c_first_page=https%3A//blog.csdn.net/weixin_49065061/article/details/136030886; c_dsid=11_1709018491886.940654; c_ref=https%3A//blog.csdn.net/weixin_49065061%3Fspm%3D1010.2135.3001.5343; creativeSetApiNew=%7B%22toolbarImg%22%3A%22https%3A//img-home.csdnimg.cn/images/20230921102607.png%22%2C%22publishSuccessImg%22%3A%22https%3A//img-home.csdnimg.cn/images/20230920034826.png%22%2C%22articleNum%22%3A32%2C%22type%22%3A2%2C%22oldUser%22%3Atrue%2C%22useSeven%22%3Afalse%2C%22oldFullVersion%22%3Atrue%2C%22userName%22%3A%22weixin_49065061%22%7D; __gads=ID=880263f08407b0a2-22b2c8a4f0de00f9:T=1680225167:RT=1709018821:S=ALNI_MaX561SPpvig2_eELLwngPofwkqvw; __gpi=UID=00000be91cf6ffb1:T=1680225167:RT=1709018821:S=ALNI_Ma-9bIbEuZj8QGIT5So6pfMkO9v3A; __eoi=ID=1d4bfb2d82ad824c:T=1706666065:RT=1709018821:S=AA-AfjZXRr_q7qvzrD0W88T2y_Uf; c_page_id=default; Hm_lpvt_6bcd52f51e9b3dce32bec4a3997715ac=1709018848; waf_captcha_marker=3dd1e06207e934f225847b1c138c3bb847e875cc063d4e1ad985224b1cd3d2a2; dc_tos=s9i7hy',
        'user-agent' : 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.72 Safari/537.36 Edg/89.0.774.45'
    }

    # 发送HTTP请求
    response = requests.get(urlmtself, headers=headers)
    # 解析响应内容
    soup = BeautifulSoup(response.text, 'html.parser')
    # 搜索包含所需数据的div
    divs = soup.find_all('div', class_='user-profile-statistics-num', attrs={'data-v-d1dbb6f8': True})
    people = divs[3].get_text(strip=True)
    people2 = int(1000-int(people))
    current_date = datetime.now()

    # 设置目标日期
    target_date = datetime(2024, 2, 27)
    # 初始化天数计数器
    days = 0
    # 遍历当前日期到目标日期之间的每一天
    while current_date >= target_date:
        # 如果不是周末，则计数器加1
        if target_date.weekday() not in (5, 6):
            days += 1
        # 将当前日期加1天
        target_date += timedelta(days=1)

    # 离最近的周末的天数
    if current_date.weekday() == 4:  # 星期五
        days_to_weekend = 1
    elif current_date.weekday() == 5:  # 星期六
        days_to_weekend = 0
    else:
        days_to_weekend = 5 - current_date.weekday()
    datat =current_date.strftime("%m-%d %H:%M")
    # 定义2024年的节假日列表
    holidays_2024 = [
        (datetime(2024, 1, 1), "元旦"), (datetime(2024, 2, 10), "春节"),
        (datetime(2024, 4, 4), "清明节"), (datetime(2024, 5, 1), "劳动节"),
        (datetime(2024, 6, 22), "端午节"), (datetime(2024, 9, 10), "中秋节"),
        (datetime(2024, 10, 1), "国庆节")
    ]
    # 获取当前日期
    current_date = datetime.now()
    # 计算即将到来的节假日列表
    upcoming_holidays = [holiday for holiday in holidays_2024 if holiday[0] >= current_date]
    # 如果没有即将到来的节假日，返回空列表
    if not upcoming_holidays:
        return []
    # 获取最近的节假日
    next_holiday = min(upcoming_holidays, key=lambda x: x[0])
    # 计算距离最近节假日的天数
    days_to_next_holiday = (next_holiday[0] - current_date).days
    result_str = "".join(journalism())
    return f"这个世界不存在摸鱼，所有的摸鱼都是在工作！[face]emoji:008.png[/face]\n北京时间： {datat} 赛博打卡第【{days}】天[face]emoji:040.png[/face]\n离最近的周末(双休)还有： {days_to_weekend} 天[face]emoji:015.png[/face]\n离最近的节假日({next_holiday[1]})还有： {days_to_next_holiday} 天[face]emoji:019.png[/face]\n当前粉丝数【{people}】人，距离破千还差{people2}人！[face]emoji:070.png[/face]\n当前微博热搜TOP3[face]emoji:003.png[/face]\n{result_str}"

def journalism():
    news = []
    # 新建数组存放热搜榜
    hot_url = 'https://s.weibo.com/top/summary/'
    headers = {
        'cookie' : 'SUB=_2AkMTMA8Cf8NxqwFRmfkXyG_raox1yAzEieKlbP7ZJRMxHRl-yT9kqlw6tRB6OLAh7Yh8trdoGd0QitJuaFD0zt6OTvsz; SUBP=0033WrSXqPxfM72-Ws9jqgMF55529P9D9WFkr76gz-ywjR3B4_RK.WRw; XSRF-TOKEN=A9Uyy44XXf0F-XGQEtalUWpY; WBPSESS=HOKMwFaOhMG7Cl30d6Y-8Tk1wcgMcvzj-Oi9kmdQoNWQumlmoUxQjnMuVSE1591_6lAdjMnpPInNTLW84FbBhCMtVg79IxI9ypLcyx3QyS_FxqOy_ayYaVN_4FmJkWWw',
        'user-agent' : 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.72 Safari/537.36 Edg/89.0.774.45'
    }
    # 热搜榜链接
    r = requests.get(hot_url, headers=headers)
    # 向链接发送get请求获得页面
    soup = BeautifulSoup(r.text, 'lxml')
    # 解析页面

    urls_titles = soup.select('#pl_top_realtimehot > table > tbody > tr > td.td-02 > a')
    hotness = soup.select('#pl_top_realtimehot > table > tbody > tr > td.td-02 > span')

    for i in range(len(urls_titles)-1):
        hot_news = {}
        # 将信息保存到字典中
        hot_news['title'] = urls_titles[i+1].get_text()
        # get_text()获得a标签的文本
        hot_news['url'] = "https://s.weibo.com"+urls_titles[i]['href']
        # ['href']获得a标签的链接，并补全前缀
        hot_news['hotness'] = hotness[i].get_text()
        # 获得热度文本
        news.append(hot_news)
        # 字典追加到数组中
    return [f"{i['title']}:{i['url']}\n" for i in  news[:3]]
    
def myfunction():
    headers1 = {
        'cookie' : 'uuid_tt_dd=10_37405698930-1680225117508-356392; __bid_n=1873539657233b05ea4207; FPTOKEN=R8ipn7MZiqZ3C8p7U2POhzlTFORSp2xwnNLasGcaI4NayFDrcWHuhRXo4mphJPbUZevDKlLqgaoUNvfwUyDKSHJ3906kDNLogX85XPqb/YmfKNqGiJRkeajUs8IKeueHFHxnBrlfWAB8regLRdRt8Zj+moSGTlQolTupZu9rKFJWcji7Ah9L97qbmPOJRu7BeSNVFPcwpmNLXKiqUBj7ZLIX5N32/y4vPrR+tod1ZxABwt30VH0i4PecX7WbGnE7+v8/p7AmDTiQpB0SCq8S3nCh3JEq3JGGY4XEkbqMH+jvkLQf9logmZTKBU1SJ1ujnhgupEZ8CYSLPs0Y6eRmmLzcspngL+7Ut+cvmfYh/ZzBo/iguO5DzQtcNQ+5HYjKYIaTPmX7dYCOpuSFiRBBBA==|V03GGoPMaQepDb3oWVddiSNzaH5M6DBKduMH5PaOxmM=|10|b53af8cd6f43b8386d7d9d238beca048; ins_first_time=1692610128427; p_uid=U010000; log_Id_click=1603; log_Id_pv=1496; log_Id_view=10661; c_dl_prid=1707037408879_530733; c_dl_rid=1707187439505_304243; c_dl_fref=https://blog.csdn.net/weixin_49065061/article/details/129584649; c_dl_fpage=/download/weixin_43871665/86515510; c_dl_um=distribute.pc_search_result.none-task-blog-2%7Eall%7Esobaiduend%7Edefault-1-85280823-null-null.142%5Ev99%5Econtrol; weixin_49065061comment_new=1708915702250; creative_btn_mp=3; c_segment=8; Hm_lvt_6bcd52f51e9b3dce32bec4a3997715ac=1708650593,1708909495,1708995939,1708997067; Hm_lvt_e5ef47b9f471504959267fd614d579cd=1708999338; Hm_lpvt_e5ef47b9f471504959267fd614d579cd=1708999338; ssxmod_itna=YqRxuD2DB7D=VDBaqPrGkD0AY1wgS40KpYtdD/iDfo4iNDnD8x7YDv+mkWR+KRKj8uPW4=BkpAQfzjeuDefiQAhfqA8Sd4GLDmKDyQYgHeDxOq0rD74irDDxD3DbSdDSDWKD9D0+kSBuqtDm4GWCqGfDDoDYR=nDitD4qDBGEdDKqGgCdmQ+dqD0dwruAxbR00PMS2MUjtqFexeCqDMReGXFgFNveH1uAXPhaF4rx=DzM7DtdXMlMLdx0pB/TN4ZE3f+Avx3OPabGwKR0Dr80vxKimR=S+YDh+QYAkPe2+62NqDGbhbSxxxD; ssxmod_itna2=YqRxuD2DB7D=VDBaqPrGkD0AY1wgS40KpYtG9tPPxBLgfx7PwxmBaFMhOjlBj7PCZdFX1K3b7A6eqr8m7NkDrn8v=ADV0SO2d/A/Z8rLPZR4i=IME7Kghh5zZL8j6mhalSXRIu8CaV65Hq3qapgyPxZFHZYRGcZKxdtGxjAA=+QDdTfpGWOgWXoS4ezAEXxeD60Wqjj3FAhK4jASt228WspMEf+yO/fk6K/If3+kvDsQ0oyqLTfSwYGF6Yj3zMFaQfFH9YOQ0Yl=z608kr5tn7UMDN4Uu6X=IpNG8jbpb98uZPTu98N68aw6cKB6foPYuZXlYi7Zmj1te7vfoq/gQoU=eUdP2fo4tdUL3+DVmLCgj5hm9PRAWt2665F=Ah0F464I=t8=G=iFo6FK24fG8OGaaX+hDr/mF3bCarj=QggpTDG2RDdD08DijQYD; SESSION=61c03aeb-efd0-4ade-990f-88b7c3b3c987; hide_login=1; loginbox_strategy=%7B%22taskId%22%3A317%2C%22abCheckTime%22%3A1708999660770%2C%22version%22%3A%22ExpA%22%2C%22nickName%22%3A%22%E5%A5%A5%E7%89%B9%E6%9B%BC%E7%8B%82%E6%89%81%E5%B0%8F%E6%80%AA%E5%85%BD%22%7D; tfstk=eXrkxbsdjzuS8DK_H7n5DQJSyg_YPQiI1WKK9DhFujlj9TG-wJP3KRuU2bCQ-rV0sHhrwzd3-XPCFDFdRDc3a0uQyXh-tD2QstCOXGe7FDgN61I90EYb0DogwWE0F8iBhwtwQteWxEE9xMukk1HcKoWomfvMjavZJ_X-s8lDj-ZmUWY7ejxeYo4mrfoniH-UmYjrIE8ambtI39E2RegrhxcTOZNXEyEYmcBcnFeIzxMs6tXDRngrhx0lntY1T4kjF71..; UserName=weixin_49065061; UserInfo=46eb35a12b37485b8c59843d8fdc9340; UserToken=46eb35a12b37485b8c59843d8fdc9340; UserNick=%E5%A5%A5%E7%89%B9%E6%9B%BC%E7%8B%82%E6%89%81%E5%B0%8F%E6%80%AA%E5%85%BD; AU=ECD; UN=weixin_49065061; BT=1709000306131; Hm_up_6bcd52f51e9b3dce32bec4a3997715ac=%7B%22islogin%22%3A%7B%22value%22%3A%221%22%2C%22scope%22%3A1%7D%2C%22isonline%22%3A%7B%22value%22%3A%221%22%2C%22scope%22%3A1%7D%2C%22isvip%22%3A%7B%22value%22%3A%220%22%2C%22scope%22%3A1%7D%2C%22uid_%22%3A%7B%22value%22%3A%22weixin_49065061%22%2C%22scope%22%3A1%7D%7D; c_utm_source=vip_chatgpt_common_pc_toolbar; fe_request_id=1709002195292_3516_2752991; csrfToken=Bexiyixp7EypdAEH1EK9GC5z; dc_session_id=10_1709010626594.487419; SidecHatdocDescBoxNum=true; dc_sid=46407ebe960f8e788ef46c2c9676a57d; log_Id_pv=1497; log_Id_view=10662; log_Id_click=1604; dc_tos=s9i693; c_utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ERate-4-114632808-blog-113869295.235%5Ev43%5Epc_blog_bottom_relevance_base7; c_utm_relevant_index=9; firstDie=1; c_pref=default; c_first_ref=default; c_first_page=https%3A//blog.csdn.net/weixin_49065061/article/details/136030886; c_dsid=11_1709018491886.940654; c_ref=https%3A//blog.csdn.net/weixin_49065061%3Fspm%3D1010.2135.3001.5343; creativeSetApiNew=%7B%22toolbarImg%22%3A%22https%3A//img-home.csdnimg.cn/images/20230921102607.png%22%2C%22publishSuccessImg%22%3A%22https%3A//img-home.csdnimg.cn/images/20230920034826.png%22%2C%22articleNum%22%3A32%2C%22type%22%3A2%2C%22oldUser%22%3Atrue%2C%22useSeven%22%3Afalse%2C%22oldFullVersion%22%3Atrue%2C%22userName%22%3A%22weixin_49065061%22%7D; __gads=ID=880263f08407b0a2-22b2c8a4f0de00f9:T=1680225167:RT=1709018821:S=ALNI_MaX561SPpvig2_eELLwngPofwkqvw; __gpi=UID=00000be91cf6ffb1:T=1680225167:RT=1709018821:S=ALNI_Ma-9bIbEuZj8QGIT5So6pfMkO9v3A; __eoi=ID=1d4bfb2d82ad824c:T=1706666065:RT=1709018821:S=AA-AfjZXRr_q7qvzrD0W88T2y_Uf; c_page_id=default; Hm_lpvt_6bcd52f51e9b3dce32bec4a3997715ac=1709018848; waf_captcha_marker=3dd1e06207e934f225847b1c138c3bb847e875cc063d4e1ad985224b1cd3d2a2; dc_tos=s9i7hy',
        'user-agent' : 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.72 Safari/537.36 Edg/89.0.774.45'
    }

    # 定义请求的URL
    urlnew = 'https://blog.csdn.net/community/home-api/v1/get-business-list?page=1&size=20&businessType=blog&orderby=&noMore=false&year=&month=&username=weixin_39786569'

    headersnew = {
        'cookie' : 'uuid_tt_dd=10_37405698930-1680225117508-356392; __bid_n=1873539657233b05ea4207; FPTOKEN=R8ipn7MZiqZ3C8p7U2POhzlTFORSp2xwnNLasGcaI4NayFDrcWHuhRXo4mphJPbUZevDKlLqgaoUNvfwUyDKSHJ3906kDNLogX85XPqb/YmfKNqGiJRkeajUs8IKeueHFHxnBrlfWAB8regLRdRt8Zj+moSGTlQolTupZu9rKFJWcji7Ah9L97qbmPOJRu7BeSNVFPcwpmNLXKiqUBj7ZLIX5N32/y4vPrR+tod1ZxABwt30VH0i4PecX7WbGnE7+v8/p7AmDTiQpB0SCq8S3nCh3JEq3JGGY4XEkbqMH+jvkLQf9logmZTKBU1SJ1ujnhgupEZ8CYSLPs0Y6eRmmLzcspngL+7Ut+cvmfYh/ZzBo/iguO5DzQtcNQ+5HYjKYIaTPmX7dYCOpuSFiRBBBA==|V03GGoPMaQepDb3oWVddiSNzaH5M6DBKduMH5PaOxmM=|10|b53af8cd6f43b8386d7d9d238beca048; ins_first_time=1692610128427; p_uid=U010000; log_Id_click=1603; log_Id_pv=1496; log_Id_view=10661; c_dl_prid=1707037408879_530733; c_dl_rid=1707187439505_304243; c_dl_fref=https://blog.csdn.net/weixin_49065061/article/details/129584649; c_dl_fpage=/download/weixin_43871665/86515510; c_dl_um=distribute.pc_search_result.none-task-blog-2%7Eall%7Esobaiduend%7Edefault-1-85280823-null-null.142%5Ev99%5Econtrol; Hm_lvt_e5ef47b9f471504959267fd614d579cd=1708999338; ssxmod_itna=YqRxuD2DB7D=VDBaqPrGkD0AY1wgS40KpYtdD/iDfo4iNDnD8x7YDv+mkWR+KRKj8uPW4=BkpAQfzjeuDefiQAhfqA8Sd4GLDmKDyQYgHeDxOq0rD74irDDxD3DbSdDSDWKD9D0+kSBuqtDm4GWCqGfDDoDYR=nDitD4qDBGEdDKqGgCdmQ+dqD0dwruAxbR00PMS2MUjtqFexeCqDMReGXFgFNveH1uAXPhaF4rx=DzM7DtdXMlMLdx0pB/TN4ZE3f+Avx3OPabGwKR0Dr80vxKimR=S+YDh+QYAkPe2+62NqDGbhbSxxxD; ssxmod_itna2=YqRxuD2DB7D=VDBaqPrGkD0AY1wgS40KpYtG9tPPxBLgfx7PwxmBaFMhOjlBj7PCZdFX1K3b7A6eqr8m7NkDrn8v=ADV0SO2d/A/Z8rLPZR4i=IME7Kghh5zZL8j6mhalSXRIu8CaV65Hq3qapgyPxZFHZYRGcZKxdtGxjAA=+QDdTfpGWOgWXoS4ezAEXxeD60Wqjj3FAhK4jASt228WspMEf+yO/fk6K/If3+kvDsQ0oyqLTfSwYGF6Yj3zMFaQfFH9YOQ0Yl=z608kr5tn7UMDN4Uu6X=IpNG8jbpb98uZPTu98N68aw6cKB6foPYuZXlYi7Zmj1te7vfoq/gQoU=eUdP2fo4tdUL3+DVmLCgj5hm9PRAWt2665F=Ah0F464I=t8=G=iFo6FK24fG8OGaaX+hDr/mF3bCarj=QggpTDG2RDdD08DijQYD; loginbox_strategy=%7B%22taskId%22%3A317%2C%22abCheckTime%22%3A1708999660770%2C%22version%22%3A%22ExpA%22%2C%22nickName%22%3A%22%E5%A5%A5%E7%89%B9%E6%9B%BC%E7%8B%82%E6%89%81%E5%B0%8F%E6%80%AA%E5%85%BD%22%7D; tfstk=eXrkxbsdjzuS8DK_H7n5DQJSyg_YPQiI1WKK9DhFujlj9TG-wJP3KRuU2bCQ-rV0sHhrwzd3-XPCFDFdRDc3a0uQyXh-tD2QstCOXGe7FDgN61I90EYb0DogwWE0F8iBhwtwQteWxEE9xMukk1HcKoWomfvMjavZJ_X-s8lDj-ZmUWY7ejxeYo4mrfoniH-UmYjrIE8ambtI39E2RegrhxcTOZNXEyEYmcBcnFeIzxMs6tXDRngrhx0lntY1T4kjF71..; UserName=weixin_49065061; UserInfo=46eb35a12b37485b8c59843d8fdc9340; UserToken=46eb35a12b37485b8c59843d8fdc9340; UserNick=%E5%A5%A5%E7%89%B9%E6%9B%BC%E7%8B%82%E6%89%81%E5%B0%8F%E6%80%AA%E5%85%BD; AU=ECD; UN=weixin_49065061; BT=1709000306131; Hm_up_6bcd52f51e9b3dce32bec4a3997715ac=%7B%22islogin%22%3A%7B%22value%22%3A%221%22%2C%22scope%22%3A1%7D%2C%22isonline%22%3A%7B%22value%22%3A%221%22%2C%22scope%22%3A1%7D%2C%22isvip%22%3A%7B%22value%22%3A%220%22%2C%22scope%22%3A1%7D%2C%22uid_%22%3A%7B%22value%22%3A%22weixin_49065061%22%2C%22scope%22%3A1%7D%7D; weixin_49065061comment_new=1709024917087; dp_token=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpZCI6MTQ5MDc4LCJleHAiOjE3MDk3Nzc1ODAsImlhdCI6MTcwOTE3Mjc4MCwidXNlcm5hbWUiOiJ3ZWl4aW5fNDkwNjUwNjEifQ.Ela8ycssG_rdqxiIgngFA_g2vQB4XORCeDkugQXXQWE; c_segment=8; Hm_lvt_6bcd52f51e9b3dce32bec4a3997715ac=1708909495,1708995939,1708997067,1709190201; is_advert=1; creative_btn_mp=3; c_utm_source=zhuzhantoolbar; fpv=4febbe1d2f5f3e02e99db2aaa1f435fb; csrfToken=bp74QE72CZ-o60-gxou1oQSA; dc_sid=0c971cf4713690fbdf3c27759df3c66e; dc_session_id=10_1709275406075.604677; fe_request_id=1709275771866_7456_0725161; log_Id_pv=1497; log_Id_view=10662; log_Id_click=1604; c_pref=default; c_ref=default; c_first_ref=default; c_first_page=https%3A//blog.csdn.net/csdngeeknews/article/details/136388222; c_dsid=11_1709281636384.931265; c_page_id=default; Hm_lpvt_6bcd52f51e9b3dce32bec4a3997715ac=1709281637; __gads=ID=880263f08407b0a2-22b2c8a4f0de00f9:T=1680225167:RT=1709281636:S=ALNI_MaX561SPpvig2_eELLwngPofwkqvw; __gpi=UID=00000be91cf6ffb1:T=1680225167:RT=1709281636:S=ALNI_Ma-9bIbEuZj8QGIT5So6pfMkO9v3A; __eoi=ID=1d4bfb2d82ad824c:T=1706666065:RT=1709281636:S=AA-AfjZXRr_q7qvzrD0W88T2y_Uf; yd_captcha_token=ycvv6kVJoi05TaKwlgJkcfaYBqxVleLfb1YcxnBnGmLs2Q5UahmOyACBlHC2OX9QHI6+KQXZ+3iydhH965E7Mw%3D%3D; dc_tos=s9nver',
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36 Edg/122.0.0.0',
        'Host':'blog.csdn.net',
        'Referer':'https://blog.csdn.net/csdngeeknews?type=lately'
    }

    # 发送GET请求
    response = requests.get(urlnew, headers=headersnew)
    # 确保请求成功
    if response.status_code == 200:
        # 获取XML响应内容
        json_content = response.text
        jsondata = json.loads(json_content)
        # 提取文章列表
        articles = jsondata['data']['list']
        hre = articles[1]['url']
        # 发送GET请求
        response = requests.get(hre, headers=headers1)
        # 解析响应内容
        soup = BeautifulSoup(response.text, 'html.parser')
        # 搜索包含所需数据的div
        divs = soup.find_all('span', class_='time')
        curdata = divs[0].get_text(strip=True)
        now = datetime.now()
        date_format = "于 %Y-%m-%d %H:%M:%S 发布"
        # 尝试使用初始格式解析
        try:
            post_date = datetime.strptime(curdata, date_format)
        except ValueError:
            # 如果失败，尝试使用其他格式
            date_format = "最新推荐文章于 %Y-%m-%d %H:%M:%S 发布"
            try:
                post_date = datetime.strptime(curdata, date_format)
            except ValueError:
                # 如果仍然失败，则可能需要更复杂的处理，或者使用默认值
                post_date = None
        # 比较两个日期是否为同一天
        is_same_day = now.year == post_date.year and now.month == post_date.month and now.day == post_date.day
        #end

        url1 = 'https://blog.csdn.net/phoenix/web/v1/comment/submit'

        headers2 = {
            'cookie' : 'uuid_tt_dd=10_37405698930-1680225117508-356392; __bid_n=1873539657233b05ea4207; FPTOKEN=R8ipn7MZiqZ3C8p7U2POhzlTFORSp2xwnNLasGcaI4NayFDrcWHuhRXo4mphJPbUZevDKlLqgaoUNvfwUyDKSHJ3906kDNLogX85XPqb/YmfKNqGiJRkeajUs8IKeueHFHxnBrlfWAB8regLRdRt8Zj+moSGTlQolTupZu9rKFJWcji7Ah9L97qbmPOJRu7BeSNVFPcwpmNLXKiqUBj7ZLIX5N32/y4vPrR+tod1ZxABwt30VH0i4PecX7WbGnE7+v8/p7AmDTiQpB0SCq8S3nCh3JEq3JGGY4XEkbqMH+jvkLQf9logmZTKBU1SJ1ujnhgupEZ8CYSLPs0Y6eRmmLzcspngL+7Ut+cvmfYh/ZzBo/iguO5DzQtcNQ+5HYjKYIaTPmX7dYCOpuSFiRBBBA==|V03GGoPMaQepDb3oWVddiSNzaH5M6DBKduMH5PaOxmM=|10|b53af8cd6f43b8386d7d9d238beca048; ins_first_time=1692610128427; p_uid=U010000; log_Id_click=1603; log_Id_pv=1496; log_Id_view=10661; c_dl_prid=1707037408879_530733; c_dl_rid=1707187439505_304243; c_dl_fref=https://blog.csdn.net/weixin_49065061/article/details/129584649; c_dl_fpage=/download/weixin_43871665/86515510; c_dl_um=distribute.pc_search_result.none-task-blog-2%7Eall%7Esobaiduend%7Edefault-1-85280823-null-null.142%5Ev99%5Econtrol; weixin_49065061comment_new=1708915702250; creative_btn_mp=3; c_segment=8; Hm_lvt_6bcd52f51e9b3dce32bec4a3997715ac=1708650593,1708909495,1708995939,1708997067; Hm_lvt_e5ef47b9f471504959267fd614d579cd=1708999338; Hm_lpvt_e5ef47b9f471504959267fd614d579cd=1708999338; ssxmod_itna=YqRxuD2DB7D=VDBaqPrGkD0AY1wgS40KpYtdD/iDfo4iNDnD8x7YDv+mkWR+KRKj8uPW4=BkpAQfzjeuDefiQAhfqA8Sd4GLDmKDyQYgHeDxOq0rD74irDDxD3DbSdDSDWKD9D0+kSBuqtDm4GWCqGfDDoDYR=nDitD4qDBGEdDKqGgCdmQ+dqD0dwruAxbR00PMS2MUjtqFexeCqDMReGXFgFNveH1uAXPhaF4rx=DzM7DtdXMlMLdx0pB/TN4ZE3f+Avx3OPabGwKR0Dr80vxKimR=S+YDh+QYAkPe2+62NqDGbhbSxxxD; ssxmod_itna2=YqRxuD2DB7D=VDBaqPrGkD0AY1wgS40KpYtG9tPPxBLgfx7PwxmBaFMhOjlBj7PCZdFX1K3b7A6eqr8m7NkDrn8v=ADV0SO2d/A/Z8rLPZR4i=IME7Kghh5zZL8j6mhalSXRIu8CaV65Hq3qapgyPxZFHZYRGcZKxdtGxjAA=+QDdTfpGWOgWXoS4ezAEXxeD60Wqjj3FAhK4jASt228WspMEf+yO/fk6K/If3+kvDsQ0oyqLTfSwYGF6Yj3zMFaQfFH9YOQ0Yl=z608kr5tn7UMDN4Uu6X=IpNG8jbpb98uZPTu98N68aw6cKB6foPYuZXlYi7Zmj1te7vfoq/gQoU=eUdP2fo4tdUL3+DVmLCgj5hm9PRAWt2665F=Ah0F464I=t8=G=iFo6FK24fG8OGaaX+hDr/mF3bCarj=QggpTDG2RDdD08DijQYD; SESSION=61c03aeb-efd0-4ade-990f-88b7c3b3c987; hide_login=1; loginbox_strategy=%7B%22taskId%22%3A317%2C%22abCheckTime%22%3A1708999660770%2C%22version%22%3A%22ExpA%22%2C%22nickName%22%3A%22%E5%A5%A5%E7%89%B9%E6%9B%BC%E7%8B%82%E6%89%81%E5%B0%8F%E6%80%AA%E5%85%BD%22%7D; tfstk=eXrkxbsdjzuS8DK_H7n5DQJSyg_YPQiI1WKK9DhFujlj9TG-wJP3KRuU2bCQ-rV0sHhrwzd3-XPCFDFdRDc3a0uQyXh-tD2QstCOXGe7FDgN61I90EYb0DogwWE0F8iBhwtwQteWxEE9xMukk1HcKoWomfvMjavZJ_X-s8lDj-ZmUWY7ejxeYo4mrfoniH-UmYjrIE8ambtI39E2RegrhxcTOZNXEyEYmcBcnFeIzxMs6tXDRngrhx0lntY1T4kjF71..; UserName=weixin_49065061; UserInfo=46eb35a12b37485b8c59843d8fdc9340; UserToken=46eb35a12b37485b8c59843d8fdc9340; UserNick=%E5%A5%A5%E7%89%B9%E6%9B%BC%E7%8B%82%E6%89%81%E5%B0%8F%E6%80%AA%E5%85%BD; AU=ECD; UN=weixin_49065061; BT=1709000306131; Hm_up_6bcd52f51e9b3dce32bec4a3997715ac=%7B%22islogin%22%3A%7B%22value%22%3A%221%22%2C%22scope%22%3A1%7D%2C%22isonline%22%3A%7B%22value%22%3A%221%22%2C%22scope%22%3A1%7D%2C%22isvip%22%3A%7B%22value%22%3A%220%22%2C%22scope%22%3A1%7D%2C%22uid_%22%3A%7B%22value%22%3A%22weixin_49065061%22%2C%22scope%22%3A1%7D%7D; c_utm_source=vip_chatgpt_common_pc_toolbar; fe_request_id=1709002195292_3516_2752991; csrfToken=Bexiyixp7EypdAEH1EK9GC5z; dc_session_id=10_1709010626594.487419; SidecHatdocDescBoxNum=true; dc_sid=46407ebe960f8e788ef46c2c9676a57d; log_Id_pv=1497; log_Id_view=10662; log_Id_click=1604; dc_tos=s9i693; c_utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ERate-4-114632808-blog-113869295.235%5Ev43%5Epc_blog_bottom_relevance_base7; c_utm_relevant_index=9; firstDie=1; c_pref=default; c_first_ref=default; c_first_page=https%3A//blog.csdn.net/weixin_49065061/article/details/136030886; c_dsid=11_1709018491886.940654; c_ref=https%3A//blog.csdn.net/weixin_49065061%3Fspm%3D1010.2135.3001.5343; creativeSetApiNew=%7B%22toolbarImg%22%3A%22https%3A//img-home.csdnimg.cn/images/20230921102607.png%22%2C%22publishSuccessImg%22%3A%22https%3A//img-home.csdnimg.cn/images/20230920034826.png%22%2C%22articleNum%22%3A32%2C%22type%22%3A2%2C%22oldUser%22%3Atrue%2C%22useSeven%22%3Afalse%2C%22oldFullVersion%22%3Atrue%2C%22userName%22%3A%22weixin_49065061%22%7D; __gads=ID=880263f08407b0a2-22b2c8a4f0de00f9:T=1680225167:RT=1709018821:S=ALNI_MaX561SPpvig2_eELLwngPofwkqvw; __gpi=UID=00000be91cf6ffb1:T=1680225167:RT=1709018821:S=ALNI_Ma-9bIbEuZj8QGIT5So6pfMkO9v3A; __eoi=ID=1d4bfb2d82ad824c:T=1706666065:RT=1709018821:S=AA-AfjZXRr_q7qvzrD0W88T2y_Uf; c_page_id=default; Hm_lpvt_6bcd52f51e9b3dce32bec4a3997715ac=1709018848; waf_captcha_marker=3dd1e06207e934f225847b1c138c3bb847e875cc063d4e1ad985224b1cd3d2a2; dc_tos=s9i7hy',
            'referer': '{0}?spm=1010.2135.3001.5343'.format(hre),
            'user-agent': 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36'
        }

        hre = hre + "@"

        ex = '/article/details/(.*?)@'

        hr = re.findall(ex , hre)[0]

        data1 = {
            'commentId' : '',
            'content': calculate_days_to_holidays(),
            'articleId': '{0}'.format(hr)
        }

        try:
            if is_same_day:
                requests.post(url=url1, headers=headers2, data=data1)
                print("成功!")
                return True
            else:
                print(f"{now}还未发布文章!")
                return False
        except:
            print("失败！")
            return False
    else:
        print("请求网站失败！")
def condition_check():
    result = myfunction()
    return result

attempts = 0
max_attempts = 15
wait_time = 10 * 60 # 10分钟转换成秒

while attempts < max_attempts:
    if condition_check():
        print("条件满足，程序结束。")
        break
    else:
        attempts += 1
        print(f"条件不满足，等待再次检查...尝试次数 {attempts}/{max_attempts}")
        if attempts < max_attempts:
            time.sleep(wait_time)

if attempts == max_attempts:
    print("已达到最大尝试次数，程序结束。")

#     tree = etree.HTML(res)

#     lst = tree.xpath('//*[@id="floor-user-profile_485"]/div/div[2]/div/div[2]/div/div[2]/div/div')


#     for i in lst :

#         href = i.xpath('//article[@class="blog-list-box"]/a/@href')

#         hre = href[0]