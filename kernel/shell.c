#include <kernel.h>

WINDOW shell_wnd = {0, 9, 61, 16, 0, 0, 0xDC};
WINDOW train_wnd = {0, 0, 80, 8, 0, 0, ' '};

typedef struct _input_buffer
{
  int length;
  char buffer[20];
} input_buffer;

input_buffer *input;

/*
compares strings
 */
int t_strcmp(char *c1, char *c2)
{
  int i;
  while(*c1 != '\0' && *c2 != '\0') 
  {
    i = *c1++ - *c2++;
    if (i != 0) return i;
  }
  return *c1 - *c2;
}

/*
clears the window
 */
void clear_cmd(WINDOW *wnd)
{
  clear_window(wnd);
}

void exec_sleep()
{
  int i;

  for (i=0; i < 10; i++)
    sleep(10);
    wprintf(&shell_wnd, ".\n");
  // wprintf(&shell_wnd, "Hi\n");
  // sleep(2000);
  // wprintf(&shell_wnd, "10 seconds later\n");
}

/*
prints help
 */
void print_help()
{
  wprintf(&shell_wnd, "List of commands supported:\n");
  wprintf(&shell_wnd, "ps         show list of processes\n");
  wprintf(&shell_wnd, "clear      clears the window buffer\n");
  wprintf(&shell_wnd, "train      launches the train process\n");
  wprintf(&shell_wnd, "help       this help\n");
}

/*
compares input and executes appropriate commands
 */
void run_command()
{
  char *cmd = &input->buffer[0];
  if (t_strcmp(cmd, "clear") == 0) {
    clear_window(&shell_wnd);
  } else if (t_strcmp(cmd, "help") == 0) {
    print_help();
  } else if (t_strcmp(cmd, "ps") == 0) {
    print_all_processes(&shell_wnd);
  } else if (t_strcmp(cmd, "train") == 0) {
    clear_cmd(&train_wnd);
    init_train(&train_wnd);
  } else if (t_strcmp(cmd, "sleep") == 0) {
    exec_sleep();
  } else {
    wprintf(&shell_wnd, "command not found. try help\n");
  }  
}

/*
reads from the input buffer
 */
void read_input()
{
  char ch;
  Keyb_Message msg;
  input->length = 0;
  int i;
  do
  {
    msg.key_buffer = &ch;
    send(keyb_port, &msg);
    switch (ch) {
      case 13:
        wprintf(&shell_wnd, "\n");
        break;
      default:
        input->buffer[input->length++] = ch;
        wprintf(&shell_wnd, "%c", ch);
        break;
    }
  } while (ch != '\n' && ch != '\r' && input->length < 20);
  input->buffer[input->length] = '\0';
}

/*
removes the newline from the input string
 */
void clean_buffer()
{
  input->length = 0;
  while(input->buffer[input->length++] != '\0')
    if (input->buffer[input->length] == '\n')
      input->buffer[input->length] = '\0';
}

/*
main shell process function
 */
void shell_process (PROCESS self, PARAM param)
{
  clear_window(&shell_wnd);
  wprintf(&shell_wnd, "This is not an egg shell\n");

  while (1) {
    wprintf(&shell_wnd, "$ ");
    read_input();
    clean_buffer();
    run_command();

  }
}

/*
initializes the shell process
 */
void init_shell()
{
  create_process(shell_process, 5, 0, "Shell Process");
  init_train_settings();
  resign();
}
