/* List of builtin commands */
#include <stddef.h>
#include "main.h"

// Array of supported builtin shell commands
char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
						"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
						"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help","jobs","fg","bg"};

// Extract the command name from the input string
char *get_command(char *input_string)
{
	int k=0;
	int i=0;

	// Clear the command buffer
	for(int i=0;i<50;i++)
	{
		buf[i]=0;
	}

	// Copy characters until space or end of string
	while(input_string[i] != ' ' && input_string[i] != '\0')
	{
		buf[k++] = input_string[i];
		i++;
	}

	// Append null terminator
	buf[k] = '\0';

	// Return extracted command
	return buf;

}

// Check whether command is builtin, external or invalid
int check_command_type(char *str)
{

	// Calculate total number of builtin commands
	int total_elements = sizeof(builtins) / sizeof(builtins[0]);

	// Compare with builtin command list
	for(int i=0;i<total_elements;i++)
	{
		if(strcmp(str,builtins[i]) == 0)
		{
			return BUILTIN;
		}
	}

	// Compare with external command list
	for(int i=0; external_commands[i] != NULL;i++)
	{
		if(strcmp(str,external_commands[i])==0)
		{
			//printf("hi");
			return EXTERNAL;
		}
	}

	// Return if command is not found
	return NO_COMMAND;

}

// Read external commands from file and store them
void extract_external_commands()
{

	// Open external command file
	int fd = open("external.txt",O_RDONLY);

	char buf[50];
	int i=0;
	char ch;
	int cmd=0;

	// Read file character by character
	while(read(fd,&ch,1)>0)
	{

		// Ignore carriage return
		if (ch == '\r')
		{
    		continue;
		}

		// Store command when newline is encountered
		if(ch=='\n'){
			buf[i]='\0';
			external_commands[cmd]=malloc(strlen(buf)+1);
			strcpy(external_commands[cmd],buf);
			cmd++;
			i=0;
		}
		else{

			// Store characters of command
			buf[i++] = ch;
		}

	}

	// Store last command if file doesn't end with newline
	if(i>0)
	{
		buf[i]='\0';
		external_commands[cmd] =malloc(strlen(buf)+1);
		strcpy(external_commands[cmd],buf);
		cmd++;
	}

	// Mark end of command list
	external_commands[cmd] = NULL;

	// Close the file
	close(fd);
}

// Execute builtin shell commands
void excute_internal_commands(char *input_string)
{

	char buf[1024];

	// Handle cd command
	if(strncmp(input_string,"cd",2)==0)
	{

		// Check if directory is provided
		if (strlen(input_string) <= 3)
        {
            printf("Usage: cd <directory>\n");
        }
		else
        {
			// Change current working directory
            if (chdir(input_string + 3) == -1)
            {
                perror("chdir");
				last_status=1;
            }
			else
			{
				last_status=0;
			}
        }
	}

	// Handle pwd command
	else if(strncmp(input_string,"pwd",3)==0)
	{

		// Print current working directory
		if (getcwd(buf, sizeof(buf)) != NULL)
        {
            printf("%s\n", buf);
			last_status=0;
        }
        else
        {
            perror("getcwd");
			last_status=1;
        }
	}

	// Handle exit command
	else if(strncmp(input_string,"exit",4)==0)
	{
		exit(0);
	}

	// Handle echo $$ command
	else if(strcmp(input_string,"echo $$")==0)
	{
		printf("%d\n",getpid());
		last_status=0;
	}

	// Handle echo $? command
	else if(strcmp(input_string,"echo $?")==0)
	{
		printf("%d\n", last_status);
	}

	// Handle echo $SHELL command
	else if(strcmp(input_string,"echo $SHELL")==0)
	{

		// Get SHELL environment variable
		char *shell = getenv("SHELL");

		if(shell)
		{
			printf("%s\n",shell);
			last_status=0;
		}
		else
		{
			last_status=1;
		}
	}

	// Handle jobs command
	else if(strcmp(input_string,"jobs")==0)
	{
		print_list();
		last_status=0;
	}

	// Handle fg command
	else if(strcmp(input_string,"fg")==0)
	{

		// Resume first stopped process in foreground
		if(head !=NULL)
		{
			kill(head->pid,SIGCONT);
			fg=1;
			waitpid(head->pid,&status,WUNTRACED);
			//fg =0;

			delete_first();
			last_status=0;
		}
		else if (head==NULL)
		{
			printf("INFO : No stopped process \n");
		}
	}

	// Handle bg command
	else if(strcmp(input_string,"bg")==0)
	{

		// Resume first stopped process in background
		kill(head->pid,SIGCONT);
		signal(SIGCHLD,signal_handler);
		delete_first();
		last_status=0;
	}
}

