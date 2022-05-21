import queue
import threading

__colornum__ = 1000
queue = queue.Queue
routers = [[-1 for i in range(10)] for j in range(10)]
routers[0][0] = 2
routers[2][0] = 0

class Router(threading.Thread):
    
    def __init__(self,id,queue):
        threading.Thread.__init__(self, name=id)
        print(threading.current_thread)
        self.data = queue
    
    def run(self):
        choose = -1
        neb = []
        for temp in routers[0]:
            if temp >= 0:
                neb.append(temp)
        num = len(neb)
        print(neb)
        print(num)
        
if "__name__" == "__main__":
    lrouters = []
    test = Router("test",queue)
    test.start()
    test.join()
    for i in range(10):
        temp = Router(i,queue)
        lrouters.append(temp)
    for i in range(10):
        lrouters[i].start()


