#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>

void printError(void);  // function to display error messages
int receive_File(SOCKET cSocket, FILE *fpo, char request[], int nByte);
int send_File(SOCKET cSocket, FILE *fpi, char file_found, int nByte);

#define SERV_PORT 32980  // port to be used by server
#define BLK_SIZE 1000  // max data block size in bytes
#define FILENAME 230  // header value for file name
#define FILEDATA 234  // header value for data

int main()
{
    WSADATA wsaData;  // create structure to hold winsock data
    int retVal, nRx, nIn, j;
    int endLine = 0, stop = 0;  // flags to control loops
    char request[100];  // array to hold received bytes
    char response[100]; // array to hold our response
    char file_found = 'y'; // file to know if the file has been found or not
    char data[BLK_SIZE];  // array of characters
    int end_of_Filename; // integer to know when to start reading nBytes from string


    // Initialise winsock, version 2.2, giving pointer to data structure
    retVal = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (retVal != 0)  // check for error
    {
        printf("*** WSAStartup failed: %d\n", retVal);
        printError();
        return 1;
    }
    printf("WSAStartup succeeded\n" );

    // Create a handle for a socket, to be used by the server for listening
    SOCKET serverSocket = INVALID_SOCKET;  // handle called serverSocket

    // Create the socket, and assign it to the handle
    // AF_INET means IP version 4,
    // SOCK_STREAM means socket works with streams of bytes,
    // IPPROTO_TCP means TCP transport protocol.
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)  // check for error
    {
        printf("*** Failed to create socket\n");
        printError();
    }
    else printf("Socket created\n" );

    // Build a structure to identify the service offered
    struct sockaddr_in service;  // IP address and port structure
    service.sin_family = AF_INET;  // specify IP version 4 family
    service.sin_addr.s_addr = htonl(INADDR_ANY);  // set IP address
    // function htonl() converts 32-bit integer to network format
    // INADDR_ANY means we accept connection on any IP address
    service.sin_port = htons(SERV_PORT);  // set port number
    // function htons() converts 16-bit integer to network format

    // Bind the socket to the IP address and port just defined
    retVal = bind(serverSocket, (SOCKADDR *) &service, sizeof(service));
    if( retVal == SOCKET_ERROR)  // check for error
    {
        printf("*** Error binding to socket\n");
        printError();
    }
    else printf("Socket bound\n");

    // Listen for connection requests on this socket,
    // second argument is maximum number of requests to allow in queue
    retVal = listen(serverSocket, 2);
    if( retVal == SOCKET_ERROR)  // check for error
    {
        printf("*** Error trying to listen\n");
        printError();
    }
    else printf("Listening on port %d\n", SERV_PORT);

    // Create a new socket for the connection we expect
    // The serverSocket stays listening for more connection requests,
    // so we need another socket to connect with the client...
    SOCKET cSocket = INVALID_SOCKET;

    // Create a structure to identify the client (optional)
    struct sockaddr_in client;  // IP address and port structure
    int len = sizeof(client);  // initial length of structure

    // Wait until a connection is requested, then accept the connection.
    // If no need to know who is connecting, arguments 2 and 3 can be NULL
    cSocket = accept(serverSocket, (SOCKADDR *) &client, &len );
    if( cSocket == INVALID_SOCKET)  // check for error
    {
        printf("*** Failed to accept connection\n");
        printError();
    }
    else  // we have a connection, report who it is (if we care)
    {
        int clientPort = client.sin_port;  // get port number
        struct in_addr clientIP = client.sin_addr;  // get IP address
        // in_addr is a structure to hold an IP address
        printf("Accepted connection from %s using port %d\n",
               inet_ntoa(clientIP), ntohs(clientPort));
        // function inet_ntoa() converts IP address structure to string
        // function ntohs() converts 16-bit integer from network form to normal
    }

    // Main loop to receive requests and send responses
    // This example assumes that client sends first, so server receives first


    do
    {
        endLine = 0;
        char mode;
        char request[100], number[10];
        int i =0, nByte = 0, start_of_data =0;

        // Receive bytes - will not return until it gets at least one
        // recv() arguments: socket handle, array to hold rx bytes,
        // maximum number of bytes to receive, last argument 0.
        nRx = recv(cSocket, request, 100, 0);
        // nRx will be number of bytes received, or error indicator


        if( nRx < 0)  // error
        {
            printf("Problem receiving, connection closed by client?\n");
            //printError();
            stop = 1;  // exit the loop if problem
        }
        else if (nRx == 0)  // connection closing
        {
            stop = 1;  // exit the loop in that case
        }
        else // we got some data
        {
            for (j = 0; j < nRx; j++)
            {
                printf("%c", request[j]);  // print each character
            }

            mode = request[0];
            char filename[100];

            for(i=1; i<nRx && request[i]!= '@' ; i++)
            {
                filename[i-1] = request[i];
                end_of_Filename = i;
            }
            printf("Filename is %s \n", filename);

            nByte = 0; // initialise counter
            int n = 0; // used as begnning of our number array

            for(i=end_of_Filename; i<nRx || request[i] != '@'; i++)
            {
                number[n] = request[i];
                n++;
                start_of_data = n;
            }// read in size of file from client
            nByte = atoi(number);  // extracts our number from the char string into an int


            if(mode=='D')
            {
                FILE *fpi;  // file handle for input file

                // Open the input file and check for failure
                printf("\nSend: Opening %s for input\n", filename);
                fpi = fopen(filename, "rb");  // open for binary read
                if (fpi == NULL)
                {
                    perror("Send: Error opening input file");
                    file_found = 'n'; //?
                    return 1;
                }

                //download functions
                send_File(cSocket, fpi, file_found , nByte);
            }

            if(mode='U')
            {
                // read in filename and length
                // create file and ask for file to be sent
                // accept bytes up until filesize and put them in created file
                // close file
                // tell them we have received the file
                FILE *fpo;

//nBytes and filename
                // Open the output file and check for failure
                printf("\nSend: Opening %s for output\n", filename);
                fpo = fopen(filename, "wb");
                if (fpo == NULL)
                {
                    perror("Send: Error opening output file");
                    return 1;
                }

                retVal = (int) fwrite(data, 1, nByte, fpo); //fwrite(data, 1,filename, "wb");  // open for binary read

                //upload functions
                receive_File(cSocket, fpo, request,nByte);
            }
            else
            {
                break;
            }
        }


        // If the incoming message is finished (LF found),
        // then get our response from the user
        if (endLine == 1)  // LF character has been received
        {
            printf("Enter response (max 90 char, $ to close connection): ");
            gets(response);  // read in string
            // gets() reads until enter (CR), but does not put CR in string
            nIn = strlen(response);  //find the length
            response[nIn] = 13;  // replace null terminator with CR
            response[nIn+1] = 10;  // add LF
            if (response[0] == '$') stop = 1;  // set stop flag if $ entered

            if (stop == 0)
            {
                // send() arguments: socket handle, array of bytes to send,
                // number of bytes to send, and last argument of 0.
                retVal = send(cSocket, response, nIn+2, 0);  // send nIn+2 bytes
                // retVal will be number of bytes sent, or error indicator

                if( retVal == SOCKET_ERROR)  // check for error
                {
                    printf("*** Error sending response\n");
                    printError();
                }
                else printf("Sent %d bytes, waiting for reply...\n", retVal);

            }  // end if stop == 0

        }  // end if endline == 1
    }
    while (stop == 0);  // repeat until told to stop
    // When this loop exits, it is time to close the connection and tidy up
    printf("Connection closing...\n");

    // Shut down the sending side of the TCP connection first
    retVal = shutdown(cSocket, SD_SEND);
    if( retVal != 0)  // check for error
    {
        printf("*** Error shutting down sending\n");
        printError();
    }

    // Then close the client socket
    retVal = closesocket(cSocket);
    if( retVal != 0)  // check for error
    {
        printf("*** Error closing client socket\n");
        printError();
    }
    else printf("Client socket closed\n");

    // Then close the server socket
    retVal = closesocket(serverSocket);
    if( retVal != 0)  // check for error
    {
        printf("*** Error closing server socket\n");
        printError();
    }
    else printf("Server socket closed\n");

    // Finally clean up the winsock system
    retVal = WSACleanup();
    printf("WSACleanup returned %d\n",retVal);

    // Prompt for user input, so window stays open when run outside CodeBlocks
    printf("\nPress return to exit:");
    gets(response);
    return 0;

}


