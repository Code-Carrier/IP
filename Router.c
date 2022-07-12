#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define SITE_NUM 10000 //节点总数量
#define GRADE 3        //层级数量，即共分几个层级
#define COLOR_NUM 108
//色块总数量，为色块基础数量*2^(GRADE-1)，建议设置为108，一块ip数量600个，一个节点分配4块，共2400个。
//此时共用去64800个IP，剩余736个可以留作所有路由器的备用IP，以防连接人数超过2400。
//此时在备用IP中采用随机分配的方法。
#define SELF_COLOR 4 //自己分配的色块数量
#define NEB_NUM 1000 //最大邻居数量
#define TIMEOUT 5    //初始timeout时间
#define CEILING 50   //最长等待时间

// pcolor存放的是该节点使用每个颜色块的概率，千分制，其中第一个色块概率应当不为10的倍数，其余色块概率应当为10的倍数。
int pcolor[4] = {205, 100, 50, 10};

struct router
{
    int id;                     //编号
    int neb_num;                //邻居数量
    int neb[NEB_NUM];           //邻居数组
    int self_color[SELF_COLOR]; //自己选的色块编号
    int color[COLOR_NUM][2];    //颜色数组,第一列存放色块编号，第二列存放色块目前使用情况
};

//初始化router变量
int init(struct router *p, int id, int neb[])
{
    p->id = id;
    init_array(p->color, 1, SELF_COLOR, -1);
    init_array(p->color, 1, COLOR_NUM, 0);
    init_array(p->neb, 1, NEB_NUM, -1);
    int i = 0;
    for (; i < NEB_NUM; i++)
    {
        if (neb[i] < 0)
        { //其中neb数组中-1为无效数据，所有有效数据都排在无效数据之前
            break;
        }
        else
        {
            p->neb[i] = neb[i];
        }
    }
    p->neb_num = i;
    qsort(p->neb, p->neb_num, sizeof(int), cmp_neb); //将neb数组排序，方便后续可以折半查找（也可以用hash,让查找更快）
    return 0;
}

//发送信息给goal_id
int send_message(int self_id, int goal_id, int message[], int flag)
{
    //节点间通信
}

//从rec_id那里接受信息
int get_message(int self_id, int rec_id, int message[], int flag)
{
    //节点间通信
}

//给每个邻居发送message信息
int send_message_to_all_neb(int self_id, int neb[], int message[], int flag)
{
    for (int i = 0, goal_id; i < NEB_NUM; i++)
    {
        goal_id = neb[i];
        if (goal_id < -1)
        {
            break;
        }
        else
        {
            send_message(self_id, goal_id, message, flag);
        }
    }
}

//初始化数组为val值
int init_array(int *array, int x, int y, int val)
{
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            *(array + (y * i) + j) = val;
        }
    }
}

//查找val值在neb中是否存在
int id_in_neb(int val, int neb[], int neb_num)
{
    int low = 0, high = neb_num, temp;
    while (low <= high)
    {
        temp = (low + high) / 2;
        if (neb[temp] == val)
        {
            return 1;
        }
        else if (neb[temp] < val)
        {
            low = temp + 1;
        }
        else
        {
            high = temp - 1;
        }
    }
    return 0;
}

