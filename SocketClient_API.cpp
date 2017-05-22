#include "SocketClient_API.h"


//---------------------------------------------------------------------------
/** constructor*/
TSocketClient_API::TSocketClient_API()
{
}
//---------------------------------------------------------------------------
/**destructor.
shutdown the connection before destruction
*/
TSocketClient_API::~TSocketClient_API()
{
shutdown (fdsock, 2);
//int sock_err = closesocket (fdsock);
closesocket (fdsock);
fdsock = INVALID_SOCKET;
/*
if (sock_err == SOCKET_ERROR)
	{
	perror("Error: closesocket()");
	return 1;
	}
*/
//Si Windows
#if defined (WIN32)
WSACleanup ();
#endif
}
//---------------------------------------------------------------------------
/**Connection configuration.
This function has to be called first, and especially before attempting a connection.
@param IP_address IP address in a string format
@param port port to open for communication
*/
bool TSocketClient_API::ConfigureConnection(string IP_address,int port)
{
#if defined (WIN32)
/* Initialiser WSAStartup() */
WSADATA wsa_data;
if ( WSAStartup (MAKEWORD (2, 2), &wsa_data) )
	{
        StatusMessage="Error: init WSAStartup()";
	return false;
	}
else
	StatusMessage="WIN: winsock2: OK";
#endif
ip_server= IP_address;
SERVER_PORT_CONNEXION=port;
return true;
}
//---------------------------------------------------------------------------
/**Establish Connection.
Open the connection with the socket server. 
*/
bool TSocketClient_API::ConnectToServer()
{
/* Creation du socket: */
if ( ( fdsock = socket (AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET )
	{
        StatusMessage="Erreur creation socket";
	return false;
	}

StatusMessage="socket %d est maintenant ouvert en mode TCP/IP";

sin.sin_addr.s_addr = inet_addr(ip_server.c_str());
sin.sin_port = htons (SERVER_PORT_CONNEXION);
sin.sin_family = AF_INET;

if ( connect(fdsock, (sockaddr *)&sin, sizeof sin) == SOCKET_ERROR )
	{
    StatusMessage="Erreur connection socket";
	return false;
	}
return true;
}
//---------------------------------------------------------------------------
/**Send a data packet.
This is a blocking call function. The number of data defined by length is transfered.
@param data pointer to the data buffer
@param length number of byte to transfer
*/
bool TSocketClient_API::SendData (char *data , int length)
{
int data_sent=0;
//int n;
int n=0;
int error;


//cout<<" length : "<<length;//HIROKI

while (data_sent < length)
	{
	//cout<<" data_sent : "<<data_sent;//HIROKI
	//usleep(100)	;//HIROKI
	n = send (fdsock, data + data_sent, length - data_sent, 0);
	//cout<<" n : "<<n<<endl;//HIROKI

	if (n >= 0)
		data_sent += n;
	else
		{
		#if defined (WIN32)
			error=WSAGetLastError();
		#else
			//error=getErrno();
		#endif
		stringstream out;
		out << error;

		StatusMessage="Erreur send TCP: " + out.str();
		return false;
		}
	}

return true;
}
//---------------------------------------------------------------------------
/**Receive a data packet.
This is a blocking call function. The function returns when the specified number of data
 defined by length is received.
@param data pointer to the data buffer
@param length number of byte to transfer
*/
bool TSocketClient_API::ReceiveData (char *data , int length)
{
int data_received=0;
//int pipo;
int n;
int error;
while (data_received < length)
	{
        //pipo=length - data_received;
	n = recv (fdsock, data + data_received, length - data_received, 0);

	if (n >= 0)
		data_received += n;
	else
		{
		#if defined (WIN32)
			error=WSAGetLastError();
		#else
			//error=getErrno();
		#endif
		stringstream out;
		out << error;

		StatusMessage="Erreur receive TCP: " + out.str();
		return false;
		}
	}

return true;
}
//---------------------------------------------------------------------------
/**Get status message.
@return the last error message
*/
string TSocketClient_API::GetStatus ()
{
return StatusMessage;
}
//---------------------------------------------------------------------------
bool TSocketClient_API::SetSocketTimeout(int Value_ms)
{
int result;

result=setsockopt(fdsock,SOL_SOCKET,SO_SNDTIMEO,(char *)&Value_ms, sizeof(Value_ms));
if (result==0)
	{
	StatusMessage="Echec sur positionnement timeout d'emission";
	return false;
	}
result=setsockopt(fdsock,SOL_SOCKET,SO_RCVTIMEO,(char *)&Value_ms, sizeof(Value_ms));
if (result==0)
	{
	StatusMessage="Echec sur positionnement timeout de reception";
	return false;
	}
return true;
}
//---------------------------------------------------------------------------
/**Close the socket.
*/
bool TSocketClient_API::Close()
{
if (closesocket(fdsock))
        return false;
else
        return true;

}
