/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 62 "unified.y" /* yacc.c:339  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include "utf8.c"
#include "utf8.h"
//#include <fstream.h>

enum LANG {
    MALAYALAM=1,TAMIL,TELUGU,KANNADA,HINDI,BENGALI,GUJARATHI,ODIYA,ENGLISH
}currLang;

#define ROW 128 //utf8 lang size max
#define COL 2
char *symbolTable[ROW][COL];
char *syllableList[100]={}; //max syllable limit.
int DEBUG=0, parseLevel=0, pruiningFlag=0, syllTagFlag=0, fliteHTSFlag=0, doubleParseSyl=0;//parselevel - 0-mono 1-syll  //syldictFlag=0,
//wordpronunciation   wordpronunciationunified   commonunified
char *commonFile = "common", *outputFile="wordpronunciation", *outputPruneFile="rag_pho";
//char phoneDelimiter ='*',syllableDelimiter = '#';
int writeFormat=0, LangSpecificCorrectionFlag=1; //0 - phone; 1 - syllable

char *wordCopy, *syllabifiedWord, *phonifiedWord, *unicodeWord, *syllabifiedWordOut, *outputText;
int langId=0,isSouth=0,syllableCount=0;

/* all phone list */
enum listSize{
 VOWELSSIZE=18 ,CONSONANTSSIZE=25, SEMIVOWELSSIZE=13
}lists;

char * VOWELS[VOWELSSIZE] = {"a","e","i","o","u","aa","mq","aa","ii", "uu","rq",
                              "au","ee","ei","ou","oo","ax","ai"};
char * CONSONANTS[CONSONANTSSIZE]={"k","kh","g","gh","ng","c","ch","j","jh","nj","tx",
                                    "txh","dx","dxh","nx","t","th","d","dh","n","p",
                                    "ph","b","bh","m"};
char * SEMIVOWELS[SEMIVOWELSSIZE] ={"y","r","l","w","sh","sx","zh","s","h","lx","rx",
                                    "f","dxq"};
char * VOWELMODIFERS[] = {};

/* all phone list end */

void yyerror(const char * str){
   fprintf(stderr,"Error: %s\n",str);
}
int yywrap(){
	return 1;
}

//Check if symbol is in symbolTable
int CheckSymbol(char * input){
	register int i=0;
	for(;i<ROW;i++)
	{
		if(strcmp(symbolTable[i][1],input)==NULL)return 1;
	}
	return 0;
}

// //Replace substr in string
// char * ReplaceString(const char *s, char ch, const char *repl) {
//     int count = 0;
//     const char *t;
//     for(t=s; *t; t++)
//         count += (*t == ch);
//     size_t rlen = strlen(repl);
//     char *res = malloc(strlen(s) + (rlen-1)*count + 1);
//     char *ptr = res;
//     for(t=s; *t; t++) {
//         if(*t == ch) {
//             memcpy(ptr, repl, rlen);
//             ptr += rlen;
//         } else {
//             *ptr++ = *t;
//         }
//     }
//     *ptr = 0;
//     return res;
// }

//Remove unwanted chars in UTF8 word initially -- make lang correction if necessary
char* RemoveUnwanted(char * input){
   char * output,output1;
   output=input;
   register int i=0;
   char c2[100];

   char * removeList[18]={"!",";",":","@","#","$","%","^","&","*",",",".","/","'","’","”","“","।"};
   for(;i<18;i++){
   	strcpy(c2,removeList[i]);
   	if(strstr(output, c2) != NULL)output =Utf8ReplaceAll(c2,"",output);
   }

   //invisible char correction -not working
	if(strstr(output, "﻿") != NULL)output =Utf8ReplaceAll("﻿","",output);

   //hindi correction
	if(strstr(output, "ऩ") != NULL)output =Utf8ReplaceAll("ऩ","ऩ",output);
   if(strstr(output, "ऱ") != NULL)output =Utf8ReplaceAll("ऱ","ऱ",output);
   if(strstr(output, "क़") != NULL)output =Utf8ReplaceAll("क़","क़",output);
   if(strstr(output, "ख़") != NULL)output =Utf8ReplaceAll("ख़","ख़",output);
   if(strstr(output, "ग़") != NULL)output =Utf8ReplaceAll("ग़","ग़",output);
   if(strstr(output, "ज़") != NULL)output =Utf8ReplaceAll("ज़","ज़",output);
   if(strstr(output, "ड़") != NULL)output =Utf8ReplaceAll("ड़","ड़",output);
   if(strstr(output, "ढ़") != NULL)output =Utf8ReplaceAll("ढ़","ढ़",output);
   if(strstr(output, "फ़") != NULL)output =Utf8ReplaceAll("फ़","फ़",output);
   if(strstr(output, "य़") != NULL)output =Utf8ReplaceAll("य़","य़",output);
   if(strstr(output, "ऴ") != NULL)output =Utf8ReplaceAll("ऴ","ऴ",output);

   //tamil correction
   if(strstr(output, "ொ") != NULL)output =Utf8ReplaceAll("ொ","ொ",output);
   if(strstr(output, "ோ") != NULL)output =Utf8ReplaceAll("ோ","ோ",output);

   //malayalam correction
   if(strstr(output, "ൊ") != NULL)output =Utf8ReplaceAll("ൊ","ൊ",output);
   if(strstr(output, "ോ") != NULL)output =Utf8ReplaceAll("ോ","ോ",output);
   if(strstr(output, "ല്‍‌") != NULL)output =Utf8ReplaceAll("ല്‍‌","ൽ",output);
   if(strstr(output, "ള്‍") != NULL)output =Utf8ReplaceAll("ള്‍","ൾ",output);
   if(strstr(output, "ര്‍") != NULL)output =Utf8ReplaceAll("ര്‍","ർ",output);
   if(strstr(output, "ന്‍") != NULL)output =Utf8ReplaceAll("ന്‍","ൻ",output);
   if(strstr(output, "ണ്‍") != NULL)output =Utf8ReplaceAll("ണ്‍","ൺ",output);

   return	output;
}

//set the language id of the word
void SetlangId(int id){
   if(id>=3328 && id<=3455)currLang = MALAYALAM; //malayalam
   else if(id>=2944 && id<=3055)currLang =TAMIL; //tamil
   else if(id>=3202 && id<=3311)currLang =KANNADA; //KANNADA
   else if(id>=3072 && id<=3198)currLang =TELUGU; //telugu
   else if(id>=2304 && id<=2431)currLang =HINDI; //hindi
   else if(id>=2432 && id<=2559)currLang =BENGALI; //BENGALI
   else if(id>=2688 && id<=2815)currLang =GUJARATHI; //gujarathi
   else if(id>=2816 && id<=2943)currLang =ODIYA; //odia
   else if(id>=64 && id<=123)currLang = ENGLISH; //english
   langId = currLang;
   if(langId < 5)isSouth=1;
   if(langId == 0){
   	printf("UNKNOWN language  with id %d:(\n",id);
   	exit(0);
   }
   return 1;
}

//set language features
int SetlanguageFeat(char* input){
   FILE *inFile;
   inFile = fopen(commonFile, "r");
   if (!inFile) {
      printf("Couldn't open common file for reading\n");
      return 0;
   }

   char line[200];
   char * line1;
   int i = -1;
   setlocale(LC_ALL, "");
   wchar_t * str1;
   char * firstLet;
   	str1 = (wchar_t*)(malloc(1000*sizeof(wchar_t)*strlen(input)));//giving more space -optimise
   	mbstowcs(str1,input,strlen(input)+1);
   int j;
   int len = wcslen(str1);
   if(len==0)len=1;

   //printf(" string : %ls\n", str1);
   for(j=0;j<len;j++)
   {
      if(str1[j]<8204)
      { // for invisible char skipping
         firstLet = str1[j];
         break;
      }
   }
	 //firstLet = str1[0];
	 //printf(" %s  len : %d\n",input,len);

   SetlangId((int)firstLet);

	wchar_t *buf=0;
	line[200];
	i = -1;
	int ii=0;
	while(fgets(line, 200, inFile))
	{
      // unicode  english
      if(i>=127)break; //end of commonfile
            i++;
      buf = strtok(line, "\t\n");
      //loop to select the language
      buf = strtok(NULL, "\t\n");
      if(buf) symbolTable[i][1] = strdup(buf);
      for(ii=0;ii<langId;ii++)
      {
         buf = strtok(NULL, "\t\n");
      }
      if(buf)symbolTable[i][0] =strdup(buf);
	}
	fclose(inFile);
   return 1;
}

