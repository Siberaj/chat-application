#include<iostream>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>

using namespace std;

#pragma comment(lib,"ws2_32.lib")
/*
  // initialize winsock library
  // create the socket
  // get ip and port
  // bind the ip/port with the socket
  // accept
  // recv and send
  // close socket
*/
bool Initialize()
{
    WSADATA  data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;

}


void Interactwithclient(SOCKET clientSocket, vector<SOCKET>& clients)
{
    //send/recv
    cout << "client connected" << endl;
    char buffer[4096];
    while (1) {
        int bytesrecv = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesrecv <= 0)
        {
            cout << "client disconnected" << endl;
            break;
        }
        string message(buffer, bytesrecv);
        cout << "message from client : " << message << endl;

        for (auto client : clients)
        {
            if (client != clientSocket)
            {
                send(client, message.c_str(), message.length(), 0);
            }
        }
    }
    auto it = find(clients.begin(), clients.end(), clientSocket);
    if (it != clients.end())
    {
        clients.erase(it);
    }

    closesocket(clientSocket);

}

int main()
{
    if (!Initialize())
    {
        cout << " winsock initialize failed " << endl;
        return 1;
    }

    cout << "server program" << endl;

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (listenSocket == INVALID_SOCKET)
    {
        cout << "socket creation failed" << endl;
        return 1;
    }

    // create address structure
    int port = 12345;
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);

    //convert the ipaddress(0.0.0.0) put it inside the sin_family in binary form
    if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1)
    {
        cout << "setting address structure failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    //bind

    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
    {
        cout << "bind failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }


    //listen

    if (listen(listenSocket, 5) == SOCKET_ERROR)
    {
        cout << "listen failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    cout << " server has started listening to port : " << port << endl;
    vector<SOCKET> clients;


    while (1) {
        //accept

        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
        {
            cout << "invalid client socket" << endl;
        }
        clients.push_back(clientSocket);
        thread t1(Interactwithclient, clientSocket, std::ref(clients));
        t1.detach();
    }


    closesocket(listenSocket);

    WSACleanup();

    return 0;
}
