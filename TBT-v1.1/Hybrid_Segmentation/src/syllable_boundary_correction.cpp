/*-------------------------------------------------------------------------
 *  syllable_boundary_correction.cpp - A Program for Correcting Syllable Boundaries
 *  Version: 2.0
 *
 *  Purpose: Forced alignment syllable boundary correction using STE & SBSF   
 *
 *  Author: Aswin Shanmugam S  (sas91@outlook.com)
 *
 *  Created: Some Time in 2015
 *
 -------------------------------------------------------------------------*/

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <string.h>
#include <fstream>
#include <sstream>
#include <cmath>
using namespace std;

string energyBoundary[1000], fluxBoundary[1000], energyBoundary2[1000];
string hmmLab[1000][5];
string hybridLab[1000][4]; //flag in 4th column. 1-->HMM 2-->STE 3-->Flux

string affricateList[100], fricativeList[100], nasalList[100], semivowelList[100], sibilantList[100], silenceList[100], stopList[100];
int affricateListCount, fricativeListCount, nasalListCount, semivowelListCount, sibilantListCount, silenceListCount, stopListCount;

int topEnergy=0;
int topFlux=0;
int topEnergy2=0;
int numberOfSyllables, numberOfBoundariesEnergy, numberOfBoundariesFlux, numberOfBoundariesEnergy2;

string PopEnergy()
{
	topEnergy = topEnergy + 1;
	return energyBoundary[topEnergy-1];
}

string TopViewEnergy()
{
	return energyBoundary[topEnergy];
}

string PopFlux()
{
	topFlux = topFlux + 1;
	return fluxBoundary[topFlux-1];
}

string TopViewFlux()
{
	return fluxBoundary[topFlux];
}

string PopEnergy2()
{
        topEnergy2 = topEnergy2 + 1;
        return energyBoundary2[topEnergy2-1];
}

string TopViewEnergy2()
{
        return energyBoundary2[topEnergy2];
}

int IsAffricate(string phone)
{
	int k=0, flag=0;
	while(k < affricateListCount && phone != affricateList[k])
		k++;
	if(k < affricateListCount)
		flag=1;
	return flag;
}

int IsFricative(string phone)
{
        int k=0, flag=0;
        while(k < fricativeListCount && phone != fricativeList[k])
                k++;
        if(k < fricativeListCount)
                flag=1; 
        return flag;
}

int IsNasal(string phone)
{
        int k=0, flag=0;
        while(k < nasalListCount && phone != nasalList[k])
                k++;
        if(k < nasalListCount)
                flag=1; 
        return flag;
}

int IsSemivowel(string phone)
{
        int k=0, flag=0;
        while(k < semivowelListCount && phone != semivowelList[k])
                k++;
        if(k < semivowelListCount)
                flag=1; 
        return flag;
}

int IsSibilant(string phone)
{
        int k=0, flag=0;
        while(k < sibilantListCount && phone != sibilantList[k])
                k++;
        if(k < sibilantListCount)
                flag=1; 
        return flag;
}

int IsSilence(string phone)
{
        int k=0, flag=0;
        while(k < silenceListCount && phone != silenceList[k])
                k++;
        if(k < silenceListCount)
                flag=1; 
        return flag;
}

int IsStop(string phone)
{
        int k=0, flag=0;
        while(k < stopListCount && phone != stopList[k])
                k++;
        if(k < stopListCount)
                flag=1; 
        return flag;
}

