/* 
 * CS 460 Assignment 2
 * Winston Moh T.
 * Linux shell program
 * 3/29/2018
 */

#include <iostream>	// I/O fcns like cout
#include <string>	// for getline fcn
#include <stdexcept>	// throw exceptions.
#include <vector>	// for vector data structure.
#include <fstream>	// for File I/O
#include <sstream>	// for stringstream
#include <unistd.h>	// for running OS commands - ex. create processes, cd etc.
#include <sys/wait.h>	// for wait command
#include <sys/param.h>	// cd
#include <sys/types.h>	// print error fcn
using namespace std;

/* Global variable storing the history size. Default size is 4 */
int HISTORY_SIZE = 4;

/* Global variable storing the list of commands executed */
vector<string> history;

/*Hold individual tokens in each line.*/
vector<string> tokens;

/* Return status for process. */
bool RETURNSTATUS = true;

/* Variables for $HOME, $PWD and $PATH */
string home, pwd, PATH;

string getFirstToken(string str)
{
	string tok="";
	unsigned int i=0;
	while (str[i] != ' ' && i != str.length())
	{
		tok += str[i++];
	}
	return tok;
}	// getFirstToken()

void extract(string str, string &env_var, string &prompt)
{
	bool flag_dollar = false; int dollar_i=0;
	bool flag_equals = false; int equals_i=0;
	bool env_var_flag = false;
	env_var="", prompt="";
	
	for (unsigned int i=2; i<str.length() && !flag_dollar; i++)
	{
		if (str[i] == '$')
		{
			flag_dollar = true;
			dollar_i = i;
		}
	}
	if (flag_dollar)
	{
		for (unsigned int i=dollar_i; str[i] != '"' && !flag_equals; i++)
		{
			for (int j=dollar_i; str[j] != '=' && !env_var_flag; j++)
			{
				env_var += str[j];
				
			}
			if (env_var.length() > 0)
				env_var_flag = true;
			if (str[i] == '=')
			{
				flag_equals = true;
				equals_i = i;
			}
		}
	}
	if (flag_equals)
	{
		for (unsigned int i=equals_i+1; i<str.length(); i++)
		{
			if (str[i] == '"')
			{
				for (unsigned int j=i+1; j<str.length()-1; j++)
				{
					prompt += str[j];
				}
			}			
		}
	}
}	// extract()

void printHistory(vector<string> vec)
{
	int vec_size = vec.size() - 1;
	for (int i=vec_size; i>vec_size-HISTORY_SIZE; i--)
	{
		cout<<"- "<<vec[i]<<endl;
	}
}	// printHistory()

string getNextToken(string s)
{
	static unsigned int i=0;
	static string oldString = s;
	string t="";
	if (oldString != s)
	{
		i=0;
		oldString = s;
	}
	while ((s[i]==' ' or s[i]=='"') && i<s.length())
		i++;
	while ((s[i]!=' ' && s[i]!='"') && i<s.length())
	{
		t += s[i];
		i++;
	}
	i++;
	return t;
}	// getNextToken()

string getValue(string s)
{
	string tok = getNextToken(s);
	while (tok.length() > 0)
	{
		if (getNextToken(s).length() > 0)
			{
				tok = getNextToken(s);
				break;
			}
	}
	return tok;
}	// getValue()

string getString(string s)
{
	unsigned int i=0, j;
	string str="";
	while (s[i] != '"')
		i++;
	for (j=i+1; j<s.length()-1; j++)
		str += s[j];
	return str;
}	// getString()

void readFile(string &prompt, int &size)
{
	ifstream myfile;
	string text="", tok="";
	myfile.open ("shell.config");
	while (getline(myfile, text))
	{
		tok = getFirstToken(text);
		if (tok == "history")
		{
			 stringstream geek(getValue(text));	// object from the class stringstream
			 geek >> size;	// object has the value and then stream it to size.
		}
		if (tok == "prompt")
		{
			prompt = getString(text);
		}	
		// Get $HOME, $PWD and $PATH values.
		if (tok == "home")
		{
			home = getString(text);
		}
		if (tok == "pwd")
		{
			pwd = getString(text);
		}
		if (tok == "PATH")
		{
			PATH = getString(text);
		}
	}
	myfile.close();
}	// readFile()

bool accExec(string s)
{
	if (access(s.c_str(), F_OK) == 0) // File exists
	{
		if (access(s.c_str(), X_OK) == 0) // File is executable
			return true;
		else 
			perror("exec");		// print error message
	}
	return false;
}	// accExec()

