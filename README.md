# 项目介绍

## 概述

本项目制作了一个简单的贪吃蛇小游戏，实现了UI界面，食物，地雷，毒草，排行榜，文件存储等功能。

## 游戏介绍

打开游戏后输入用户名以加载完成到哪一关卡。

共有四个关卡，关卡的区别为地图不同。

进入某一关卡后，玩家通过上下左右键控制小蛇的移动。小蛇移动的速度取决于游戏难度。小蛇在撞到墙壁（从第二关开始，地图的中间也会出现墙壁）或者撞到自身后死亡。

地图上会有不同种类的物品：

> 1. 食物 吃到后长度+1，同一时间只有1个
> 2. 毒草 吃到后长度-1，若长度为1会被毒死，同一时间有多个，数量取决于游戏难度
> 3. 炸弹 吃到后长度减半，若长度为1会被炸死，同一时间只有1个
> 4. 传送门 吃到后结束本关卡并开启下一关卡，在长度达到目标长度（取决于游戏难度）后开启

这些物品在关卡开始时生成，每吃到一个就会刷新一个（除传送门）

游戏有两个排行榜，一个是长度排行榜，一个是通关时间排行榜，分别记录各关卡和各难度的长度前五名和通关时间前五名的用户名和长度/通关时间。容易知道这两个目标是矛盾的，玩家可以选择挑战更快地通关或者尽可能达到更长的长度。

## 部分功能实现

### 图形界面

采用EasyX图形库实现图形界面。实现过程中遇到的主要困难是输出图片时，会遇到图片透明部分变成黑色覆盖背景的问题。为解决这一问题，查找相关资料后使用了基于三元光栅操作的透明贴图法。

`/*  输出透明图片  */
void PrintImg(IMAGE img, int x, int y)
{
    int width = img.getwidth();
    int height = img.getheight();
    IMAGE MaskImg(width, height);//掩码图
    DWORD* PmemImg = GetImageBuffer(&img);
    DWORD* PmemMaskImg = GetImageBuffer(&MaskImg);
    COLORREF MaskBk = PmemImg[0];
    for (int i = 0; i < width * height; i++)
    {
        if (PmemImg[i] <= 0x555555 && PmemImg[i] >= 0)
        {
            PmemMaskImg[i] = WHITE;
        }
        else
            PmemMaskImg[i] = BLACK;
    }
    putimage(x, y, &MaskImg, SRCAND);
    putimage(x, y, &img, SRCPAINT);
}`

每个界面都有对应的类，基本都包含以下三个函数：

> 1.OpenUI()
>
> > 加载该界面的图形并进行一些预处理，并调用RunUI().有些界面会返回RunUI()的值
>
> 2.RunUI()
>
> > 运行该界面，从鼠标或键盘读取操作并进行处理。有些界面的该函数会根据读取的操作返回不同的值，一般表示用户点击了什么按钮。
>
> 3.CloseUI()
>
> > ~~其实就是closegraph()~~

每个界面有一个入口函数，调用对应类的OpenUI()。根据OpenUI()的返回值进入新的界面。

~~（后来想想，这些函数的内容好像也可以放在类里面。算了不管了）~~

### 蛇的存储

采用双向链表存储蛇。

`struct Node
    {
        int x, y;//坐标
        struct Node* prev;//前驱
        struct Node* next;//后继
    };
    Node* Head, * Tail;//头节点和尾节点`

正常情况下移动，相当于以下操作：

> 1.根据当前方向在头节点“将要移动到的位置”创建一个新的节点
>
> 2.令新的节点为头节点，更新前驱后继等信息
>
> 3.移除尾节点

如果吃到了食物，则不进行操作3

代码实现：

