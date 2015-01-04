#include "plog.h"
#define IPADDRESS "127.0.0.1"
#define PORT 60070
#define BUFS	1024
using namespace std;
int sock;
struct sockaddr_in server;
struct hostent *hp;
char buf[BUFS];
bool socketConnect() {
	/* Create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("Opening stream socket");
		exit(1);
	}
	/* Connect socket using server name indicated in the command line */
	server.sin_family = AF_INET;
	hp = gethostbyname(IPADDRESS);
	if (hp == NULL) {
		perror("Trying to get host by name");
		exit(2);
	}
	memcpy((char *) &server.sin_addr, (char *) hp->h_addr, hp->h_length);
	server.sin_port = htons(PORT);
	if (connect(sock, (struct sockaddr *) &server, sizeof server) < 0) {
		perror("Connecting stream socket");
		exit(1);
	}
	// Send and receive data.
	printf("Connected\n");
	return true;
}
void send(char *s, int len) {
	if (write(sock, s, len) < 0)
		perror("Writing on stream socket");
}
int recieve(char *ans) {
	return read(sock, ans, 256);
}
void quit() {
	cout << "Asking prolog to quit" << endl;
	char buff[] = "quit.\n";
	send(buff, 6);
	char ans[128];
	recieve(ans);
}
