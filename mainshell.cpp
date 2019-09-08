#include "header.h"
using namespace std;

unordered_map<string,string> mp;
unordered_map<string,string> mpassign;
struct termios ttyorig;
int cd(char **path)	{
	
	char path1[1000],cwd[1000];
	int l,i,j;
	if(path[1])	{
		if((strcmp(path[1],"..")==0) ||(strcmp(path[1],"-")==0) || (strcmp(path[1],"-/")==0) || (strcmp(path[1],"../")==0))	{
			getcwd(cwd,sizeof(cwd));
			l=strlen(cwd);
			for(i=l-1;i>=0;i--)	{
				if(cwd[i]=='/')
					break;
			}
			for(j=0;j<i;j++)	{
				path1[j]=cwd[j];
			}
			path1[j]='\0';
			return chdir(path1);
		}
		if(strcmp(path[1],"~")==0)	{
			return chdir(getenv("HOME"));
		}
		return chdir(path[1]);
	}
	return 0;
}

void echocall(char **argv)	{
	char env[1000]={};
	string assign;
	string env_val;
	int k;
	for(k=1;k<strlen(argv[1]);k++)	
		env[k-1]=argv[1][k];
	//env[k]='\0';
	if((strcmp(env,"HOME")==0)||(strcmp(env,"PATH")==0)||(strcmp(env,"USER")==0)||(strcmp(env,"HOSTNAME")==0)||(strcmp(env,"PS1")==0))	{
		env_val=getenv(env);
		if(env_val != "")
			cout<<env_val<<"\n";
	}
	else	{ 
		for(int k=1;k<strlen(argv[1]);k++)		
			assign+=argv[1][k];
			
		if(mpassign.find(assign)!=mp.end())
			cout<<mpassign[assign];
			cout<<"\n";

	}
}

void history(char *hist[],int hc)	{
	
	int min,c;
	min=hc-1000;
	if(min>=0)	{
		c=1;
		for(int i=min;c<1000;i++)	{
			if(hist[i])
				cout<<i+1<<" "<<hist[i]<<"\n";

			c++;
		}
	}
	else	{
		for(int i=0;i<hc;i++)	{
			if(hist[i])
				cout<<i+1<<" "<<hist[i]<<"\n";
		}
	}
}
void print_prompt(string user,char hostname[])	{
	char prompt[100];
	cout<<user<<"@"<<hostname<<":";
	getcwd(prompt,sizeof(prompt));
	cout<<prompt;
	cout<<"$ ";
}

void pipe1(string str)	{
	char ch[1000];
	strcpy(ch,str.c_str());
	char *command[1000];
	char *token=strtok(ch,"|");
	int j=0,i;
	while(token!=NULL)	{
		command[j]=token;
		token=strtok(NULL,"|");
		j++;
	}
	command[j]=NULL;
	pid_t pid1=fork();
	if(pid1<0)	{
		cout<<"Failed";
		exit(0);
	}
	if(pid1==0)	{
		for(i=0;i<j-1;i++)	{
			char *token1=strtok(command[i]," ");
			int k=0;
			char *argv[1000];
			while(token1!=NULL)	{
				argv[k]=token1;
				token1=strtok(NULL," ");
				k++;
			}
			argv[k]=NULL;
			int fd[2],in=0;
			pipe(fd);

			pid_t pid;
			pid=fork();
			if(pid<0)	{
				cout<<"Failed";
				exit(0);
			}
			if(pid==0)	{
				dup2(fd[1],1);
				if(execvp(argv[0],argv)<0)	{
					cout<<argv[0]<<": command not found\n";
					//exit(0);
				}
			}
			else
				wait(NULL);
			dup2(fd[0],0);
			close(fd[1]);
		}

		char *token1=strtok(command[i]," ");
		char *argv[1000];
		int k=0;
		while(token1!=NULL)	{
			argv[k]=token1;
			token1=strtok(NULL," ");
			k++;
		}
		argv[k]=NULL;
		if(execvp(argv[0],argv)<0)	{
			cout<<argv[0]<<": command not found\n";
			//exit(0);
		}
	}
	else
		wait(NULL);
}


