def csv_processing(name):
    array = []
    for line in open("./data/"+name+".csv","r",encoding="utf-8"):
        array.append(line.strip())
    return array
