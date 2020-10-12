#include <graphics.h>      
#include <conio.h>
#include <tchar.h>
#include <iostream>
#include <cstdio>
#include <ctime>
#define Realx(x) 30*x-15
#define Realy(y) 30*y-16

using namespace std;

void Main();
void LevelChoose();
void Settings();
void Tips();
void Game();

/*  输出透明图片  */
void PrintImg(IMAGE img, int x, int y)
{
    int width = img.getwidth();
    int height = img.getheight();
    IMAGE MaskImg(width, height);
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
}

/*  返回按钮  */
class BackToMain
{
public:
    IMAGE Button;
    int x, y, height, width;
    BackToMain(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("BACKTOMAIN"));
        else loadimage(&Button, _T("IMAGE"), _T("BACKTOMAIN"), Inwidth, Inheight);
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

/*  界面方块    */
class NormalSquare
{
public:
    IMAGE Square;
    int x, y, height, width;
    void SetSquare(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Square, _T("IMAGE"), _T("NORMALSQUARE"));
        else loadimage(&Square, _T("IMAGE"), _T("NORMALSQUARE"), Inwidth, Inheight);
        width = Square.getwidth();
        height = Square.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintSquare() {
        PrintImg(Square,x,y);
    }
};

/*  墙壁方块    */
class WallSquare
{
public:
    IMAGE Square;
    int x, y, height, width;
    void SetSquare(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Square, _T("IMAGE"), _T("WALLSQUARE"));
        else loadimage(&Square, _T("IMAGE"), _T("WALLSQUARE"), Inwidth, Inheight);
        width = Square.getwidth();
        height = Square.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintSquare() {
        PrintImg(Square, x, y);
    }
};

/*  蛇   */
class Snake
{
public:
    IMAGE SnakeHead, SnakeBody, NormalSquare;
    int Direction,Length;
    int dxy[5][2] = { {0,0},{0,-1},{1,0},{0,1},{-1,0} };
    bool EatenFood;
    //用链表实现蛇的存储
    struct Node
    {
        int x, y;
        struct Node* prev;
        struct Node* next;
    };
    Node* Head, * Tail;
    void BuildSnake() 
    {
        loadimage(&SnakeHead, _T("IMAGE"), _T("SNAKEHEAD"), 30, 30);
        loadimage(&SnakeBody, _T("IMAGE"), _T("SNAKEBODY"), 30, 30);
        getimage(&NormalSquare, Realx(3), Realy(3), 30, 30);
        Head = (Node*)malloc(sizeof(Node));
        Tail = (Node*)malloc(sizeof(Node));
        Head->x = 13;
        Head->y = 11;
        PrintImg(SnakeHead, Realx(Head->x), Realy(Head->y));
        Tail = Head;
        Direction = 1;
        Length = 1;
        EatenFood = 1;
    }
    void Move() 
    {
        Node* Div;
        Div = (Node*)malloc(sizeof(Node));
        Div->x = Head->x + dxy[Direction][0];
        Div->y = Head->y + dxy[Direction][1];
        Div->next = Head;
        Head->prev = Div;
        Head = Div;
        PrintImg(SnakeHead, Realx(Head->x), Realy(Head->y));
        putimage(Realx(Head->next->x), Realy(Head->next->y), &NormalSquare);
        PrintImg(SnakeBody, Realx(Head->next->x), Realy(Head->next->y));
        if (!EatenFood)
        {
            putimage(Realx(Tail->x), Realx(Tail->y), &NormalSquare);
            Tail = Tail->prev;
            Tail->next = nullptr;
        }
        else
        {
            Length++;
            EatenFood = 0;
        }
    }
    bool Poisoned()
    {
        if (Length == 1) {
            return 1;
        }
        putimage(Realx(Tail->x), Realx(Tail->y), &NormalSquare);
        Tail = Tail->prev;
        Tail->next = nullptr;
        Length--;
    }
    bool Bombed()
    {
        if (Length == 1)
        {
            return 1;
        }
        int TargetLength = Length / 2;
        while (Length > TargetLength)
        {
            putimage(Realx(Tail->x), Realx(Tail->y), &NormalSquare);
            Tail = Tail->prev;
            Tail->next = nullptr;
            Length--;
        }
        return 0;
    }
    bool Check()
    {
        if (Length < 5) return 0;
        Node* Div = Head;
        while (Div->next != nullptr)
        {
            Div = Div->next;
            if (Div->x == Head->x && Div->y == Head->y)
            {
                return 1;
            }
        }
        return 0;
    }
    void ChangeDirection(int TargetDirection)
    {
        if ((Direction != TargetDirection) && (Direction != TargetDirection + 2) && (Direction != TargetDirection - 2))
        {
            Direction = TargetDirection;
        }
    }
};

/*  游戏  */
class GameUI
{
public:
    void RunUI()
    {
        Snake GameSnake;
        GameSnake.BuildSnake();
        clock_t GameSecond = 1 * CLOCKS_PER_SEC;
        clock_t BeginTime = clock();
        int SecondPast = 0;
        while (1)
        {
            if (clock() - BeginTime >= (SecondPast + 1) * GameSecond)
            {
                SecondPast++;
                GameSnake.Move();
            }
            if (_kbhit())
            {
                int kb = _getch();
                switch (kb)
                {
                case 72:
                    GameSnake.ChangeDirection(1);
                    break;
                case 77:
                    GameSnake.ChangeDirection(2);
                    break;
                case 80:
                    GameSnake.ChangeDirection(3);
                    break;
                case 75:
                    GameSnake.ChangeDirection(4);
                    break;
                default:
                    break;
                }
            }
        }
    }
    void OpenUI() 
    {
        IMAGE BackImage;
        loadimage(&BackImage, _T("IMAGE"), _T("GAMEUI_BACKIMAGE"), 1080, 680);
        initgraph(1080, 680);
        putimage(0, 0, &BackImage);
        /*  输出返回图标  */
        BackToMain Button(860, 30, 200, 30);
        Button.PrintButton();
        /*  输出界面方块  */
        NormalSquare Square1[30][25];
        for (int i = 2; i <= 25; i++)
        {
            for (int j = 2; j <= 21; j++)
            {
                Square1[i][j].SetSquare(Realx(i), Realy(j), 30, 30);
                Square1[i][j].PrintSquare();
            }
        }
        WallSquare Square2[30][25];
        for (int i = 1; i <= 26; i++)
        {
            for (int j = 1; j <= 22; j++)
            {
                if (i == 1 || i == 26 || j == 1 || j == 22)
                {
                    Square2[i][j].SetSquare(Realx(i), Realy(j), 30, 30);
                    Square2[i][j].PrintSquare();
                }
            }
        }
        RunUI();
    }
};

/*  游戏说明    */
class TipsUI
{
public:
    void CloseUI()
    {
        closegraph();
    }
    void RunUI(BackToMain Button)
    {
        int tag;
        while (1)
        {
            tag = 0;
            FlushMouseMsgBuffer();
            MOUSEMSG M1 = GetMouseMsg();
            if (M1.uMsg == WM_LBUTTONDOWN)
            {
                if (M1.x >= Button.x && M1.x <= Button.x + Button.width && M1.y >= Button.y && M1.y <= Button.y + Button.height)
                {
                    tag = 4;
                    break;
                }
            }
        }
        if (tag == 4)
        {
            CloseUI();
            Main();
        }
    }
    void OpenUI()
    {
        IMAGE BackImage;
        loadimage(&BackImage, _T("IMAGE"), _T("TIPSUI_BACKIMAGE"), 1080, 680);
        initgraph(1080, 680);
        putimage(0, 0, &BackImage);
        /*  输出返回图标  */
        BackToMain Button(860, 50, 200, 30);
        Button.PrintButton();
        RunUI(Button);
    }
};

/*  音乐  */
class Music
{
public:
    IMAGE Button;
    int x, y, height, width;
    Music(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("MUSIC"));
        else loadimage(&Button, _T("IMAGE"), _T("MUSIC"), Inwidth, Inheight);
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

/*  音效  */
class Sound
{
public:
    IMAGE Button;
    int x, y, height, width;
    Sound(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("SOUND"));
        else loadimage(&Button, _T("IMAGE"), _T("SOUND"), Inwidth, Inheight);
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

/*  游戏难度设置  */
class Difficulty
{
public:
    IMAGE Button;
    int x, y, height, width;
    Difficulty(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("DIFFICULTY"));
        else loadimage(&Button, _T("IMAGE"), _T("DIFFICULTY"), Inwidth, Inheight);
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

/*  高中低 */
class HMLChoose
{
public:
    IMAGE High, Middle, Low, Back;
    int x, y, height, width, tag;
    HMLChoose(int Inx, int Iny, int Inwidth, int Inheight)
    {
        tag = 1;
        x = Inx;
        y = Iny;
        if (Inheight == 0 && Inwidth == 0)
        { 
            loadimage(&High, _T("IMAGE"), _T("HIGH"));
            loadimage(&Middle, _T("IMAGE"), _T("MIDDLE"));
            loadimage(&Low, _T("IMAGE"), _T("LOW"));
        }
        else
        {
            loadimage(&High, _T("IMAGE"), _T("HIGH"),Inwidth,Inheight);
            loadimage(&Middle, _T("IMAGE"), _T("MIDDLE"),Inwidth,Inheight);
            loadimage(&Low, _T("IMAGE"), _T("LOW"),Inwidth,Inheight);
        }
        width = High.getwidth();
        height = High.getheight();
        getimage(&Back, x, y, width, height);
    }
    void PrintButton() 
    {
        PrintImg(High,x,y);
    }
    /*  高中低切换   */
    void ChangeButton() 
    {
        putimage(x, y, &Back);
        switch (tag)
        {
        case 1:
            tag = 2;
            PrintImg(Middle, x, y);
            break;
        case 2:
            tag = 3;
            PrintImg(Low, x, y);
            break;
        case 3:
            tag = 1;
            PrintButton();
            break;
        default:
            break;
        }
    }
};

/*  游戏设置界面  */
class SettingsUI
{
public:
    void CloseUI()
    {
        closegraph();
    }
    void RunUI(Music Button1, Sound Button2, Difficulty Button3, BackToMain Button4, HMLChoose HML1, HMLChoose HML2, HMLChoose HML3)
    {
        int tag;
        while (1)
        {
            tag = 0;
            FlushMouseMsgBuffer();
            MOUSEMSG M1 = GetMouseMsg();
            if (M1.uMsg == WM_LBUTTONDOWN)
            {
                if (M1.x >= Button1.x && M1.x <= Button1.x + Button1.width && M1.y >= Button1.y && M1.y <= Button1.y + Button1.height)
                {
                    tag = 1;
                    HML1.ChangeButton();
                }
                if (M1.x >= Button2.x && M1.x <= Button2.x + Button2.width && M1.y >= Button2.y && M1.y <= Button2.y + Button2.height)
                {
                    tag = 2;
                    HML2.ChangeButton();
                }
                if (M1.x >= Button3.x && M1.x <= Button3.x + Button3.width && M1.y >= Button3.y && M1.y <= Button3.y + Button3.height)
                {
                    tag = 3;
                    HML3.ChangeButton();
                }
                if (M1.x >= Button4.x && M1.x <= Button4.x + Button4.width && M1.y >= Button4.y && M1.y <= Button4.y + Button4.height)
                {
                    tag = 4;
                    break;
                }
                if (M1.x >= HML1.x && M1.x <= HML1.x + HML1.width && M1.y >= HML1.y && M1.y <= HML1.y + HML1.height)
                {
                    tag = 1;
                    HML1.ChangeButton();
                }
                if (M1.x >= HML2.x && M1.x <= HML2.x + HML2.width && M1.y >= HML2.y && M1.y <= HML2.y + HML2.height)
                {
                    tag = 2;
                    HML2.ChangeButton();
                }
                if (M1.x >= HML3.x && M1.x <= HML3.x + HML3.width && M1.y >= HML3.y && M1.y <= HML3.y + HML3.height)
                {
                    tag = 3;
                    HML3.ChangeButton();
                }
            }
        }
        if (tag == 4) {
            CloseUI();
            Main();
        }
    }
    void OpenUI() 
    {
        IMAGE BackImage;
        loadimage(&BackImage, _T("IMAGE"), _T("SETTINGSUI_BACKIMAGE"), 1080, 680);
        initgraph(1080, 680);
        putimage(0, 0, &BackImage);
        /*  输出音量图标  */
        Music Button1(300, 170, 0, 0);
        Button1.PrintButton();
        /*  输出音效图标  */
        Sound Button2(300, 280, 0, 0);
        Button2.PrintButton();
        /*  输出难度选择图标    */
        Difficulty Button3(300, 390, 0, 0);
        Button3.PrintButton();
        /*  输出返回图标  */
        BackToMain Button4(460, 120, 0, 0);
        Button4.PrintButton();
        /*  输出高中低   */
        HMLChoose HML1(380, 145, 0, 0);
        HMLChoose HML2(380, 255, 0, 0);
        HMLChoose HML3(580, 365, 0, 0);
        HML1.PrintButton();
        HML2.PrintButton();
        HML3.PrintButton();
        RunUI(Button1, Button2, Button3, Button4, HML1, HML2, HML3);
    }
};

/*  关卡  */
class Level1
{
public:
    IMAGE Button;
    int x, y, height, width;
    Level1(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("LEVEL1"));
        else loadimage(&Button, _T("IMAGE"), _T("LEVEL1"), Inwidth, Inheight);
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

class Level2
{
public:
    IMAGE Button;
    int x, y, height, width;
    Level2(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("LEVEL2"));
        else loadimage(&Button, _T("IMAGE"), _T("LEVEL2"), Inwidth, Inheight);
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

class Level3
{
public:
    IMAGE Button;
    int x, y, height, width;
    Level3(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("LEVEL3"));
        else loadimage(&Button, _T("IMAGE"), _T("LEVEL3"), Inwidth, Inheight);
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

class Level4
{
public:
    IMAGE Button;
    int x, y, height, width;
    Level4(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("LEVEL4"));
        else loadimage(&Button, _T("IMAGE"), _T("LEVEL4"), Inwidth, Inheight);
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

/*  关卡选择界面  */
class LevelChooseUI
{
public:
    void CloseUI()
    {
        closegraph();
    }
    void RunUI(Level1 Button1, Level2 Button2, Level3 Button3, Level4 Button4,BackToMain Button5)
    {
        int tag;
        while (1)
        {
            FlushMouseMsgBuffer();
            MOUSEMSG M1 = GetMouseMsg();
            if (M1.uMsg == WM_LBUTTONDOWN)
            {
                if (M1.x >= Button1.x && M1.x <= Button1.x + Button1.width && M1.y >= Button1.y && M1.y <= Button1.y + Button1.height)
                {
                    tag = 1;
                    break;
                }
                if (M1.x >= Button2.x && M1.x <= Button2.x + Button2.width && M1.y >= Button2.y && M1.y <= Button2.y + Button2.height)
                {
                    tag = 2;
                    break;
                }
                if (M1.x >= Button3.x && M1.x <= Button3.x + Button3.width && M1.y >= Button3.y && M1.y <= Button3.y + Button3.height)
                {
                    tag = 3;
                    break;
                }
                if (M1.x >= Button4.x && M1.x <= Button4.x + Button4.width && M1.y >= Button4.y && M1.y <= Button4.y + Button4.height)
                {
                    tag = 4;
                    break;
                }
                if (M1.x >= Button5.x && M1.x <= Button5.x + Button5.width && M1.y >= Button5.y && M1.y <= Button5.y + Button5.height)
                {
                    tag = 5;
                    break;
                }
            }
        }
        CloseUI();
        switch (tag)
        {
        case 1:
            Game();
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            Main();
            break;
        default:
            break;
        }
    }
    void OpenUI()
    {
        /*  输出背景    */
        IMAGE BackImage;
        initgraph(1080, 680);
        loadimage(&BackImage, _T("IMAGE"), _T("LEVELCHOOSEUI_BACKIMAGE"), 1080, 680);
        putimage(0, 0, &BackImage);
        /*  输出关卡按钮  */
        Level1 Button1(399, 300, 0, 0);
        Button1.PrintButton();
        Level2 Button2(581, 300, 0, 0);
        Button2.PrintButton();
        Level3 Button3(399, 450, 0, 0);
        Button3.PrintButton();
        Level4 Button4(581, 450, 0, 0);
        Button4.PrintButton();
        /*  输出返回按钮  */
        BackToMain Button5(580, 20, 0, 0);
        Button5.PrintButton();
        RunUI(Button1,Button2,Button3,Button4,Button5);
    }
};

/*  难度选择按钮  */
class LevelChooseButton 
{
public:
    IMAGE Button;
    int x, y, height, width;
    LevelChooseButton(int Inx,int Iny,int Inwidth,int Inheight) 
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("LEVELCHOOSEBUTTON"));
        else loadimage(&Button, _T("IMAGE"), _T("LEVELCHOOSEBUTTON"),Inwidth,Inheight);
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

/*  游戏设置按钮  */
class SettingsButton 
{
public:
    IMAGE Button;
    int x, y, height, width;
    SettingsButton(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("SETTINGSBUTTON"));
        else loadimage(&Button, _T("IMAGE"), _T("SETTINGSBUTTON"), Inwidth, Inheight);
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

/*  游戏说明按钮  */
class TipsButton
{
public:
    IMAGE Button;
    int x, y, height, width;
    TipsButton(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("TIPSBUTTON"));
        else loadimage(&Button, _T("IMAGE"), _T("TIPSBUTTON"), Inwidth, Inheight);
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

/*  退出游戏按钮  */
class ExitButton
{
public:
    IMAGE Button;
    int x, y, height, width;
    ExitButton(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("EXITBUTTON"));
        else loadimage(&Button, _T("IMAGE"), _T("EXITBUTTON"), Inwidth, Inheight);
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

/*  主菜单 */
class MainUI 
{
public:
    void RunUI(LevelChooseButton Button1,SettingsButton Button2,TipsButton Button3,ExitButton Button4)
    {
        int tag;
        while (1)
        {
            FlushMouseMsgBuffer();
            MOUSEMSG M1 = GetMouseMsg();
            if (M1.uMsg == WM_LBUTTONDOWN)
            {
                if (M1.x >= Button1.x && M1.x <= Button1.x + Button1.width && M1.y >= Button1.y && M1.y <= Button1.y + Button1.height)
                {
                    tag = 1;
                    break;
                }
                if (M1.x >= Button2.x && M1.x <= Button2.x + Button2.width && M1.y >= Button2.y && M1.y <= Button2.y + Button2.height)
                {
                    tag = 2;
                    break;
                }
                if (M1.x >= Button3.x && M1.x <= Button3.x + Button3.width && M1.y >= Button3.y && M1.y <= Button3.y + Button3.height)
                {
                    tag = 3;
                    break;
                }
                if (M1.x >= Button4.x && M1.x <= Button4.x + Button4.width && M1.y >= Button4.y && M1.y <= Button4.y + Button4.height)
                {
                    tag = 4;
                    break;
                }
            }
        }
        CloseUI();
        switch (tag)
        {
        case 1:
            LevelChoose();
            break;
        case 2:
            Settings();
            break;
        case 3:
            Tips();
            break;
        case 4:
            break;
        default:
            break;
        }
    }
    void OpenUI()
    {
        /*  输出背景    */
        IMAGE BackImage;
        initgraph(1080,680);
        loadimage(&BackImage, _T("IMAGE"), _T("MAINUI_BACKIMAGE"),1080,680);
        putimage(0, 0, &BackImage);
        /*  输出开始游戏按钮    */
        LevelChooseButton Button1(500, 400, 0, 0);
        Button1.PrintButton();
        /*  输出游戏设置按钮    */
        SettingsButton Button2(500, 450, 0, 0);
        Button2.PrintButton();
        /*  输出游戏说明按钮    */
        TipsButton Button3(500, 500, 0, 0);
        Button3.PrintButton();
        /*  输出退出游戏按钮    */
        ExitButton Button4(500, 550, 0, 0);
        Button4.PrintButton();
        /*  开始运行    */
        RunUI(Button1,Button2,Button3,Button4);
    }
    void CloseUI() {
        closegraph();
    }
};

MainUI UI1;
LevelChooseUI UI2;
SettingsUI UI3;
TipsUI UI4;
GameUI UI5;

void Main() 
{
    UI1.OpenUI();
}

void LevelChoose() 
{
    UI2.OpenUI();
}

void Settings() 
{
    UI3.OpenUI();
}

void Tips() 
{
    UI4.OpenUI();
}

void Game()
{
    UI5.OpenUI();
}

int main()
{
    Main();
    return 0;
}
