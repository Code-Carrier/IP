from TXT_processing import txt_processing
from DATA_processing import data_processing
import csv


def txt_to_array(name):
    filepath = "./data/"+name+".txt"
    array = txt_processing(filepath)
    array = data_processing(array)
    num = len(array)
    print(num)
    routers = [[0 for i in range(num)] for j in range(num)]
    for i in range(num):
        for j in array[i]:
            routers[i][j] = routers[j][i] = 1
    return num,0,routers

if __name__ == "__main__":
    name = input("请输入文件名：")
    txt_to_array(name)