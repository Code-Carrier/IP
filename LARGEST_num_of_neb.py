from CSV_processing import csv_processing
import csv

def largest_num_of_neb(name):
    array = csv_processing(array)
    max = 0
    for elem in array:
        temp = len(elem)
        if max < temp:
            max = temp
    print(max)

if __name__ == "__main__":
    name = input("请输入文件名：")
    largest_num_of_neb(name)