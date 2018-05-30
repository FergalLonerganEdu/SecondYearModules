#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>
void printError(void);  // function to display error messages

#define SERV_PORT 32980
int main()
{
    WSADATA wsaData;  // create structure to hold winsock data
    int retVal, retSend, ret, ret2, nByte= 0, nDigits= 0; //explained further in the code when used
    int nData= 0, request_length, nRx= 0, bytes_rec = 0;  //explained further in the code when used
    int stop = 0, bytes_send = 0, i = 0; // flags to control loops
    char serverIP[20];      // IP address of server
    int serverPort;         // port used by server
    int threshold=10;       // marker counting in 10% intervals for the percentage which has already been sent/received
    double percentage=0;    // the updated percentage sent/received.
    char downup[10];        // array to hold user input of when choosing download or upload
    char header[100];       // array to hold y+filesize+@ and the start of the data
    FILE *fp;               // declaring file
    char filename_download[100];      // array to hold user input for filename  (download)
    char filename_upload[100];      //array to hold the user input for filename (upload)
    char ACK[10];          //acknowledgement received from server to make sure they are ready for upload
    char downloaded_data[100];        // array to hold received bytes from server to be written to file

    // Initialise winsock, version 2.2, giving pointer to data structure
    retVal = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (retVal != 0)  // check for error
    {
        printf("*** WSAStartup failed: %d\n", retVal);
        printError();

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
    else printf("Socket created\n\n" );

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
        printError();
        stop = 1;  // make sure we do not go into the while loop
    }
    else printf("Connected!\n");
    printf("\n------------------------------------\n");

    //-------------------------------------------------------------------------------------------------------------------

    // Main loop to send messages and receive responses
    // This example assumes client will send first.
    while (stop == 0)
    {
        percentage=0;
        threshold=10;
        printf("Do you wish to download or upload or exit?"); //instructions for user
        scanf("%10s", downup);                         //scans user input for specifying download or upload
        gets(filename_download);                        //clears unnecessary characters after user input. gets() reads until enter (CR), but does not put CR in string
        if(downup[0]=='e'||downup[0]=='e')
        {
            char E[10] = "E@";
            send(clientSocket, E, 10, 0);
            printf("exited as requested\n");
            break;
        }
        if(downup[0]=='d'||downup[0]=='D')          // if the user inputs d or download, enters this function
        {
            bytes_rec=0;
            char D[50] = "D";                     //Creates an array to hold the filename to be sent to server, setting first char as D for server to interpret.
            printf("Enter Filename: ");           // instructions for user.
            gets(filename_download);    // read in the filename. gets() reads until enter (CR), but does not put CR in string
            printf("------------------------------------\n");
            strcat(D, filename_download);     //strcat concatenates the filename onto the character "D".
            strcat(D, "@");                   // concatenates an "@" symbol after the filename in char array D. "@" is an endbyte marker.
            request_length = strlen(D);       // gets the length for the send function, assigns request_length that value.


            retSend = send(clientSocket, D, request_length, 0);     //sends D array to server
            if( retSend == SOCKET_ERROR) // checks for error sending D array to server
            {
                printf("Error sending\n");
                printError();
            }
            else printf("\nSent request, waiting for server to send %s...\n", filename_download);

            nRx = recv(clientSocket, header, 100, 0); // receives the first 100 bytes from server starting with "yFilesize@" , places in the header array

            if( nRx < 0)
            {
                printf("Problem receiving\n");      //error checking: if nRx returns a value less than 0, we close the connection.
                printError();
                stop = 1;           // exit the loop if problem
            }
            else if(nRx == 0)               // error checking : if nRx returns 0 bytes from server, we close the connection.
            {
                stop=1;             // exit the loop if problem
                printf("Connection closed by server\n");
            }

            else    //will enter this loop if we have received data
            {
                if(header[0]=='n')   //if the file is not present on server, server will send 'n' as the first character of the array we called header.
                {
                    stop=1;  //closes connection
                    printf("File not found on server, closing connection\n");
                }

                if(header[0]=='y') // indicates the desired file has been found on server.
                {
                    fp = fopen(filename_download, "wb");   //creates and opens a file in binary writing mode, of the same name user inputs above.
                    i=0;
                    for(i=1; header[i] !='@'; i++)    //loops which gets the number of digits of the byte size (e.g. 244047 = 6 digits)
                    {
                        nDigits++;
                    }
                    nByte = atoi(header+1);          //Turns the byte size in array from the header array into an integer. header+1 is a pointer
                                                    // which points to the first character after y (i.e. the first digit in file size). atoi function
                                                    // reads until the "@" symbol and returns the integer value.


                    printf("Server is sending %s, file size of: %d bytes.\n",filename_download, nByte);

                    if(nByte==0)        //error checking, closes connection if no data is receive from server.
                    {
                        printf("Data size of 0 received from server\n");
                        stop=1;
                    }

                    nData = nRx-(nDigits+2); //nData is the amount of data in the header array found after the "@" symbol
                    if(nData>0)              //if there is data present in the header array
                    {
                        fwrite(header+nDigits+2, 1, nData, fp);             //writes the data after the "@" symbol in header to the download file
                        bytes_rec=nData;                                       //increases bytes_rec(a counter for how many bytes have been been received/written increments
                                                                                // each time we receive/write more bytes.

                    }
                    printf("\n---------------------------------------------------------------");
                    printf("\n\t\t\t  PROGRESS BAR \n");
                    printf("[");
                    while(bytes_rec<nByte && stop==0)                        //while the amount of bytes received/written is still less than the total number of data bytes
                    {
                        nRx = recv(clientSocket, downloaded_data, 100, 0);    //receives 100 bytes from server, puts it in array called downloaded_data, returns a value for nRx.
                        if( nRx == SOCKET_ERROR)  // check for error
                        {
                            printf("Problem receiving\n");
                            printError();
                            stop = 1;  // exit the loop if problem
                        }
                        else if  (nRx == 0)  //if there are no more bytes to be received (i.e end of file has been reached by server)
                        {
                            printf("Connection closed by server\n");
                            stop = 1; //closes connection
                        }
                        else     //data is received from server
                        {
                            fwrite(downloaded_data, 1, nRx, fp);            //writes nRx bytes from the downloaded_data array into the file.
                            bytes_rec += nRx;                       // increments the bytes_rec which is bytes received/written,
                                                                    // so that the loop ends when bytes_rec reaches the total number of bytes
                            percentage = (bytes_rec/(double)nByte)*100; //calculates the current percentage of bytes received
                            if(percentage>threshold)
                            {
                                printf("===%d%%",threshold);   //when the current percentage reaches threshold, we print the threshold which indicates each time an extra 10% has been received
                                threshold+=10;                 //threshold is increased by 10 each time for the next 10% marker to be reached
                            }
                        }
                    }
                    printf("===100%%]\n");
                    printf("\t\t\tFILE DOWNLOADED");
                    printf("\n---------------------------------------------------------------\n\n\n");
                    fclose(fp);         //closes the file when all of the bytes have been received
                }
            }
        }

        if(downup[0]=='u'||downup[0]=='U')          // if the user inputs u or upload, enters this function
        {
            char U[50] = "U";                      //makes a char array "U" where the first char will be "U", where we will concatenate file size and "@" symbol
            printf("Enter filename: ");             //asks user to input the file they want to upload to server.
            gets(filename_upload);                  //reads in filename
            printf("------------------------------------\n");
            strcat(U, filename_upload);             //concatenates the filename onto char U
            strcat(U, "@");                         //concatenates "@" onto the previous concatenation.

            fp=fopen(filename_upload, "rb");        //opens the file for binary reading.

            if(fp==NULL)                            //if the file is not opened.
            {
                printError();                   //printError will return the error code.
                stop=1;                         //exits the loop.
            }
            else                                    // if the file is found
            {
                bytes_send=0;
                ret = fseek(fp,0, SEEK_END);            //seeks to the last byte of the file.
                nByte = ftell(fp);                      //ftell returns where it is in the file (i.e. how many bytes there are), nByte is given this value.
                ret = fseek(fp, 0, SEEK_SET);           //seeks back to first byte of the file.
                char size[50] = "";
                sprintf(size, "%d@", nByte);     //prints the size of the file along with an "@" symbol, into an array called "size".
                strcat(U, size);                //concatenates this new array size onto the array U. ( the array U will be: Ufilename@filesize@ )

                request_length=strlen(U);       //calculates the length of the array U, which is needed for the send function to be sent to server.

                retSend=send(clientSocket, U, request_length, 0); //sending the server the upload request (Ufilename@filesize@)

                if( retSend == SOCKET_ERROR) // check for error
                {
                    printf("*** Error sending\n");
                    printError();
                    return 3;
                }
                else printf("\nSent the request for %s, waiting for reply from server...", filename_upload);      //message for user to know the request has been sent to server.
                nRx = recv(clientSocket, ACK, 10, 0);     // we get an ACK that Server has created a file and is ready to upload. ACK should be 'y' when it is ready to continue.

                if(nRx < 0 )  //error checking: if nRx returns a value less than 0, we close the connection.
                {
                    printf("Problem Receiving ACK \n");
                    printError();
                    stop=1; //closes connection
                }
                else if(nRx == 0)   // error checking : if nRx returns 0 bytes from server, we close the connection.
                {
                    stop=1;
                    printf("Connection closed by server\n");
                }
                else                    //otherwise we have received an acknowledgement
                {
                    if(ACK[0]!='y')      //if the acknowledgement is not 'y', server has not created a file for bytes to be transferred.
                    {
                        stop=1;
                        printf("File not created by server. \n");
                    }

                    else if(ACK[0]=='y') //acknowledgement 'y' is received.
                    {

                        printf("Received reply, starting to upload %s.\n",filename_upload); //tells user the data is beginning to be uploaded to server.
                        printf("Uploading %d bytes to server...\n\n\n", nByte);
                        printf("---------------------------------------------------------------");
                        printf("\n\t\t       PROGRESS BAR \n");
                        printf("[");

                        while(bytes_send<nByte)   //while the number of bytes sent is less than the total number of bytes in the file
                        {
                            retSend = (int) fread(filename_upload, 1, 100, fp); //fread attempts to read in up to 100 bytes from the file . retSend is how many bytes are read.
                            if (ferror(fp))  // check for error
                            {
                                printError();
                                fclose(fp);   // close input file
                                return 3;
                            }
                            else
                            {
                                retVal = send(clientSocket, filename_upload, retSend, 0); //sends retSend amount of bytes to the server each time. retVal is the amount sent each time.
                                //printf("bytes_send: %d", bytes_send);
                                //printf("retval: %d", retVal);
                                bytes_send+=retVal;                              //bytes_send increments each time by how many bytes are sent. This is used

                                percentage = (bytes_send/(double)nByte)*100; //calculates the current percentage of bytes sent
                                if(percentage>threshold)
                                {
                                    printf("===%d%%",threshold);    //when the current percentage reaches threshold, we print the threshold which indicates each time an extra 10% has been sent
                                    threshold+=10;   //threshold is increased by 10 each time for the next 10% marker to be reached
                                }                                                // to exit the while loop when the entire file has been read/sent
                            }
                        }
                        fclose(fp);  //closes the file
                       printf("===100%%]\n");
                        printf("\t\t       FILE UPLOADED");
                        printf("\n---------------------------------------------------------------\n\n\n");

                    }
                }
            }
        }
        else if(downup[0]!='d'||downup[0]!='D'||downup[0]!='u'||downup[0]!='U'||downup[0]!='e'||downup[0]!='E')
        {
            printf("\nInput not accepted\n");
        }
        if (stop == 1) printf("Closing connection...\n");          //if stop = 1 at any point, the connection is closed


        // Shut down the sending side of the TCP connection first



    }
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
        gets(filename_download);
        return 0;

    /* Function to print informative error messages
       when something goes wrong...  */


}
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
