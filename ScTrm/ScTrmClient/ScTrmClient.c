// ScTrmClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main()
{
    // Fixed defined constants
    const char dispAuth[] = "SecretDisplayAuthorization";
    const char fpReaderAuth[] = "SecretFPReaderAuthorization";
    const unsigned char ekName[0x22] = {0x00, 0x0b, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f };
    const char message[] = "Please Authorize XYZ.";

    // Insecure side: variables
    unsigned int pubContext = 0;

    // Secure side: variables
    ScTrmResult_t secReturn = ScTrmResult_Ongoing;
    ScTrmStateObject_t secState = { 0 };

    // Secure side: Fill out the parameters for the call
    secState.param.func.GetConfirmation.displayAuth.t.size = (UINT16)strlen(dispAuth); // Display authorization
    strcpy_s((char*)secState.param.func.GetConfirmation.displayAuth.t.buffer,
        sizeof(secState.param.func.GetConfirmation.displayAuth.t.buffer),
        dispAuth);
    secState.param.func.GetConfirmation.fpReaderAuth.t.size = (UINT16)strlen(fpReaderAuth); // FP reader authorization
    strcpy_s((char*)secState.param.func.GetConfirmation.fpReaderAuth.t.buffer,
        sizeof(secState.param.func.GetConfirmation.fpReaderAuth.t.buffer),
        fpReaderAuth);
    secState.param.func.GetConfirmation.displayMessage.t.size = (UINT16)strlen(message) + 1; // Include the terminator
    strcpy_s((char*)secState.param.func.GetConfirmation.displayMessage.t.buffer,
        sizeof(secState.param.func.GetConfirmation.displayMessage.t.buffer),
        message);
    secState.param.func.GetConfirmation.ekName.t.size = (UINT16)sizeof(ekName); // Expected EK to ensure we are talking to the right device
    memcpy(secState.param.func.GetConfirmation.ekName.t.name, ekName, sizeof(ekName));
    secState.param.func.GetConfirmation.timeout = 20 * 1000; // 20 second timeout to wait for a fingerprint

    // Secure side: Start the operation ping-pong
    do
    {
        // Secure side: Crank the state machine 
        if ((secReturn = ScTrmGetConfirmation(&secState)) == ScTrmResult_Ongoing)
        {
            // Insecure side: Execute the command
            if ((!pubContext) &&
                ((pubContext = ScTrm_Open()) == 0))
            {
                secReturn = ScTrmResult_CommError;
                break;
            }
            if (!ScTrm_Execute(pubContext, secState.param.pbCmd, secState.param.cbCmd, secState.param.pbRsp, sizeof(secState.param.pbRsp), &secState.param.cbRsp))
            {
                secReturn = ScTrmResult_CommError;
                break;
            }
        }
    } while (secReturn == ScTrmResult_Ongoing);
    // Secure side: The ping-pong has completed, let's parse the result to see what happend
    if (secReturn >= 0 && secReturn <= ScTrmResult_MatchMax)
    {
        printf("Finger %u recognized, operation confirmed.\n", secReturn);
    }
    else if (secReturn == ScTrmResult_Unrecognized)
    {
        printf("Unrecognized finger pressed, operation canceled.\n");
    }
    else if (secReturn == ScTrmResult_Timeout)
    {
        printf("No finger pressed, operation canceled.\n");
    }
    else
    {
        printf("Error occurred.\n");
    }

    // Insecure side: Release the insecure resources
    if (pubContext != 0)
    {
        ScTrm_Close(pubContext);
        pubContext = 0;
    }
    return 0;
}