void SyllableBoundaryCorrection()
{
	hybridLab[0][0] = hmmLab[0][0];
	hybridLab[0][1] = hmmLab[0][1];
	hybridLab[0][2] = hmmLab[0][2];
	hybridLab[0][3] = "1";
	hybridLab[numberOfSyllables-1][0] = hmmLab[numberOfSyllables-1][0];
	hybridLab[numberOfSyllables-1][1] = hmmLab[numberOfSyllables-1][1];
	hybridLab[numberOfSyllables-1][2] = hmmLab[numberOfSyllables-1][2];
	hybridLab[numberOfSyllables-1][3] = "1";
	float temp1=0, temp2=0, temp3, temp4, temp5, temp6=0, temp7=0, temp8=0, temp9=0;
        string buffer1, buffer2;
 	int flag, j, k;
	for (int i=1; i<numberOfSyllables-1; i++)
	{	
		temp3=0;temp4=0;temp5=0;
	        hybridLab[i][1] = hmmLab[i][1];
        	hybridLab[i][2] = hmmLab[i][2];
		flag=0;
		buffer1 = hmmLab[i][4];
		buffer2 = hmmLab[i+1][3];
		hybridLab[i][0] = hmmLab[i][0];
		hybridLab[i][3] = "1"; 
		if ( IsStop(buffer2) && !IsSilence(buffer1) )
		{
			istringstream(hmmLab[i][0]) >> temp3;
			istringstream(hybridLab[i-1][0]) >> temp4;
			istringstream(hmmLab[i+1][0]) >> temp5;
			while (temp3 > temp2 && topEnergy < numberOfBoundariesEnergy-1)
			{
				istringstream(PopEnergy()) >> temp1;
				istringstream(TopViewEnergy()) >> temp2;
			}
			if (abs(temp2 - temp3) > abs(temp3 - temp1))
			{
				if((temp1 - temp4) > 0.1 && (temp5 - temp1) > 0.1)
				{
					hybridLab[i][0] = energyBoundary[topEnergy-1];
					hybridLab[i][3] = "2";
					flag=1;
				}
			}
			else 
			{
				if ((temp5 - temp2) > 0.1 && (temp2 - temp4) > 0.1)
				{
					hybridLab[i][0] = energyBoundary[topEnergy];
					hybridLab[i][3] = "2";
					PopEnergy();
					flag=1;
				}
			}
		}
		else if ( IsStop(buffer1) && !IsSemivowel(buffer2) && !IsSilence(buffer2) )
                {
                        istringstream(hmmLab[i][0]) >> temp3;
                        istringstream(hybridLab[i-1][0]) >> temp4;
                        istringstream(hmmLab[i+1][0]) >> temp5;
                        while (temp3 > temp9 && topEnergy2 < numberOfBoundariesEnergy2-1)
                        {
                                istringstream(PopEnergy2()) >> temp8;
                                istringstream(TopViewEnergy2()) >> temp9;
                        }
                        if (abs(temp9 - temp3) > abs(temp3 - temp8))
                        {
                                if((temp8 - temp4) > 0.1 && (temp5 - temp8) > 0.1)
                                {
                                        hybridLab[i][0] = energyBoundary2[topEnergy2-1];
                                        hybridLab[i][3] = "2";
                                        flag=1;
                                }
                        }
                        else
                        {
                                if ((temp5 - temp9) > 0.1 && (temp9 - temp4) > 0.1)
                                {
                                        hybridLab[i][0] = energyBoundary2[topEnergy2];
                                        hybridLab[i][3] = "2";
                                        PopEnergy2();
                                        flag=1;
                                }
                        }
                }
		if (    (   flag == 0   ) && (   (  ( IsSibilant(buffer1) || IsSibilant(buffer2) ) && !( IsSibilant(buffer1) && IsSibilant(buffer2) )  ) || (  ( IsAffricate(buffer1) || IsAffricate(buffer2) ) && !( IsAffricate(buffer1) && IsAffricate(buffer2) )  ) || (  IsNasal(buffer1) && IsStop(buffer2)  )   )    )
		{	
			istringstream(hmmLab[i][0]) >> temp3;
			istringstream(hybridLab[i-1][0]) >> temp4;
			istringstream(hmmLab[i+1][0]) >> temp5;
			while (temp3 > temp7 && topFlux < numberOfBoundariesFlux-1)
                        {
                                istringstream(PopFlux()) >> temp6;
                                istringstream(TopViewFlux()) >> temp7;
                        }
                        if (abs(temp7 - temp3) > abs(temp3 - temp6))
                        {
				if ((temp6 - temp4) > 0.1 && (temp5 - temp6) > 0.1)
				{
                                	hybridLab[i][0] = fluxBoundary[topFlux-1];
					hybridLab[i][3] = "3";
				}
                        }
                        else
                        {
				if ((temp5 - temp7) > 0.1 && (temp7 - temp4) > 0.1)
				{
                                	hybridLab[i][0] = fluxBoundary[topFlux];
					hybridLab[i][3] = "3";
                                	PopFlux();
				}
                        }
		}
	}
}