`void Move()
    {
        Node* Div;
        Div = (Node*)malloc(sizeof(Node));
        Div->x = Head->x + dxy[Direction][0];
        Div->y = Head->y + dxy[Direction][1];
        Div->next = Head;
        Head->prev = Div;   //增加新节点并使其成为头节点
        Head = Div;
        PrintImg(SnakeHead, Realx(Head->x), Realy(Head->y));
        putimage(Realx(Head->next->x), Realy(Head->next->y), &NormalSquare);
        PrintImg(SnakeBody, Realx(Head->next->x), Realy(Head->next->y));//图片操作
        if (!EatenFood)
        {
            putimage(Realx(Tail->x), Realx(Tail->y), &NormalSquare);   
            Map.ChangeObject(Tail->x, Tail->y, 0);
            Tail = Tail->prev;
            Tail->next = nullptr;   //删除尾节点，相当于蛇移动了
        }
        else
        {
            Length++;
            EatenFood = 0;
        }
    }`

吃到毒草或炸弹，从尾部删除节点直到长度达到要求：

`bool Poisoned()//吃到毒草
    {
        if (Length == 1) {
            return 1;   //被毒死
        }
        putimage(Realx(Tail->x), Realx(Tail->y), &NormalSquare);
        Map.ChangeObject(Tail->x, Tail->y, 0);
        Tail = Tail->prev;
        Tail->next = nullptr;   //删除尾节点
        Length--;
        return 0;
    }
    bool Bombed()//吃到炸弹
    {
        if (Length == 1)
        {
            return 1;
        }
        int TargetLength = Length / 2;
        while (Length > TargetLength) //删除尾节点直到长度变为一半
        {
            putimage(Realx(Tail->x), Realx(Tail->y), &NormalSquare);
            Map.ChangeObject(Tail->x, Tail->y, 0);
            Tail = Tail->prev;
            Tail->next = nullptr;
            Length--;
        }
        return 0;
    }`

检测是否撞到自身：遍历所有节点，如果某个节点坐标与头部坐标相同则说明撞到了自身。

`bool Check()//是否合法
    {
        Node* Div = Head;
        if (Length < 5) return 0;
        while (Div->next != nullptr)
        {
            Div = Div->next;
            if (Div->x == Head->x && Div->y == Head->y)
            {
                return 1;
            }
        }
        return 0;
    }`

### 文件操作

排行榜文件的格式如下：

> 用户名/数据/用户名/数据/...用户名/数据

一共有8个排行榜文件，分别是4个长度排行榜和4个时间排行榜。每个文件有15个“用户名/数据/”，每种难度3个。游戏开始时会从文件中读取并记录在data1/data2中（以其中一个为例）：

`    freopen_s(&stream, "ScoreRank1.txt", "r", stdin);
    for (int i = 0; i <= 2; i++)
    {
        for (int j = 0; j <= 4; j++)
        {
            char c = getchar();
            while (c != '/')
            {
                data1[0][i][j].Name += c;
                c = getchar();
            }
            c = getchar();
            while (c != '/')
            {
                data1[0][i][j].Score *= 10;
                data1[0][i][j].Score += c - '0';
                c = getchar();
            }
        }
    }
    fclose(stdin);`

玩家完成一关后，会更新排行榜数据并按格式写入文件中：

`    freopen_s(&stream, "ScoreRank1.txt", "w", stdout);
    for (int i = 0; i <= 2; i++)
    {
        for (int j = 0; j <= 4; j++)
        {
            cout << data1[0][i][j].Name<<"/";
            printf_s("%d/", data1[0][i][j].Score);
        }
    }
    fclose(stdout);`

记录用户通过的关卡数的文件与排行榜数据文件格式类似，但在开头多了一个数字，表示记录的用户数量。玩家打开游戏输入用户名后，会读取文件并检测其中是否有玩家输入的用户名。如果有则会直接解锁玩家已完成的关卡。玩家完成一个关卡后会更新数据写入文件。

## 总结

在完成本项目的过程中，从未涉及图形界面的我学会了很多新知识，包括但不限于面向对象编程（还不太懂），图形界面的实现，寻路算法（可惜暂时没有实现）等。无论我能否进入社团，这都是一次难忘且收获颇丰的经历。最后感谢给予我帮助的学长学姐们。
