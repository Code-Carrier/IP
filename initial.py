from random import *
def initArray():
    __rnum__ = 200
    routers = [[0 for i in range(__rnum__)] for j in range(__rnum__)]
    for i in range(__rnum__):
        temp = int(random()*__rnum__)
        if temp != i:
            routers[i][temp] = routers[temp][i] = 1
        for j in range(__rnum__):
            if i != j:
                if random()<0.2:
                    routers[i][j] = routers[j][i] = 1
    lines = 0
    for i in range(__rnum__):
        for j in range(i):
            if routers[i][j] == 1:
                lines = lines + 1
    print("随机完成，共有 ",lines," 条边。")
    return routers