//Appened two strings
char * AppendString(char * str1,char * str2 ){
   char * new_str ;
   if((new_str = malloc(strlen(str1)+strlen(str2)+1)) != NULL)
   {
      new_str[0] = '\0';   // ensures the memory is an empty string
      strcat(new_str,str1);
      strcat(new_str,str2);
   }
   else
   {
      printf("malloc failed!\n");
      // exit?
   }
   return new_str;
}

//concatenate two strings
char * ConcatenateString(char *str1, char *str2){
   register int i=0;
   char * str3 = malloc(1+(strlen(str1)+strlen(str2))) ;
   while(*str1)
   {
      str3[i++]=*str1++;
   }
   while(*str2)
   {
      str3[i++]=*str2++;
   }
   str3[i]='\0';
   return str3;
}

//UTF-8 to CPS mapping
char* ConvertToSymbols(char * input){
   char* output;
   setlocale(LC_ALL, "");
   wchar_t * str1;
	str1 = (wchar_t*)(malloc(3*sizeof(wchar_t)*strlen(input)));
	unicodeWord = (wchar_t*)(malloc(3*sizeof(wchar_t)*strlen(input)));
	syllabifiedWord = (wchar_t*)(malloc(100*sizeof(wchar_t)*strlen(input)));
   mbstowcs(str1,input,strlen(input)+1);
   register int j;
   int len = wcslen(str1);
   //
   // for(j=0;j<len;j++)
   // {
   //    printf("%d\n",(int)str1[j]);
   //    //if((int)str1[j] == 8205)str1[j] ='#';
   // }
   // printf("1 %ls\n",str1 );
   // if(strstr(str1, "#") != NULL)str1 = Utf8ReplaceAll("#","",str1);
   // printf("2 %ls\n",str1 );


   char uni[100];
   strcpy(syllabifiedWord,"&");

   for(j=0;j<len;j++){
      if((int)str1[j] < 8204 ){ //invisible char correction && ((int)str1[j] != 8204)
         sprintf(uni, "%d", (int)str1[j]%128);
         //unicodeWord = ConcatenateString(ConcatenateString(unicodeWord,"&"),uni);
         syllabifiedWord = ConcatenateString(ConcatenateString(syllabifiedWord,"&"),symbolTable[(int)str1[j]%128][1]);//getsymbol

      }
   }
   unicodeWord++;
   syllabifiedWord++;
   output = syllabifiedWord;
   return output;
}

//get the dict file name
char* GetFile(int langId,int type){//1-dict; 2-lang rules
   char * fileName;
   fileName= malloc(100*sizeof(char));
   if(type == 1)strcpy(fileName,"dict/");
   else if(type == 2)strcpy(fileName,"rules/");

   if(langId == MALAYALAM)strcat(fileName, "malayalam");
   else if(langId == TAMIL)strcat(fileName, "tamil");
   else if(langId == KANNADA)strcat(fileName, "kannada");
   else if(langId == TELUGU)strcat(fileName, "telugu");
   else if(langId == HINDI)strcat(fileName, "hindi");
   else if(langId == BENGALI)strcat(fileName, "bengali");
   else if(langId == GUJARATHI)strcat(fileName, "gujarathi");
   else if(langId == ODIYA)strcat(fileName, "odiya");
   else if(langId == ENGLISH)strcat(fileName, "english");

   if(type == 1)strcat(fileName,".dict");
   else if(type == 2)strcat(fileName,".rules");

   return fileName;
}

//check the word in Dict
int CheckDictionary(char * input){
   FILE *output;
   char buffer[2000];
   wchar_t *buf=0;
   char * fileName=strdup(GetFile(langId,1));
   if(DEBUG)printf("dict : %s\n",fileName );
   output = fopen(fileName,"r");
   if(!output){
      if(DEBUG)printf("Dict not found\n");
      if(langId == ENGLISH) exit(1);
      return 0;
   }
   while (fgets(buffer,2000,output)!=NULL){
      buf = strtok(buffer, "\t\n");
      if(DEBUG)printf("word : %s\n",buf);
      if(strcmp(input,buf)==NULL){
         if(DEBUG)printf("match found\n" );
         buf = strtok(NULL, "\t\n");
         if(writeFormat == 1)WriteFile(buf);
         if(DEBUG)printf("Syllables : %s\n",buf);
         buf = strtok(NULL, "\t\n");
         if(writeFormat == 0)WriteFile(buf);
         if(DEBUG)printf("monophones : %s\n",buf);
         return 1;
      }
   }
   fclose(output);
   // enum LANG corrLang;
   // corrLang = ENGLISH;
   if(langId == ENGLISH) exit(1);
   return 0;
}

//Syllabilfy the words
char* Syllabilfy(char * phone){
	if(strstr(phone, "&&") != NULL)phone =Utf8ReplaceAll("&&","&",phone);
	if(strstr(phone, "&eu&") != NULL)phone =Utf8ReplaceAll("&eu&","@",phone);
	if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@","&eu&#&",phone);
   if(strstr(phone, "&euv&") != NULL)phone =Utf8ReplaceAll("&euv&","@",phone);
	if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@","&euv&#&",phone);
	if(strstr(phone, "&avq") != NULL)phone =Utf8ReplaceAll("&avq","&q&av",phone);
	if(strstr(phone, "&av&") != NULL)phone =Utf8ReplaceAll("&av&","@",phone);
	if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@","&av&#&",phone);
	if(strstr(phone, "&q") != NULL)phone =Utf8ReplaceAll("&q","@",phone);
	if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@","&q&#",phone);

	register int i=0;
	char c1[100],c2[100];
	char * removeList[45]={"&nwv&","&nnv&","&rwv&","&lwv&","&lnv&","&aav&","&iiv&","&uuv&","&rqv&","&eev&",
                           "&eiv&","&ouv&","&axv&","&oov&","&aiv&","&auv&","&aev&",
                           "&nnx&","&nxx&","&rrx&","&llx&","&lxx&",
									"&aa&","&iv&","&ov&","&mq&","&aa&","&ii&","&uu&","&rq&","&au&","&ee&",
                           "&ei&","&ou&","&oo&","&ax&","&ai&","&ev&","&uv&","&ae&",
									"&a&","&e&","&i&","&o&","&u&"};
	for(;i<45;i++){
      strcpy(c1,removeList[i]);
      strcpy(c2,c1);
      strcat(c2,"#&");
      if(strstr(phone, c1) != NULL)phone =Utf8ReplaceAll(c1,"@",phone);
      if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@",c2,phone);
	}

  if(strstr(phone, "&#&hq&") != NULL)phone =Utf8ReplaceAll("&#&hq&","&hq&#&",phone);

  //for vowel in between correction

  char * pureVowelList[5] ={"&a&","&e&","&i&","&o&","&u&"};
  for(i=0;i<5;i++){
    strcpy(c1,"&#");
    strcat(c1,pureVowelList[i]);
    if(strstr(phone, pureVowelList[i]) != NULL)phone =Utf8ReplaceAll(pureVowelList[i],"@",phone);
    if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@",c1,phone);
  }

  char * consonantList[39]={"k","kh","g","gh","ng","c","ch","j","jh","nj","tx","txh","dx","dxh",
                                                   "nx","t","th","d","dh","n","p","ph","b","bh","m","y",
                                                   "r","l","w","sh","sx","zh","y","s","h","lx","rx","f","dxq"};

  // &eu&#&r&eu&#& syllabification correction

  for(i=0;i<39;i++){
    strcpy(c1,"&eu&#&");
    strcat(c1,consonantList[i]);
    strcat(c1,"&euv&#&");
    strcpy(c2,"&eu&");
    strcat(c2,consonantList[i]);
    strcat(c2,"&av&#&");
    if(strstr(phone, c1) != NULL)phone = Utf8ReplaceAll(c1,"@",phone);
    if(strstr(phone, "@") != NULL)phone = Utf8ReplaceAll("@",c2,phone);
  }

  for(i=0;i<39;i++){
    strcpy(c1,"&euv&#&");
    strcat(c1,consonantList[i]);
    strcat(c1,"&euv&#&");
    strcpy(c2,"&euv&");
    strcat(c2,consonantList[i]);
    strcat(c2,"&av&#&");
    if(strstr(phone, c1) != NULL)phone = Utf8ReplaceAll(c1,"@",phone);
    if(strstr(phone, "@") != NULL)phone = Utf8ReplaceAll("@",c2,phone);
  }
  if(strstr(phone, "&eu&") != NULL)phone =Utf8ReplaceAll("&eu&","@",phone);
  if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@","&eu&#&",phone);

  return phone;
}

