#include <emu.h>

extern cpu_context cpu_ctx;

FILE* testLogFile;

/* TESTS BLARGG:
01  ✅✅
02  
03  ✅✅
04  ✅✅
05  ✅✅
06  ✅✅
07  ✅✅
08  ✅✅
09  ✅✅
10  ✅✅
11  ✅✅
*/

int main(int argc, char* argv[])
{
    testLogFile = fopen("testLog.txt", "w");
    
    init_EMU(argv[1]);
    run_EMU();

    fclose(testLogFile);
    printf("DIVINE INTELLECT");
    
    return 0;
}