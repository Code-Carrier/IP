#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define SITE_NUM 10000 //节点总数量
#define GRADE 3        //层级数量，即共分几个层级。
#define COLOR_NUM 108
//色块总数量，为色块基础数量*2^(GRADE-1)，建议设置为108，一块ip数量600个，一个节点分配4块，共2400个。
//此时共用去64800个IP，剩余736个可以留作所有路由器的备用IP，以防连接人数超过2400。
//此时在备用IP中采用随机分配的方法。
#define SELF_COLOR 3 //自己分配的色块数量
#define NEB_NUM 1000 //最大邻居数量
#define TIMEOUT 5    //初始timeout时间
#define CEILING 50   //最长等待时间

struct router
{
    int id;                     //编号
    int neb[NEB_NUM];           //邻居数组
    int self_color[SELF_COLOR]; //自己选的色块编号
    int color[COLOR_NUM];       //颜色数组
};

//初始化router变量
int init(struct router *p, int id)
{
    p->id = id;
    for (int i = 0; i < SELF_COLOR; i++)
    {
        p->self_color[i] = -1;
    }
    for (int i = 0; i < COLOR_NUM; i++)
    {
        p->color[i] = 0;
    }
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

//初始化数组
int init_array(int *array, int x, int y)
{
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            *(array + (y * i) + j) = -1;
        }
    }
}

//从每个邻居那里获得邻居数列
int rec_neblist_from_every_neb(int self_id, int neb[], int neb_neb[][NEB_NUM])
{
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
                    ok = get_message(self_id, rec_id, message, 0);
                    if (ok == 1)
                    {
                        mark[rec_id] = 1;
                        memcpy(neb_neb[rec_id], message, sizeof(int) * NEB_NUM);
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
int add_wait(int self_priority[], int neb_priority[], int wait[][4], int self_id, int neb_id)
{
    if (self_priority[0] < neb_priority[0])
    {
        memcpy(wait[neb_id], neb_priority, sizeof(int) * 3);
        wait[neb_id][3] = neb_id;
    }
    else if (self_priority[0] == neb_priority[0] && self_priority[1] < neb_priority[1])
    {
        memcpy(wait[neb_id], neb_priority, sizeof(int) * 3);
        wait[neb_id][3] = neb_id;
    }
    else if (self_priority[0] == neb_priority[0] && self_priority[1] == neb_priority[1] &&
             self_priority[2] < neb_priority[2])
    {
        memcpy(wait[neb_id], neb_priority, sizeof(int) * 3);
        wait[neb_id][3] = neb_id;
    }
    return 0;
}

//设置等待队列
int set_wait(int self_id, int neb[], int self_priority[], int wait[][4])
{
    for (int i = 0, rec_id, mark[NEB_NUM] = {0}; i < NEB_NUM; i++)
    { //从每个邻居i那里获得i的优先级数组,mark为标记数组
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
                        add_wait(self_priority, message, wait, self_id, rec_id); //根据优先级数组选择性添加wait数组
                        break;
                    }
                }
            }
        }
    }
}