void execute(string s)
{
	int status;
	pid_t pid = fork();	// create process and get process id.
	char* args[] = {(char*)s.c_str(), NULL};	// NULL terminated char pointer array
	if (pid < 0)
	{
		cout<<"Process could not be created..."<<endl;
		return;
	}
	else if (pid == 0)	// child process
	{
		execv(args[0], args);	// execute command.
	}
	else 
	{
		// parent process
		if ( waitpid(pid, &status, 0)  == -1 )
		{
			perror("waitpid failed");
			return;
		}
		if ( WIFEXITED(status) && RETURNSTATUS )	// return status for the child
		{
			cout<<"  "<<s<<" ("<<pid<<") returned status: "<<WEXITSTATUS(status)<<endl;
		}
		wait(NULL);	// wait for child process to terminate.
	}
}	// execute()

void flushTokens()
{
	if (tokens.size() > 0)
	{
		tokens.clear();	// clear the vector.
	}
}	// flushTokens()

void loadTokens(string str)
{
	string s="";
	unsigned int i;
	for (i=0; i<str.length(); i++)
	{
		if ( str[i] == ' ')
		{
			tokens.push_back(s);
			s="";
		}
		else if (i == str.length()-1 && str[i] != ' ')
		{
			s += str[i];
			tokens.push_back(s);
		}
		else if (i < str.length() && str[i] != ' ')
		{
			s += str[i];
		}
	}	
}	// loadTokens()

string getReturnStatus(string str)
{
	string t="";
	unsigned int i;
	for (i=0; i<str.length(); i++)
	{
		if (str[i] == '=')
			break;
	}
	i++;	// move i to next character.
	while (i <str.length())
	{
		t += str[i];	//populate string with returnSTATUS
		i++;
	}
	return t;
}	// getReturnStatus()

void upOneDir()
{
	unsigned int i,j;
	string temp="";
	for (i=pwd.length()-1; i>0; i--)
	{
		if (pwd[i] == '/')
			break;			// we know stop index position.
	}
	for (j=0; j<i; j++)
	{
		temp += pwd[j];
	}
	if (temp.length() == 0)		// give home dir  '/'
		temp = "/";
	pwd = temp;			// give pwd new directory.
}	// upOneDir()

void changeDir(string dir)
{
	string s="", temp="";
	vector<string> vec;
	unsigned int i;
	stringstream err_msg;	// hold error message.
	err_msg<<"  shell: cd: "<<dir<<": No such file or directory";
	
	s = pwd + '/' + dir;	// crete new directory string.

	if (access(s.c_str(), F_OK) == 0) // check if directory exists
	{
		if (dir[0] == '/')	// absolute path is given.
		{
			pwd += dir;		// move to new directory.
		}
		else if (dir[0] == '.')		// the destination has to be calculated.
		{
			for (i=0; i<dir.length(); i++)		// loop thru entire directory string
			{
				if (dir[i] == '/')		// check for delimiter.
				{
					vec.push_back(temp);		// put string into vector.
					temp = "";
				}
				else if (i == dir.length()-1 && dir[i] != '/')
				{
					temp += dir[i];
					vec.push_back(temp);
				}
				else if (i < dir.length() && dir[i] != '/')
				{
					temp += dir[i];				// create string.
				}
			}	// end of loop.
			
			for (i=0; i<vec.size(); i++)		// loop thru vector with directory strngs.
			{
				if (vec[i] == ".")
				{
					// do nothing - stay in current directory.
				}
				else if (vec[i] == "..")
				{
					s = pwd + '/' + vec[i];
					if (s != "//..") // File or directory exists
					{
						upOneDir();		// move up one directory.
					}
				}
				else
				{
					pwd += '/' + vec[i];
				}
			}
		}
		else 			// directory is relative to current working directory.
		{
			pwd += '/' + dir;
		}
	}
	else if (s == "//..")
	{
		// do nothing.
	}
	else
	{
		cout<<err_msg.str()<<endl;
	}
	
	// Working cd.
	/*int rc = chdir(dir.c_str());	// change directory
	if (rc == 0)	// change of directory was successful.
	{
		char buffer[MAXPATHLEN];
		char *path = getcwd(buffer, MAXPATHLEN);
		if (!path)
		{
			// error.
		}
		else
		{
			pwd = path;
		}
	}
	else
	{
		cout<<"  shell: cd: "<<dir<<": No such file or directory"<<endl;
	} */
}	// changeDir()

void runLS()
{
	string s = "/bin/ls";
	pid_t pid = fork();	// create process and get process id.
	
	/* char pointer array for ls commands and populate it. */
	char* args[100];				
	unsigned int i;
	args[0] = (char*)s.c_str();

	for (i=1; i<tokens.size(); i++)
	{
		args[i] = (char*)tokens[i].c_str();
	}
	args[i] = (char*)pwd.c_str();	// add current working directory to list of arguments.
	args[i+1] = NULL;		// end pointer array with NULL character.
	
	/* pid checks. */
	if (pid < 0)
	{
		cout<<"Process could not be created..."<<endl;
		return;
	}
	else if (pid == 0)	// child process
	{
		execv(args[0], args);		// execute command.
	}
	else 
	{
		// parent process
		wait(NULL);	// wait for child process to terminate.
	}	
}	// runLS()

