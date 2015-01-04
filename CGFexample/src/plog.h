#ifndef SRC_SOCKT_H_
#define SRC_SOCKT_H_
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <string.h>	// for memcpy()...
#include <stdlib.h>	// for atoi()...
#include <unistd.h>	// for gethostname()...
#include <ctype.h>	// for tolower()...
#include <iostream>
bool socketConnect();
void send(char *s, int len);
int recieve(char *s);
void quit();
#endif /* SRC_SOCKT_H_ */
