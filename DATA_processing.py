from tempfile import tempdir
from TXT_processing import txt_processing

def takefirst(elem):
    return elem[0]

def renum(array,temparray):
    dic = {}
    temp = -1
    now = -1
    for elem in temparray:
        if elem != temp:
            temp = elem
            now = now + 1
            dic[temp] = now
            print (dic)
    for elem in array:
        elem[0] = dic[elem[0]]
        elem[1] = dic[elem[1]]
    return array,now+1

def data_processing(array):
    temparray = []
    for elem in array:
        temparray.extend(elem)
    temparray.sort()
    array,max = renum(array,temparray)
    array.sort(key = takefirst)
    routers = [[] for i in range(max)]
    for elem in array:
        routers[elem[0]].append(elem[1])
    for elem in routers:
        elem.sort()
    return routers