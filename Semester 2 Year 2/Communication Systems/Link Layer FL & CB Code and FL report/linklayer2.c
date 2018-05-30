/* Functions to implement link layer protocol.
   LL_connect() connects to another computer;
   LL_discon()  disconnects
   LL_send()    sends a block of data
   LL_receive() waits to receive a block of data
   All functions take a debug argument - if 1, they print
   messages explaining what is happening.
   Regardless of debug, functions print messages on errors.
   All functions return negative values on error or failure.
   Definitions of constants are in the header file.  */

typedef unsigned char byte;

#include <stdio.h>      // input-output library: print & file operations
#include <time.h>       // for timing functions
#include "physical.h"   // physical layer functions
#include "linklayer2.h"  // these functions
#define BITRATE 38400

static int seqNumTx;        // transmit frame sequence number
static int lastSeqRx;       // sequence number of last good block received
static int connected = 0;   // keep track of state of connection
static int framesSent = 0;      // count of frames sent
static int acksSent = 0;        // count of ACKs sent
static int naksSent = 0;        // count of NAKs sent
static int acksRx = 0;          // count of ACKs received
static int naksRx = 0;          // count of NAKs received
static int badFrames = 0;       // count of bad frames received
static int goodFrames = 0;      // count of good frames received
static int timeouts = 0;        // count of timeouts
static long timerRx;            // time value for timeouts at receiver

// ===========================================================================
/* Function to connect to another computer.
   It just calls PHY_open() and reports any error.
   It also initialises counters for debug purposes.  */
int LL_connect(int debug)
{
    // Try to connect - set suitable parameters here...
    int retCode = PHY_open(1,BITRATE,8,0,1000,50,PROB_ERR);
    if (retCode == 0)   // check if succeeded
    {
        connected = 1;      // record that we are connected
        seqNumTx = 0;       // set first sequence number for sender
        lastSeqRx = -1;     // set impossible value for last seq. received
        framesSent = 0;     // initialise counters for report
        acksSent = 0;
        naksSent = 0;
        acksRx = 0;
        naksRx = 0;
        badFrames = 0;
        goodFrames = 0;
        timeouts = 0;
        if (debug) printf("LL: Connected\n");
        return 0;
    }
    else  // failed
    {
        connected = 0;  // record lack of connection
        printf("LL: Failed to connect, PHY returned code %d\n",retCode);
        return -retCode;  // return negative error code
    }
}


// ===========================================================================
/* Function to disconnect from other computer.
   It just calls PHY_close() and prints debug info.  */
int LL_discon(int debug)
{
    int retCode = PHY_close();  // try to disconnect
    connected = 0;  // assume no longer connected
    if (retCode == 0)   // check if succeeded
    {
        if (debug) // print all counters (don't know if were sending or receiving)
        {
            printf("LL: Disconnected.  Sent %d data frames\n", framesSent);
            printf("LL: Received %d good and %d bad frames, had %d timeouts\n",
                   goodFrames, badFrames, timeouts);
            printf("LL: Sent %d ACKs, %d NAKs\n", acksSent, naksSent);
            printf("LL: Received %d ACKs, %d NAKs\n", acksRx, naksRx);
        }
        return 0;

    }
    else  // failed
    {
        printf("LL: Failed to disconnect, PHY returned code %d\n", retCode);
        return -retCode;  // return negative error code
    }
}


// ===========================================================================
/* Function to send a block of data in a frame.
   Arguments:  Data block as array of bytes,
               number of bytes to send, debug.
   Return value is 0 on success, negative on failure.
   If connected, builds a frame, then sends the frame using PHY_send,
   then waits for a reply, up to time limit.
   What happens next is up to you...  */
