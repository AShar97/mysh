#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

char * home_address;

int mysh_execute(char **args);
char **mysh_split_line(char *line);

int mysh_cd(char **args);
int mysh_help(char **args);
int mysh_exit(char **args);
int mysh_history(char **args);
int mysh_issue(char **args);
int mysh_rmexcept(char **args);
int mysh_timerun(char **args);

char *builtin_str[] = {
	"cd",
	"help",
	"exit",
	"history",
	"issue",
	"rmexcept",
	"timerun"
};

int (*builtin_func[]) (char **) = {
	&mysh_cd,
	&mysh_help,
	&mysh_exit,
	&mysh_history,
	&mysh_issue,
	&mysh_rmexcept,
	&mysh_timerun
};

int mysh_num_builtins(void) {
	return sizeof(builtin_str) / sizeof(char *);
}

int mysh_cd(char **args)
{
	if (args[1] == NULL) {
		fprintf(stderr, "mysh: expected argument to \"cd\"\n");
	} else {
		if (chdir(args[1]) != 0) {
			perror("mysh");
		}
	}
	return 1;
}

int mysh_help(char **args)
{
	int i;
	printf("MYSH\n");
	printf("Type program names and arguments, and hit enter.\n");
	printf("The following are built in:\n");

	for (i = 0; i < mysh_num_builtins(); i++) {
		printf("  %s\n", builtin_str[i]);
	}

	printf("Use the man command for information on other programs.\n");
	return 1;
}

int mysh_exit(char **args)
{
	return 0;
}

void seek_line(FILE *fp){
	long pos = ftell(fp);
	char c = '\0';
	while(pos>0 && c!='\n'){
		fseek(fp, -1L, SEEK_CUR);
		c = fgetc(fp);
		fseek(fp, -1L, SEEK_CUR);
		pos = ftell(fp);
	}
}

void print_line(FILE *fp){
	char *line_temp = (char *) malloc(1024 * sizeof(char));
	//fscanf(fp, "%[^\n]s", line_temp);
	fgets(line_temp, 1024, fp);
	printf("%s", line_temp);
	free(line_temp);
}

int mysh_history(char **args)	//latest first order
{
	int n = atoi(args[1]);
	
	FILE *fp = fopen(home_address,"a+");
	
	fseek(fp, -1L, SEEK_END);	seek_line(fp);
	long pos = ftell(fp);
	int i = 0;
	
	while((i < n) && (pos > 0)){
		seek_line(fp);	fseek(fp, 1L, SEEK_CUR);
		print_line(fp);	fseek(fp, -1L, SEEK_CUR);	i++;
		seek_line(fp);
		pos = ftell(fp);
	}
	
	fclose(fp);

	return 1;
}
/*
int mysh_history(char **args)	//oldest first order
{
	int n = atoi(args[1]);
	
	FILE *fp = fopen(home_address,"a+");
	
	fseek(fp, -1L, SEEK_END);
	long pos = ftell(fp);
	int i = 0; char c;
	
	while((i <= n) && (pos > 0)){
		fseek(fp, -1L, SEEK_CUR);
		c = fgetc(fp);
		if (c == '\n'){
			i++;
		}
		fseek(fp, -1L, SEEK_CUR);
		pos = ftell(fp);
	}
	
	fseek(fp, 1L, SEEK_CUR);

	i = 0;
	while(i < n){
		print_line(fp);
		i++;
	}

	fclose(fp);

	return 1;
}
*/
int mysh_issue(char **args)
{
	int n = atoi(args[1]);
	
	FILE *fp = fopen(home_address,"a+");
	
	fseek(fp, -1L, SEEK_END);
	long pos = ftell(fp);
	int i = 0; char c;
	
	while((i <= n) && (pos > 0)){
		fseek(fp, -1L, SEEK_CUR);
		c = fgetc(fp);
		if (c == '\n'){
			i++;
		}
		fseek(fp, -1L, SEEK_CUR);
		pos = ftell(fp);
	}
	
	fseek(fp, 1L, SEEK_CUR);

	char *line_temp = (char *) malloc(1024 * sizeof(char));
	//fscanf(fp, "%[^\n]s", line_temp);
	fgets (line_temp, 1024, fp);
	char **args_temp = mysh_split_line(line_temp);
	int status = mysh_execute(args_temp);

	fclose(fp);

	return status;
}