void io_redirection(string str,int i)	{
	char ch[1000];
	strcpy(ch,str.c_str());
	char *command[1000],*filename;
	char *thiscommand=strtok(ch,">");
	char *thiscommand1;
	if(i==1)	{	
		while(thiscommand!=NULL)	{
			filename=thiscommand;
			thiscommand=strtok(NULL,">");
		}
		thiscommand1=strtok(ch,">");
	}
	else	{
		while(thiscommand!=NULL)	{
			filename=thiscommand;
			thiscommand=strtok(NULL,">>");
		}
		thiscommand1=strtok(ch,">>");
	}
	
	char *token=strtok(thiscommand1," ");
	int j=0;
	int fd,in=0;
	int savedstdout=dup(1);
	while(token!=NULL)	{
		if((strcmp(token,">")==0) || (strcmp(token,">>")==0))
			break;		
		command[j]=token;
		token=strtok(NULL," ");
		j++;
	}
	command[j]=NULL;
	
	if(i==1)	{
		fd=open(filename, O_CREAT|O_WRONLY|O_TRUNC,0666);		
		dup2(fd,1);
	}
	else if(i==2)	{
		fd=open(filename,O_APPEND|O_WRONLY,0666);
		dup2(fd,1);
	}
	pid_t pid;
	pid=fork();
	if(pid<0)	{
		cout<<"Failed";
		exit(0);
	}
	if(pid==0)	{
		if(execvp(command[0],command)<0)	{
			cout<<command[0]<<": command not found\n";
			//exit(0);
		}
	}
	else
		wait(NULL);
	close(fd);
	dup2(savedstdout,1);
	close(savedstdout);

}


void aliasfunc(string str,int aliaspoint)	{
	char ch[1000];
	strcpy(ch,str.c_str());
	char *command[1000];
	char *token=strtok(ch,"=");
	int j=0,i;
	while(token!=NULL)	{
		command[j]=token;
		token=strtok(NULL,"=");
		j++;
	}
	command[j]=NULL;
	string orgcommand1="";
	string orgcommand(command[1]);
	for(i=0;i<orgcommand.length();i++)	{
		if(orgcommand[i]!='\"')
			orgcommand1+=orgcommand[i];
	}

	string aliascommand(command[0]);
	aliascommand=aliascommand.substr(aliaspoint+1);


	mp[aliascommand]=orgcommand1;

}

string check(string str)	{
	string s="",newstring="";
	str+=" ";
	int wc=0;
	for(int i=0;i<str.length();i++)		{

		if(str[i]==' ')	{
			wc++;
			if(mp.find(s)!=mp.end())
				s=mp[s];
			if(wc>1)
				newstring=newstring+" "+s;
			else
				newstring+=s;
			s="";		
		}
		else
			s+=str[i];
	}

	return newstring;
}
string echocheck(string str)	{
	//cout<<"str in func: "<<str<<endl;
	string s="",newstring="";
	str+=" ";
	int i=0,echopoint=0,echoflag=1;
	char s1[10000];
	for(i=0;i<str.length();i++)		{
		if(str[i]==' ')	{
			strcpy(s1,s.c_str());
			if(strcmp(s1,"echo")==0)	{
				echoflag=1;
				echopoint=i+1;
			}
			
			s="";		
		}
		else
			s+=str[i];
	}
	if(echoflag==0)
		return str;
	else	{
		for(i=0;i<echopoint;i++)	{
			newstring+=str[i];
		}
		for(i=echopoint;i<str.length();i++)	{
			if((str[i]!='\"') && (str[i]!='\''))
				newstring+=str[i];
		}
			
		return newstring;
	}
}

void disable()	{
	tcsetattr(STDIN_FILENO,TCSAFLUSH,&ttyorig);
}

