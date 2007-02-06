#include <stdio.h>
#include <stdlib.h>       // for exit,atoi
#include <unistd.h>
#include "debug.h"
#include "phone_book.h"
#include "init.h"

void print_help(unsigned char* name) {
  fprintf(stderr, "Usage: %s <parameters>\n",name);
  fprintf(stderr, "  -p   port to listen on (defaults to 6400)\n");
  fprintf(stderr, "  -t   trace flags: (can be combined)\n");
  fprintf(stderr, "       's' = modem input\n");
  fprintf(stderr, "       'S' = modem output\n");
  fprintf(stderr, "       'i' = IP input\n");
  fprintf(stderr, "       'I' = IP input\n");
  fprintf(stderr, "  -l   0 (NONE), 1 (FATAL) - 7 (DEBUG_X) (defaults to 0)\n");
  fprintf(stderr, "  -L   log file (defaults to stderr)\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  The following can be repeated for each modem desired\n");
  fprintf(stderr, "  (-s, -S, and -i will apply to any subsequent device if not set again)\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  -d   serial device (defaults to /dev/ttyS0)\n");
  fprintf(stderr, "  -s   serial port speed (defaults to 38400)\n");
  fprintf(stderr, "  -S   speed modem will report (defaults to -s value)\n");
  fprintf(stderr, "  -D   invert DCD pin\n");
  fprintf(stderr, "  -n   add phone entry (number=replacement)\n");
  fprintf(stderr, "  -a   filename to send to local side upon answer\n");
  fprintf(stderr, "  -A   filename to send to remote side upon answer\n");
  fprintf(stderr, "  -c   filename to send to local side upon connect\n");
  fprintf(stderr, "  -C   filename to send to remote side upon connect\n");
  fprintf(stderr, "  -I   filename to send when no answer\n");
  fprintf(stderr, "  -B   filename to send when modem(s) busy\n");
  fprintf(stderr, "  -T   filename to send upon inactivity timeout\n");
  fprintf(stderr, "  -i   modem init string (defaults to '', leave off 'at' prefix when specifying)\n");
  exit(1);
}

int init(int argc, 
         char** argv, 
         modem_config cfg[],
         int max_modem, 
         int* port,
         unsigned char* all_busy,
         int all_busy_len
         ) {
  int i=0;
  int j=0;
  int opt=0;
  int trace_flags=0;
  unsigned char* tok;
  unsigned char* tty=NULL;
  int dce_set=FALSE;

  LOG_ENTER();
  *port=6400;
  mdm_init_config(&cfg[0]);
  cfg[0].dte_speed=38400;
  cfg[0].dce_speed=38400;
  strncpy(cfg[0].dce_data.tty,"/dev/ttyS0",sizeof(cfg[0].dce_data.tty));

  while(opt>-1 && i < max_modem) {
    opt=getopt(argc,argv,"p:s:S:d:hw:i:Dl:L:t:n:a:A:c:C:I:B:T:");
    switch(opt) {
      case 't':
        trace_flags=log_get_trace_flags();
        for(j=0;j<strlen(optarg);j++) {
          switch(optarg[j]) {
            case 's':
              trace_flags|=TRACE_MODEM_IN;
              break;
            case 'S':
              trace_flags|=TRACE_MODEM_OUT;
              break;
            case 'i':
              trace_flags|=TRACE_IP_IN;
              break;
            case 'I':
              trace_flags|=TRACE_IP_OUT;
              break;
          }
          log_set_trace_flags(trace_flags);
        }
        break;
      case 'a':
        strncpy(cfg[i].data.local_answer,optarg,sizeof(cfg[i].data.local_answer));
        break;
      case 'A':
        strncpy(cfg[i].data.remote_answer,optarg,sizeof(cfg[i].data.remote_answer));
        break;
      case 'c':
        strncpy(cfg[i].data.local_connect,optarg,sizeof(cfg[i].data.local_connect));
        break;
      case 'C':
        strncpy(cfg[i].data.remote_connect,optarg,sizeof(cfg[i].data.remote_connect));
        break;
      case 'B':
        strncpy(all_busy,optarg,all_busy_len);
        break;
      case 'I':
        strncpy(cfg[i].data.no_answer,optarg,sizeof(cfg[i].data.no_answer));
        break;
      case 'T':
        strncpy(cfg[i].data.inactive,optarg,sizeof(cfg[i].data.inactive));
        break;
      case 'i':
        strncpy(cfg[i].config0,optarg,255);
        break;
      case 'D':
        cfg[i].invert_dcd=TRUE;
        break;
      case 'p':
        *port = (atoi(optarg));
        break;
      case 'n':
        tok=strtok(optarg,"=");
        pb_add(tok,strtok(NULL,"="));
        break;
      case 'l':
        log_set_level(atoi(optarg));
        break;
      case 'L':
        log_set_file(fopen(optarg,"w+"));
        // should check to see if an error occurred...
        break;
      case 's':
        cfg[i].dte_speed=atoi(optarg);
        LOG(LOG_ALL,"Setting DTE speed to %d",cfg[i].dte_speed);
        if(dce_set==FALSE)
          cfg[i].dce_speed=cfg[i].dte_speed;
        break;
      case '?':
      case 'h':
        print_help(argv[0]);
        break;
      case 'd':
        if(tty != NULL) {
          strncpy(cfg[i].dce_data.tty,tty,sizeof(cfg[i].dce_data.tty));
          i++;
          dce_set=FALSE;
          mdm_init_config(&cfg[i]);
          cfg[i].dte_speed=cfg[i-1].dte_speed;
          cfg[i].dce_speed=cfg[i-1].dce_speed;
          strncpy(cfg[i].config0,cfg[i-1].config0,sizeof(cfg[i].config0));
          strncpy(cfg[i].data.local_connect,cfg[i-1].data.local_connect,sizeof(cfg[i].data.local_connect));
          strncpy(cfg[i].data.remote_connect,cfg[i-1].data.remote_connect,sizeof(cfg[i].data.remote_connect));
          strncpy(cfg[i].data.local_answer,cfg[i-1].data.local_answer,sizeof(cfg[i].data.local_answer));
          strncpy(cfg[i].data.remote_answer,cfg[i-1].data.remote_answer,sizeof(cfg[i].data.remote_answer));
          strncpy(cfg[i].data.no_answer,cfg[i-1].data.no_answer,sizeof(cfg[i].data.no_answer));
          strncpy(cfg[i].data.inactive,cfg[i-1].data.inactive,sizeof(cfg[i].data.inactive));
        }
        tty=optarg;
        break;
      case 'S':
        cfg[i].dce_speed=atoi(optarg);
        dce_set=TRUE;
        break;
    }
  }

  if(i<max_modem) {
    if(tty != NULL)
      strncpy(cfg[i].dce_data.tty,tty,sizeof(cfg[i].dce_data.tty));
    i++;
  }

  LOG(LOG_DEBUG,"Read configuration for %i serial port(s)",i);

  LOG_EXIT();
  return i;
}


