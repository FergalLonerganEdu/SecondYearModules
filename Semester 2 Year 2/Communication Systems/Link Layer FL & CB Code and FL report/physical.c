/*  Physical Layer functions using serial port.
       PHY_open    opens and configures the port
       PHY_close   closes the port
       PHY_send    sends bytes
       PHY_get     gets received bytes
    All functions print explanatory messages if there is
    a problem, and return values to indicate failure.
    This version uses standard C functions.  */

#include <stdio.h>   // needed for printf
#include <windows.h>  // needed for port functions
#include <stdlib.h>  // for random number functions
#include <math.h>    // for pow function
#include <time.h>    // for time function, used to seed rand
#include "physical.h"  // header file for these functions


/* Creating a variable this way allows it to be shared
   by the functions in this file only.  */
static HANDLE serial = INVALID_HANDLE_VALUE;  // handle for serial port
static double rxProbErr = 0.0; // probability of error, used in PHY_get()

/* PHY_open function - to open and configure the serial port.
   Arguments are port number, bit rate, number of data bits, parity,
   receive timeout constant, rx timeout interval, rx probability of error.
   See comments below for more detail on timeouts.
   Returns zero if it succeeds - anything non-zero is a problem.*/
int PHY_open(int portNum,       // port number: e.g. 1 for COM1, 5 for COM5
             int bitRate,       // bit rate: e.g. 1200, 4800, etc.
             int nDataBits,     // number of data bits: 7 or 8
             int parity,        // parity: 0 = none, 1 = odd, 2 = even
             int rxTimeConst,   // rx timeout constant in ms: 0 waits forever
             int rxTimeIntv,    // rx timeout interval in ms: 0 waits forever
             double probErr)    // rx probability of error: 0.0 for none
{
    // Define variables
    int bitRatio, bitRatioValid, i;  // for bit rate checking
    DCB serialParams = {0};  // Device Control Block (DCB) for serial port
    COMMTIMEOUTS serialTimeLimits = {0};  // COMMTIMEOUTS structure for port
    int timeMult;   // for calculating time limits
    char portName[10];  // string to hold port name

    // First check that parameters given are valid - first bit rate
    bitRatio = bitRate/1200;  // all valid rates are multiples of 1200
    if (bitRate != bitRatio*1200) // bit rate is not multiple of 1200
    {
        printf("PHY: Invalid bit rate requested: %d\n", bitRate);
        return 3;
    }
    bitRatioValid = 0;
    for (i=1; i<=32; i*=2)
    {
        if (bitRatio == i)   // restrict to ratios that are powers of 2
            bitRatioValid = 1;
    }
    if (bitRatioValid==0)
    {
        printf("PHY: Invalid bit rate requested: %d\n", bitRate);
        return 3;
    }

    // now check number of data bits
    if ((nDataBits!=7) && (nDataBits!= 8))
    {
        printf("PHY: Invalid number of data bits: %d\n", nDataBits);
        return 3;
    }

    // now check parity
    if ((parity<0) || (parity>2))
    {
        printf("PHY: Invalid parity requested: %d\n", parity);
        return 3;
    }

    // Make port name string, by adding port number to letters COM
    sprintf(portName, "COM%d", portNum);  // print to string

    // Try to open the port
    serial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE,
                                0, 0, OPEN_EXISTING, 0, 0);
    // Check for failure
    if (serial == INVALID_HANDLE_VALUE)
    {
        printf("PHY: Failed to open port\n");
        printError();  // give details of the error
        return 1;  // non-zero return value indicates error
    }

    // Set length of device control block before use
    serialParams.DCBlength = sizeof(serialParams);

    // Fill the DCB with the parameters of the port, and check for failure
    if (!GetCommState(serial, &serialParams))
    {
        printf("PHY: Error getting port parameters\n");
        printError();  // give details of the error
        CloseHandle(serial);
        return 2;
    }

    /* Change the parameters to configure the port as required,
       without interpreting or substituting any characters,
       and with no added flow control.  */
    serialParams.BaudRate = bitRate;    // bit rate
    serialParams.ByteSize = nDataBits;  // number of data bits in group
    serialParams.Parity = parity;      // parity mode
    serialParams.StopBits = ONESTOPBIT;  // just one stop bit
    serialParams.fOutxCtsFlow = FALSE;   // ignore CTS signal
    serialParams.fOutxDsrFlow = FALSE;   // ignore DSR signal on transmit
    serialParams.fDsrSensitivity = FALSE; // ignore DSR signal on receive
    serialParams.fOutX = FALSE;           // ignore XON/XOFF on transmit
    serialParams.fInX = FALSE;           // do not send XON/XOFF on receive
    serialParams.fParity = FALSE;        // ignore parity on receive
    serialParams.fNull = FALSE;   // do not discard null bytes on receive

    // Apply the new parameters to the port
    if (!SetCommState(serial, &serialParams))
    {
        printf("PHY: Error setting port parameters\n");
        printError();  // give details of the error
        CloseHandle(serial);
        return 4;
    }

