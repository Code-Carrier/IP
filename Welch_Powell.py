from initial import initArray
def welch_powell(routers):
    color = 0
    wait = []
    dic = {}
    for i in range(len(routers)):
        dic[i] = len(routers[i])
    temp = sorted(dic.items(), key = lambda kv:(kv[1], kv[0]),reverse = True)
    for i in range(len(routers)):
        wait.append(temp[i][0])
    now_wait = []
    while(len(wait) > 0):
        now_wait.extend(wait)
        while (len(now_wait) > 0):
            now = now_wait.pop(0)
            wait.remove(now)
            for elem in routers[now]:
                if (elem in now_wait):
                    now_wait.remove(i)
        color = color + 1
    print (color)
    return color

if __name__ == "__main__":
    __rnum__,lines,routers = initArray()
    welch_powell(routers)