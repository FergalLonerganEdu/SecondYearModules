/* EEEN20060 Communication Systems, Assignment 1
   This is an outline of the application program for file transfer.
   The protocol is very simple:  the first byte of each block transferred
   is a header value, identifying the type of block. This requires that
   the link layer protocol preserve the block boundaries.
   There are 3 block types:  file name, file data, end of file marker. */

typedef unsigned char byte;

#include <stdio.h>  // standard input-output library
#include "linklayer2.h"  // link layer functions

#define BLK_SIZE 100  // max data block size in bytes
#define FILENAME 233  // header value for file name
#define FILEDATA 234  // header value for data
#define FILEEND 235   // header value to mark end of file

// Function prototypes
int sendFile(char *fName, int debug);
int receiveFile(int debug);

int main()
{
    char inString[10];  // string to hold user command
    char fName[80];  // string to hold filename
    int retVal;  // return value from functions
    int debug = 0;  // flag to select more printing

    printf("Link Layer Assignment - Application Program\n");  // welcome message
    // First ask if the user wants lots of debug information
    printf("\nSelect debug or quiet mode: ");
    scanf("%7s", inString);  // get user response
    if ((inString[0] == 'd')||(inString[0] == 'D')) debug = 1;

    // Then ask what the user wants to do
    printf("\nSelect send or receive: ");
    scanf("%7s", inString);  // get user response

    // Decide what to do, based on what the user entered
    switch (inString[0])
    {
        case 's':
        case 'S':
            printf("\nEnter name of file to send with extension (name.ext): ");
            scanf("%70s", fName);  // get filename
            printf("\n");  // blank line
            retVal = sendFile(fName, debug);  // call function to send file
            if (retVal == 0) printf("\nFile sent!\n");
            else printf("\n*** Send failed, code %d\n", retVal);
            break;

        case 'r':
        case 'R':
            retVal = receiveFile(debug);  // call function to receive file
            if (retVal == 0) printf("\nFile received!\n");
            else printf("\n*** Receive failed, code %d\n", retVal);
            break;

        default:
            printf("\nCommand not recognised\n");
            break;

   } // end of switch

    return 0;
}  // end of main


// ============================================================================
/* Function to send a file, using the link layer protocol.
   It opens the given input file, connects to another computer and sends the
   file name.  Then it reads blocks of data of fixed size from the input,
   and sends each block over the connection.  When end-of-file is reached,
   it sends an END block, then closes the connection.
   If debug is 1, it prints progress messages, if 0, only error messages.
   Returns 0 for success, or a non-zero error code.  */

int sendFile(char *fName, int debug)
{
    FILE *fpi;  // file handle for input file
    byte data[BLK_SIZE+2];  // array of bytes
    int nByte;  // number of bytes read or found in filename
    int retVal;  // return code from functions
    long byteCount = 0; // total number of bytes read

    // Open the input file and check for failure
    if (debug) printf("\nSend: Opening %s for input\n", fName);
    fpi = fopen(fName, "rb");  // open for binary read
    if (fpi == NULL)
    {
        perror("Send: Error opening input file");
        return 1;
    }

    // Ask link layer to connect to other computer
    if (debug) printf("Send: Connecting...\n");
    retVal = LL_connect(debug);  // try to connect
    if (retVal < 0)  // problem connecting
    {
        fclose(fpi);     // close input file
        return retVal;  // pass back the error
    }

    // Send a block of data containing the name of the file
    data[0] = (byte) FILENAME;  // header byte
    nByte = 0;  // initialise counter
    do  // loop to copy file name into data array
    {
        data[nByte+1] = fName[nByte]; // copy byte from file name
    }
    while (fName[nByte++] != 0);  // including end of string

    // print message about this
    if (debug) printf("Send: Sending file name %d bytes...\n", nByte);
    retVal = LL_send(data, nByte+1, debug);  // send bytes to link layer
    if (retVal < 0)
    {
        printf("Send: Error sending file name\n");
        fclose(fpi);  // close the file
        LL_discon(debug);  // disconnect
        return retVal;  // and quit
    }

    // Start sending the contents of the file, one block at a time
    do  // loop block by block
    {
        data[0] = (unsigned char) FILEDATA;  // header byte
        // read bytes from file, store in array starting after header
        nByte = (int) fread(data+1, 1, BLK_SIZE, fpi);
        if (ferror(fpi))  // check for error
        {
            perror("Send: Error reading input file");
            fclose(fpi);   // close input file
            LL_discon(debug);  // disconnect link
            return 3;  // we are giving up on this
        }
        if (debug) printf("Send: Read %d bytes, sending...\n", nByte);
        byteCount += nByte;  // add to byte count

        retVal = LL_send(data, nByte+1, debug);  // send bytes to link layer
        // retVal is 0 if succeeded, non-zero if failed
    }
    while ((retVal == 0) && (feof(fpi) == 0));  // until input file ends or error

    if (retVal < 0)   // deal with error
    {
        printf("Send: Error sending data\n");
        fclose(fpi);  // close the file
        LL_discon(debug);  // disconnect
        return retVal;  // and quit
    }

    // if here, entire file has been sent
    if (debug) printf("Send: End of input file after %ld bytes\n",byteCount);
    fclose(fpi);    // close input file

    // Now send an ending mark
    data[0] = (unsigned char) FILEEND;  // header byte (and only byte)
    retVal = LL_send(data, 1, debug);  // send block of one byte
    if (retVal < 0) printf("Send: Error sending end block\n");

    // Ask link layer to disconnect
    if (debug) printf("Send: Disconnecting...\n");
    LL_discon(debug);  // ignore return code here...

    return retVal;  // indicate success or failure
}  // end of sendFile