/*  Set timeout values for write and read functions.  If the requested
    number of bytes  have not been sent or receiveed in the time allowed,
    the function returns anyway.  The number of bytes actually written or
    read will be returned, so the problem can be detected.
    Time allowed for write operation, in ms, is given by
    WriteTotalTimeoutConstant + WriteTotalTimeoutMultipier * no. bytes requested.
    If both values are zero, timeout is not used - waits forever.
    Time allowed for read operation is similar.  The read function can also
    return if the time interval between bytes, in ms, exceeds ReadIntervalTimeout.
    This can be useful when waiting for a large block of data, and only a small
    block of data arrives - the read function can return promptly with the data.

    In this function, time multipliers are derived from the bit rate, allowing
    11 transmitted bits per data byte (the maximum possible value).
    Transmit constant is 100 ms. Receive constant is set by user - a value of 0
    forces the receive multiplier to 0 also, to disable receive time limits.*/

    // Calculate multiplier, based on bit rate, minimum 1 ms
    timeMult = 1 + 11000/bitRate;  // 10 ms at 1200, 1 ms above 9600 bit/s

    // Set the transmit timeout values.
    serialTimeLimits.WriteTotalTimeoutConstant = 100;
    serialTimeLimits.WriteTotalTimeoutMultiplier = (DWORD)timeMult;

    // Modify multiplier for receive if necessary
    if (rxTimeConst==0) timeMult = 0;

    // Set receive timeout values
    serialTimeLimits.ReadTotalTimeoutConstant = (DWORD)rxTimeConst;
    serialTimeLimits.ReadTotalTimeoutMultiplier = timeMult;
    serialTimeLimits.ReadIntervalTimeout = (DWORD)rxTimeIntv;

    // Apply the time limits to the port
    if (!SetCommTimeouts(serial, &serialTimeLimits))
    {
        printf("PHY: Error setting timeouts\n");
        printError();  // give details of the error
        CloseHandle(serial);
        return 5;
    }

    // Clear the receive buffer, in case there is rubbish waiting
    if (!PurgeComm(serial, PURGE_RXCLEAR))
    {
        printf("PHY: Error purging receive buffer\n");
        printError();  // give details of the error
        CloseHandle(serial);
        return 6;
    }

    /* Set up simulated errors on receive path:
       Set the seed for the random number generator,
       and check the probability of error value. */
    srand(time(NULL));  // get time and use as seed
    if ((probErr>=0.0) && (probErr<=1.0))  // check valid
        rxProbErr = probErr; // pass value to shared variable

    // If we get this far, the port is open and configured
    return 0;
}

//===================================================================
/* PHY_close function, to close the serial port.
   Takes no arguments, returns 0 always.  */
int PHY_close()
{
    CloseHandle(serial);
    return 0;
}

//===================================================================
/* PHY_send function, to send bytes.
   Arguments: pointer to array holding bytes to be sent;
              number of bytes to send.
   Returns number of bytes sent, or negative value on error.  */
int PHY_send(byte *dataTx, int nBytesToSend)
{
     DWORD nBytesTx;  // double-word - number of bytes actually sent
     int nBytesSent;    // integer version of the same

    // First check if the port is open
    if (serial == INVALID_HANDLE_VALUE)
    {
        printf("PHY: Port not valid\n");
        return -9;  // negative return value indicates error
    }

    // Try to send the bytes as requested
    if (!WriteFile(serial, dataTx, nBytesToSend, &nBytesTx, NULL ))
    {
        printf("PHY: Error sending data\n");
        printError();  // give details of the error
        CloseHandle(serial);
        return -5;
    }
    else if ((nBytesSent = (int)nBytesTx) != nBytesToSend)  // check for timeout
    {
        printf("PHY: Timeout in transmission, sent %d of %d bytes\n",
               nBytesSent, nBytesToSend);
    }
    return nBytesSent; // if no error, return number of bytes sent
    // note that timeout is not regarded as error
}

//===================================================================
/* PHY_get function, to get received bytes.
   Arguments: pointer to array to hold received bytes;
              maximum number of bytes to get.
   Returns number of bytes actually got, or negative value on error.  */
int PHY_get(byte *dataRx, int nBytesToGet)
{
     DWORD nBytesRx;  // double-word - number of bytes actually got
     int nBytesGot;      // integer version of above
     int threshold = 0;  // threshold for error simulation
     int i;             // for use in loop
     int flip;          // bits to change in simulating error

    // First check if the port is open
    if (serial == INVALID_HANDLE_VALUE)
    {
        printf("PHY: Port not valid\n");
        return -9;  // negative return value indicates error
    }
    // Try to get bytes as requested
    if (!ReadFile(serial, dataRx, nBytesToGet, &nBytesRx, NULL ))
    {
        printf("PHY: Error receiving data\n");
        printError();  // give details of the error
        CloseHandle(serial);
        return -4;
    }
    // No need to complain about timeout here - will happen regularly

    nBytesGot = (int) nBytesRx;  // convert to integer

    // Add an error, with specified probability
    if (rxProbErr != 0.0)
    {
        // set threshold as fraction of max, scaling for 8 bit bytes
        threshold = 1 + (int)(8.0 * (double)RAND_MAX * rxProbErr);
        for (i = 0; i < nBytesGot; i++)
        {
            if (rand() < threshold)  // we want to cause an error
            {
                flip = rand() % 8;  // random integer 0 to 7
                flip = pow(2.0, flip); // bit pattern with single 1
                dataRx[i] ^= (byte) flip;  // invert a bit
                printf("PHY_get:  ####  Simulated error...  ####\n");
            }
        }
    }

    return nBytesGot; // if no problem, return number of bytes we got
}

/* Function to print informative error messages
   when something goes wrong...  */
void printError(void)
{
	char lastError[1024];
	int errCode;
	errCode = GetLastError();  // get the error code for the last error
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		lastError, 1024, NULL);  // convert error code to error message
	printf("PHY: Code %d = %s\n", errCode, lastError);
}
