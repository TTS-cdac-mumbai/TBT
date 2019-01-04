#!/usr/bin/env python 
# -*- coding:utf-8 -*- 


from langdetect import detect
from langdetect import DetectorFactory
import sys

f= open("check.txt","w+")
DetectorFactory.seed = 0

#text ="ସାଂପ୍ରତିକ ବିଷୟ ବସ୍ତୁ"
text ="पैसे" #sys.argv[0]
text1=text.decode('utf-8')
lang = detect(text1)

print("Language of the text:", lang)
f.write(lang)
