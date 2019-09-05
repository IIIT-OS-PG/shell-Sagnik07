#include "header.h"
using namespace std;

int cd(char **path)	{
	
	char path1[1000],cwd[1000];
	int l,i,j;
	if(path[1])	{
		if((strcmp(path[1],"..")==0) ||(strcmp(path[1],"-")==0) || (strcmp(path[1],"-/")==0) || (strcmp(path[1],"../")==0))	{
			getcwd(cwd,sizeof(cwd));
			//cout<<cwd<<endl;
			l=strlen(cwd);
			for(i=l-1;i>=0;i--)	{
				if(cwd[i]=='/')
					break;
			}
			for(j=0;j<i;j++)	{
				path1[j]=cwd[j];
			}
			path1[j]='\0';
			//cout<<"Path1: "<<path1<<endl;
			return chdir(path1);
		}
		return chdir(path[1]);
	}
	return 0;
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
	cout<<user<<"@"<<hostname<<":";
}

void pipe1(string str)	{
	char ch[1000];
	strcpy(ch,str.c_str());
	char *command[1000];
	char *token=strtok(ch,"|");
	int j=0,i;
	//int fd[2],in=0;
	while(token!=NULL)	{
		command[j]=token;
		token=strtok(NULL,"|");
		j++;
	}
	command[j]=NULL;
	//cout<<"j: "<<j<<endl;
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
			//for(int x=0;x<k;x++)	
			//	cout<<"argv["<<x<<"]: "<<argv[x];

			pid_t pid;
			pid=fork();
			if(pid<0)	{
				cout<<"Failed";
				exit(0);
			}
			if(pid==0)	{
				//close(in);
				dup2(fd[1],1);
				//close(out);
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
		//for(int x=0;x<k;x++)	
		//	cout<<"argv["<<x<<"]:"<<argv[x];
		if(execvp(argv[0],argv)<0)	{
			cout<<argv[0]<<": command not found\n";
			//exit(0);
		}
	}
	else
		wait(NULL);
}


void io_redirection(string str,char *filename,int i)	{
	char ch[1000];
	strcpy(ch,str.c_str());
	char *command[1000];
	char *token=strtok(ch," ");
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
	//for(int x=0;x<j;x++)
	//	cout<<"command["<<x<<"]: "<<command[x];
	
	//cout<<"\nfilename: "<<filename<<endl;
	if(i==1)	{
		fd=open(filename, O_CREAT | O_WRONLY,0777);
		dup2(fd,1);
		//close(fd);
	}
	else if(i==2)	{
		fd=open(filename,O_CREAT | O_WRONLY| O_APPEND,0777);
		dup2(fd,1);
		//close(fd);
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

int main()	{							
	
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
	string str;
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

	char *hist[1000];
	int hc=0;
	for(int i=0;i<100;i++)
		hist[i]=NULL;
	
	cout<<"***Welcome to my shell***\n";
	while(1)	{
		print_prompt(p->pw_name,hostbuffer);
		char ch[100],prompt[100];
		getcwd(prompt,sizeof(prompt));
		cout<<prompt;
		cout<<"$ ";
		char *argv[100],c;
		int j=0;
		
		int flag=0,pipefd[2];

		str="";
		getline(cin,str);
		strcpy(ch,str.c_str());

		free(hist[hc]);
		hist[hc]=strdup(ch);
		hc=(hc+1)%1000;		
		
		char *token=strtok(ch," ");

		int flagio=0;		
	
		while(token!=NULL)	{
			argv[j]=token;
			if(strcmp(token,"|")==0)	{
				flag=1;
			}
			if(strcmp(token,">")==0)	{
				flagio=1;
			}
			if(strcmp(token,">>")==0)	{
				flagio=2;
			}
			token=strtok(NULL," ");
			j++;
		}
		argv[j]=NULL;
		
		if(flagio==1)	{
			io_redirection(str,argv[j-1],1);
			continue;
		}

		if(flagio==2)	{
			io_redirection(str,argv[j-1],2);
			continue;
		}

		if(strcmp(argv[0],"history")==0)	{
			history(hist,hc);
			continue;
		}

		if(strcmp(argv[0],"cd")==0)	{
			if(cd(argv)<0)	{
				cout<<"Error!!\n";
			}
			continue;		
		}

		if(strcmp(argv[0],"echo")==0)	{
			char env[1000];
			string env_val;
			if((argv[1][0]=='$') || ((argv[1][0]=='\'') && (argv[1][1]=='\'') && (argv[1][2]=='$')))	{
				for(int k=1;k<strlen(argv[1]);k++)	
					env[k-1]=argv[1][k];

				env_val=getenv(env);
				if(env_val != "")
					cout<<env_val<<"\n";
				else 
					cout<<"\n";

			continue;

			}		
		}
		if(strcmp(argv[0],"exit")==0)
			exit(0);

		if(flag==1)	{
			pipe1(str);			
			continue;
		}

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
		//tcsetattr(STDIN_FILENO,TCSAFLUSH,&ttyorig);	
	}
	return 0;
}


