int LL_send(byte *dataTx, int nData, int debug)
{
    static byte frameTx[3*MAX_BLK];  // array large enough for frame
    static byte frameAck[2*ACK_SIZE]; // twice expected ack frame size
    int nFrame = 0;         // size of frame
    int nAck = 0;           // size of ACK frame received
    int attempts = 0;       // number of attempts to send
    int success = 0;        // flag to indicate block sent and ACKed
    int retVal;  // return value from other functions

    // First check if connected
    if (connected == 0)
    {
        printf("LL: Attempt to send while not connected\n");
        return -10;  // error code
    }

    // Then check if block size OK - adjust limit for your design
    if (nData > MAX_BLK)
    {
        printf("LL: Cannot send block of %d bytes, max %d\n", nData, MAX_BLK);
        return -11;  // error code
    }

    // Build the frame
    nFrame = buildDataFrame(frameTx, dataTx, nData, seqNumTx);

    // Then loop, sending the frame and waiting for response
    do
    {

        // Send the frame, then check for problems
        retVal = PHY_send(frameTx, nFrame);  // send frame bytes
        if (retVal != nFrame)  // problem!
        {
            printf("LL: Block %d, failed to send frame\n", seqNumTx);
            return -12;  // error code
        }

        framesSent++;  // increment frame counter (for debug)
        attempts++;    // increment attempt counter, so don't try forever
        if (debug) printf("LL: Sent frame %d bytes, block %d, attempt %d\n",
                          nFrame, seqNumTx, attempts);

        // Now wait to receive a response
        nAck = getFrame(frameAck, 2*ACK_SIZE, TX_WAIT);
        if (nAck < 0)  // some problem receiving
        {
            return nAck;  // quit if error
        }
        if (nAck == 0)  // timeout
        {
            if (debug) printf("LL: Timeout waiting for response\n");
            timeouts++;  // increment counter for report

        }
        else  // we have received a frame - check it
        {
            if (checkFrame(frameAck, nAck) == 1)  // good frame
            {
                goodFrames++;  // increment counter for report
                // Need to check if this is a positive ACK,
                // and if it relates to the data block just sent...
                if ( frameAck[2] == seqNumTx ) // if ACK for correct block
                {
                    success = 1;                // job is done
                    acksRx++;                   // increment counter for report
                     // increment sequence number so as to send next frame
                }
                else /* if ACK for wrong block... */
                {
                    if (debug) printf("LL: Response received, seq %d\n", seqNumTx);
                    naksRx++;                   // increment counter for report

                }
            }
            else  // bad frame
            {
                badFrames++;  // increment counter for debug
                if (debug) printf("LL: Bad frame received\n");
                              // What should be done about that?
            }

        }  // end of received frame processing
    }
    while (!success && attempts < MAX_TRIES);  // repeat until succeed?

    if (success)  // data block has been sent and acknowledged
    {
        seqNumTx = next(seqNumTx);  // increment sequence number
        return 0;
    }
    else
    {
        if (debug) printf("LL: Block %d, tried %d times, failed\n",
                          seqNumTx, MAX_TRIES);
        return -15;  // tried enough times, giving up
    }

}  // end of LL_send


// ===========================================================================
/* Function to receive a frame and extract a block of data.
   Arguments:  array to hold data block,
               max size of data block.
   Return value is actual size of data block, or negative on error.
   If connected, tries to get frame from received bytes.
   If frame found, check if good frame, with expected sequence number.
   If so, extract data and return number of data bytes.
   If wrong data, bad frame or timeout, try again.  */