void searchPATH()
{
	// tokenize the paths with ':' as delimiter.
	vector<string> paths;
	string t=""; unsigned int i=0;
	while (i < PATH.length())
	{
		if (PATH[i] == ':')
		{
			paths.push_back(t);
			t="";
		}
		else if (PATH[i] != ':' && i==PATH.length()-1)
		{
			t+=PATH[i];
			paths.push_back(t);
		}
		else if (PATH[i] != ':' && i<PATH.length())
		{
			t+=PATH[i];
		}
		i++;
	}

	bool flag = false;
	string s="";
	// load up paths.
	for (unsigned int j=0; j<paths.size(); j++)
	{
		/* char pointer array for commands and populate it. */
		char* args[100];			
		s = paths[j] + "/" + tokens[0];
		args[0] = (char*)s.c_str();

		for (i=1; i<tokens.size(); i++)
		{
			args[i] = (char*)tokens[i].c_str();
		}
		
		if (accExec(s) && !flag)		// file exists and is executable.
		{
			args[i] = NULL;		// insert NULL character at end of array.
			pid_t pid = fork();	// create process and get process id.
			/* pid checks. */
			if (pid < 0)
			{
				cout<<"Process could not be created..."<<endl;
				return;
			}
			else if (pid == 0)	// child process
			{
				execv(args[0], args);		// execute command.
			}
			else 
			{
				// parent process
				wait(NULL);	// wait for child process to terminate.
			}				
			flag = true;
		}
	}
	
	stringstream err_msg;	// hold error message.
	err_msg<<"-shell: "<<tokens[0]<<": command not found";
	if (!flag)
		cout<<err_msg.str()<<endl;
}	// searchPATH()

int main(int argc, char **argv)
{
	string prompt="", cmd="", env_name="", str_name="";
		
	/* Read shell.config file and retrieve ENVIRONMENTAL VARIABLES */
	readFile(prompt, HISTORY_SIZE);
	
	/* execute shell UI */
	do
	{
		cout<<prompt<<" ";
		getline(cin, cmd);
		flushTokens();			// clear tokens vector.
		history.push_back(cmd);		// update the history.
		loadTokens(cmd);		// update tokens vector.
		
		stringstream err_msg;	// hold error message.
		err_msg<<"-shell: "<<cmd<<": command not found";
		
		if (getFirstToken(cmd) == "set")	// change prompt variable
		{
			extract(cmd, env_name, str_name);
			if (env_name == "$RETURNSTATUS")
			{
				// get RETURNSTATUS Value.				
				getReturnStatus(tokens[1]) == "false" ? RETURNSTATUS = false : RETURNSTATUS = true; 
			}
			else 
			{
				prompt = str_name;	// change prompt string
			}
		}
		else if (getFirstToken(cmd) == "echo")	// print out something
		{
			if (tokens[1] == "$RETURNSTATUS")
			{
				RETURNSTATUS ? cout<<"TRUE"<<endl : cout<<"FALSE"<<endl;
			}
			else if (tokens[1] == "$PWD")
			{
				cout<<pwd<<endl;
			}
			else if (tokens[1] == "$HOME")
			{
				cout<<home<<endl;
			}
			else if (tokens[1] == "$PROMPT")
			{
				cout<<prompt<<endl;
			}
			else if (tokens[1] == "$PATH")
			{
				cout<<PATH<<endl;
			}
		}
		else if (tokens[0] == "pwd")
		{
			cout<<pwd<<endl;
		}
		else if (getFirstToken(cmd) == "history")	// print out history of commands entered.
		{
			printHistory(history);
		}	
		else if (cmd[0] == '/')	// absolute directory
		{
			if (accExec(cmd))	// directory exists and is executable
			{
				execute(cmd);	// then execute command
			}
			else 
			{
				perror("exec");	// print error message.
			}
		}
		else if (tokens[0] == "cd")	// change directory.
		{
			if (tokens.size() == 1)
			{
				cout<<pwd<<endl;
			}
			else
			{
				changeDir(tokens[1]);	// change directory.
			}
		}
		else if (tokens[0] == "ls")		// list directory contents.
		{
			runLS();				// run ls command.
		}
		else if (tokens[0] == "addpath")	// Modify $PATH variable.
		{
			if (tokens.size() == 2)			// make sure second parameter exists
			{
				if (access(tokens[1].c_str(), F_OK) == 0) // File or path exists
					PATH += ":" + tokens[1];
				else
					perror("access");
			}
			else
				cout<<err_msg.str()<<endl;
		}
		else if (cmd == "exit")	// exit shell program.
		{
			return 0;
		}
		/* For any other command, run it thru the SearchPath */
		else 		
		{ 
			searchPATH();
		}
		
	}while (true);
	return 0;
}	// main()