// ============================================================================
/* Function to receive a file, using the link layer protocol.
   It connects to another computer, and waits to receive a block of data.
   The first block should contain the file name, and it opens the output file,
   with a modified file name (to avoid over-writing anything important.
   The following blocks of data received should be data blocks, and are written
   to the file. The final block should be an end marker, then the file is closed
   and the link disconnected.
   If debug is 1, it prints progress messages, if 0, only error messages.
   It returns 0 for success, or a non-zero error code.  */
int receiveFile(int debug)
{
    FILE *fpo;  // file handle for output file
    byte data[BLK_SIZE+2];  // array of bytes
    int nByte, nWrite;  // number of bytes received or written
    int header = 0;  // header value from received block
    int retVal;  // return code from other functions
    long byteCount = 0; // total number of bytes received

    // Connect to other computer
    if (debug) printf("RX: Connecting...\n");
    retVal = LL_connect(debug);  // try to connect
    if (retVal < 0)  // problem connecting
    {
        return retVal;  // pass back the error
    }
    printf("RX: Connected, waiting to receive...\n");

    // Try to receive one block of data
    nByte = LL_receive(data, BLK_SIZE+1, debug);
    // nByte will be number of bytes received, or negative if error
    if (nByte < 0)  // check for error
    {
        printf("RX: Error receiving first data block, code %d\n",nByte);
        if (debug) printf("RX: Disconnecting...\n");
        LL_discon(debug);  // disconnect
        return nByte;   // return error code
    }
    if (nByte == 0)  // empty data block
    {
        printf("RX: Received empty data block at start\n");
        if (debug) printf("RX: Disconnecting...\n");
        LL_discon(debug);  // disconnect
        return 5;   // return error code
    }

    // If we get here, we have received a data block
    if (debug) printf("RX: Received first block of %d bytes\n", nByte);

    header = (int) data[0];  // extract the header byte
    if (header != FILENAME)  // wrong type of block
    {
        printf("RX: Unexpected block type: %d\n", header);
        if (debug) printf("RX: Disconnecting...\n");
        LL_discon(debug);  // disconnect
        return 6;   // return error code
    }

    // If we get here, we have a filename!
    data[0] = 'Z';  // put Z as first character

    // Open the output file and check for failure
    if (debug) printf("RX: Opening %s for output\n", (char*)data);
    fpo = fopen((char*)data, "wb");  // open for binary write
    if (fpo == NULL)
    {
        perror("RX: Error opening output file");
        LL_discon(debug);  // disconnect
        return 2;
    }

    // Finally, we can start to receive the data
    // Get each block of data and write to file
    do  // loop block by block
    {
        nByte = LL_receive(data, BLK_SIZE+1, debug);  // try to receive data block
        // nByte will be number of bytes received, or negative if error

        // First check nByte, to see what to do...
        if (nByte < 0 ) printf("RX: Error receiving data, code %d\n",nByte);
        else if (nByte == 0)
        {
            if (debug) printf("RX: Zero bytes received\n");
        }
        else // we got some data!
        {
            // Now check header to see what to do...
            header = (int) data[0];  // extract the header
            if (header == FILEDATA)  // got data block - write data to file
            {
                byteCount += nByte-1;  // add to byte count
                // write bytes to file, starting after header
                nWrite = (int) fwrite(data+1, 1, nByte-1, fpo);
                if (ferror(fpo))  // check for error
                {
                    perror("RX: Error writing output file");
                    nByte = -9;  // fake value to end loop
                }
                else if (debug)
                    printf("RX: Wrote %d bytes to file\n", nWrite);
            }
            else if (header == FILEEND)  // got end marker
            {
                if (debug)
                    printf("RX: End marker after %ld bytes\n", byteCount);
                nByte = -1;  // fake value to end loop
            }
            else
            {
                if (debug)
                printf("RX: Unexpected block type: %d\n", header);
            } // end of inner if - checking header

        } // end of outer if - checking nByte
    }
    while (nByte >= 0);  // repeat until error or end marker

    fclose(fpo);  // close output file
    // Ask link layer to disconnect
    if (debug) printf("RX: Disconnecting...\n");
    LL_discon(debug);  // ignore return code here...

    return (nByte < -1) ? -nByte : 0;  // indicate success or failure
}  // end of receiveFile
