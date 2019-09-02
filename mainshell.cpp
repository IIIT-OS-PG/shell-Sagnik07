#include "header.h"
using namespace std;

int main()	{
	pid_t pid;
	string str;
	
	cout<<"***Welcome to my shell***\n";
	int k=0;
	while(1)	{
		char ch[100];
		cout<<"$ ";
		char *argv[100];
		int j=0;
		str="";
		getline(cin,str);
		strcpy(ch,str.c_str());
		char *token=strtok(ch," ");

		while(token!=NULL)	{
			argv[j]=token;
			token=strtok(NULL," ");
			j++;
		}
		argv[j]=NULL;
		/*for(int i=0;i<j;i++)
			cout<<argv[i]<<" ";
			
		cout<<endl;*/
		pid=fork();
		if(pid<0)	{
			cout<<"Failed";
			exit(0);
		}
		if(pid==0)	{		
			if(execvp(argv[0],argv)<0)	{
				cout<<"Error!\n";
				exit(0);
				}
		}
		else	{
			wait(NULL);
		}	
	}
	return 0;
}

