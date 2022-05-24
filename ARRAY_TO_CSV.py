from numpy import array
from CSV_TO_ARRAY import csv_to_array
import csv

def array_to_csv(name):
    num,rubish,routers = csv_to_array(name)
    i = 0
    array = []
    temp = []
    for elem in routers:
        for j in range(num):
            if elem[j] == 1:
                temp.append(j)
        array.append(temp)
        temp = []
    with open ("./data/"+name+".csv","w",encoding ="utf-8",newline="") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerows(array)


if __name__ == "__main__":
    name = input("请输入文件名：")
    array_to_csv(name)