//cant use this as break syllable rules
char* DoubleModifierCorrection(char * phone){
   //&iv&#&aav&  DoubleModifierCorrection
	register int i=0,j=0;
	char c1[100],c2[100];
	char * doubleModifierList[21]={"&nwv&","&nnv&","&rwv&","&lwv&","&lnv&","&aav&","&iiv&","&uuv&","&rqv&","&eev&",
                           "&eiv&","&ouv&","&axv&","&oov&","&aiv&","&auv&","&aev&",
									"&iv&","&ov&","&ev&","&uv&"};
	for(i=0;i<21;i++){
      for(j=0;j<21;j++){
         strcpy(c1,doubleModifierList[i]);
      	strcpy(c2,doubleModifierList[i]);
         strcat(c1,"#");
         strcat(c1,doubleModifierList[j]);
      	strcat(c2,doubleModifierList[j]);
      	strcat(c2,"#&");
         //if(DEBUG)printf("%s %s\n",c1,c2 );
      	if(strstr(phone, c1) != NULL)phone =Utf8ReplaceAll(c1,"@",phone);
      	if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@",c2,phone);
      }
	}

   if(strstr(phone, "&#&hq&") != NULL)phone =Utf8ReplaceAll("&#&hq&","&hq&#&",phone);
   if(strstr(phone, "&&") != NULL)phone =Utf8ReplaceAll("&&","&",phone);
   return phone;
}

//for eu&C&C&V
char* SchwaDoubleConsonent(char * phone){
   register int i=0,j=0,k=0;
   char c1[100],c2[100];
   char * consonentList[39]={"k","kh","lx","rx","g","gh","ng","c","ch","j","jh","nj","tx","txh","dx","dxh","nx","t","th","d","dh","n","p","ph","b","bh","m","y","r","l","w","sh","sx","zh","y","s","h","f","dxq"};
   char * vowelList[42]={"av&","nwv&","nnv&","rwv&","lwv&","lnv&","aav&","iiv&","uuv&","rqv&","eev&","eiv&","ouv&",
                                                    "axv&","oov&","aiv&","nnx&","nxx&","rrx&","llx&","lxx&",
                                                    "aa&","iv&","ov&","mq&","aa&","ii&","uu&","rq&",
                                                    "ee&","ei&","ou&","oo&","ax&","ai&","ev&","uv&",
                                                    "a&","e&","i&","o&","u&"};
   for(i=0;i<39;i++){
      for(j=0;j<39;j++){
         for(k=0;k<42;k++){
            strcpy(c1,"&euv&");
            strcat(c1,consonentList[i]);
            strcpy(c2,c1);
            strcat(c1,"&");
            strcat(c2,"&av&");
            strcat(c1,consonentList[j]);
            strcat(c2,consonentList[j]);
            strcat(c2,"&");
            strcat(c1,"&");
            strcat(c1,vowelList[k]);
            strcat(c2,vowelList[k]);

            //if(DEBUG)printf("%s\t%s\n",c1,c2);
            if(strstr(phone, c1) != NULL)phone =Utf8ReplaceAll(c1,"@",phone);
            if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@",c2,phone);
         }
      }
   }
  //if(strstr(phone, "#") != NULL)phone =Utf8ReplaceAll("#","",phone);
  if(strstr(phone, "$") != NULL)phone =Utf8ReplaceAll("$","",phone);
  return phone;
}

//halant specific correction for aryan langs
char* SchwaSpecificCorrection(char * phone){
   register int i=0,j;
   char c1[100],c2[100],c3[100],c4[100];
   char * schwaList[38]={"k","kh","g","gh","ng","c","ch","j","jh","nj","tx","txh","dx","dxh",
                                                    "nx","t","th","d","dh","n","p","ph","b","bh","m","y",
                                                    "r","l","s","w","sh","sx","zh","h","lx","rx","f","dxq"};
   char * vowelList[42]={"av&","nwv&","nnv&","rwv&","lwv&","lnv&","aav&","iiv&","uuv&","rqv&","eev&","eiv&","ouv&",
                                                    "axv&","oov&","aiv&","nnx&","nxx&","rrx&","llx&","lxx&",
                                                    "aa&","iv&","ov&","mq&","aa&","ii&","uu&","rq&",
                                                    "ee&","ei&","ou&","oo&","ax&","ai&","ev&","uv&",
                                                    "a&","e&","i&","o&","u&"};



   if(DEBUG)printf("%d\n", strlen(phone));
   int len=strlen(phone);
   phone[len]='!';
   phone[len+1]='\0';
   if(DEBUG)printf("phone cur%s\n",phone);
   // for end correction &av&t&aav&
   for(i=0;i<38;i++){
      for(j=1;j<42;j++){//dont want av
         strcpy(c1,"&av&");
         strcat(c1,schwaList[i]);
         strcat(c1,"&");
         strcat(c1,vowelList[j]);
         strcat(c1,"!");
         strcpy(c2,"&euv&");
         strcat(c2,schwaList[i]);
         strcat(c2,"&");
         strcat(c2,vowelList[j]);
         strcat(c2,"!");
         if(strstr(phone, c1) != NULL)phone =Utf8ReplaceAll(c1,"@",phone);
         if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@",c2,phone);
      }
      strcat(c1,schwaList[i]);
   }
   if(strstr(phone, "!") != NULL)phone =Utf8ReplaceAll("!","",phone);

    for(i=0;i<38;i++){
      strcpy(c1,"&av&");
      strcat(c1,schwaList[i]);
      strcat(c1,"&av&");
      strcpy(c2,"&euv$&");
      strcat(c2,schwaList[i]);
      strcat(c2,"&av$&");
      if(strstr(phone, c1) != NULL)phone =Utf8ReplaceAll(c1,"@",phone);
      if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@",c2,phone);
    }
    if(DEBUG)printf("inside schwa%s\n", phone);
   //  for(j=0;j<42;j++)
   //  {
   //     strcpy(c4,"&eu&");
   //     strcat(c4,c3);
   //     strcat(c4,"$");
   //  	strcat(c4,vowelList[j]);
   //  	strcpy(c2,c1);
   //  	strcat(c2,vowelList[j]);
   //    for(;i<39;i++)
   //    {
   //       strcpy(c1,"&av&");
   //       strcat(c1,schwaList[i]);
   //       strcat(c1,"&");
   //       strcpy(c3,"&");
   //       strcat(c3,schwaList[i]);
   //       strcat(c3,"&");
   //        //if(DEBUG)printf("%s\t%s\n",c2,c4);
   //     	if(strstr(phone, c2) != NULL)phone =Utf8ReplaceAll(c2,"@",phone);
   //     	if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@",c4,phone);
   //     }
   //  }

   for(i=0;i<38;i++){
      strcpy(c1,"&av&");
      strcat(c1,schwaList[i]);
      strcat(c1,"&");
      strcpy(c3,"&");
      strcat(c3,schwaList[i]);
      strcat(c3,"&");
      for(j=0;j<41;j++){
         strcpy(c4,"&euv&");
         strcat(c4,c3);
         strcat(c4,"$");
      	strcat(c4,vowelList[j]);
      	strcpy(c2,c1);
      	strcat(c2,vowelList[j]);
         //if(DEBUG)printf("%s\t%s\n",c2,c4);
      	if(strstr(phone, c2) != NULL)phone =Utf8ReplaceAll(c2,"@",phone);
      	if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@",c4,phone);
      }
   }
   //if(strstr(phone, "#") != NULL)phone =Utf8ReplaceAll("#","",phone);
   if(strstr(phone, "$") != NULL)phone =Utf8ReplaceAll("$","",phone);


   //&q&w&eu&
   for(i=0;i<39;i++){
      strcpy(c1,"&q&");
      strcat(c1,schwaList[i]);
      strcat(c1,"&euv&");
      strcpy(c2,"&q&");
      strcat(c2,schwaList[i]);
      strcat(c2,"&av&");
      if(strstr(phone, c1) != NULL)phone =Utf8ReplaceAll(c1,"@",phone);
      if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@",c2,phone);
   }

   // for(i=0;i<39;i++){
   //    strcpy(c1,"&eu&");
   //    strcat(c1,schwaList[i]);
   //    strcat(c1,"&eu&");
   //    strcpy(c2,"&eu&");
   //    strcat(c2,schwaList[i]);
   //    strcat(c2,"&av&");
   //    if(strstr(phone, c1) != NULL)phone =Utf8ReplaceAll(c1,"@",phone);
   //    if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@",c2,phone);
   //
   // }

   return phone;
}

//checnk if vowel is in input. 'q' special case, 'rq' special case
int CheckVowel(char * input, int q, int rq){
   if(strstr(input,"a")!=NULL)return 1;
   if(strstr(input,"e")!=NULL)return 1;
   if(strstr(input,"i")!=NULL)return 1;
   if(strstr(input,"o")!=NULL)return 1;
   if(strstr(input,"u")!=NULL)return 1;
   if(q)if(strstr(input,"q")!=NULL)return 1;
   if(rq)if(strstr(input,"rq")!=NULL)return 1;
   return 0;
}