int LL_receive(byte *dataRx, int maxData, int debug)
{
    static byte frameRx[3*MAX_BLK];  // create array to hold frame
    int nData = 0;   // number of data bytes received
    int nFrame = 0;  // number of bytes in frame received
    int seqNumRx = 0;  // sequence number of received frame
    int success = 0;   // flag to indicate success
    int attempts = 0;  // attempt counter
    int expected = next(lastSeqRx);  // expected sequence number

    // First check if connected
    if (connected == 0)
    {
        printf("LL: Attempt to receive while not connected\n");
        return -10;  // error code
    }

    // Loop to receive a frame, repeats until good frame received
    do
    {
        // First get a frame, up to maximum size of array.
        // Function returns number of bytes in frame, or negative if error
        nFrame = getFrame(frameRx, 3*MAX_BLK, RX_WAIT);
        if (nFrame < 0)  // some problem receiving
        {
            return -9;  // quit if error
        }

        attempts++;  // increment attempt counter
        if (nFrame == 0)  // timeout
        {
            printf("LL: Timeout trying to receive frame, attempt %d\n",
								attempts);
        }
        else  // we have received a frame
        {
            if (debug) printf("LL: Got frame, %d bytes, attempt %d\n",
								        nFrame, attempts);

            // Next step is to check it for errors
            if (checkFrame(frameRx, nFrame) == 0 ) // frame is bad
            {
                if (debug) printf("LL: Bad frame received\n");
                printFrame(frameRx, nFrame);
                badFrames++;  // increment bad frame counter
                sendAck(BAD, lastSeqRx); // WE ADDED THIS IN***********************************


            }
            else  // we have a good frame - process it
            {
                goodFrames++;  // increment good frame counter
                nData = processFrame(frameRx, nFrame, dataRx, maxData, &seqNumRx);
                if (debug) printf("LL: Received block %d with %d data bytes\n",
                                  seqNumRx, nData);

                // Now check the sequence number
                if (seqNumRx == expected)  // expected data block
                {
                    success = 1;  // job is done
                    lastSeqRx = seqNumRx;  // update sequence number

                    sendAck(GOOD, lastSeqRx); // WE ADDED THIS IN
                    // ? send a response to the sender ?
                    // if so, what sequence number ?

                }
                else if (seqNumRx == lastSeqRx) // duplicate data block
                {
                    if (debug) printf("LL: Duplicate rx seq. %d, expected %d\n",
                                  seqNumRx, expected);
                    // what should be done about this?
                    sendAck(BAD, lastSeqRx); // WE ADDED THIS IN

                }
                else // some other data block??
                {
                    if (debug) printf("LL: Unexpected block rx seq. %d, expected %d\n",
                                  seqNumRx, expected);
                    sendAck(BAD, lastSeqRx); // WE ADDED THIS IN


                }  // end of sequence number checking

            }  // end of good frame processing

        } // end of received frame processing
    }
    while (!success && attempts < MAX_TRIES);

    if (success)  // have received good frame with expected sequence number
        return nData;  // return number of data bytes extracted from frame
    else // failed to get a good frame within limits
        return -15;  // this is an error situation - give up

}  // end of LL_receive


// ===========================================================================
/* Function to build a frame from a block of data.
   This function puts the header bytes into the frame,
   then copies in the data bytes.  Then it adds the trailer bytes.
   It keeps track of the total number of bytes in the frame,
   and returns this value to the calling function.
   Arguments: array to hold frame,
              array of data,
              number of data bytes to be sent,
              sequence number to include in header.
   Return value is number of bytes in the frame.  */
int buildDataFrame(byte *frameTx, byte *dataTx, int nData, int seq)
{
    int i, sum, check ;// for use in loop

    // Build the header
    frameTx[0] = STARTBYTE;  // start of frame marker
    frameTx[1] = nData + 5;    //number of bytes in frame
    frameTx[2] = (byte) seq;  // sequence number

    sum = (seq + nData + 5);  // This includes the sequence bytes in our checkbit

    // Copy data bytes into frame
    for (i = 0; i < nData; i++)
    {
        frameTx[3+i] = dataTx[i]; // copy the data byte
        sum += frameTx[3+i]; // summing bytes sent in order to compare to bytes received later
     }

    check = sum%200; // checkbyte to be added to trailer for error detection

    // Build the trailer - just end marker for now
    frameTx[3+nData] = check; // Adding checkbyte into trailer to detect errors
    frameTx[4+nData] = ENDBYTE; // end of frame marker byte

    // Return the size of the frame
    return 5+nData;
}


