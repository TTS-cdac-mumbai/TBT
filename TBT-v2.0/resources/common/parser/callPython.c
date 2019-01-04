#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>

int main()
{
char a[900]="ioo";
char * ret=system("python /home/cdac/tts_work/Unified-parser-23rdAugujst2018/Atish-Edited-Working-Parser/lan.py a");
      printf("the return ===%s",ret);

return 0;
}