//check single vowels
int CheckSingleVowel(char input,int q){
   if(input == 'a') return 1;
   if(input == 'e') return 1;
   if(input == 'i') return 1;
   if(input == 'o') return 1;
   if(input == 'u') return 1;
   if(q)if(input == 'q') return 1;
   return 0;
}

//get the type of phone in the position
int GetPhoneType(char * input, int pos){
   char * phone =strdup(input);
   char * pch;
   int count=1,i=0;
   pch = strtok(phone,"&");
   while (pch != NULL && pch != "")
   {
      if(count++>=pos)break;
      pch = strtok (NULL, "&");
   }
   if(DEBUG)printf("str : %s %d\n ",pch,GetType(pch));
   return GetType(pch);
}

//get the type of given input
int GetType(char * input){
   int i=0;
   for(i=0;i<VOWELSSIZE;i++)if(strcmp(VOWELS[i],input)==NULL)return 1;
   for(i=0;i<CONSONANTSSIZE;i++)if(strcmp(CONSONANTS[i],input)==NULL)return 2;
   for(i=0;i<SEMIVOWELSSIZE;i++)if(strcmp(SEMIVOWELS[i],input)==NULL)return 3;
   return 0;
}

//check if chillaksharas are there --for malayalam
int CheckChillu(char * input){
   if(strstr(input,"nwv")!=NULL)return 1;
   if(strstr(input,"nnv")!=NULL)return 1;
   if(strstr(input,"rwv")!=NULL)return 1;
   if(strstr(input,"lwv")!=NULL)return 1;
   if(strstr(input,"lnv")!=NULL)return 1;
   return 0;
}

//get UTF-8 from CPS
char * GetUTF(char * input){
   register int i=0;
   for(;i<ROW;i++)
   {
   	if(strcmp(symbolTable[i][1],input)==0)
   	{
         return symbolTable[i][0];
   	}
   }
   return 0;
}

//verify the letter is english char -- CLS
int isEngLetter(char * p){
   if((int)p >= 97 && (int)p <= 122)return 1;
   else return 0;
}

//remove unwanted Symbols from word
char * CleanseWord(char * phone){
   register int i=0;
   while(phone[i] != '\0'){
      if(!(phone[i] =='&') && !isEngLetter(phone[i])){
         phone[i] = '#';
      }
      i++;
   }
   phone[i]='\0';
   //if(strstr(phone, "#") != NULL)phone =Utf8ReplaceAll("#","",phone);
   if(strstr(phone, "$") != NULL)phone =Utf8ReplaceAll("$","",phone);
   if(strstr(phone, "&&") != NULL)phone =Utf8ReplaceAll("&&","&",phone);
   return phone;
}

//correct the geminate syllabification ,isReverse --reverse correction
char* GeminateCorrection(char * phone ,int isReverse){
   register int i=0;
   char c1[100],c2[100];
   char * geminateList[39]={"k","kh","lx","rx","g","gh","ng","c","ch","j","jh","nj","tx","txh","dx","dxh","nx","t","th","d","dh","n","p","ph","b","bh","m","y","r","l","w","sh","sx","zh","y","s","h","f","dxq"};
   for(;i<39;i++){
      strcpy(c1,"&");
      strcat(c1,geminateList[i]);
      strcat(c1,"&eu&");//check for av also
      strcat(c1,geminateList[i]);
      strcat(c1,"&");
      strcpy(c2,"&");
      strcat(c2,geminateList[i]);
      strcat(c2,"&");
      strcat(c2,geminateList[i]);
      strcat(c2,"&");
      //if(DEBUG)printf("%s %s\n",c1 ,c2 );
      if(isReverse){
         if(strstr(phone, c2) != NULL)phone =Utf8ReplaceAll(c2,c1,phone);
      }else{
         if(strstr(phone, c1) != NULL)phone =Utf8ReplaceAll(c1,c2,phone);
      }
   }
   return phone;
}


int CountChars( char* s, char c ){
   return *s == '\0'? 0: CountChars( s + 1, c ) + (*s == c);
}

int bi=0;
char * leftStr[10000];
char * rightStr[10000];

void GenerateAllCombinations(int j,char * s,char * c[256][256],int isRight){
   int i=0;
   char *t;
   t=malloc(1000);
   if(c[j][0][0]=='#'){
      if(isRight){
         rightStr[bi]=malloc(30);
         strcpy(rightStr[bi],s);
         strcat(rightStr[bi],"&");
         bi++;
      }else{
         leftStr[bi]=malloc(30);
         strcpy(leftStr[bi],s);
         strcat(leftStr[bi],"&");
         bi++;
      }
   }else{
      for(i=0;c[j][i][0]!='#';i++){
         strcpy(t,s);
         strcat(t,"&");
         strcat(t,c[j][i]);
         GenerateAllCombinations(j+1,t,c,isRight);
      }
   }
}

void GenerateMatrix(char * combMatrix[256][256],char * regex){
   int row=0,col=0;
   combMatrix[0][0] = malloc(10);
   for(int i=0,item=0;i<strlen(regex);i++)
   {
      if(regex[i]=='&'){
         combMatrix[row][col+1] = malloc(10);
         strcpy(combMatrix[row][col+1],"#");
         row++;col=0;item=0;
         combMatrix[row][col] = malloc(10);
      }else if(regex[i]=='|'){
         col++;item=0;
         combMatrix[row][col] = malloc(10);
      }else{
         combMatrix[row][col][item++]=regex[i];
      }
      if(DEBUG)printf("%d %d %s\n",row,col,combMatrix[row][col]);
   }
   combMatrix[row][col+1] = malloc(10);
   strcpy(combMatrix[row][col+1],"#");
   combMatrix[row+1][0] = malloc(10);
   strcpy(combMatrix[row+1][0],"#");
}

char * CombinationCorrection(char * phone, char * left, char * right, int isReverse){
   char * leftComb[256][256];
   char * rightComb[256][256];
   GenerateMatrix(leftComb,left);
   GenerateMatrix(rightComb,right);
   bi=0;GenerateAllCombinations(0,"",leftComb,0);
   bi=0;GenerateAllCombinations(0,"",rightComb,1);

   int i=0;
   while(leftStr[i]!= NULL){
      if(isReverse){
         if(strstr(phone, rightStr[i]) != NULL)phone =Utf8ReplaceAll(rightStr[i],"!",phone);
         if(strstr(phone, "!") != NULL)phone =Utf8ReplaceAll("!",leftStr[i],phone);
      }else{
         if(DEBUG)printf("%s %s\n",leftStr[i],rightStr[i]);
         if(strstr(phone, leftStr[i]) != NULL)phone =Utf8ReplaceAll(leftStr[i],"!",phone);
         if(strstr(phone, "!") != NULL)phone =Utf8ReplaceAll("!",rightStr[i],phone);
      }
      i++;
   }
   return phone;
}


