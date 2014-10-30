#include <iostream>
#include <cstring>
#include <string>

using namespace std;


char exitSmallString[]  = "exit";
char  exitBigString[] = "Exit";
char ls[] = "ls"; 
char lsi[] = "-l";
int main (int argc, char *argv [ ] ) {

cout << "\nRunning Heba's Shell\n" << "Created By: Heba Basiony\n" << endl;

while ( true ) {

  string input = "";     
  char * cinput = NULL;  
  char* savedTokens[256] = {NULL}; 
  int nTokens = 0;   
  cout << "%_ ";
    getline (cin, input);	
  cinput = new char [input.size()+1];
  strcpy (cinput, input.c_str());
  char* tokens = strtok (cinput," ");
  while (tokens != NULL ) {
    savedTokens[nTokens] = strdup(tokens);  
    nTokens++;
    tokens = strtok(NULL," ");
  }         
  if(strcmp(exitSmallString, savedTokens[0]) == 0 || strcmp(exitBigString, savedTokens[0]) == 0){
    cout << "Have a great day! Shell shutting down." << endl;
    return 0;
  }
  if (strcmp(ls,savedTokens[0]) == 0 && strcmp(lsi,savedTokens[1]) == 0)
    cout << "test";
  for(int i = 0; i < nTokens; i++){
    cout << "savedTokens[ " << i << " ] = " << savedTokens[i] << endl;
  }
}
return 0;
}






