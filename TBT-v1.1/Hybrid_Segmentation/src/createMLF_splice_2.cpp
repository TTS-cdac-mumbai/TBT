#include <iostream>
#include <fstream>

using namespace std;

int main()
{
   ifstream ipfile;
   ofstream opfile;
   string str1;
   string str2;

   ipfile.open("Transcription_splice_2.txt");
   opfile.open("words_splice.mlf");
   if (ipfile.is_open())
   {
	opfile<<"#!MLF!#"<<endl;
	while (!ipfile.eof())
	{
		ipfile>>str1;
		ipfile>>str2;
		opfile<<"\""<<str1.c_str()<<".lab\""<<endl;
		opfile<<str2.c_str()<<endl;
		opfile<<"."<<endl;
	}
    }

    ipfile.close();
    opfile.close();

}