// ===========================================================================
/* Function to get a frame from the received bytes.
   Arguments: pointer to array of bytes to hold frame,
              maximum number of bytes to receive,
              time limit for receiving those bytes.
   Return value is number of bytes recovered, or negative if error. */
int getFrame(byte *frameRx, int maxSize, float timeLimit)
{
    int nRx = 0;  // number of bytes received so far
    int retVal = 0;  // return value from other functions

    timerRx = timeSet(timeLimit);  // set time limit to wait for frame

    // First search for the start of frame marker
    do
    {
        retVal = PHY_get(frameRx, 1); // get one byte at a time
        // Return value is number of bytes received, or negative for error
        if (retVal < 0) return retVal;  // check for error and give up
     }
    while (((retVal < 1) || (frameRx[0] != STARTBYTE)) && !timeUp(timerRx));
    // until we get a byte which is start of frame marker, or timeout

    // If we are out of time, return 0 - no useful bytes received
    if (timeUp(timerRx))
    {
        printf("LLGF: Timeout A with %d bytes received\n", nRx);
        return 0;
    }


    // If still within time limit, collect more bytes, until end marker
    nRx = 1;  // got 1 byte already
    retVal = PHY_get((frameRx + nRx), 1);  // get one byte at a time
    if (retVal < 0) return retVal;  // check for error and give up
    else nRx += retVal;  // update the bytes received count

    // If time up, no end marker, so bad frame, return 0
    if (timeUp(timerRx))
    {
        printf("LLGF: Timeout B with %d bytes received\n", nRx);
        return 0;
    }


    retVal = PHY_get((frameRx + nRx), (frameRx[1]-2));  // get remaining bytes in frame
    if (retVal < 0) return retVal;  // check for error and give up
    else nRx += retVal;  // update the bytes received count

    // If time up, no end marker, so bad frame, return 0
    if (timeUp(timerRx))
    {
        printf("LLGF: Timeout C with %d bytes received\n", nRx);
        return 0;
    }


    // Otherwise, we found the end marker
    return nRx;  // return number of bytes in frame
}  // end of getFrame


// ===========================================================================
/* Function to check a received frame for errors.
   Arguments: pointer to array of bytes holding frame,
              number of bytes in frame.
   As a minimum, should check error detecting code.
   This example also checks start and end markers.
   Returns 1 if frame is good, 0 otherwise.   */
int checkFrame(byte *frameRx, int nFrame)
{
    int i, sum=0, check; // For use in for loop

    if (frameRx[0] != STARTBYTE)  // check start merker
    {
        printf("LLCF: Frame bad - start marker\n");
        return 0;
    }

    // Check the end-of-frame marker
    if (frameRx[nFrame-1] != ENDBYTE)
    {
        printf("LLCF: Frame bad - end marker\n");
        return 0;
    }

	// Need to check the error-detecting code here
	// return 0 if the check fails...

	// Counting bytes in frame
    for (i = 1; i < nFrame-2; i++)
    {
        sum += frameRx[i]; // summing bytes received
    }

    check = sum%200; // checkbyte to be added to trailer for error detection

    // Comparing recieved no. of bytes to sent no. of bytes. if discrepancy print error and return 0.
	if (check != frameRx[nFrame-2])
	{
	    printf("CFLL: Error! Received bytes not equal to sent bytes");
	    return 0;
	}


    // If all tests passed, return 1
    return 1;
}  // end of checkFrame


// ===========================================================================
/* Function to process a received frame, to extract data.
   Frame has already been checked for errors, so this simple
   implementation assumes everything is where it should be.
   Arguments: pointer to array holding frame,
              number of bytes in the frame,
              pointer to array to hold data bytes,
              max number of bytes to extract,
              pointer to sequence number.
   Return value is number of bytes extracted. */
