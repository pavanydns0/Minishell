#include "main.h"

// Store child process PID
int pid;

// Array to store external commands
char *external_comands[154];

// Signal handler for SIGINT, SIGTSTP and SIGCHLD
void signal_handler(int signum)
{
    // Handle Ctrl+C signal
    if(signum==SIGINT)
    {
        // Redisplay prompt if shell is idle
        if(pid==0 && fg == 0)
        {
            char buf[100];
            getcwd(buf, sizeof(buf));
            printf("\n%s%s$ ",prompt_string,buf);
            fflush(stdout);
        }
    }

    // Handle Ctrl+Z signal
    else if(signum == SIGTSTP)
    {
        // Redisplay prompt if shell is idle
        if(pid==0 && fg == 0)
        {
            char buf[100];
            getcwd(buf, sizeof(buf));
            printf("\n%s%s$ ",prompt_string,buf);
            fflush(stdout);
        }
        else
        {
            // Store stopped process in job list
            insert_at_first();
        }
    }

    // Handle child process termination
    else if(signum == SIGCHLD)
    {
        waitpid(head->pid,&status,WNOHANG);
    }
}

// Insert stopped process at beginning of job list
void insert_at_first(void)
{
	// Allocate memory for new job node
	struct Slist *new_node=malloc(sizeof(struct Slist));

    // Return if memory allocation fails
    if(new_node==NULL)
    {
        return;
    }

	// Store process PID
	new_node->pid = pid;

    // Store command name
    strcpy(new_node->command,input_string);

    // Insert node at beginning of list
    new_node->next=head;
    head=new_node;

    return ;
}

// Display all stopped jobs
void print_list(void)
{
	struct Slist *temp=head;

	// Check if job list is empty
	if (temp == NULL)
	{
		printf("INFO : No jobs\n");
	}
    else
    {
	    // Traverse and print all jobs
	    while (temp)
	    {
		    printf("Stopped %s\n", temp -> command);
		    temp = temp -> next;
	    }
    }
}

// Delete first job from list
void delete_first(void)
{
    // Return if list is empty
    if(head==NULL)
    {
        return;
    }

    struct Slist *temp;

    // Store first node
    temp=head;

    // Move head to next node
    head=temp->next;

    // Free deleted node
    free(temp);

    return;
}

// Read user input and execute commands
void scan_input(char *prompt, char *input_string)
{
    //extract_external_commands(external_commnads);

    // Register SIGINT handler
    signal(SIGINT,signal_handler);

    // Register SIGTSTP handler
    signal(SIGTSTP,signal_handler);

    // Infinite shell loop
    while(1)
    {
        char buf[100];

        // Get current working directory
        getcwd(buf, sizeof(buf));

        // Display shell prompt
        printf("\n%s%s$ ", prompt, buf);

        // Read user input
        scanf(" %[^\n]",input_string);

        //printf("Input : %s\n", input_string);

        // Check for prompt change command
        if(strncmp(input_string, "PS1=", 4) == 0)
        {
            // Validate empty prompt
            if (strlen(input_string) == 4)
            {
                printf("Prompt cannot be empty.\n");
                exit(0);
            }

            // Validate space after '='
            if (input_string[4] == ' ')
            {
                printf("There should not be a space after '='\n");
                exit(0);
            }

            // Update shell prompt
            strcpy(prompt, input_string + 4);
        }
        else
        {
            // Extract command from input
            char *cmd = get_command(input_string);

            // printf("cmd = %s \n",cmd);

            // Identify command type
            int ret =check_command_type(cmd);

            //printf("ret = %d \n",ret);

            // Execute builtin command
            if(ret == BUILTIN)
            {
                excute_internal_commands(input_string);
            }

            // Execute external command
            else if(ret==EXTERNAL)
            {
                // Create child process
                pid = fork();

                // Parent process
                if(pid>0)
                {
                    // Wait for child process
                    waitpid(pid,&status, WUNTRACED);

                    // Store exit status
                    if(WIFEXITED(status))
                    {
                        last_status = WEXITSTATUS(status);
                    }

                    // Store signal termination status
                    else if(WIFSIGNALED(status))
                    {
                        last_status = 128 + WTERMSIG(status);
                    }

                    // Reset PID after child completion
                    pid=0;
                }

                // Child process
                else if(pid == 0)
                {
                    // Restore default Ctrl+C handling
                    signal(SIGINT,SIG_DFL);

                    // Restore default Ctrl+Z handling
                    signal(SIGTSTP,SIG_DFL);

                    // Execute external command
                    excute_external_commands(input_string);
                }
            }

            // Handle invalid command
            else if(ret == NO_COMMAND)
            {
                printf("Command %s not found \n",cmd);
                last_status=1;
            }
        }
    }
}