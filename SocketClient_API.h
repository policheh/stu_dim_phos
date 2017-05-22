#ifndef TSocketClient_APIH
#define TSocketClient_APIH

#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <string>
#include <sstream>

using namespace std ;
/*==================[ Si Windows ]================*/
#if defined (WIN32)
#include <winsock2.h>
/*================[ Si Gnu/Linux ]================*/
#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include <errno.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close /*  Utiliser close() sous  linux */

#endif

/*****************************************************************************/
/** client Socket API.
This class provides a simple interface for accessing TCP socket.
This software can be compiled under linux or windows


*/
class TSocketClient_API
{
private:
	int fdsock;
	sockaddr_in sin;
	string ip_server;
	int SERVER_PORT_CONNEXION;
    string StatusMessage;
public :
    string GetStatus();
	bool ConfigureConnection(string IP_address,int port);
	bool ConnectToServer ();
	bool SendData (char *data , int length);
	bool ReceiveData (char *data , int length);
	bool SetSocketTimeout(int Value_ms);
	bool Close ();

    TSocketClient_API();         // constructeur
    virtual ~TSocketClient_API();         // destructeur
 };
#endif