//Language specific corrections
char* LangSpecificCorrection(char * phone, int langSpecFlag){
   if(isSouth){
      if(strstr(phone, "&ei&") != NULL)phone =Utf8ReplaceAll("&ei&","&ai&",phone);
      if(strstr(phone, "&eiv&") != NULL)phone =Utf8ReplaceAll("&eiv&","&aiv&",phone);
   }else{
      if(strstr(phone, "&oo&") != NULL)phone =Utf8ReplaceAll("&oo&","&o&",phone);
      if(strstr(phone, "&oov&") != NULL)phone =Utf8ReplaceAll("&oov&","&ov&",phone);
   }
   // 'q' corrections for all langs
   //if(strstr(phone, "&a&hq&") != NULL)phone =Utf8ReplaceAll("&a&hq&","&hq&",phone);
   if(strstr(phone, "&q&") != NULL)phone =Utf8ReplaceAll("&q&","!",phone);
   if(strstr(phone, "!") != NULL)phone =Utf8ReplaceAll("!","&av&q&",phone);
   if(strstr(phone, "&a&av&") != NULL)phone =Utf8ReplaceAll("&a&av&","&a&",phone);
   if(strstr(phone, "&e&av&") != NULL)phone =Utf8ReplaceAll("&e&av&","&e&",phone);
   if(strstr(phone, "&i&av&") != NULL)phone =Utf8ReplaceAll("&i&av&","&i&",phone);
   if(strstr(phone, "&o&av&") != NULL)phone =Utf8ReplaceAll("&o&av&","&o&",phone);
   if(strstr(phone, "&u&av&") != NULL)phone =Utf8ReplaceAll("&u&av&","&u&",phone);
   if(strstr(phone, "&a&rqv&") != NULL)phone =Utf8ReplaceAll("&a&rqv&","&rq&",phone);
   if(strstr(phone, "&aa&av&") != NULL)phone =Utf8ReplaceAll("&aa&av&","&aa&",phone);
   if(strstr(phone, "&ae&av&") != NULL)phone =Utf8ReplaceAll("&ae&av&","&ae&",phone);
   if(strstr(phone, "&ax&av&") != NULL)phone =Utf8ReplaceAll("&ax&av&","&ax&",phone);
   if(strstr(phone, "&ee&av&") != NULL)phone =Utf8ReplaceAll("&ee&av&","&ee&",phone);
   if(strstr(phone, "&ii&av&") != NULL)phone =Utf8ReplaceAll("&ii&av&","&ii&",phone);
   if(strstr(phone, "&ai&av&") != NULL)phone =Utf8ReplaceAll("&ai&av&","&ai&",phone);
   if(strstr(phone, "&au&av&") != NULL)phone =Utf8ReplaceAll("&au&av&","&au&",phone);
   if(strstr(phone, "&oo&av&") != NULL)phone =Utf8ReplaceAll("&oo&av&","&oo&",phone);
   if(strstr(phone, "&uu&av&") != NULL)phone =Utf8ReplaceAll("&uu&av&","&uu&",phone);
   if(strstr(phone, "&rq&av&") != NULL)phone =Utf8ReplaceAll("&rq&av&","&rq&",phone);
   if(strstr(phone, "&av&av&") != NULL)phone =Utf8ReplaceAll("&av&av&","&av&",phone);
   if(strstr(phone, "&ev&av&") != NULL)phone =Utf8ReplaceAll("&ev&av&","&ev&",phone);
   if(strstr(phone, "&iv&av&") != NULL)phone =Utf8ReplaceAll("&iv&av&","&iv&",phone);
   if(strstr(phone, "&ov&av&") != NULL)phone =Utf8ReplaceAll("&ov&av&","&ov&",phone);
   if(strstr(phone, "&uv&av&") != NULL)phone =Utf8ReplaceAll("&uv&av&","&uv&",phone);
   if(strstr(phone, "&av&rqv&") != NULL)phone =Utf8ReplaceAll("&av&rqv&","&rqv&",phone);
   if(strstr(phone, "&aav&av&") != NULL)phone =Utf8ReplaceAll("&aav&av&","&aav&",phone);
   if(strstr(phone, "&aev&av&") != NULL)phone =Utf8ReplaceAll("&aev&av&","&aev&",phone);
   if(strstr(phone, "&auv&av&") != NULL)phone =Utf8ReplaceAll("&auv&av&","&auv&",phone);
   if(strstr(phone, "&axv&av&") != NULL)phone =Utf8ReplaceAll("&axv&av&","&axv&",phone);
   if(strstr(phone, "&aiv&av&") != NULL)phone =Utf8ReplaceAll("&aiv&av&","&aiv&",phone);
   if(strstr(phone, "&eev&av&") != NULL)phone =Utf8ReplaceAll("&eev&av&","&eev&",phone);
   if(strstr(phone, "&eiv&av&") != NULL)phone =Utf8ReplaceAll("&eiv&av&","&eiv&",phone);
   if(strstr(phone, "&iiv&av&") != NULL)phone =Utf8ReplaceAll("&iiv&av&","&iiv&",phone);
   if(strstr(phone, "&oov&av&") != NULL)phone =Utf8ReplaceAll("&oov&av&","&oov&",phone);
   if(strstr(phone, "&ouv&av&") != NULL)phone =Utf8ReplaceAll("&ouv&av&","&ouv&",phone);
   if(strstr(phone, "&uuv&av&") != NULL)phone =Utf8ReplaceAll("&uuv&av&","&uuv&",phone);
   if(strstr(phone, "&rqv&av&") != NULL)phone =Utf8ReplaceAll("&rqv&av&","&rqv&",phone);

   if(!langSpecFlag)return phone;

   char buffer[2000];
   char *buf=0;
   FILE *output;
   char * fileName=strdup(GetFile(langId,2));
   if(DEBUG)printf("rules file : %s\n",fileName );
   output = fopen(fileName,"r");
   if(!output){
      if(DEBUG)printf("rules not found\n");
      return phone;
   }
   char left[1000], right[1000];
   //update head and tail in phone
   char * tempstr =strdup(phone);
   strcpy(phone,"^");
   strcat(phone,tempstr);
   strcat(phone,"$");
   if(DEBUG)printf("phone : %s\n",phone );
   while (fgets(buffer,2000,output)!=NULL){
      if(strstr(buffer,"#")!=NULL || strlen(buffer)<5)continue;
      buf = strtok(buffer, "\t\n");
      strcpy(left,buf);
      buf = strtok(NULL, "\t\n");
      strcpy(right,buf);
      //if(writeFormat == 1)WriteFile(buf);
      if(DEBUG)printf("left right : %s   %s\n",left ,right);
      //operate
      if(strstr(left,"|")!=NULL){
         char * a1=strdup(left);a1++;a1[strlen(a1)-1] = 0;
         char * a2=strdup(right);a2++;a2[strlen(a2)-1] = 0;
         if(DEBUG)printf("%s\t %s\n",a1,a2 );
         phone = CombinationCorrection(phone,a1,a2,0);
      }else{
         if(strstr(phone, left) != NULL)phone =Utf8ReplaceAll(left,"!",phone);
         if(strstr(phone, "!") != NULL)phone =Utf8ReplaceAll("!",right,phone);
      }
   }
   //remove head and tail in phone
   if(strstr(phone, "^") != NULL)phone =Utf8ReplaceAll("^","",phone);
   if(strstr(phone, "$") != NULL)phone =Utf8ReplaceAll("$","",phone);
   if(DEBUG)printf("phone : %s\n",phone );
   fclose(output);
   //return phone;

   int i=0,count=0;
   for(;i<strlen(phone);i++)//end correction
   {
      if(phone[i] == '&')count=i;
   }
   phone[count+1]='\0';
   return phone;
}

//Reverse syllable correction for syllable parsing
char* SyllableReverseCorrection(char * phone, int langSpecFlag){
   if(isSouth){
      if(strstr(phone, "&ai&") != NULL)phone =Utf8ReplaceAll("&ai&","&ei&",phone);
      if(strstr(phone, "&aiv&") != NULL)phone =Utf8ReplaceAll("&aiv&","&eiv&",phone);
   }else{
      if(strstr(phone, "&o&") != NULL)phone =Utf8ReplaceAll("&o&","&oo&",phone);
      if(strstr(phone, "&ov&") != NULL)phone =Utf8ReplaceAll("&ov&","&oov&",phone);
   }
   if(!langSpecFlag)return phone;

   char buffer[2000];
   char *buf=0;
   FILE *output;
   char * fileName=strdup(GetFile(langId,2));
   if(DEBUG)printf("rules file : %s\n",fileName );
   output = fopen(fileName,"r");
   if(!output){
      if(DEBUG)printf("rules not found\n");
      return phone;
   }
   char left[100], right[100];
   //update head and tail in phone
   char * tempstr =strdup(phone);
   strcpy(phone,"^");
   strcat(phone,tempstr);
   strcat(phone,"$");
   if(DEBUG)printf("phone : %s\n",phone );
   while (fgets(buffer,2000,output)!=NULL){
      buf = strtok(buffer, "\t\n");
      strcpy(left,buf);
      buf = strtok(NULL, "\t\n");
      strcpy(right,buf);
      if(DEBUG)printf("left right : %s   %s\n",left ,right);
      //operate
      if(strstr(phone, right) != NULL)phone = Utf8ReplaceAll(right,"!",phone);
      if(strstr(phone, "!") != NULL)phone = Utf8ReplaceAll("!",left,phone);
   }
   //remove head and tail in phone
   if(strstr(phone, "^") != NULL)phone = Utf8ReplaceAll("^","",phone);
   if(strstr(phone, "$") != NULL)phone = Utf8ReplaceAll("$","",phone);
   if(DEBUG)printf("phone : %s\n",phone );
   fclose(output);

   // enum LANG corrLang;
   // corrLang = TAMIL;
   // if(langId == corrLang) //tamil
   // {
   //    if(DEBUG)printf("Tamil reverse corrections\n" );
   //    if(strstr(phone, "&f&") != NULL)phone =Utf8ReplaceAll("&f&","&hq&p&",phone);
   // }
   // corrLang = MALAYALAM;
   // if(langId == corrLang) //malayalam
   // {
   //    if(DEBUG)printf("malayalam reverse corrections\n" );
   //    if(strstr(phone, "&au&") != NULL)phone =Utf8ReplaceAll("&au&","&ou&",phone);
   // }
   // corrLang = KANNADA;
   // if(langId == corrLang) //kannada
   // {
   //    if(DEBUG)printf("kannada reverse corrections\n" );
   //    if(strstr(phone, "&au&") != NULL)phone =Utf8ReplaceAll("&au&","&ou&",phone);
   // }
   // corrLang = TELUGU;
   // if(langId == corrLang) //kannada
   // {
   //    if(DEBUG)printf("telugu reverse corrections\n" );
   //    if(strstr(phone, "&au&") != NULL)phone =Utf8ReplaceAll("&au&","&ou&",phone);
   // }
   return phone;
}

