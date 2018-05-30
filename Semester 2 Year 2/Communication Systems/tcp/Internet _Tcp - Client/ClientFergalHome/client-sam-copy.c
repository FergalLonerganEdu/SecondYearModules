#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>
void printError(void);  // function to display error messages

#define SERV_PORT 32980
int main()
{
    WSADATA wsaData;  // create structure to hold winsock data
    int retVal, retSend, ret,ret2, nIn, j, nByte, nDigits=0, nData=0;
    int endLine = 0, stop = 0;  // flags to control loops
    char serverIP[20];      // IP address of server
    int serverPort;         // port used by server
    char filename_download[100];      // array to hold user input for filename
    char filename_upload[100];
    char ACK[10];          //ack to make sure server is ready for upload
    char reply[100];        // array to hold received bytes

    // Initialise winsock, version 2.2, giving pointer to data structure
    retVal = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (retVal != 0)  // check for error
    {
        printf("*** WSAStartup failed: %d\n", retVal);
       // printError();

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
      //  printError();
        stop = 1;
    }
    else printf("Socket created\n" );

    // Get the details of the server from the user
    printf("Enter IP address of server: ");
    scanf("%20s", serverIP);  // get IP address as string

    printf("Enter port number: ");
    scanf("%d", &serverPort);     // get port number as integer

    gets(filename_download);  // flush the endline from the input buffer

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
      //  printError();
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
        gets(filename_download);
        if(downup[0]=='d'||downup[0]=='D')// Get user request and send it to the server
        {

            char D[50] = "D";
            int  nRx=0;
            printf("Enter Filename: ");
            gets(filename_download);  // read in the filename
            // gets() reads until enter (CR), but does not put CR in string
            strcat(D, filename_download);
            strcat(D, "@");
            request_length = strlen(D);
            printf("Request: %s, length: %d\n", D, request_length);


            retSend = send(clientSocket, D, request_length, 0);//need to send D to the server
            if( retSend == SOCKET_ERROR) // check for error
            {
                printf("*** Error sending\n");
               // printError();
            }
            else printf("Sent %d bytes, waiting for reply...\n", retSend);

            nRx = recv(clientSocket, header, 50, 0); // get nByte bytes, put in header array
            printf("nRx here = %d\n", nRx); ///for testing
            if( nRx < 0)
            {
                printf("Problem receiving\n");
               // printError();
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
                        stop=1; //closes connection???
                        printf("File not found on server, closing connection\n");
                    }

                    if(header[0]=='y')
                    {
                        int bytes_rec=0;
                        fp = fopen(filename_download, "wb");// open file
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
                            printf("nRx: %d\n", nRx); ///test
                            printf("nByte: %d\n", nByte); ///test
                            nRx = recv(clientSocket, reply, 100, 0);
                            if( nRx == SOCKET_ERROR)  // check for error
                            {
                                printf("Problem receiving\n");
                                //printError();
                                stop = 1;  // exit the loop if problem
                            }
                            else if  (nRx == 0)  // connection closed
                            {
                                printf("nRx has reached 0\n"); ///test
                                printf("Connection closed by server\n");
                                stop = 1;
                            }
                            else
                            {
                                fwrite(reply, 1, nRx, fp);
                                bytes_rec += nRx;
                                printf("bytes_rec: %d\n", bytes_rec); ///test
                            }
                        }
                        // loop to receive rest of file

                        fclose(fp);

                    }

                } // process received bytes
            }

        if(downup[0]=='u'||downup[0]=='U')
        {
          char U[50] = "U";
          int nRx = 0;
          printf("Enter filename: ");
          gets(filename_upload); //reads in filename
          strcat(U, filename_upload);
          strcat(U, "@");
          request_length=strlen(U);
          printf("Request: %s, length: %d\n", U, request_length);
          retSend=send(clientSocket, U, request_length, 0); //sending them the upload request (Ufilename@)

            if( retSend == SOCKET_ERROR) // check for error
                {
                    printf("*** Error sending\n");
                   // printError();
                    return 3;
                }
            else printf("Sent %d bytes, waiting for reply...\n", retSend);

            // we get an ACK that Server has created a file and is ready to upload.
            nRx = recv(clientSocket, ACK, 10, 0);

             //what's ACK gonna be, a Y? yes?
            if(nRx < 0 )
            {
                printf("Problem Receiving ACK \n");
                //printError();
                stop=1;
            }
            else if(nRx == 0)
            {
                stop=1;
                printf("Connection closed by server\n");
            }
            else
            {
                if(ACK[0]!='y')
                {
                    stop=1;
                    printf("File not created by server. \n");
                }
                else if(ACK=='y')
                {
                    fp=fopen(filename_upload, "rb");
                        if(fp==NULL)
                        {
                          //  printError();
                            stop=1;
                        }
                        else
                        {
                            ret = fseek(fp,0, SEEK_END);
                            nByte = ftell(fp);
                            ret = fseek(fp, 0, SEEK_SET);
                            char size[50];
                            ret2 = sprintf(size, "%d@", nByte);
                            nDigits = srlen(size);
                            retSend = send(clientSocket, size, nDigits, 0); //will send server filesize@
                            printf("nByte: %d", nByte); ///testing

                            int bytes_send=0;
                            while(bytes_send<nByte)
                            {
                                retSend = (int) fread(filename_upload, 1, 100, fp); // 1= # of bytes to read at a time, 100 in each block
                                if (ferror(fp))  // check for error
                                {
                                //    printError();
                                    fclose(fp);   // close input file
                                    return 3;  // we are giving up on this
                                }
                                else
                                {
                                    retVal = send(clientSocket, filename_upload, retSend, 0);
                                }
                            }
                        }
                }
            }
        }

            if (stop == 1) printf("Closing connection as requested...\n");


        // Shut down the sending side of the TCP connection first

        retVal = shutdown(clientSocket, SD_SEND);
        if( retVal != 0)  // check for error
        {
            printf("*** Error shutting down sending\n");
           // printError();
        }

        // Then close the socket
        retVal = closesocket(clientSocket);
        if( retVal != 0)  // check for error
        {
            printf("*** Error closing socket\n");
           // printError();
        }
        else printf("Socket closed\n");

        // Finally clean up the winsock system
        retVal = WSACleanup();
        printf("WSACleanup returned %d\n",retVal);

        // Prompt for user input, so window stays open when run outside CodeBlocks
        printf("\nPress return to exit:");
        gets(filename_download);
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

}
