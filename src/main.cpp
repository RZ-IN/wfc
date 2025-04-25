#include <cstdio>
#include <cstdlib>
#include <thread>
#include "wfc.h"



int main()
{
    system("cls");
    std::thread t1([] {
        WaveFunctionCollapse wfc(32, 64);
        wfc.init();
        wfc.print();
        if (!wfc.generate()) {
            puts("生成失败！");
        }
    });
    t1.join();
    system("pause");
}