int processFrame(byte *frameRx, int nFrame,
                 byte *dataRx, int maxData, int *seqNum)
{
    int i = 0;  // for use in loop
    int nData;  // number of data bytes in frame

    // First get sequence number from its place in header
    *seqNum = frameRx[2];

    // Calculate number of data bytes, based on frame size
    nData = nFrame - 5;  // 4 bytes in header and trailer
    if (nData > maxData) nData = maxData;  // safety check

    // Now copy data bytes from middle of frame
    for (i = 0; i < nData; i++)
    {
        dataRx[i] = frameRx[3 + i];  // copy each byte
    }

    return nData;  // return size of block extracted
}  // end of processFrame


// ===========================================================================
/* Function to send an acknowledgement - positive or negative.
   Arguments: ack type, sequence number
   Returns 0 for success, negative for failure. */
int sendAck(int type, int seq)
{
    byte ackFrame[2*ACK_SIZE];  // twice expected frame size, for byte stuff
    int nFrame = 5; // number of bytes in frame so far
    int retVal; // return value

    // First build the frame
    ackFrame[0] = STARTBYTE;
    ackFrame[1] = 5;        //send 5 bytes of data to make our build frame to be compatible with our send ack frame.
    ackFrame[2] = seq;      //sequence number
    ackFrame[3] = seq+5;      //no modulo needed as sequence number between 0 and 15. In checkframe this value will equal 0+seq. i.e. seq
    ackFrame[4] = ENDBYTE;


    // Then send the frame
    retVal = PHY_send(ackFrame, nFrame);  // send the frame
    if (retVal != nFrame)  // problem!
    {
        printf("LL: Failed to send ack, seq. %d\n", seq);
        return -12;  // error code
    }
    else  // success - update the counters for report
    {
        if (type == GOOD)  acksSent++;
        else if (type == BAD) naksSent++;
        return 0;
    }
}


// ===========================================================================
/* Function to advance the sequence number,
   wrapping around at maximum value.  */
int next(int seq)
{
    return ((seq + 1) % MOD_SEQNUM);
}


// ===========================================================================
/* Function to set time limit at a point in the future.
   limit   is time limit in seconds (from now)  */
long timeSet(float limit)
{
    long timeLimit = clock() + (long)(limit * CLOCKS_PER_SEC);
    return timeLimit;
}  // end of timeSet


// ===========================================================================
/* Function to check if time limit has elapsed.
   timer  is timer variable to check
   returns 1 if time has reached or exceeded limit,
           0 if time has not yet reached limit.   */
int timeUp(long timeLimit)
{
    if (clock() < timeLimit) return 0;  // still within limit
    else return 1;  // time limit has been reached or exceeded
}  // end of timeUP


// ===========================================================================
/* Function to print bytes of a frame, in groups of 10.
   For small frames, print all the bytes,
   for larger frames, just the start and end*/
void printFrame(byte *frame, int nByte)
{
    int i, j;

    if (nByte <= 50)  // small frame - print all the bytes
    {
        for (i=0; i<nByte; i+=10)  // step in groups of 10 bytes
        {
            for (j=0; (j<10)&&(i+j<nByte); j++)
            {
                printf("%3d ", frame[i+j]);  // print as number
            }
            printf(":  ");  // separator
            for (j=0; (j<10)&&(i+j<nByte); j++)
            {
                printf("%c", frame[i+j]);   // print as character
            }
            printf("\n");   // new line
        }  // end for
    }
    else  // large frame - print start and end
    {
        for (j=0; (j<10); j++)  // first 10 bytes
            printf("%3d ", frame[j]);  // print as number
        printf(":  ");  // separator
        for (j=0; (j<10); j++)
            printf("%c", frame[j]); // print as character
        printf("\n - - -\n");   // new line, separator
        for (j=nByte-10; (j<nByte); j++)  // last 10 bytes
            printf("%3d ", frame[j]);  // print as number
        printf(":  ");  // separator
        for (j=nByte-10; (j<nByte); j++)
            printf("%c", frame[j]); // print as character
        printf("\n");   // new line
    }

}  // end of printFrame
