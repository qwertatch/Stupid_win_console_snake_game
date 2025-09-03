#include "class/snake.h"

using namespace std;

int main()
{
    /**
    initCon();
    /**cout << "Please don't change resolution while playing, this thing will break, thank you. Press enter to start the game.";
    cin.ignore(1,'\n');/
    clearScreen();
    init();
    loadMap();

    COORD head = { 14,14 };
    COORD tail = { 14,14 };

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;
    **/
    cout << "Instruction:\nClick on screen to pause (not a mechanic just a technicality thingy idk)\nWASD to move\n";
    int dif = 1;
    cout << "Please enter prefered difficulty (1 to 100, the higher the harder): ";
    cin >> dif;
    cin.ignore(INT_MAX,'\n');
    if(dif<1)
        dif = 1;
    if(dif>100)
        dif = 10;

    const int wait_time = 1000 / dif;

    Snake gam;
    while(gam.IsRunning()){
        Sleep(wait_time);
        gam.Run();
    }
    cin.ignore(INT_MAX,'\n');
    return 0;
}
