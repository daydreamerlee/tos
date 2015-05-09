#include <kernel.h>

WINDOW shell_wnd = {0, 9, 50, 16, 0, 0, 0xDC};
WINDOW train_wnd = {0, 0, 60, 8, 0, 0, ' '};

// Struct datetype
typedef struct _input_buffer
{
  int length;
  char buffer[20];
} input_buffer;

input_buffer *input;


/**
 * Compares two strings
 * @param  c1
 * @param  c2 
 * @return
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

/**
 * Removes spaces from the input buffer
 * @param  str 
 * @return string without spaces
 */
char * deblank(char *str)
{
  char *out = str, *put = str;
  for(; *str != '\0'; ++str)
  {
    if(*str != ' ')
      *put++ = *str;
  }
  *put = '\0';

  return out;
}

/**
 * Removes newline from the input buffer
 * @param  str 
 * @return string without newline
 */
char * clean_buffer(char *str)
{
  char *out = str, *put = str;
  for(; *str != '\0'; ++str)
  {
    if(*str != '\n')
      *put++ = *str;
  }
  *put = '\0';

  return out;
}


/**
 * Clear a window buffer
 * @param wnd
 */
void clear_cmd(WINDOW *wnd)
{
  clear_window(wnd);
}

/**
 * Print help
 */
void print_help()
{
  wprintf(&shell_wnd, "List of commands supported:\n");
  wprintf(&shell_wnd, "ps               show list of processes\n");
  wprintf(&shell_wnd, "clear            clears the window buffer\n");
  wprintf(&shell_wnd, "train            launches the train process\n");
  wprintf(&shell_wnd, "train go         starts the train\n");
  wprintf(&shell_wnd, "train stop       stops the train\n");
  wprintf(&shell_wnd, "train reverse    reverses the direction of train\n");
  wprintf(&shell_wnd, "help             this help\n");
}

/**
 * Run a command based on input
 * @param cmd 
 */
void run_command(char * cmd)
{
  if (t_strcmp(cmd, "clear") == 0) {
    clear_window(&shell_wnd);
  } else if (t_strcmp(cmd, "help") == 0) {
    print_help();
  } else if (t_strcmp(cmd, "ps") == 0) {
    print_all_processes(&shell_wnd);
  } else if (t_strcmp(cmd, "train") == 0) {
    init_train(&train_wnd);
  } else if (t_strcmp(cmd, "traingo") == 0) {
    start_train();
  } else if (t_strcmp(cmd, "trainstop") == 0) {
    stop_train();
  } else if (t_strcmp(cmd, "trainreverse") == 0) {
    reverse_train();
  } else {
    wprintf(&shell_wnd, "command not found. try help\n");
  }  
}

/**
 * Read input from the keystrokes
 */
void read_input()
{
  char ch;
  Keyb_Message msg;
  input->length = 0;
  do
  {
    msg.key_buffer = &ch;
    send(keyb_port, &msg);
    switch (ch) {
      case '\b':
        if (input->length == 0)
          continue;
        input->length--;
        wprintf(&shell_wnd, "%c", ch);
        break;
      case 13:
        wprintf(&shell_wnd, "\n");
        break;
      default:
        input->buffer[input->length++] = ch;
        wprintf(&shell_wnd, "%c", ch);
        break;
    }
  } while (ch != '\n' && ch != '\r');
  input->buffer[input->length] = '\0';
}

/**
 * Main shell process
 * @param self 
 * @param param 
 */
void shell_process (PROCESS self, PARAM param)
{
  clear_window(&shell_wnd);
  wprintf(&shell_wnd, "This is ViSh: where Vi and Shell don't meet\n");

  while (1) {
    wprintf(&shell_wnd, "$ ");
    read_input();
    char *precmd;
    precmd = clean_buffer(&input->buffer[0]);
    char *cmd;
    cmd = deblank(precmd);
    run_command(cmd);
  }
}

/**
 * Initialize the shell process
 */
void init_shell()
{
  create_process(shell_process, 5, 0, "Shell Process");
  clear_cmd(&train_wnd);  
  init_train_settings();
  resign();
}
