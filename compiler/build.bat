@set BISON_SIMPLE=c:\cbuilderx\bin\bison.simple
@bison -y -d parse.y
@copy y_tab.c parse.cpp
@copy y_tab.h parse.tab.h
