//**********************************
// Shell code
//**********************************

#include <iostream>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <unistd.h>
#include <libgen.h>
#include <stdlib.h>
#include <ctype.h>
#include <pwd.h>

using namespace std;
//char pathname[MAXPATHLEN];

int main()
{
  string command;
  int ls();
  int ps();
  cout << "\n**MelShell**\n";
  while( command != "exit")
  {
    if( command == "help" )
      cout << "This is a small shell used to perform ls and ps commands";
    else if ( command == "ls" )
      ls();
    else if ( command == "ps" )
      pidaux();
    else if ( command != "\0" )
      cout << "Error: Unknown Command";
      
      cout << "\n%_ ";
      cin >> command;
}
  return 0;
}

int ls()
{
  struct dirent *f;
  DIR *md;
  char* di=(char*)get_current_dir_name();
  md = opendir(di);
  while (( f = readdir(md)) != NULL ) 
    cout <<"\n "<<f->d_name; 
  return 0;
}

int ps()
{
  cout<< "this is test";
}