//从每个邻居那里获得邻居数列
int rec_neblist_from_every_neb(int self_id, int neb_num, int neb[], int neb_neb[][NEB_NUM])
{
    for (int i = 0, rec_id, mark[SITE_NUM] = {0}, x_count = 0; i < NEB_NUM; i++)
    {
        rec_id = neb[i];
        if (rec_id < 0)
        {
            break;
        }
        else
        {
            //两重循环是为了实现限时等待以及尽量等待最少时间
            for (int timeout = TIMEOUT; mark[rec_id] == 0 && timeout < CEILING; timeout *= 2)
            {
                for (int time = 0, ok = 0; mark[rec_id] == 0 && time < timeout; time++)
                {
                    int message[5000];
                    init_array(message, 1, 5000, -1);
                    ok = get_message(self_id, rec_id, message, 0);
                    if (ok == 1)
                    {
                        mark[rec_id] = 1;
                        // neb_neb[*][0]存放id编号，[*][1]存放邻居总数量，从[*][2]开始真正存放邻居编号。
                        // 这可能会占用2个存放邻居编号的空间，但只要设定的NEB_NUM足够大就不会影响。
                        neb_neb[x_count][0] = rec_id;
                        neb_neb[x_count][1] = 0;
                        for (int j = 0, temp = message[j], y_count = 2; temp > 0; j++, temp = message[j])
                        {
                            if (id_in_neb(temp, neb, neb_num))
                            {
                                neb_neb[x_count][1]++;
                                neb_neb[x_count][y_count++] = temp;
                            }
                        }
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
        }
    }
}

//根据规则添加节点进入wait数组
int add_wait(int self_priority[], int neb_priority[], int wait_high[][3], int wait_low[][3], int self_id, int neb_id)
{
    if (self_priority[0] < neb_priority[0])
    {
        memcpy(wait_high[neb_id], neb_priority, sizeof(int) * 3);
    }
    else if (self_priority[0] == neb_priority[0] && self_priority[1] < neb_priority[1])
    {
        memcpy(wait_high[neb_id], neb_priority, sizeof(int) * 3);
    }
    else if (self_priority[0] == neb_priority[0] && self_priority[1] == neb_priority[1] &&
             self_priority[2] < neb_priority[2])
    {
        memcpy(wait_high[neb_id], neb_priority, sizeof(int) * 3);
    }
    else
    {
        memcpy(wait_low[neb_id], neb_priority, sizeof(int) * 3);
    }
}

//设置等待队列
int set_wait(int self_id, int neb[], int self_priority[], int wait_high[][3], int wait_low[][3])
{
    //从每个邻居i那里获得i的优先级数组,mark为标记数组
    for (int i = 0, rec_id, mark[NEB_NUM] = {0}; i < NEB_NUM; i++)
    {
        rec_id = neb[i];
        if (rec_id < 0)
        {
            break;
        }
        else
        {
            for (int timeout = TIMEOUT; mark[rec_id] == 0 && timeout < CEILING; timeout *= 2)
            {
                for (int time = 0, ok = 0; mark[rec_id] == 0 && time < timeout; time++)
                {
                    int message[5000];
                    ok = get_message(self_id, rec_id, message, 1);
                    if (ok == 1)
                    {
                        mark[rec_id] = 1;
                        add_wait(self_priority, message, wait_high, wait_low, self_id, rec_id); //根据优先级数组选择性添加wait数组
                        break;
                    }
                }
            }
        }
    }
}

//等待wait数组中的节点都分配完全
int wait_for_all_id_in_wait_ok(int self_id, int wait[][2], int color[], int grade)
{
    int count = pow(2, grade - 1);
    for (int i = 0, rec_id, mark[NEB_NUM] = {0}; i < NEB_NUM; i++)
    {
        rec_id = wait[i][0];
        if (rec_id < 0)
        {
            break;
        }
        else
        {
            if (wait[i][1] == count)
            {
                mark[i] = 1;
            }
            for (int timeout = 5; mark[i] == 0 && timeout < CEILING; timeout *= 2)
            {
                for (int time = 0, message[5000]; mark[i] == 0 && time < timeout; time++)
                {
                    int ok = get_message(self_id, rec_id, message, 2);
                    if (ok == 1)
                    {
                        for (int i = 1, now = message[0]; message[i] > -1; now++, i++)
                        {
                            mark_color(color, message[i], now);
                            wait[i][1]++;
                        }
                        mark[i] = 1;
                    }
                }
            }
        }
    }
    return 0;
}

// qsort排序比较大小函数参数
// wait数组需要由大到小排序
int cmp_wait(const void *a, const void *b)
{
    if (*(int *)a != *(int *)b)
    {
        return *(int *)a - *(int *)b;
    }
    else if (*(int *)a == *(int *)b && *((int *)a + 1) != *((int *)b + 1))
    {
        return *((int *)a + 1) - *((int *)b + 1);
    }
    else
    {
        return *((int *)a + 2) - *((int *)b + 2);
    }
}
// neb数组需要由小到大排序
int cmp_neb(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
}
//颜色数组需要由小到大排序
int cmp_color(const void *a, const void *b)
{
    if (*((int *)a + 1) != *((int *)b + 1))
    {
        return *((int *)a + 1) - *((int *)b + 1);
    }
    else
    {
        return *(int *)a - *(int *)b;
    }
}
// welch_powell算法需要由大到小排序
int cmp_wel(const void *a, const void *b)
{
    if (*((int *)a + 1) != *((int *)b + 1))
    {
        return *((int *)a + 1) - *((int *)b + 1);
    }
    else
    {
        return *(int *)a - *(int *)b;
    }
}

// welch_powell算法，集中式算法计算分布色块的近似算法
int welch_powell(int neb_neb[][NEB_NUM], int neb_num)
{
    int color = -1;
    //邻居数量多的节点在排序放在数组前面
    qsort(neb_neb, neb_num, sizeof(int[NEB_NUM]), cmp_wel);
    int mark[NEB_NUM];
    init_array(mark, 1, NEB_NUM, -1);
    int temp_mark[NEB_NUM];
    init_array(temp_mark, 1, NEB_NUM, -1);
    int map[SITE_NUM];
    init_array(map, 1, SITE_NUM, -1);
    for (int i = 0; i < neb_num; i++)
    {
        map[neb_neb[i][0]] = i;
    }
    int count = -1;
    while (count < neb_num)
    {
        memcpy(temp_mark, mark, sizeof(int));
        color++;
        for (int i = 0; i < neb_neb; i++)
        {
            if (temp_mark[i] < 0)
            {
                mark[i] = color;
                temp_mark[i] = color;
                count++;
                for (int j = 2; j < NEB_NUM; j++)
                {
                    if (neb_neb[i][j] < 0)
                    {
                        break;
                    }
                    temp_mark[map[neb_neb[i][j]]] = 0;
                }
            }
        }
    }
    return color;
}

//计算优先级
int caculate_color_count(int neb_num, int neb_neb[][NEB_NUM])
{
    int color = welch_powell(neb_neb, neb_num); //使用welch powell算法
    return color + 1;
}

//计算色块层级，如果自身的色块需求大于定义的最大色块数量，则返回最低层级：1。
int caculate_grade(int self_id, int color_num)
{
    int grade = COLOR_NUM / pow(2, GRADE - 1);
    if (color_num < COLOR_NUM)
    {
        return GRADE - ceil(log((color_num / grade) + 1) / log(2));
    }
    else
    {
        return 1;
    }
}

//选择一个色块
int select_color(int color[][2], int now)
{
    qsort(color, COLOR_NUM, sizeof(int[2]), cmp_color);
    if (now == 0)
    {
        for (int i = 0; i < COLOR_NUM; i++)
        {
            if (color[i][1] % 10)
            {
                continue;
            }
            else
            {
                mark_color(color, i, now);
                return i;
            }
        }
    }
}

//使用wait来初始化temp_wait
int init_wait(int wait[][3], int temp_wait[][2])
{
    for (int i = 0; i < NEB_NUM && wait[i][0] != -1; i++)
    {
        temp_wait[i][0] = wait[i][2];
        temp_wait[i][1] = 0;
    }
}

//给自己分配色块
int distribute_color(int self_id, int neb[], int self_color[], int color[], int wait_high[][4], int wait_low[][4], int grade)
{
    int ok = 0;
    int turn = pow(2, GRADE - 1);
    int myturn = 1;                         //表示当前轮次本节点是否可以分配，可以为1，不可以为0
    int per_select_num = pow(2, grade - 1); //当前节点每轮应当选择的色块数量
    int temp_wait_high[NEB_NUM][2];         //建立临时等待队列用于每轮的删除操作
    int temp_wait_low[NEB_NUM][2];
    init_wait(wait_high, temp_wait_high);
    init_wait(wait_low, temp_wait_low);
    while (ok < turn)
    {
        if (myturn == 1)
        {
            wait_for_all_id_in_wait_ok(self_id, temp_wait_high, color, grade); //等待wait数组中的所有节点都分配结束一轮
            int color_message[COLOR_NUM];
            init_array(color_message, 1, COLOR_NUM, -1); //初始化消息队列
            color_message[0] = ok;                       // color_message[0]存储的是当前节点首先是为第几个色块选颜色
            for (int i = 0; i < per_select_num; ok++, i++)
            {
                int temp_color = select_color(color, ok);
                self_color[ok] = temp_color;
                color_message[i + 1] = temp_color;
            }
            //将自己这一轮分配选的色块广播给每个邻居,color_message[0]存储的是当前节点首先是为第几个色块选颜色
            send_message_to_all_neb(self_id, neb, color_message, 2); //标记位为2表示当前发送的是色块信息
            myturn = 0;
        }
        if (myturn == 0)
        {
            wait_for_all_id_in_wait_ok(self_id, temp_wait_low, color, grade); //等待wait数组中的所有节点都分配结束一轮
            myturn = 1;
        }
    }
}

//颜色分配完成后，持续接受邻居给自己发送的色块号，继续标记color数组
int rec_neb_color()
{
    //即正常运行事务，收到邻居发送的信息时给出响应即可。运行mark_color()
    return 0;
}

//根据信息在color数组中标记对应色块
int mark_color(int color[][2], int choose, int now)
{
    int i = find_color(color, choose);
    color[i][1] += (pcolor[now]);
}

int find_color(int color[][2], int key)
{
    for (int i = 0; i < COLOR_NUM; i++)
    {
        if (color[i][0] == key)
        {
            return i;
        }
    }
}

//运行函数
int run(struct router *p)
{
    send_message_to_all_neb(p->id, p->neb, p->neb, 0);              //给每个邻居发送一份自己的邻居数组副本
    int neb_neb[NEB_NUM][NEB_NUM];                                  //建立邻居关系图
    init_array(neb_neb, NEB_NUM, NEB_NUM, -1);                      //初始化邻居关系图
    rec_neblist_from_every_neb(p->id, p->neb_num, p->neb, neb_neb); //从每个邻居i那里获得i的邻居节点数组，存放在neb_neb中
    int color_count = caculate_color_count(p->neb_num, neb_neb);    //计算所需颜色块数量
    int priority_message[3] = {color_count, len(p->neb), p->id};    //建立优先级数组，包含颜色块数量，邻居数量，自身编号
    send_message_to_all_neb(p->id, p->neb, priority_message, 1);    //向每个邻居发送自己的优先级数组
    int wait_high[NEB_NUM][3];                                      //建立优先级比自己高的等待队列
    int wait_low[NEB_NUM][3];                                       //建立优先级比自己低的等待队列
    init_array(wait_high, NEB_NUM, 3, -1);                          //初始化等待队列
    init_array(wait_low, NEB_NUM, 3, -1);
    set_wait(p->id, p->neb, priority_message, wait_high, wait_low); //根据自身优先级与邻居优先级设置等待数组
    qsort(wait_high, NEB_NUM, sizeof(int[3]), cmp_wait);            // wait数组排序，保证优先级最高的在队首
    qsort(wait_low, NEB_NUM, sizeof(int[3]), cmp_wait);
    int grade = caculate_grade(p->id, priority_message[0]);                               //计算自己的色块层级，GRADE为最高层级，一次性分配最多的色块
    distribute_color(p->id, p->neb, p->self_color, p->color, wait_high, wait_low, grade); //开始给自己分配色块
    rec_neb_color();                                                                      //运行正常任务的同时，可持续获得未分配完全的邻居的色块信息
    return 0;
}

//主函数
int main()
{
    struct router routers[SITE_NUM];
    int neb[SITE_NUM][NEB_NUM]; //根据文件导入
    for (int i = 0; i < SITE_NUM; i++)
    {
        init(&routers[i], i, neb[i]);
    }
    for (int i = 0; i < SITE_NUM; i++)
    {
        run(&routers[i]);
    }
}