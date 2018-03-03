#pragma warning( disable : 4326 )
#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma  comment (lib, "ws2_32.lib")
void main(){
	// Initialize winsock
	WSADATA WinSockData;
	WORD Version = MAKEWORD(2, 2);
	int WinSockResult = WSAStartup(Version, &WinSockData);
	if (WinSockResult != 0) {
		std::cerr << "Can't Initialize winsock! Quitting\n";
		return;
	}
	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET){
		std::cerr << "Can't create a socket! Quitting\n";
		return;
	}
	// Bind the ip adress and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(listening, (sockaddr*)&hint, sizeof(hint));
	// Tell winsock the socket is for listening
	listen(listening, SOMAXCONN);
	// wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);
	SOCKET ClientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (ClientSocket== INVALID_SOCKET){
		std::cerr << "Socket is invalid! Quitting\n";
		return;
	}
	char host[NI_MAXHOST];    //Client's remote name
	char service[NI_MAXHOST]; // Service (i.e port) the client is connected on
	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);
	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXHOST, 0) == 0) {
		std::cout << host << " connected on port " << service << '\n';
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " << ntohs(client.sin_port) << '\n';
	}
	// close listening socket
	closesocket(listening);
	// While loop: accept and echo message back to client
	char buf[4096];
	while (true)
	{
		ZeroMemory(buf, 4096);
		// Wait for client to send data
		int bytesReceived = recv(ClientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR){
			std::cerr << "Error in recv(). Quitting" << '\n';
			break;
		}
		if (bytesReceived == 0){
			std::cout << "Client disconnected\n";
			break;
		}
		// Output of Clientmessage
		std::cout << std::string(buf, 0, bytesReceived) << '\n';
		// Echo message back to client
		send(ClientSocket, buf, bytesReceived + 1, 0);
		
	}
	// Close the sock
	closesocket(ClientSocket);
	// Cleanup winsock
	WSACleanup();
	system("PAUSE");
}