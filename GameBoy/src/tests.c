#include <commons.h>
#include <cpu.h>
#include <devices.h>
#include <bus.h>
#include <assert.h>

extern cpu_context cpu_ctx;

FILE* testLogFile;

int main()
{
    testLogFile = fopen("testLog.txt", "w");
    init_Devices("rom/tests/06-ld r,r.gb");
    cpu_init();
    cpuRun();
    fclose(testLogFile);
    printf("TESTS PASSED");
    return 0;
}