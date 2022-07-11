from time import time
from random import *
from queue import Queue
from math import floor
from Welch_Powell import welch_powell
__colornum__ = 100
__selfcolornum__ = 4
class Router(object):

    def __init__(self,id,neb_id_array):
        self.id=id              #编号
        self.neb=neb_id_array   #邻居
        self.dic={}             #邻居编号重映射字典
        self.priority=[-1,-1,-1]     #自己的优先级,分别为id，邻居数量，优先级
        self.choose= []                 #自己选的色块编号
        self.color=[0 for i in range(__colornum__)]         #颜色数组
        self.first_color=[0 for i in range(__colornum__)]   #专门每个节点的第一个颜色块
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

    def init_again(self):
        self.init_array(self.priority,3,0,-1)
        self.init_array(self.color,__colornum__,0,0)
        self.init_array(self.choose,__selfcolornum__,0,-1)
        self.wait.clear()
        self.neb_neb.clear()

    def init_array(self,array,x,y,val):
        if y == 0:
            for i in range(x):
                array[i] =val        
        for i in range(x):
            for j in range(y):
                array[i][j]=val

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

    def recieve_message(self,flag,rec_id,temp_data):
        if flag == 0:
            if self.mark[self.dic[rec_id]] == 0:
                self.neb_neb.attend([self.dic[rec_id]].extend(temp_data))
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
        for elem in self.neb_neb:
            elem = elem[1:]
            for elem2 in elem:
                elem2 = self.dic[elem2]
        self.priority[0] = welch_powell(self.neb_neb)
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
            elif priority[0] == self.priority[0]&priority[1] == self.priority[1]:
                if priority[2] > self.priority[2]:
                    self.wait_high.append(priority)
                else:
                    self.wait_low.append(priority)

    def set_wait_array(self):
        self.wait_high.sort()
        self.wait_low.sort()
        self.copy_wait_array(self.wait_high,self.temp_high)
        self.copy_wait_array(self.wait_low,self.temp_low)

    def copy_wait_array(self,src,aid):
        for elem in src:
            aid.append(elem[2])

    def set_color(self,id,color_array):
        if id in self.temp_high:
            self.temp_high.remove(id)
        if id in self.temp_low:
            self.temp_low.remove(id)
        if self.choose[0] == color_array[0]:
            return False
        for i in range(1,len(color_array)):
            self.color[color_array[i]] += 1
        return True

    def select_color(self,case):
        if self.turn == 0:      #0说明该节点本轮尚未分配过
            if self.temp_high:
                return False
            self.current_choose.clear()
            if self.choose:
                for i in range(len(self.color)):
                    if self.color[i] == 0:
                        self.choose.append(i)
                        self.current_choose.append(i)
                        self.color[i] += 1
                        self.first_color[i] += 1
            if self.choose:
                for i in range(len(self.first_color)):
                    if self.first_color[i] == 0:
                        self.choose.append(i)
                        self.current_choose.append(i)
                        self.color[i] += 1
                        self.first_color[i] += 1
            if case == '1':
                if self.choose:
                    return False,0
                else:
                    return True,1
            self.count = pow(2,self.grade)
            temp_now = len(self.current_choose)
            temp_color = 0
            while(temp_now < self.count):
                for i in range(len(self.color)):
                    if self.color[i] == temp_color:
                        self.choose.append(i)
                        temp_now += 1
                        self.color[i] += 1
                    else:
                        continue
                temp_color += 1
            if temp_now == self.count:
                self.current_choose = [self.choose[0]].extend(self.current_choose)
                self.turn == 1
                self.copy_wait_array(self.wait_high,self.temp_high)
                return True,len(self.current_choose)
        if self.turn == 1:      #1说明该节点本轮已经分配结束
            if self.temp_low:
                return False,0
            self.turn == 0
            self.copy_wait_array(self.wait_low,self.temp_low)
            self.select_color(case)