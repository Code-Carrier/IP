from multiprocessing.connection import wait
from time import time
from random import *
from queue import Queue
from math import floor
from Welch_Powell import welch_powell
__colornum__ = 100
__selfcolornum__ = 4
__pcolor__ = [205,100,50,10]     #千分制

class Router(object):

    def __init__(self,id,neb_id_array):
        self.id=id              #编号
        self.neb=neb_id_array   #邻居
        self.dic={}             #邻居编号重映射字典
        self.priority=[-1,-1,-1]     #自己的优先级,分别为id，邻居数量，优先级
        self.choose= []              #自己选的色块编号
        self.color={}           #颜色字典，key为色块号，val为当前key色块已被分配的程度
        self.init_color_dic()
        #以下数据是因为代码模拟，数据必须要存放在类里面，实际中是临时数据。
        self.grade = 0          #自己的层级
        self.wait_high = []
        self.wait_low =[]
        self.current_choose = []
        self.neb_neb = []
        self.temp_high = []
        self.temp_low = []
        self.count = 0
        self.turn = 0
        self.mark = [0 for i in range(len(self.neb))]
        self.copy_wait_high = []
        self.copy_wait_low = []

    def init_again(self):
        self.init_color_dic()
        self.choose.clear()
        self.temp_high.clear()
        self.temp_low.clear()
        self.copy_wait_array(self.copy_wait_high,self.wait_high,0)
        self.copy_wait_array(self.copy_wait_low,self.wait_low,0)
        self.temp_high.extend(self.wait_high)
        self.temp_low.extend(self.wait_low)
        self.turn = 0
        self.count = 0

    def init_array(self,array,x,y,val):
        if y == 0:
            for i in range(x):
                array[i] =val        
        for i in range(x):
            for j in range(y):
                array[i][j]=val

    def init_color_dic(self):
        for i in range(__colornum__):
            self.color[i] = 0

    def set_dic(self):
        temp = 0
        for i in self.neb:
            self.dic[i] = temp
            temp = temp + 1

    def send_message_to_all_neb(self,flag):
        if flag == 0:
            return self.neb
        if flag == 1:
            return self.priority
        if flag == 2:
            return self.current_choose
    
    #输出检测
    def send_choose(self):
        return self.choose

    def recieve_message(self,flag,rec_id,temp_data):
        if flag == 0:
            if self.mark[self.dic[rec_id]] == 0:
                temp = [self.dic[rec_id]]
                temp.extend(temp_data)
                self.neb_neb.append(temp)
                self.count += 1
                if self.count == len(self.neb):
                    self.caculate_priorty()
                    self.count = 0
                    self.init_array(self.mark,1,0,0)
            return True
        if flag == 1:
            if self.mark[self.dic[rec_id]] == 0:
                self.add_wait(temp_data)
                self.count += 1
                if self.count == len(self.neb):
                    self.set_wait_array()
                    self.count = 0
                    self.init_array(self.mark,1,0,0)
            return True
        if flag == 2:
            ok = self.set_color(rec_id,temp_data)
            return ok

    def caculate_priorty(self):
        self.neb_neb.sort()
        for i in range(len(self.neb_neb)):
            self.neb_neb[i] = self.neb_neb[i][1:]
            temp_list = []
            for elem in self.neb_neb[i]:
                if elem not in self.neb:
                    temp_list.append(elem)
            for elem in temp_list:
                self.neb_neb[i].remove(elem)
            for j in range(len(self.neb_neb[i])):
                self.neb_neb[i][j] = self.dic[self.neb_neb[i][j]]
        self.priority[0] = welch_powell(self.neb_neb) + 1
        if self.priority[0] <= __colornum__/__selfcolornum__:
            self.grade = 2
        elif self.priority[0] <= 2*__colornum__/__selfcolornum__:
            self.grade = 1
        else:
            self.grade = 0
        self.priority[1] = len(self.neb)
        self.priority[2] = self.id

    def add_wait(self,priority):
        if priority[0] > self.priority[0]:
            self.wait_high.append(priority)
        elif priority[0] == self.priority[0]:
            if priority[1] > self.priority[1]:
                self.wait_high.append(priority)
            elif priority[1] == self.priority[1]:
                if priority[2] > self.priority[2]:
                    self.wait_high.append(priority)
                else:
                    self.wait_low.append(priority)
            else:
                self.wait_low.append(priority)
        else:
            self.wait_low.append(priority)

    def set_wait_array(self):
        self.wait_high.sort()
        self.wait_low.sort()
        self.copy_wait_array(self.wait_high,self.copy_wait_high,1)
        self.copy_wait_array(self.wait_low,self.copy_wait_low,1)
        self.copy_wait_array(self.wait_high,self.temp_high,0)
        self.copy_wait_array(self.wait_low,self.temp_low,0)
        self.wait_high.clear()
        self.wait_low.clear()
        self.wait_high.extend(self.temp_high)
        self.wait_low.extend(self.temp_low)
        

    def remove_neb_who_finished(self,id):
        if id in self.wait_high:
            self.wait_high.remove(id)
        if id in self.wait_low:
            self.wait_low.remove(id)
        if id in self.temp_high:
            self.temp_high.remove(id)
        if id in self.temp_low:
            self.temp_low.remove(id)

    def copy_wait_array(self,src,aid,flag):
        if flag == 0:
            for elem in src:
                aid.append(elem[2])
        if flag == 1:
            for elem in src:
                temp = []
                temp.extend(elem)
                aid.append(temp)

    def set_color(self,id,color_array):
        if id in self.temp_high:
            self.temp_high.remove(id)
        if id in self.temp_low:
            self.temp_low.remove(id)
        if color_array[0] == 0:
            if self.choose:    
                if self.choose[0] == color_array[1]:
                    return False            
        for i in range(1,len(color_array)):
            now_color = color_array[0] + i - 1
            self.color[color_array[i]] += __pcolor__[now_color]
        return True

    def select_turn(self,case):
        if self.turn == 0:      #0说明该节点本轮尚未分配过
            if self.temp_high:
                return False,0
            self.current_choose.clear()
            now_color = len(self.choose)
            self.current_choose.append(now_color)
            temp_color = self.select_color(now_color)
            self.choose.append(temp_color)
            self.current_choose.append(temp_color)
            now_color += 1
            if case == 1:
                if now_color >= 1:
                    return True,__selfcolornum__
                else:
                    return False,0
            self.count = pow(2,self.grade)
            temp_now = len(self.current_choose) - 1
            while(temp_now < self.count):
                temp_color = self.select_color(now_color)
                self.choose.append(temp_color)
                self.current_choose.append(temp_color)
                temp_now += 1
                now_color += 1
            self.turn = 1
            self.temp_high.extend(self.wait_high)
            return True,len(self.current_choose) - 1
        if self.turn == 1:      #1说明该节点本轮已经分配结束
            if self.temp_low:
                return False,0
            self.turn = 0
            self.temp_low.extend(self.wait_low)
            return self.select_turn(case)

    def select_color(self,now_color):
        temp = sorted(self.color.items(),key = lambda kv:(kv[1], kv[0]))
        if now_color == 0:
            for item in temp:
                if item[1]%10 != 0:
                    continue
                else:
                    self.color[item[0]] += __pcolor__[0]
                    return item[0]
        else:
            for item in temp:
                if item[0] not in self.choose:
                    self.color[item[0]] += __pcolor__[now_color]
                    return item[0]
                else:
                    continue