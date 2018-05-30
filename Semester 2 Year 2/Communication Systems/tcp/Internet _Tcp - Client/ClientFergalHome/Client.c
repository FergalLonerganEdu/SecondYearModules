#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>
void printError(void);  // function to display error messages

#define SERV_PORT 32980
int main()
{
    WSADATA wsaData;  // create structure to hold winsock data
    int retVal, retSend, nRx, nIn, j, nByte, BLK_SIZE=1000, nDigits=0, nData=0;
    int endLine = 0, stop = 0;  // flags to control loops
    char serverIP[20];      // IP address of server
    int serverPort;         // port used by server
    char filename_request[100];      // array to hold user input for filename
    char reply[100];        // array to hold received bytes

    // Initialise winsock, version 2.2, giving pointer to data structure
    retVal = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (retVal != 0)  // check for error
    {
        printf("*** WSAStartup failed: %d\n", retVal);
        printError();
        return 1;
    }
    else printf("WSAStartup succeeded\n" );

    // Create a handle for a socket, to be used by the client
    SOCKET clientSocket = INVALID_SOCKET;  // handle called clientSocket

    // Create the socket, and assign it to the handle
    // AF_INET means IP version 4,
    // SOCK_STREAM means socket works with streams of bytes,
    // IPPROTO_TCP means TCP transport protocol.
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)  // check for error
    {
        printf("*** Failed to create socket\n");
        printError();
        stop = 1;
    }
    else printf("Socket created\n" );

    // Get the details of the server from the user
    printf("Enter IP address of server: ");
    scanf("%20s", serverIP);  // get IP address as string

    printf("Enter port number: ");
    scanf("%d", &serverPort);     // get port number as integer

    gets(filename_request);  // flush the endline from the input buffer

    // Build a structure to identify the service required
    // This has to contain the IP address and port of the server
    struct sockaddr_in service;  // IP address and port structure
    service.sin_family = AF_INET;  // specify IP version 4 family
    service.sin_addr.s_addr = inet_addr(serverIP);  // set IP address
    // function inet_addr() converts IP address string to 32-bit number
    service.sin_port = htons(serverPort);  // set port number
    // function htons() converts 16-bit integer to network format

    // Try to connect to the service required
    printf("Trying to connect to %s on port %d\n", serverIP, serverPort);
    retVal = connect(clientSocket, (SOCKADDR *) &service, sizeof(service));
    if( retVal != 0)  // check for error
    {
        printf("*** Error connecting\n");
        printError();
        stop = 1;  // make sure we do not go into the while loop
    }
    else printf("Connected!\n");

    //-------------------------------------------------------------------------------------------------------------------

    // Main loop to send messages and receive responses
    // This example assumes client will send first.
    while (stop == 0)
    {
        char downup[10];
        char header[1000];
        int request_length;
        FILE *fp;
        printf("Do you wish to download or upload?");
        scanf("%10s", downup);
        gets(filename_request);
        if(downup[0]=='d'||downup[0]=='D')// Get user request and send it to the server
        {

            char D[50] = "D";
            printf("Enter Filename: ");
            gets(filename_request);  // read in the filename
            // gets() reads until enter (CR), but does not put CR in string
            strcat(D, filename_request);
            strcat(D, "@");
            request_length = strlen(D);
            printf("Request: %s, length: %d\n", D, request_length);


            retSend = send(clientSocket, D, request_length, 0);//need to send D to the server
            if( retSend == SOCKET_ERROR) // check for error
            {
                printf("*** Error sending\n");
                printError();
            }
            else printf("Sent %d bytes, waiting for reply...\n", retSend);

            nRx = recv(clientSocket, header, 50, 0); // get nByte bytes, put in header array
            if( nRx < 0)
            {
                printf("Problem receiving\n");
                printError();
                stop = 1;  // exit the loop if problem
                }
                // deal with error
                else if(nRx == 0)
                {
                    stop=1;
                    printf("Connection closed by server\n");
                }
                // connection has been closed

                else
                {
                    if(header[0]=='n')
                    {
                        stop=1; //closes conection???
                        printf("File not found on server, closing connection\n");
                    }

                    if(header[0]=='y')
                    {
                        int bytes_rec=0;
                        fp = fopen(filename_request, "wb");// open file
                        int i=0; //parameter for loop
                        for(i=1; header[i] !='@'; i++)
                        {
                            nDigits++; //???minus 1 for != '@'
                        }
                        nByte = atoi(header+1);
                        printf("Extracted file size %d, %d digits\n",nByte,nDigits);
                        if(nByte==0)
                        {
                            printf("No data found in file\n");
                        }
                        // is there file data in this block?
                        // how many bytes of data?
                        nData = nRx-(nDigits+2);
                        if(nData>0)
                        {
                            fwrite(header+nDigits+2, 1, nData, fp);
                            printf("first %d bytes written to file\n", nData);
                            bytes_rec=nData;

                        }

                        while(bytes_rec<nByte && stop==0)
                        {
                            nRx = recv(clientSocket, reply, 100, 0);
                            if( nRx == SOCKET_ERROR)  // check for error
                            {
                                printf("Problem receiving\n");
                                //printError();
                                stop = 1;  // exit the loop if problem
                            }
                            else if (nRx == 0)  // connection closed
                            {
                                printf("Connection closed by server\n");
                                stop = 1;
                            }
                            fwrite(reply, 1, nRx, fp);
                            bytes_rec+=nRx;
                        }
                        // loop to receive rest of file

                        fclose(fp);

                    }

                } // process received bytes
            }


            /*nIn = strlen(filename_request);  //find the length
            filename_request[nIn] = 13;  // replace null terminator with CR
            filename_request[nIn+1] = 10;  // add LF
            if (filename_request[0] == '$') stop = 1;  // set stop flag if $ entered*/

            if (stop == 1) printf("Closing connection as requested...\n");
            /*else  // send the file and try to receive a reply
            {
                // send() arguments: socket handle, array of bytes to send,
                // number of bytes to send, and last argument of 0.
                retVal = send(clientSocket, filename_request, nIn+2, 0);  // send nIn+2 bytes
                // retVal will be number of bytes sent, or error indicator

                if( retVal == SOCKET_ERROR) // check for error
                {
                    printf("*** Error sending\n");
                    printError();
                }
                else printf("Sent %d bytes, waiting for reply...\n", retVal);

                endLine = 0;
                do  // loop to receive entire reply, terminated by LF
                {
                    // Try to receive some bytes
                    // recv() arguments: socket handle, array to hold rx bytes,
                    // maximum number of bytes to receive, last argument 0.
                    nRx = recv(clientSocket, reply, 100, 0);
                    // nRx will be number of bytes received, or error indicator

                    if( nRx == SOCKET_ERROR)  // check for error
                    {
                        printf("Problem receiving\n");
                        //printError();
                        stop = 1;  // exit the loop if problem
                    }
                    else if (nRx == 0)  // connection closed
                    {
                        printf("Connection closed by server\n");
                        stop = 1;
                    }
                    else if (nRx > 0)  // we got some data
                    {
                        for (j = 0; j < nRx; j++)
                        {
                            printf("%c", reply[j]);  // print each character
                            if (reply[j] == 10) endLine = 1;  // found LF
                        }
                    }
                }
                while ((endLine == 0) && (stop == 0));
                // continue until endline or error or connection closed
                // if it was endline, the outer loop should continue

            } // end else (not stop)*/


        }// end while stop == 0
        // When this loop exits, it is time to tidy up and end the program


        // Shut down the sending side of the TCP connection first

        retVal = shutdown(clientSocket, SD_SEND);
        if( retVal != 0)  // check for error
        {
            printf("*** Error shutting down sending\n");
            printError();
        }

        // Then close the socket
        retVal = closesocket(clientSocket);
        if( retVal != 0)  // check for error
        {
            printf("*** Error closing socket\n");
            printError();
        }
        else printf("Socket closed\n");

        // Finally clean up the winsock system
        retVal = WSACleanup();
        printf("WSACleanup returned %d\n",retVal);

        // Prompt for user input, so window stays open when run outside CodeBlocks
        printf("\nPress return to exit:");
        gets(filename_request);
        return 0;
    }

    /* Function to print informative error messages
       when something goes wrong...  */
    void printError(void)
    {
        char lastError[1024];
        int errCode;

        errCode = WSAGetLastError();  // get the error code for the last error
        FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            lastError,
            1024,
            NULL);  // convert error code to error message
        printf("WSA Error Code %d = %s\n", errCode, lastError);
    }