//language specific syllable correction
int LangSyllableCorrection(char * input){
   if(strcmp(input,"&av&q&")==NULL)return 1;
   else return 0;
}

//split into syllable array
int SplitSyllables(char * input){
   char * pch;
   int i=0,len;
   pch = strtok(input,"#");
   //round 1 split
   while (pch != NULL){
      if(strcmp(pch,"&")){
      	syllableList[i] = strdup(pch);
      	i++;
      }
      pch = strtok (NULL, "#");
   }
   len =i;
   if(DEBUG){
      for (i=0;i<len;i++){
      		printf("initStack : %s\n",syllableList[i] );
      }
   }
   //isSouth &&
	if(!CheckVowel(syllableList[len-1],1,0)&& !CheckChillu(syllableList[len-1])){//south specific av addition
      if(isSouth){
      	syllableList[len-1] = AppendString(syllableList[len-1],"&av&");
      }else{
         syllableList[len-1] = AppendString(syllableList[len-1],"&euv&");
      }
	}
   //round 2 correciton
	len = i;
   int euFlag=1;
	if(i>1)
   {

      for(i--;i>=0;i--)
      {
         if(LangSyllableCorrection(syllableList[i])){
            strcat(syllableList[i-1],syllableList[i]);
            syllableList[i]="";
         }
      	if(strstr(syllableList[i],"&eu&")!= NULL){
      		syllableList[i] =Utf8ReplaceAll("&eu&","!",syllableList[i]);
            euFlag=1;
      	}
         if(strstr(syllableList[i],"&euv&")!= NULL){
      		syllableList[i] =Utf8ReplaceAll("&euv&","!",syllableList[i]);
            euFlag=2;
      	}
      	if(!CheckVowel(syllableList[i],0,1)){
      		if(i-1>=0){
      			strcat(syllableList[i-1],syllableList[i]);
      			syllableList[i]="";
      		}else{
      			strcat(syllableList[i],syllableList[i+1]);
      			syllableList[i+1]="";
      		}
      	}
      	if(i-1>0){
      		if(euFlag==1)if(strstr(syllableList[i-1], "!") != NULL)syllableList[i-1] =Utf8ReplaceAll("!","&eu&",syllableList[i-1]);
            if(euFlag==2)if(strstr(syllableList[i-1], "!") != NULL)syllableList[i-1] =Utf8ReplaceAll("!","&euv&",syllableList[i-1]);
      		if(strstr(syllableList[i-1], "&&") != NULL)syllableList[i-1] =Utf8ReplaceAll("&&","&",syllableList[i-1]);
      	}
      	if(euFlag==1)if(strstr(syllableList[i], "!") != NULL)syllableList[i] =Utf8ReplaceAll("!","&eu&",syllableList[i]);
         if(euFlag==2)if(strstr(syllableList[i], "!") != NULL)syllableList[i] =Utf8ReplaceAll("!","&euv&",syllableList[i]);
      }
	}else{
		if(!CheckVowel(syllableList[0],1,0)){
         strcat(syllableList[0],"&av");
		}
	}
	int j=0;
   //temp fix e1 to eu
   //if(strstr(syllableList[0], "&e1") != NULL)syllableList[0] =Utf8ReplaceAll("&e1","&euv&",syllableList[0]);

	if(DEBUG){
   	for (i=0;i<len;i++)
   	{
         printf("syllablifiedStack : %s\n",syllableList[i] );
   	}
	}
   //round 3 double syllable correction
   for (i=0;i<len;i++)
   {
      //corrections
      syllableList[i] =Utf8ReplaceAll("1","",syllableList[i]);
      //syllableList[i] =CleanseWord(syllableList[i]);
      if(DEBUG)printf("LenStack : %d\n",strlen(syllableList[i]));
      if(strlen(syllableList[i])>0){
         if(strstr(syllableList[i],"&eu&")!= NULL){
      		syllableList[i] =Utf8ReplaceAll("&eu&","!",syllableList[i]);
            euFlag=1;
      	}
         if(strstr(syllableList[i],"&euv&")!= NULL){
      		syllableList[i] =Utf8ReplaceAll("&euv&","!",syllableList[i]);
            euFlag=2;
      	}

         if(!CheckVowel(syllableList[i],0,1)){
            if(DEBUG)printf("Stack : %s\n",syllableList[i] );
            strcat(syllableList[i],"&av");
         }
         if(strstr(syllableList[i],"!")!= NULL){
            if(euFlag==1)if(strstr(syllableList[i], "!") != NULL)syllableList[i] =Utf8ReplaceAll("!","&eu&",syllableList[i]);
            if(euFlag==2)if(strstr(syllableList[i], "!") != NULL)syllableList[i] =Utf8ReplaceAll("!","&euv&",syllableList[i]);
            syllableList[i] =Utf8ReplaceAll("!","eu",syllableList[i]);
         }
      }
      if(strstr(syllableList[i],"&&")!= NULL){
         syllableList[i] =Utf8ReplaceAll("&&","&",syllableList[i]);
      }
      syllableList[i] =GeminateCorrection(syllableList[i],1);
      //syllableList[i]=CleanseWord(syllableList[i]);

   }
   if(DEBUG){
   	for (i=0;i<len;i++)
   	{
   		printf("syllablifiedStack1 : %s\n",syllableList[i] );
   	}
   }
   if(DEBUG)printf("No of syllables : %d\n", len);
   syllableCount=len;
   return 1;

}



//make to write format
int WritetoFiles(){
   int i,j;
   if(DEBUG){
      for (i=0;i<syllableCount;i++)
      {
         //syllableList[i] =CleanseWord(syllableList[i]);
         printf("syllablifiedStackfinal : %s\n",syllableList[i] );
      }
   }
   char * pch;
   outputText = (char *)malloc(1000);//*strlen(syllableList[0]));
   // if(syldictFlag){
   //    syllableList[0] = Utf8ReplaceAll("&eu","!",syllableList[0]);
   //    if(!CheckVowel(syllableList[0],1,0)){
   //       strcat(syllableList[0],"&av&");
   //    }
   //    syllableList[0] =Utf8ReplaceAll("!","&eu",syllableList[0]);
   // }
   int validSyllable=0;
   for (i=0;i<syllableCount;i++)
   {
      if(strcmp(syllableList[i],""))validSyllable++;
   }

   if(DEBUG)printf("a correction%s\n", syllableList[0]);
   strcpy(outputText,"");
   if(writeFormat == 0)//phone
   {
      int syllablesPrint=0;
      for (i=0;i<syllableCount;i++)
      {
         strcat(outputText ,"(( ");
         pch = strtok(syllableList[i],"&");
         while (pch != NULL && pch != "")
         {
            if(DEBUG)printf("syl  output%s  %s\n", pch,outputText);
            j=1;
            //if(strcmp(pch,"av") == NULL || CheckSymbol(pch)){
            strcat(outputText ,"\"");
            strcat(outputText , pch);
            strcat(outputText ,"\" ");
            //}
            pch = strtok (NULL, "&");
         }
         if(j){
            if(syllTagFlag)
            {
               if(syllablesPrint==0){strcat(outputText ,"_beg");}
               else if(syllablesPrint==validSyllable-1){strcat(outputText ,"_end");}
               else {strcat(outputText ,"_mid");}
               syllablesPrint++;
            }
            strcat(outputText ,") 0) ");
         }else{
            outputText[strlen(outputText)-3]='\0';
         }
         j=0;
      }
      if(strstr(outputText, "v") != NULL)outputText =Utf8ReplaceAll("v","",outputText);
      if(strstr(outputText, " \"eu\"") != NULL)outputText =Utf8ReplaceAll(" \"eu\"","",outputText);
      if(strstr(outputText, "!") != NULL)outputText =Utf8ReplaceAll("!","",outputText);
   }
   else if(writeFormat == 1)//syllable
   {
      //printf("syl count%d\n",validSyllable );
      int syllablesPrint=0;
      for (i=0;i<syllableCount;i++)
      {
         if(strstr(syllableList[i], "euv") != NULL)syllableList[i] =Utf8ReplaceAll("euv","eu",syllableList[i]);
         syllableList[i] = SyllableReverseCorrection(syllableList[i],LangSpecificCorrectionFlag);
         strcat(outputText ,"(( \"");
         //currentSyllable = strdup(syllableList[i]);
         pch = strtok(syllableList[i],"&");
         while (pch != NULL && pch != "")
         {
            if(DEBUG)printf("syl %s\n",pch );
            j=1;
            if(CheckSymbol(pch))
            {
               if(strcmp(pch,"av"))
               {
                  strcat(outputText , GetUTF(pch));
               }else{
                  if(DEBUG)printf("av found");
                  strcat(outputText , GetUTF(pch));
               }
            }
            pch = strtok (NULL, "&");
         }
         if(j){
            if(syllTagFlag)
            {
               if(syllablesPrint==0){strcat(outputText ,"_beg");}
               else if(syllablesPrint==validSyllable-1){strcat(outputText ,"_end");}
               else {strcat(outputText ,"_mid");}
               syllablesPrint++;
            }
            strcat(outputText ,"\" ) 0) ");

         }else{
            outputText[strlen(outputText)-4]='\0';
         }

         j=0;
      }
   }
   //final unwanted # correction. due to hidden chars
   if(strstr(outputText, "#") != NULL)outputText =Utf8ReplaceAll("#","",outputText);

   if(DEBUG)printf("Print text : %s\n",outputText);
   // if(syldictFlag && !doubleParseSyl){
   //   //for (i=0; outputText[i]; outputText[i]=='0' ? i++ : *outputText++);
   //   printf("count 0 : %d\n", CountChars(outputText,'0'));
   //   if(CountChars(outputText,'0')>1){
   //     doubleParseSyl=1;
   //
   //     main(1,"");
   //     return 1;
   //   }
   // }
   WriteFile(outputText);
   return 1;
}