int main()	{							
	tcgetattr(STDIN_FILENO,&ttyorig);
	atexit(disable);
	struct termios raw=ttyorig;
	raw.c_lflag &=~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw);
	ifstream file;
	int count=1;
	string word;
	char name[1000],value[1000];
	file.open(".shellrc");
	while(file>>word)	{
		if(count%2 !=0)
			strcpy(name,word.c_str());
		else
			strcpy(value,word.c_str());
		count=(count+1)%2;
		setenv(name,value,1);
	}
	file.close();
	pid_t pid;
	uid_t uid;
	string str,str1,str2;
	struct passwd *p;
	if((p=getpwuid(uid=getuid()))==NULL)
		cout<<"Error!";
	//cout<<"name: "<<p->pw_name<<endl;
	//cout<<"uid: "<<p->pw_uid<<endl;
	//cout<<"dir: "<<p->pw_dir<<endl;
	//cout<<"name: "<<p->pw_name<<endl;
	
	char hostbuffer[1000];
	int hostname;
	hostname=gethostname(hostbuffer,sizeof(hostbuffer));
	//cout<<"Hostname: "<<hostbuffer<<endl;
	setenv("HOSTNAME",hostbuffer,1);
	setenv("USER",p->pw_name,1);
	setenv("HOME",p->pw_dir,1);
	char *hist[1000];
	int hc=0;
	for(int i=0;i<100;i++)
		hist[i]=NULL;
	
	cout<<"***Welcome to my shell***\n";
	while(1)	{
		print_prompt(p->pw_name,hostbuffer);
		char ch[100];
		
		char *argv[100],c;
		int j=0;
		
		int flag=0,pipefd[2];

		str="";str1="",str2="";

		
		//char c;
		
		while(c=getchar())	{	
			if(iscntrl(c))	{
				if((int)c==10)	{
					cout<<"\n";
					break;
				}
				if((int)c==127)	{
					str1.erase(str1.begin()+str1.length()-1);
					cout<<"\b \b";
				}
				if((int)c==27)	{
					c=getchar();
					c=getchar();
					continue;
				}				
			}
			else	{
				cout<<c;
				str1+=c;
			}
		}

		//getline(cin,str1);
		if(str1=="")
			continue;
		if(str1=="~")	{
			cout<<"shell: "<<getenv("HOME")<<": Is a directory\n";
			continue;
		}
		str2=check(str1);
		str=echocheck(str2);
		strcpy(ch,str.c_str());

		free(hist[hc]);
		hist[hc]=strdup(ch);
		hc=(hc+1)%1000;			


		char *token=strtok(ch," ");

		int flagio=0,pipeflag=0,aliaspoint=0,aliasflag=0,assignmentflag=0;
		string var="",word="";	
	
		while(token!=NULL)	{
			argv[j]=token;

			if(strcmp(token,"alias")==0)	
				aliasflag=1;

			token=strtok(NULL," ");
			j++;
		}
		argv[j]=NULL;
		
		
		for(int x=0;x<str.length();x++)	{
			if(str[x]=='=' && aliasflag==0)	{
				assignmentflag=1;
				var=word;
				word="";
				continue;
			}
			if(str[x]=='|')
				pipeflag=1;
			if(str[x]=='>')	{
				if(str[x+1]=='>')	{
					flagio=2;
					break;
				}
				else	{
					flagio=1;
				}
			}
			if(str[x]!=' ')
				word+=str[x];
		}	
		if(assignmentflag==1)	{
			mpassign[var]=word;
			continue;
		}

		if(aliasflag==1)	{
			if(str[6]=='\0')	{
				//cout<<"Here!"<<endl;
				if(!mp.empty())	{
					for(auto it=mp.begin();it!=mp.end();it++)	{
						cout<<"alias "<<it->first<<"='"<<it->second<<"'"<<endl;
					}
				}
				else 
					cout<<"\n";
			}
			else
				aliasfunc(str,5);
			continue;
		}

		if(flagio==2)	{
			io_redirection(str,2);
			continue;
		}

		if(flagio==1)	{
			io_redirection(str,1);
			continue;
		}

				
		if(pipeflag==1)	{
			pipe1(str);			
			continue;
		}

		if(strcmp(argv[0],"history")==0)	{
			history(hist,hc);
			continue;
		}

		if(strcmp(argv[0],"cd")==0)	{
			if(cd(argv)<0)	{
				cout<<"shell: cd: "<<argv[1]<<": No such file or directory\n";
			}
			continue;		
		}

		if(strcmp(argv[0],"echo")==0)	{
			if(argv[1][0]=='$') { 
				echocall(argv);	
				continue;
			}		
		}


		if(strcmp(argv[0],"exit")==0)
			exit(0);

		
		pid=fork();
		if(pid<0)	{
			cout<<"Failed";
			exit(0);
		}
		if(pid==0)	{
			if(execvp(argv[0],argv)<0)	{
				cout<<argv[0]<<": command not found\n";
				//exit(0);
			}
		}
		else	{
			wait(NULL);
		}	
	}
	return 0;
}


















