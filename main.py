from router import Router
from CSV_processing import csv_processing
import xlwt

def main(routers):
    __rnum__ = len(routers)
    #建立路由器结点数组
    lrouter = []
    for i in range(__rnum__):
        lrouter.append(Router(i,routers[i]))
        lrouter[i].set_dic()

    #交换邻居信息，交换完成后则计算优先级
    for i in range(__rnum__):
        temp_list=lrouter[i].send_message_to_all_neb(0)
        for x in routers[i]:
            lrouter[x].recieve_message(0,i,temp_list)
    #交换优先级信息，设置等待队列
    for i in range(__rnum__):
        temp_list = lrouter[i].send_message_to_all_neb(1)
        for x in routers[i]:
            lrouter[x].recieve_message(1,i,temp_list)
    
    case = "yes"
    while(case):
        case = input("选择使用的方法--1:只分配一个色块,剩余用随机分配. 2:分配四个色块. 其余任意键退出")
        case = int(case)
        __colornum__ = 4
        if (case != 1) & (case != 2):
            print("now")
            break

        wait_for_update_from_their_neb = []
        right = [0 for i in range(__rnum__)]
        nowtime = 0
        #每轮所有节点都应当判断自己本轮是否可以参与分配色块
        go = True
        correct = 0
        while go:
            nowtime = nowtime + 1
            wait_for_update_because_finish = []
            for i in range(__rnum__):
                if right[i] < __colornum__:
                    ok,count = lrouter[i].select_color(case)
                else:
                    continue
                if ok:
                    wait_for_update_from_their_neb.append(i)
                    right[i] += count
                    if right[i] >= __colornum__:
                        correct = correct + 1
                        wait_for_update_because_finish.append(i)


            #每轮分配完成后应当更新所有节点的等待队列
            for elem in wait_for_update_from_their_neb:
                temp_list = lrouter[elem].send_message_to_all_neb(2)
                for x in routers[elem]:
                    ok = lrouter[x].recieve_message(2,elem,temp_list)
                    if not ok:
                        return "有冲突"
            wait_for_update_from_their_neb.clear()
            
            for elem in wait_for_update_because_finish:
                for x in routers[elem]:
                    lrouter[x].remove_neb_who_finished(elem)
            wait_for_update_because_finish.clear()

            #如果所有节点都分配结束，应当结束循环
            if correct == __rnum__:
                go = False
        
        #输出本次分配所有节点的第一个色块号中的最大块号
        temp_list = []
        for i in range(__rnum__):
            temp = lrouter[i].send_choose()   
            if temp[0] not in temp_list:
                temp_list.append(temp[0])
        print("共需要 ",len(temp_list)," 个颜色。")
        for i in range(__rnum__):
            lrouter[i].init_again()
        nowtime = 0
    case = input("选择使用的方法--1:只分配一个色块,剩余用随机分配. 2:分配四个色块. 其余任意键退出")

if __name__ == "__main__":
    name = input("输入文件名：")
    while(name):
        name += "_total"
        routers = csv_processing(name)
        ok = main(routers)
        print(ok)
        name = input("输入文件名：")    