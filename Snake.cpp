#include <graphics.h>      
#include <conio.h>
#include <tchar.h>
#include <iostream>
#include <cstdio>

using namespace std;

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

class LevelChooseButton 
{
private:
    IMAGE Button;
    int x, y, height, width;
public:
    LevelChooseButton(int Inx,int Iny,int Inwidth,int Inheight) 
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("LEVELCHOOSEBUTTON"));
        else loadimage(&Button, _T("IMAGE"), _T("LEVELCHOOSEBUTTON",Inwidth,Inheight));
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

class SettingsButton 
{
private:
    IMAGE Button;
    int x, y, height, width;
public:
    SettingsButton(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("SETTINGSBUTTON"));
        else loadimage(&Button, _T("IMAGE"), _T("SETTINGSBUTTON", Inwidth, Inheight));
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

class TipsButton
{
private:
    IMAGE Button;
    int x, y, height, width;
public:
    TipsButton(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("TIPSBUTTON"));
        else loadimage(&Button, _T("IMAGE"), _T("TIPSBUTTON", Inwidth, Inheight));
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

class ExitButton
{
private:
    IMAGE Button;
    int x, y, height, width;
public:
    ExitButton(int Inx, int Iny, int Inwidth, int Inheight)
    {
        if (Inheight == 0 && Inwidth == 0) loadimage(&Button, _T("IMAGE"), _T("EXITBUTTON"));
        else loadimage(&Button, _T("IMAGE"), _T("EXITBUTTON", Inwidth, Inheight));
        width = Button.getwidth();
        height = Button.getheight();
        x = Inx;
        y = Iny;
    }
    void PrintButton() {
        PrintImg(Button, x, y);
    }
};

class MainUI 
{
public:
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
    }
};

int main()
{
    MainUI UI1;
    UI1.OpenUI();
    int ToolNum=_getch();
    closegraph();
    return 0;
}