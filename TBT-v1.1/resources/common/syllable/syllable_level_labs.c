/*
###########################################################################
##                                                                       ##
##                Centre for Development of Advanced Computing           ##
##                               Mumbai                                  ##
##                         Copyright (c) 2017                            ##
##                        All Rights Reserved.                           ##
##                                                                       ##
##  Permission is hereby granted, free of charge, to use and distribute  ##
##  this software and its documentation without restriction, including   ##
##  without limitation the rights to use, copy, modify, merge, publish,  ##
##  distribute, sublicense, and/or sell copies of this work, and to      ##
##  permit persons to whom this work is furnished to do so, subject to   ##
##  the following conditions:                                            ##
##   1. The code must retain the above copyright notice, this list of    ##
##      conditions and the following disclaimer.                         ##
##   2. Any modifications must be clearly marked as such.                ##
##   3. Original authors' names are not deleted.                         ##
##   4. The authors' names are not used to endorse or promote products   ##
##      derived from this software without specific prior written        ##
##      permission.                                                      ##
##                                                                       ##
##                                                                       ##
###########################################################################
##                                                                       ##
##                     TTS Building Toolkit                              ##
##                                                                       ##
##            Designed and Developed by Atish and Rachana                ##
##          		Date:  April 2017                                ##
##                                                                       ## 
###########################################################################
*/

#include<stdio.h>
#include<string.h>
int main(int argc, char *argv[])
{
		char command[1111];
	 	char str1[1111];

		system("touch unit_size.sh");   // create a file unit_size.sh
		
		system("$FESTVOXDIR/src/unitsel/setup_clunits syllable level lab");

		printf("%s", argv[1]);

		//sprintf(str1,"cp -r %s%s%s %s","../resources/languages/",argv[1], "/syllable/il_parser_train.pl","bin/");	
		//system(str1);
		
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
		
		sprintf(str1,"cp -r %s %s","../resources/common/parser/*", "./");
		system(str1);    //copy parser to current directory
		system("make");

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

