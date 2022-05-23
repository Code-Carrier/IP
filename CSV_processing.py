def csv_processing(name):
    array = []
    with open("./data/"+name+".csv","r",encoding="utf-8") as csvfile:
        temp = csvfile.readline().split(",")
        while(temp != ['']):
            if temp == ['\n']:
                array.append([])
                temp = csvfile.readline().split(",")
                continue
            temp = [int(elem) for elem in temp]
            print(temp,type(temp))
            array.append(temp)
            temp = csvfile.readline().split(",")
        print (array)
    return array

if __name__ == "__main__":
    name = input("请输入文件名：")
    csv_processing(name)
