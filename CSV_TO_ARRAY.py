from CSV_processing import csv_processing
import csv


def csv_to_array(name):
    array = csv_processing(name)
    num = len(array)
    routers = [[0 for i in range(num)] for j in range(num)]
    for i in range(num):
        for j in array[i]:
            routers[i][j] = routers[j][i] = 1
    return num,0,routers

if __name__ == "__main__":
    name = input("请输入文件名：")
    csv_to_array(name)
