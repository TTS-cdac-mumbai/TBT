/*******************************************************************************
Authors- Atish && Rachana 
The designed code for Syllable level lab file generation and syldict generation.

*******************************************************************************/


#include<stdio.h>
#include<string.h>
int main(int argc, char *argv[])
{
		char command[1111];
	 	char str1[1111];

		system("touch unit_size.sh");   // create a file unit_size.sh
		
		system("$FESTVOXDIR/src/unitsel/setup_clunits syllable level lab");

		printf("%s", argv[1]);

		sprintf(str1,"cp -r %s%s%s %s","../resources/languages/",argv[1], "/syllable/il_parser_train.pl","bin/");	
		system(str1);
		
		sprintf(str1,"cp -r %s%s%s %s","../resources/languages/",argv[1], "/syllable/PhoneSet.txt","bin/");
		system(str1);

		sprintf(str1,"cp -r %s %s","../input/txt.done.data","etc/");
		system(str1);

		sprintf(str1,"cp %s%s%s %s","../resources/languages/",argv[1], "/syllable/syllable_level_lab_lexicon.scm","festvox/");
		system(str1);

		sprintf(str1,"rm -rf %s%s%s","../resources/languages/",argv[1], "/syllable/syllable_level_lab_phoneset.scm");
		system(str1);

		system("rm -rf festvox/syllable_level_lab_phoneset.scm");
		
		sprintf(str1,"cat %s%s%s %s","../resources/languages/",argv[1], "/syllable/phoneset.scm"," > syllable_level_lab_phoneset.scm");
		system(str1);
	
		sprintf(str1,"cat %s%s%s %s","../resources/languages/",argv[1], "/syllable/phoneset_base.scm"," >> syllable_level_lab_phoneset.scm");
		system(str1);

		system("cp -r syllable_level_lab_phoneset.scm festvox/");
		system("rm -rf syllable_level_lab_phoneset.scm");

		printf("Start building syllable level lab files................>>>>>>>>>\n\n\n");

		strcpy(command,"festival -b festvox/build_clunits.scm '(build_prompts \"etc/txt.done.data\")'");   /* generate prompts*/

		system(command); // generate the syllable level lab file
		
		printf("COMPLETED building syllable level lab files................>>>>>>>>>\n\n\n");

		system("cat prompt-lab/* | cut -d \" \" -f3 > all_sys_unsorted");

		system("sort -u all_sys_unsorted > all_sys");		

		sprintf(str1,"cp %s %s","../resources/common/syllable/generate_syl_dict.pl", "./");
		system(str1);
		
		system("perl generate_syl_dict.pl $PWD" );  // generate the SYLDICT (syldict)

		return 0;
}