// Execute external commands
void excute_external_commands(char *input_string)
{

	char buf[50];
	char *arr[100];
	int cmd=0;
	int k=0;
	int i=0;

	// Split input string into command arguments
	for(int i=0;input_string[i] != '\0';i++)
	{

		// Store completed argument
		if(input_string[i] == ' ')
		{
			if(k>0){
				buf[k] = '\0';
				arr[cmd] = malloc(strlen(buf)+1);
				strcpy(arr[cmd],buf);
				cmd++;
				k=0;
			}
		}
		else
		{

			// Collect argument characters
			buf[k++] = input_string[i];
		}
	}

	// Store last argument
	if (k > 0)
	{
    	buf[k] = '\0';
    	arr[cmd] = malloc(strlen(buf) + 1);
    	strcpy(arr[cmd], buf);
    	cmd++;
	}

	// Mark end of argument list
	arr[cmd]=NULL;

	// Execute normal external command
	if(strchr(input_string, '|') == NULL)
	{
		execvp(arr[0], arr);
    	perror("execvp");
    	exit(1);
	}
	else
	{

		// Store starting index of each piped command
		int cmd_pos[20];
    	int count = 1;
    	int k = 1;
    	cmd_pos[0] = 0;

		// Separate commands using pipe symbol
    	for (int i = 0; i < cmd; i++)
    	{
        	if (arr[i] != NULL && strcmp(arr[i], "|") == 0)
        	{
            	arr[i] = NULL;

            	if (arr[i + 1] != NULL)
            	{
                	cmd_pos[k++] = i + 1;
                	count++;
            	}
            	else
           		{
                	printf("Invalid command\n");
                	return;
            	}
        	}
    	}

		// Initialize input file descriptor
    	int fd_in = 0;

		// Execute each command in pipeline
    	for (int i = 0; i < count; i++)
    	{
        	int fd[2];

			// Create pipe except for last command
        	if (i < count - 1)
       	 	{
            	if (pipe(fd) == -1)
            	{
                	perror("pipe");
                	exit(1);
           	 	}
        	}

			// Create child process
        	int pid = fork();

			// Child process execution
        	if (pid == 0)
        	{

				// Redirect input if required
            	if (fd_in != 0)
            	{
                	dup2(fd_in, 0);
                	close(fd_in);
            	}

				// Redirect output if not last command
            	if (i < count - 1)
            	{
                	close(fd[0]);
                	dup2(fd[1], 1);
                	close(fd[1]);
            	}

				// Execute pipeline command
            	execvp(arr[cmd_pos[i]], &arr[cmd_pos[i]]);
            	perror("execvp");
            	exit(1);
        	}

		// Close previous input descriptor
        	if (fd_in != 0)
            	close(fd_in);

		// Prepare next command input
        	if (i < count - 1)
        	{
            	close(fd[1]);
            	fd_in = fd[0];
        	}
    	}

		// Wait for all child processes to complete
    	while (wait(NULL) > 0);

		// Exit after pipeline execution
    	exit(0);
	}
}