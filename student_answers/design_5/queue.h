typedef struct { 
  long type;                 /* must be of type long */ 
  char payload[200];  /* bytes in the message */
} queuedMessage;
