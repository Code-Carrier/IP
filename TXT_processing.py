def txt_processing(filepath):
    array = []
    G =open(filepath,"r")
    print("open file successfully")
    temp = G.readline()
    while(temp != "end"):
        temp = temp.split("-->")
        id1 = int(temp[0])
        id2 = int(temp[1])
        array.append([id1,id2])
        temp = G.readline()
    G.close()
    return array

