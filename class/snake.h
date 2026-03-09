#ifndef SNAKE_H_INCLUDED
#define SNAKE_H_INCLUDED

#include <iostream>
#include <unordered_set>
#include <iterator>
#include <stdlib.h>
#include <time.h>
#include <cstdio>
#include <conio.h>
#include <vector>
#include <conio.h>
#include <string>

#define NOMINMAX
#define _WIN32_WINNT 0x0500
#include <windows.h>

using namespace std;

class Snake{
    struct cellFill{
        int type;
        int len;
    };

    HANDLE handle;
    CONSOLE_SCREEN_BUFFER_INFO screen;

    vector< string > shape;
    unordered_set< int > apple_loc;
    vector< vector< int > > curMap;
    vector< vector<cellFill> > Dmap;
    vector< WORD > cellType;

    int dir;
    int snake_len;
    COORD tail,head;
    bool running,eaten;

    void initCon(){
        ///-------------------------------------Set console size & fixed it----------------------------
        HWND console = GetConsoleWindow();
        RECT r;
        GetWindowRect(console,&r);
        //MoveWindow(window_handle, x, y, width, height, redraw_window);
        MoveWindow(console, 500, 150, 465, 600, TRUE);
        SetWindowLong(console, GWL_STYLE, GetWindowLong(console, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);

        ///--------------------------------------Initiate handle values--------------------------------
        handle = GetStdHandle( STD_OUTPUT_HANDLE );
        GetConsoleScreenBufferInfo(handle, &screen);

        CONSOLE_CURSOR_INFO cur_info;
        GetConsoleCursorInfo(handle,&cur_info);
        cur_info.bVisible = false;
        SetConsoleCursorInfo(handle,&cur_info);
        return;
    }

    void initGame(){
        Dmap.clear();
        Dmap.push_back( vector<cellFill>{ {5,27} } );
        for(int i = 0;i<25;i++){
            Dmap.push_back( vector<cellFill>{ {5,1}, {0,25}, {5,1} } );
        }
        Dmap.push_back( vector<cellFill>{ {5,27} } );

        cellType.clear();
        cellType.resize(7);
        cellType[0] = 0;                                                                            //blank
        cellType[1] = BACKGROUND_GREEN | BACKGROUND_INTENSITY;                                      //snake
        cellType[2] = BACKGROUND_GREEN | BACKGROUND_INTENSITY;                                      //snake
        cellType[3] = BACKGROUND_GREEN | BACKGROUND_INTENSITY;                                      //snake
        cellType[4] = BACKGROUND_GREEN | BACKGROUND_INTENSITY;                                      //snake
        cellType[5] = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY | BACKGROUND_RED;   //Wall
        cellType[6] = BACKGROUND_RED;                                                               //Apple

        shape.clear();
        shape.resize(13,"ab");
        if(true)
        {
            //up-right
            shape[0b0011][0] = char(212);
            shape[0b0011][1] = char(207);
            //up-down
            shape[0b0101][0] = '|';
            shape[0b0101][1] = '|';
            //up-left
            shape[0b1001][0] = char(207);
            shape[0b1001][1] = char(190);
            //right-down
            shape[0b0110][0] = char(213);
            shape[0b0110][1] = char(209);
            //right-left
            shape[0b1010][0] = '=';
            shape[0b1010][1] = '=';
            //down-left
            shape[0b1100][0] = char(209);
            shape[0b1100][1] = char(184);
        }
        return;
    }

    void drawApple(){
        if(apple_loc.empty()){
            running = false;
            return;
        }
        unordered_set< int >::iterator itr = apple_loc.begin();
        advance(itr,rand() % apple_loc.size());
        int pos = *itr;
        COORD apple = { pos%27 , pos/27 };
        curMap[apple.X][apple.Y] = 6;
        DWORD written;

        apple.X *= 2;
        FillConsoleOutputAttribute(
            handle, cellType[6], 2, apple, &written
        );
        return;
    }

    void loadMap(){
        srand( time(NULL) );

        COORD pos = {0,0};
        DWORD written;

        SetConsoleCursorPosition(handle,pos);

        for(const auto &layer : Dmap){
            for(const auto &bloc : layer){
                FillConsoleOutputAttribute(
                    handle, cellType[ bloc.type ],
                    bloc.len * 2, pos, &written);
                pos.X += bloc.len * 2;
            }
            pos.X = 0;
            pos.Y++;
        }

        curMap.clear();
        curMap.resize(27, vector<int>(27));

        for(int i = 0,j;i<27;i++){
            j = 0;
            for(const auto &bloc : Dmap[i])
                for(int x = bloc.len;x>0;x--){
                    curMap[j][i] = bloc.type;
                    j++;
                }
        }

        curMap[13][13] = 1;
        curMap[13][14] = 1;

        head.X = 13;
        head.Y = 13;

        tail.X = 13;
        tail.Y = 14;

        apple_loc.clear();
        for(int x = 0;x<27;x++){
            for(int y = 0;y<27;y++){
                if(curMap[x][y]==0){
                    apple_loc.insert( x + y*27 );
                }
            }
        }
        drawApple();
        SetConsoleCursorPosition(handle,pos);
        return;
    }

    void clearScreen() {
        COORD topLeft  = { 0, 0 };
        DWORD written;

        FillConsoleOutputCharacter(
            handle, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
        );
        SetConsoleCursorPosition(handle, topLeft);
    }

    void drawTail(){
        if(eaten){
            eaten = false;
            return;
        }
        COORD tPos = tail;
        tPos.X *= 2;
        DWORD written;
        FillConsoleOutputAttribute(
            handle, cellType[0], 2, tPos, &written
        );
        FillConsoleOutputCharacter(
            handle, ' ', 2, tPos, &written
        );

        apple_loc.insert( tail.X + tail.Y * 27 );

        tPos = tail;
        switch( curMap[ tail.X ][ tail.Y ] ){
        case 1:
            tail.Y--;
            break;
        case 2:
            tail.X++;
            break;
        case 3:
            tail.Y++;
            break;
        case 4:
            tail.X--;
            break;
        default:
            running = false;
            cout << "Error: tail is at non-snake cell... somehow";
        }

        curMap[tPos.X][tPos.Y] = 0;
        tPos = tail;
        tPos.X *= 2;

        SetConsoleCursorPosition(handle,tPos);
        switch( curMap[ tail.X ][ tail.Y ] ){
        case 1:
            cout << char(192) << char(217);
            FillConsoleOutputAttribute(
                handle, cellType[1], 2, tPos, &written
            );
            break;
        case 3:
            cout << char(218) << char(191);
            FillConsoleOutputAttribute(
                handle, cellType[1], 2, tPos, &written
            );
            break;
        case 2:
            cout << " =";
            FillConsoleOutputAttribute(
                handle, cellType[1], 2, tPos, &written
            );
            break;
        case 4:
            cout << "= ";
            FillConsoleOutputAttribute(
                handle, cellType[1], 2, tPos, &written
            );
            break;
        default:
            running = false;
            cout << "Error: tail is at non-snake cell after movement";
        }
        return;
    }

    int getinp(){
        char c = ' ',tc;
        while(_kbhit()){
            tc = _getch();
            if(tc=='w' || tc=='s' || tc=='a' || tc=='d')
                c = tc;
        }
        switch(c){
        case 'w':
            return 1;
        case 'd':
            return 2;
        case 's':
            return 3;
        case 'a':
            return 4;
        default:
            return dir;
        }
        return dir;
    }

    void drawHead(){
        int next_dir = getinp();
        int curv = 0;
        curv |= 1 << ((dir + 1)%4);
        if(abs(next_dir - dir)==2)
            next_dir = dir;
        curv |= 1 << (next_dir - 1);

        DWORD written;
        COORD hPos = head;
        hPos.X *= 2;

        SetConsoleCursorPosition(handle,hPos);
        cout << shape[curv];
        FillConsoleOutputAttribute(
            handle, cellType[1], 2, hPos, &written
        );

        curMap[ head.X ][ head.Y ] = dir = next_dir;
        switch( next_dir ){
        case 1:
            head.Y--;
            break;
        case 2:
            head.X++;
            break;
        case 3:
            head.Y++;
            break;
        case 4:
            head.X--;
            break;
        default:
            running = false;
            cout << "Error: new direction is not valid... Why";
        }

        if(curMap[ head.X ][ head.Y ]){
            if(curMap[ head.X ][ head.Y ]==6){
                eaten = true;
                snake_len++;
                if(apple_loc.find( head.X + head.Y * 27 )==apple_loc.end()){
                    running = false;
                    cout << "Error: not an available apple position despite there is an apple there... how?";
                    return;
                }
                apple_loc.erase( apple_loc.find( head.X + head.Y * 27 ) );
                drawApple();
            }else{
                running = false;
                return;
            }
        }else{
            if(apple_loc.find( head.X + head.Y * 27 )==apple_loc.end()){
                running = false;
                cout << "Error: not an available apple position despite empty space... how?";
                return;
            }
            apple_loc.erase( apple_loc.find( head.X + head.Y * 27 ) );
        }
        curMap[ head.X ][ head.Y ] = dir;

        hPos = head;
        hPos.X *= 2;

        SetConsoleCursorPosition(handle,hPos);
        cout << "  ";
        FillConsoleOutputAttribute(
            handle, cellType[1], 2, hPos, &written
        );

        return;
    };
public:
    Snake(): running(true) , dir(1) , snake_len(2), eaten(false) {
        initCon();
        clearScreen();
        initGame();

        loadMap();
        return;
    }

    bool IsRunning(){
        return running;
    }

    void Run(){
        drawTail();
        drawHead();

        SetConsoleCursorPosition(handle,{0,30});
        printf("Score: %d",snake_len);

        return;
    }
};

#endif // SNAKE_H_INCLUDED
