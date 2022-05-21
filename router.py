import queue
import threading
from initial import initArray

__colornum__ = 1000

routers = initArray()

class Router(threading.Thread):
    
    def __init__(self,id,queue):
        self.id = id
        self.data = queue
    
    def run(self):
        choose = -1
        neb = []
        for temp in routers[id]:
            if temp > 0:
                neb.append(temp)
        num = len(neb)
        neb_neb = [[0 for i in range(num)] for j in range (num)]
        