int main (int agc, char** argv)
{
	string buffer1, buffer2, buffer3, buffer4, buffer5;
	int i;
	float temp;
	ifstream inFile;
	inFile.open(argv[1]);
	if (inFile >> buffer1)
	{
	       i=0;
		while (inFile >> buffer1 && inFile >> buffer2 && inFile >> buffer3 && inFile >> buffer4 && inFile >> buffer5) {
			hmmLab[i][0]=buffer1;
			hmmLab[i][1]=buffer2;
			hmmLab[i][2]=buffer3;
			hmmLab[i][3]=buffer4;
                        hmmLab[i][4]=buffer5;
			i++;
		}
	}
	inFile.close();
	numberOfSyllables=i;
		
	inFile.open("boundaryEgy");
	i=0;
	while (inFile >> buffer1 && inFile >> buffer2) {
		if (buffer2 == "1")
		{
			energyBoundary[i]=buffer1;
			i=i+1;
		}
	}
	inFile.close(); 
        numberOfBoundariesEnergy=i;
	
	inFile.open("boundaryEgy");
        i=0;
        while (inFile >> buffer1 && inFile >> buffer2) {
                if (buffer2 != "-1" && buffer2 != "3")
                {
                        energyBoundary2[i]=buffer1;
                        i=i+1;
                }
        }
        inFile.close(); 
        numberOfBoundariesEnergy2=i;

	
	inFile.open("boundaryFlux");
	i=0;
	while (inFile >> buffer1 && inFile >> buffer2) {
		if (buffer2 != "2")
		{
			fluxBoundary[i]=buffer1;
			i=i+1;
		}
	}
	inFile.close(); 
        numberOfBoundariesFlux=i;

	inFile.open("Phonelist_Description/Affricates");
        i=0;
        while (inFile >> buffer1) {
                affricateList[i]=buffer1;
                i=i+1;
        }   
        inFile.close(); 
        affricateListCount=i;

        inFile.open("Phonelist_Description/Fricatives");
        i=0;
        while (inFile >> buffer1) {
                fricativeList[i]=buffer1;
                i=i+1;
        }   
        inFile.close(); 
        fricativeListCount=i;	
	
	inFile.open("Phonelist_Description/Nasals");
        i=0;
        while (inFile >> buffer1) {
                nasalList[i]=buffer1;
                i=i+1;
        }   
        inFile.close(); 
        nasalListCount=i;

	inFile.open("Phonelist_Description/SemiVowels");
        i=0;
        while (inFile >> buffer1) {
                semivowelList[i]=buffer1;
                i=i+1;
        }   
        inFile.close(); 
        semivowelListCount=i;

	inFile.open("Phonelist_Description/SibilantFricatives");
        i=0;
        while (inFile >> buffer1) {
                sibilantList[i]=buffer1;
                i=i+1;
        }   
        inFile.close(); 
        sibilantListCount=i;
	
	inFile.open("Phonelist_Description/Silence");
        i=0;
        while (inFile >> buffer1) {
                silenceList[i]=buffer1;
                i=i+1;
        }   
        inFile.close(); 
        silenceListCount=i;
	
	inFile.open("Phonelist_Description/StopConsonants");
        i=0;
        while (inFile >> buffer1) {
                stopList[i]=buffer1;
                i=i+1;
        }   
        inFile.close(); 
        stopListCount=i;

	SyllableBoundaryCorrection();

	std::cout << std::fixed;
	std::cout << "#" << std::endl;
        for (i=0; i<numberOfSyllables; i++)
        {   
		istringstream(hybridLab[i][0]) >> temp;
                std::cout << std::setprecision(5) << temp << " " << hybridLab[i][1] << " " << hybridLab[i][2] << " " << hybridLab[i][3] << std::endl;
        }
}
