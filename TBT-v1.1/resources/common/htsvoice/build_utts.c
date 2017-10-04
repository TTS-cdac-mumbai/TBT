#include<stdio.h>

int main()
{
	printf("=============Building Utterance files===============\n\n");
	
	system("festival -b festvox/build_clunits.scm \'(build_utts \"etc/txt.done.data\")\'");

	printf("================DONE WITH UTT FILE BUILDING======================\n\n");

return 0;
}