int receive_File(SOCKET cSocket, FILE *fpo, char request[], int nByte)
{
    char data[BLK_SIZE+3];  // array of characters
    char number [10]; // our character array read from string for nBytes
    int retVal;  // return code from functions
    int start_of_data; // marker to note the end of header and start of data
    long byteCount = 0; // total number of bytes read
    int bytes_rec; //amount of bytes rec and sent to file

    int x=0;
    while (bytes_rec < nByte)// change nByte
    {
        data[x] = request[i];
        x++;
        nByteWritten = fwrite(data, 1, nRx, fpo); //
        if (ferror(fpo))  // check for error
        {
            perror("Send: Error reading input file");
            fclose(fpo);   // close input file
            return 3;  // we are giving up on this
        }
        bytes_rec += nByteWritten;
    }

}

int send_File(SOCKET cSocket, FILE *fpi, char file_found, int nByte)
{
    char data[BLK_SIZE+3];  // array of characters
    int retVal;  // return code from functions
    long byteCount = 0; // total number of bytes read

    printf("\nFile size is %ld bytes\n", nByte);  // print it
    ret = sprintf( data, "%c%d@",file_found, nByte); //adding our value of nByte to string
    retVal = send(cSocket, data, ret, 0);  // send bytes to TCP
    // retVal is 0 if succeeded, non-zero if failed

    data[ret] = 0;
    // print message about this
    printf("Send: Reply %d bytes...\n|%s|\n", ret, data);


    // To read the file, you probably want to start at the beginning
    retVal = fseek(fpi, 0, SEEK_SET);   // set current position to start of file
    if (retVal != 0)  // there was an error
    {
        perror("Error in fseek");
        printf("errno = %d\n", errno);
        fclose (fpi);
        return 3;
    }


    // Start sending the contents of the file, one block at a time
    do  // loop block by block
    {
        // read bytes from file, store in array starting after header
        nByte = (int) fread(data, 1, BLK_SIZE, fpi);
        if (ferror(fpi))  // check for error
        {
            perror("Send: Error reading input file");
            fclose(fpi);   // close input file
            // LL_discon(debug);  // disconnect link
            return 3;  // we are giving up on this
        }


        retVal = send(cSocket, data, nByte, 0);  // send bytes to TCP
        // retVal is 0 if succeeded, non-zero if failed

    }
    while ((retVal == 0) && (feof(fpi) == 0));   // until input file ends or error

    if (retVal < 0)   // deal with error
    {
        printf("Send: Error sending data\n");
        fclose(fpi);  // close the file
        // LL_discon(debug);  // disconnect
    }

}// end of sendFile

return ;  // indicate success or failure

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