//write to wordpronunciation file
void WriteFile(char * text){
   FILE *output;
   output = fopen(outputFile,"w");
   if(fliteHTSFlag){
      // text =Utf8ReplaceAll("0","#",text);
      // text =Utf8ReplaceAll("\" \"","*",text);
      // text =Utf8ReplaceAll("(","",text);
      // text =Utf8ReplaceAll(")","",text);
      // text =Utf8ReplaceAll(" ","",text);
      // text =Utf8ReplaceAll("\"","",text);
      text[strlen(text)-1]='\0';
      printf("phones : %s\n",text);
      fprintf(output,"%s",text);
      fclose(output);
      return;
   }
   fprintf(output,"%s","(set! wordstruct '( ");
   fprintf(output,"%s",text);
   fprintf(output,"%s","))\n");
   fclose(output);
   if(!writeFormat && pruiningFlag)WritePruneFile(text);
}

//output for pruning
void WritePruneFile(char * text){
   FILE *output;
   output = fopen(outputPruneFile,"w");
   text =Utf8ReplaceAll("(","",text);
   text =Utf8ReplaceAll(")","",text);
   text =Utf8ReplaceAll("0","",text);
   text =Utf8ReplaceAll("   "," ",text);
   text =Utf8ReplaceAll("  ","",text);
   text =Utf8ReplaceAll("\"\"","\" \"",text);
   fprintf(output,"%s",text);
   fclose(output);
}

//print the help of syntax
void printHelp () {
   printf ("UnifiedParser : v2.4\n");
   printf ("Usage : ./unified-parser word LangSpecificCorrection WriteFormat ForSylldict IsPruning\n");
   printf ("LangSpecificCorrection : \n\t0-No\n\t1-Yes \n");
   printf ("WriteFormat : \n\t0-Phone\n\t1-Syllable \n");
   printf ("ForSylldict : writes output to wordpronunciationsyldict\n\t0-No\n\t1-Yes \n");
   printf ("IsPruning : writes output for pruning format\n\t0-No\n\t1-Yes \n");
   //printf ("(test)outPutFormat :\n\t0-Festival\n\t1-Flite \n");
   return;
}

// //Lex replacer -- in dev stage. 
// char * LexReplacer(char * phone){
//    //printf("1 %s\n", phone);
//    //&iv&#&aav&  DoubleModifierCorrection
//    strcat(phone,"&");
// 	register int i=0,j=0;
// 	char c1[10],c2[10];
//    //k|kh|lx|rx|g|gh|ng|c|ch|j|jh|nj|tx|txh|dx|dxh|nx|t|th|d|dh|n|p
//    //|ph|b|bh|m|y|r|r|l|w|sh|sx|zh|y|s|h|f|dxq|z|kq|khq|gq|dxhq
// 	char * consonentList[44]={"k","kh","lx","rx","g","gh","ng","c","ch","j","jh","nj","tx","txh",
//                            "dx","dxh","nx","t","th","d","dh","n","p","ph","b","bh","m","y","r",
//                         "l","w","sh","sx","zh","y","s","h","f","dxq","z","kq","khq","gq","dxhq" };
//    char * vowelList[43]={"av","nwv","nnv","rwv","lwv","lnv","aav","iiv","uuv","rqv","eev","eiv","ouv",
//                                                  "axv","oov","aiv","nnx","nxx","rrx","llx","lxx",
//                                                  "aa","iv","ov","mq","aa","ii","uu","rq","eu",
//                                                  "ee","ei","ou","oo","ax","ai","ev","uv",
//                                                  "a","e","i","o","u"};
//    for(j=0;j<44;j++){
//       strcpy(c1,"&");
//       strcat(c1,consonentList[j]);
//       strcat(c1,"&");
//       strcpy(c2,c1);
//       strcat(c2,"av&#&");
//       //if(DEBUG)            printf("%s %s\n",c1,c2 );
//    	if(strstr(phone, c1) != NULL)phone =Utf8ReplaceAll(c1,"@",phone);
//    	if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@",c2,phone);
//    }

//    for(j=0;j<43;j++){
//       strcpy(c1,"&av&#&");
//       strcat(c1,vowelList[j]);
//       strcat(c1,"&");
//       strcpy(c2,"&");
//       strcat(c2,vowelList[j]);
//       strcat(c2,"&#&");
//       //if(DEBUG)
//       printf("%s %s\n",c1,c2 );
//    	if(strstr(phone, c1) != NULL)phone =Utf8ReplaceAll(c1,"@",phone);
//    	if(strstr(phone, "@") != NULL)phone =Utf8ReplaceAll("@",c2,phone);
//    }
//    printf("1 :%s\n", phone);
//    //if(strstr(phone, "&av&#&av&") != NULL)phone =Utf8ReplaceAll("&av&#&av&","&av&#&",phone);
//    if(strstr(phone, "&&") != NULL)phone =Utf8ReplaceAll("&&","&",phone);
//    strcat(phone,"$");
//    if(strlen(phone)>9){
//       if(strstr(phone, "&av&#&$") != NULL)phone =Utf8ReplaceAll("&av&#&$","&",phone);
//       if(strstr(phone, "$") != NULL)phone =Utf8ReplaceAll("$","",phone);
//    }

//    printf("2 :%s\n", phone);
//    return phone;
// }

