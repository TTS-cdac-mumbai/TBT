#include <iostream>
#include <fstream>

using namespace std;

int main()
{
   ifstream ipfile;
   ofstream opfile;
   string str1;
   string str2;
   string temp;
   ipfile.open("Transcription.txt");
   opfile.open("words.mlf");
   if (ipfile.is_open())
   {
	opfile<<"#!MLF!#"<<endl;
	ipfile>>str1;
	opfile<<"\""<<str1.c_str()<<".lab\""<<endl;
	while (!ipfile.eof())
	{
		
		ipfile>>str2;
		while(str2[0] != 'w' || str2[1] != 'a' || str2[2] != 'v' || str2[3] != '_')
                {
                        temp = str2;			
			opfile<<str2.c_str()<<endl;
                        ipfile>>str2;  
                        if (str2 == "SIL")
                        {
                                 if (temp == "SIL")
                                 break;
                        }    
   		}
		opfile<<"."<<endl;
		if (str2 != "SIL")
		opfile<<"\""<<str2.c_str()<<".lab\""<<endl;
	}
    }

    ipfile.close();
    opfile.close();

}