//等待wait数组中的节点都分配完全
int wait_for_all_id_in_wait_ok(int self_id, int wait[][4], int color[], int grade)
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
                        int turn = message[0];
                        wait[i][1] += turn;
                        mark[i] = 1;
                        for (int i = 1; i < turn; i++)
                        {
                            mark_color(color, message[i]);
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int cmp_wel(void *a,void *b){

}

// welch_powell算法，集中式算法计算分布色块的近似算法,未完成
int welch_powell(int neb_neb[][NEB_NUM], int neb_num)
{
    int color = 0;
    int mark[NEB_NUM];
    init_array(mark, 1, NEB_NUM);
    int sort_array[NEB_NUM][2];
    init_array(sort_array, NEB_NUM, 2);
    int count = 0;
    for (int i = 0; i < NEB_NUM; i++)
    {
        if (neb_neb[i][0]<0){
            continue;
        }
        sort_array[count][0] = i;
        sort_array[count][1] =0;
        for(int j = 0;j<NEB_NUM;j++){
            if (neb_neb[i][j]< 0){
                break;
            }
            sort_array[count][1] ++;
        }
        count ++;
    }
    qsort(sort_array,count,sizeof(int[2]),cmp_wel);
    int temp_mark[NEB_NUM] = {0};
    while()


    return color;
}

//计算优先级
int caculate_priority(int neb_neb[][NEB_NUM])
{
    int count = 0;
    for (int i = 0; i < NEB_NUM; i++)
    {
        if (neb_neb[i][0] < 0)
        {
            break;
        }
        count++;
    }
    int color = welch_powell(neb_neb, count); //使用welch powell算法
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

// qsort排序比较大小函数参数
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

//选择一个色块
int select_color(int color[])
{
    for (int count = 0;; count++)
    {
        for (int i = 0; i < COLOR_NUM; i++)
        {
            if (color[i] == count)
            {
                return i;
            }
        }
    }
}

//重调wait数组，目的是将neb编号放在首位，第二位空出来存放对应节点已分配的色块数量
int reset_wait(int wait[][4])
{
    for (int i = 0; i < NEB_NUM, wait[i][0] != -1; i++)
    {
        wait[i][0] = wait[i][3];
        wait[i][1] = 0;
        wait[i][2] = -1;
        wait[i][3] = -1;
    }
}

//给自己分配色块
int distribute_color(int self_id, int neb[], int self_color[], int color[], int wait[][4], int grade)
{
    int ok = 0;
    int turn = pow(2, GRADE - 1);
    int per_select_num = pow(2, grade - 1); //当前节点每轮应当选择的色块数量。
    while (ok < turn)
    {
        wait_for_all_id_in_wait_ok(self_id, wait, color, grade); //等待wait数组中的所有节点都分配结束一轮
        int color_message[COLOR_NUM];
        init_array(color_message, 1, COLOR_NUM); //初始化消息队列
        color_message[0] = per_select_num;       // color_message[0]存储的是当前节点的per_select_num，代表该节点一次分配grade个色块。
        for (int i = 0; i < per_select_num; ok++, i++)
        {
            int temp_color = select_color(color);
            mark_color(color, temp_color);
            self_color[ok] = temp_color;
            color_message[ok + 1] = temp_color;
        }
        //将自己这一轮分配选的色块广播给每个邻居,color_message[0]存储的是当前节点的per_select_num，代表该节点一次分配per_select_num个色块。
        send_message_to_all_neb(self_id, neb, color_message, 2); //标记位为2表示当前发送的是色块信息
    }
}

//颜色分配完成后，持续接受邻居给自己发送的色块号，继续标记color数组
int rec_neb_color()
{
    //即正常运行事务，收到邻居发送的信息时给出响应即可。运行mark_color()
    return 0;
}

//根据信息在color数组中标记对应色块
int mark_color(int color[], int choose)
{
    color[choose]++;
}

//运行函数
int run(struct router *p)
{
    send_message_to_all_neb(p->id, p->neb, p->neb, 0);                     //给每个邻居发送一份自己的邻居数组副本
    int neb_neb[NEB_NUM][NEB_NUM];                                         //建立邻居关系图
    init_array(neb_neb, NEB_NUM, NEB_NUM);                                 //初始化邻居关系图
    rec_neblist_from_every_neb(p->id, p->neb, neb_neb);                    //从每个邻居i那里获得i的邻居节点数组，存放在neb_neb中
    int color_count = caculate_color_count(neb_neb);                       //计算所需颜色块数量
    int priority_message[3] = {color_count, len(p->neb), p->id};           //建立优先级数组，包含颜色块数量，邻居数量，自身编号
    send_message_to_all_neb(p->id, p->neb, priority_message, 1);           //向每个邻居发送自己的优先级数组
    int wait[NEB_NUM][4];                                                  //建立等待队列
    init_array(wait, NEB_NUM, 4);                                          //初始化等待队列
    set_wait(p->id, p->neb, priority_message, wait);                       //根据自身优先级与邻居优先级设置等待数组
    qsort(wait, NEB_NUM, sizeof(int[4]), cmp_wait);                        //wait数组排序，保证优先级最高的在队首
    int grade = caculate_grade(p->id, priority_message[0]);                //计算自己的色块层级，GRADE为最高层级，一次性分配最多的色块
    reset_wait(wait);                                                      //将wait数组调整一下，此时已经不需要其中的优先级信息
    distribute_color(p->id, p->neb, p->self_color, p->color, wait, grade); //开始给自己分配色块
    rec_neb_color();                                                       //运行正常任务的同时，可持续获得未分配完全的邻居的色块信息
    return 0;
}

//主函数
int main()
{
    struct router routers[SITE_NUM];
    for (int i = 0; i < SITE_NUM; i++)
    {
        init(&routers[i], i);
    }
    for (int i = 0; i < SITE_NUM; i++)
    {
        run(&routers[i]);
    }
}