//The MAIN Function
void main(int argc,char** argv){

   //    int oc;
   //    while((oc = getopt (argc,argv,":DlspdP")) != -1) {
   //     switch (oc) {
   //         case 'D' :
   //          printf("Dict enabled\n" );
   //           //isDump = 1;
   //           break;
   //         case 'l' :
   //           //printHelp ();
   //           exit(0);
   //           break;
   //     }
   // }
   //printf("argv : %s\n",argv );
   if (argc<= 5) {
      printHelp ();
      exit(1);
   }
   if(strcmp(argv[2],"1")) LangSpecificCorrectionFlag = 0;
   //printf(" lang %d\n", LangSpecificCorrectionFlag);
   if(strcmp(argv[3],"0")) writeFormat = 1;
   if(!strcmp(argv[4],"1")) {
      //syldictFlag = 0;
      writeFormat = 0;
      outputFile="wordpronunciationsyldict";
   }
   if(!strcmp(argv[5],"1")) {
      pruiningFlag = 1;
      //syldictFlag = 0;
      outputFile="wordpronunciation";
      writeFormat = 0;
   }

 //   	FILE *f;
 //   	char line1[128];
 //    f = fopen("inputunified", "r");
 //    if (!f) {
 //       printf("error");
 //    } else {
 //        fscanf(f, "%127[^\n]", line1);
 //        printf("1:%s\n", line1);
 //        fclose(f);
 //    }
 //    //char* word=strdup(line1);

	// char* word="नहिस";

      //fliteHTSFlag=1;
   char * word;
   word = (char *)calloc(1048576, sizeof(char));
   strcpy(word,argv[1]);
   if(DEBUG){
   	printf("Word : %s\n", word);
   }

   //if for syl dict
   //remove unwanted letters
   word = RemoveUnwanted(word);
   if(DEBUG){printf("Cleared Word : %s\n", word);}
   if(!SetlanguageFeat(word)){return 0; }
   if(CheckDictionary(word)){return 0; }//!syldictFlag &&
   if(DEBUG){printf("langId : %d\n", langId);}
   word = ConvertToSymbols(word);
   // if(syldictFlag){
   //   syllableCount = 1;
   //   syllableList[0] = word;
   //   WritetoFiles();
   //   if(DEBUG)printf("syldict word : %s\n",word );
   //   return 0;
   // }

   if(DEBUG){
      printf("Symbols code : %s\n", unicodeWord);
      printf("Symbols syllables : %s\n", syllabifiedWord);
   }
   syllabifiedWordOut = (char *)calloc(1048576, sizeof(char));
   yy_scan_string(syllabifiedWord);
   yyparse();

   //syllabifiedWordOut = LexReplacer(word);

   // if(!isSouth)//direct south correction - not used now
   // {
   //    //if(strstr(syllabifiedWordOut, "&av&") != NULL)syllabifiedWordOut =Utf8ReplaceAll("&av&","&eu&",syllabifiedWordOut);
   // }
   if(DEBUG){printf("Syllabified Word : %s\n", syllabifiedWordOut); }
   if(strstr(syllabifiedWordOut, "&#&") != NULL)syllabifiedWordOut = Utf8ReplaceAll("&#&","&",syllabifiedWordOut);
   strcat(syllabifiedWordOut,"&\0");
   if(DEBUG)printf("Syllabified Word out : %s\n", syllabifiedWordOut);
   syllabifiedWordOut = LangSpecificCorrection(syllabifiedWordOut,LangSpecificCorrectionFlag);
   if(DEBUG)printf("Syllabified Word langCorr : %s\n", syllabifiedWordOut);
   //syllabifiedWordOut = GeminateCorrection(syllabifiedWordOut,0);
   if(DEBUG)printf("Syllabified Word gemCorr : %s\n", syllabifiedWordOut);
   syllabifiedWordOut = CleanseWord(syllabifiedWordOut);
   if(DEBUG)printf("Syllabified Word memCorr : %s\n", syllabifiedWordOut);
   if(!isSouth)//halant split word
   {
      int i=0,count=0;
      for(i=0;i<strlen(syllabifiedWordOut);i++)
      {
         if(syllabifiedWordOut[i] =='&')count++;//just to get count of phones
      }
      int splitPosition = 2;// by default split after 2 phones
      if(GetPhoneType(syllabifiedWordOut,1) == 1) //check first is vowel
      {
         if(count>2){//more than 1 phones are there
            int type = GetPhoneType(syllabifiedWordOut,2); // get type of 2nd phone
            if(type==2)splitPosition=1;
            else if(type==3)splitPosition=3;
         }else{
            splitPosition=1;
         }
      }
      count=0;
      for(i=0;i<strlen(syllabifiedWordOut);i++)
      {
         if(syllabifiedWordOut[i] =='&')count++;
         if(count>splitPosition)
         {
           count=i;
           break;
         }
      }
      char * start,* end;
      // for first char never remove 'av'
      start = strdup(syllabifiedWordOut);
      end = strdup(syllabifiedWordOut);
      end += count;
      start[count]='\0';
      if(DEBUG)printf("posi  %d %s %s\n",count,start,end);
      end = SchwaSpecificCorrection(end);
      if(DEBUG)printf("prefinal : %s\n", syllabifiedWordOut);
      strcpy(syllabifiedWordOut,start);
      strcat(syllabifiedWordOut,end);
      if(DEBUG)printf("prefinal1 : %s\n", syllabifiedWordOut);
      syllabifiedWordOut = CleanseWord(syllabifiedWordOut);
      if(DEBUG)printf("final : %s\n", syllabifiedWordOut);
      syllabifiedWordOut = SchwaDoubleConsonent(syllabifiedWordOut);//for eu&C&C&V
      if(DEBUG)printf("final0 : %s\n", syllabifiedWordOut);
   }

   syllabifiedWordOut = GeminateCorrection(syllabifiedWordOut,0);//chk here
   if(DEBUG)printf("Syllabified Word gemCorr : %s\n", syllabifiedWordOut);
   syllabifiedWordOut = Syllabilfy(syllabifiedWordOut);
   //syllabifiedWordOut = DoubleModifierCorrection(syllabifiedWordOut);
   if(DEBUG)printf("Syllabified Word final : %s\n", syllabifiedWordOut);
   SplitSyllables(syllabifiedWordOut);
   if(DEBUG)printf("Splitted to Syllables\n");
   WritetoFiles();
   if(DEBUG)printf("Files created %s\n",outputText);
   //CombinationCorrection();
}


#line 1553 "unified.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "unified.tab.h".  */
#ifndef YY_YY_UNIFIED_TAB_H_INCLUDED
# define YY_YY_UNIFIED_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    space = 258,
    fullvowel = 259,
    kaki = 260,
    conjsyll2 = 261,
    conjsyll1 = 262,
    nukchan = 263,
    yarule = 264,
    consonant = 265,
    vowel = 266,
    halant = 267,
    matra = 268
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 1549 "unified.y" /* yacc.c:355  */

	char* strval;

#line 1611 "unified.tab.c" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_UNIFIED_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 1626 "unified.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  14
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   11

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  14
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  4
/* YYNRULES -- Number of rules.  */
#define YYNRULES  14
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  16

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   268

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,  1558,  1558,  1570,  1581,  1584,  1585,  1586,  1587,  1588,
    1589,  1590,  1591,  1592,  1593
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "space", "fullvowel", "kaki",
  "conjsyll2", "conjsyll1", "nukchan", "yarule", "consonant", "vowel",
  "halant", "matra", "$accept", "sentence", "words", "syltoken", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268
};
# endif

#define YYPACT_NINF -5

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-5)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      -4,    -5,    -5,    -5,    -5,    -5,    -5,    -5,    -5,    -5,
      -5,    10,    -4,    -5,    -5,    -5
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,     0,     2,     3,     1,     4
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
      -5,    -5,    -5,    -1
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    11,    12,    13
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      14,    15
};

static const yytype_uint8 yycheck[] =
{
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
       0,    12
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    15,    16,    17,     0,    17
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    14,    15,    16,    16,    17,    17,    17,    17,    17,
      17,    17,    17,    17,    17
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 1558 "unified.y" /* yacc.c:1646  */
    {
            if(parseLevel ==0){
            	syllabifiedWordOut = (yyvsp[0].strval);
            	if(strstr(syllabifiedWordOut, "&&") != NULL)syllabifiedWordOut =Utf8ReplaceAll("&&","&",syllabifiedWordOut);
            	parseLevel++;
            }
            else{
            	  phonifiedWord = (yyvsp[0].strval);
              }
            }
#line 2711 "unified.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 1570 "unified.y" /* yacc.c:1646  */
    {  		/*	int length = strlen($1);
   			int j = 0;
   			if ( $1[length -1] == 'h'){
   				char str[length];
   				for(j=0;j<length-1;j++)
   					str[j]=$1[j];
   				str[j]='\0';
   				strcpy($1,str);
   			}*/
   			if(DEBUG)printf("\nSyll:\t%s",(yyvsp[0].strval));
   			(yyval.strval) = (yyvsp[0].strval); }
#line 2727 "unified.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 1581 "unified.y" /* yacc.c:1646  */
    { char* str = malloc(2*(strlen((yyvsp[-1].strval))+strlen((yyvsp[0].strval)))) ; sprintf( str,"%s%s",(yyvsp[-1].strval),(yyvsp[0].strval));(yyval.strval)=str;}
#line 2733 "unified.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 1584 "unified.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 2739 "unified.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 1585 "unified.y" /* yacc.c:1646  */
    { if(DEBUG)printf("kaki : %s\n",(yyvsp[0].strval) );(yyval.strval) = (yyvsp[0].strval); }
#line 2745 "unified.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 1586 "unified.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 2751 "unified.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 1587 "unified.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 2757 "unified.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 1588 "unified.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 2763 "unified.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 1589 "unified.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 2769 "unified.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 1590 "unified.y" /* yacc.c:1646  */
    {  (yyval.strval)=(yyvsp[0].strval);/*char* str = malloc(strlen($1)+strlen("्")+4); sprintf( str,"%s&#",$1 );printf("\nconsonant:\t");$$=str*/; }
#line 2775 "unified.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 1591 "unified.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 2781 "unified.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 1592 "unified.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 2787 "unified.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 1593 "unified.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 2793 "unified.tab.c" /* yacc.c:1646  */
    break;


#line 2797 "unified.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