int mysh_rmexcept(char **args)
{
	int status = 1;
		
	FILE *proc = popen("ls","r");
	char buf[100000];
	char **files;
	char *tkn[3];
	tkn[0]="rm";
	tkn[1]="-r";
	int i=0,j;
	while (!feof(proc))
	{
		buf[i]=fgetc(proc);
		i++;
	}
	files=mysh_split_line(buf);
	
	for(i=0;files[i]!=NULL;i++)
	{
		for(j=1;args[j]!=NULL;j++)
		{
			if(strcmp(files[i],args[j])==0)
				break;
		}
			if(args[j]!=NULL)
				continue;
			else
			{
				tkn[2]=files[i];
				status=mysh_execute(tkn);
			}

	}

	return status;
}

pid_t child_pid;

void kill_child(int sig)
{
	kill(child_pid,SIGKILL);
}

int mysh_timerun(char **args)
{
	int m = atoi(args[1]);
	pid_t wpid;

	int status;

	signal(SIGALRM,(void (*)(int))kill_child);
	child_pid = fork();

	if (child_pid == 0) 
	{
		// Child process
		if (execvp(args[2], args) == -1)// this is executing args[0] command with list of parameters in args. 
		{
			perror("lsh");
		}
		
		exit(EXIT_FAILURE);
	}
	
	else if (child_pid < 0) 
	{
		// Error forking
		perror("lsh");
	} 

	else 
	{
		alarm(m);
		// Parent process
		do 
		{
			wpid = waitpid(child_pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	
	return 1;
}

int mysh_launch(char **args)
{
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0) 
	{
		// Child process
		if (execvp(args[0], args) == -1)// this is executing args[0] command with list of parameters in args. 
		{
			perror("mysh");
		}
		
		exit(EXIT_FAILURE);
	} 
	
	else if (pid < 0) 
	{
		// Error forking
		perror("mysh");
	} 
	
	else 
	{
		// Parent process
		do 
		{
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

int mysh_execute(char **args)
{
	int i;

	if (args[0] == NULL) 
	{
		// An empty command was entered.
		return 1;
	}

	for (i = 0; i < mysh_num_builtins(); i++) 
	{
		if (strcmp(args[0], builtin_str[i]) == 0) 
		{
			return (*builtin_func[i])(args);
		}
	}

	return mysh_launch(args);
}

#define MYSH_RL_BUFSIZE 1024

char *mysh_read_line(void)
{
	int bufsize = MYSH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;

	if (!buffer) {
		fprintf(stderr, "mysh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		// Read a character
		c = getchar();

		// If we hit EOF, replace it with a null character and return.
		if (c == EOF || c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		}
		position++;

		// If we have exceeded the buffer, reallocate.
		if (position >= bufsize) {
			bufsize += MYSH_RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer) {
				fprintf(stderr, "mysh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

#define MYSH_TOK_BUFSIZE 64
#define MYSH_TOK_DELIM " \t\r\n\a"

char **mysh_split_line(char *line)
{
	int bufsize = MYSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if (!tokens) {
		fprintf(stderr, "mysh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, MYSH_TOK_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += MYSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "mysh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, MYSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

void write_history(char *line){
	FILE *fp = fopen(home_address,"a+");
	fprintf(fp, "%s\n", line);
	fclose(fp);
}

void mysh_userinterface(void)
{

	char *line;
	char **args;
	int status;

	do {
		printf("> ");
		line = mysh_read_line();
		write_history(line);
		args = mysh_split_line(line);
		status = mysh_execute(args);
		//write_history(line);
		free(line);
		free(args);
	} while (status);
}


int main(int argc, char **argv)
{
	// Load address of ~/mysh_history.txt in home_address
	char *address = (char *) malloc((strlen(getenv("HOME") + 20) * sizeof(char)));
	strcpy(address, getenv("HOME"));
	strcat(address, "/mysh_history.txt");
	home_address = address;

	//write_history("\n");

	// Load config files, if any.

	// Run command loop.
	printf("MYSH : A simple shell by Yugansh, Ayush & Ojasvin. Period.\n");
	mysh_userinterface();

	// Perform any shutdown/cleanup.

	return EXIT_SUCCESS;
}
