from TXT_processing import txt_processing
from DATA_processing import data_processing
import csv


def txt_to_csv(name):
    filepath = "./data/"+name+".txt"
    array = txt_processing(filepath)
    array = data_processing(array)
    with open ("./data/"+name+".csv","w",encoding ="utf-8",newline="") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerows(array)

if __name__ == "__main__":
    name = input("请输入文件名：")
    txt_to_csv(name)