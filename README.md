# IP

主要程序为：
main.py --> router.py --> welch_powell.py
其中输入是main函数调用csv_to_array函数读取“”+_total.csv中数据
输出为output文件夹下方的output.xlsx

output.xlsx sheet名称对应处理的文件名称，每个sheet中按照列顺序，依次指：
 1 节点id  2 case值（为1则说明是仅分配一个色块，为2则说明是分配四个色块） 3 case等于1时，每个节点分配的色块编号
 4 case值  5 case为2时，每个节点分配的四个色块号

Router.c 为c语言模板（不可运行）

其余的py文件为数据处理：
其中将原先的txt文件处理成链表形式的csv使用TXT_TO_CSV.py
由于此时有的节点虽然有边，但是边被归于对面的节点，所以在csv文件中会显示该点没有任何邻居节点
因此需要在进行处理，最终完全体现每个节点的邻居节点关系，则需要用到ARRAY_TO_CSV.py,其调用了CSV_TO_ARRAY.py将原先的csv文件先转化为数组再处理结束后转化为csv文件保存，此时文件夹名字变为“”+_total.csv
