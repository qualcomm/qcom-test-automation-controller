#ifndef LIME_WRAPPER
#define LIME_WRAPPER

#include "LimeClient.h"

#ifdef WIN32
  #define DllExport   __declspec( dllexport )
#else
  #define DllExport
#endif

using namespace Lime;
#define LIME_MAX_MESSAGE_SIZE 4096
#define LIME_MAX_LOG_SIZE 65536


//! cLimeClient is a singleton.
//! Use this to get the instance of cLimeClient
extern cLimeClient* Lime_Client;

// Typedefs for callback methods
#ifndef WIN32
   typedef void(* ErrorCallback)  (char* errStr);
   typedef void(* WarningCallback)(char* warningStr);
   typedef void(* InfoCallback)   (char* infoStr);
#else
   typedef void(__stdcall * ErrorCallback)(char* errStr);
   typedef void(__stdcall * WarningCallback)(char* warningStr);
   typedef void(__stdcall * InfoCallback)(char* infoStr);
#endif

extern ErrorCallback Lime_ErrorCallback;
extern InfoCallback  Lime_InfoCallback;

class Lime_Callback : public cLimeCallback
{
    void error(std::string error)
    {
        if (Lime_ErrorCallback)
        {
            (*Lime_ErrorCallback)((char *) error.c_str());
        }
    }

    void info(std::string info)
    {
        if (Lime_InfoCallback)
        {
            (*Lime_InfoCallback)((char *)info.c_str());
        }
    }
};


extern "C" {

    //! Used to initialize the lime client with the session
    /*!
    \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_OTHER_ERROR
    */
    DllExport eLimeReturnCode Lime_Initialize(char* userId,
        char* storageLocation);

   //! Used to getVersion of the LimeCLient.
   /*!
   \return string Version of the LimeClient
   */
   DllExport char* Lime_GetVersion();
   
   //! Used to set asynchronous error callbacks.
   /*!
   \param pError    Function pointer to error callback
   \return
   */
   DllExport void Lime_SetErrorCallback(ErrorCallback pErrorCB);
   
   //! Used to set asynchronous warning callbacks.
   /*!
   \param pWarning    Function pointer to warning callback
   \return
   */
   DllExport void Lime_SetWarningCallback(WarningCallback pWarningCB);

   //! Used to set asynchronous info callbacks.
   /*!
   \param pInfo    Function pointer to info callback
   \return
   */
   DllExport void Lime_SetInfoCallback(InfoCallback pInfoCB);
   
    //! Used to generate Activate License Request.
    /*!
       \param productId GUID for specific tool eg. QXDM, APEX etc.
       \param licenseId License Identifier.
       \param activationRequest Reference to return activation Request
       \return eLimeReturnCode LIME_CLIENT_SUCCESS
    */
    DllExport eLimeReturnCode Lime_ActivateLicenseRequest
        (char* productId,
         char* licenseId,
         char* activateLicenseRequest,
         int   activateLicenseRequestSize);

    //! Used to provide Activate License Response.
    /*!
      \param productId GUID for specific tool eg. QXDM, APEX etc.
      \param licenseId License Identifier.
      \param activationRequest Reference to return activation Request
      \return eLimeReturnCode LIME_CLIENT_SUCCESS
    */
    DllExport eLimeReturnCode Lime_ActivateLicenseResponse
        (char* activateLicenseResponse,
         char* activationId,
         int   activationIdSize,
         char* errorMessage,
         int   errorMessageSize);

    //! Used to generate deActivate License Request.
    /*!
        \param activationId Activation Identifier
        \return eLimeReturnCode LIME_CLIENT_SUCCESS
    */
    DllExport eLimeReturnCode Lime_DeactivateLicenseRequest(char* activationId,
        char* deactivateLicenseRequest,
        int deactivateLicenseRequestSize);

    //! Used to generate deActivate License Response
    /*!
      \param activationId Activation Identifier
      \return eLimeReturnCode LIME_CLIENT_SUCCESS
    */
    DllExport eLimeReturnCode Lime_DeactivateLicenseResponse
        (char* deactivateLicenseResponse,
         char* errorMessage,
         int   errorMessageSize);

    //! Used to get active Licenses.
    /*!
      \param activationIds Reference to return Activation Identifiers
      \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_CANNOT_REACH_SERVER
    */
    DllExport eLimeReturnCode Lime_GetActiveLicenses
        (char* activationIds,
         int   activationIdsSize);

    //! Used to get all Licenses.
    /*!
    \param activationIds Reference to return Activation Identifiers
    \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_CANNOT_REACH_SERVER
    */
    DllExport eLimeReturnCode Lime_GetLicenses
        (char* activationIds,
        int   activationIdsSize);

    //! Used to get License Information.
    /*!
      \param activationId Activation Identifier
      \param license Reference to return license information
      \return eLimeReturnCode LIME_SUCCESS, LIME_CANNOT_REACH_SERVER
                                        LIME_INVALID_ID
    */
    DllExport eLimeReturnCode Lime_GetLicenseInfo
        (char* activationId,
         char* license,
         int   licenseSize);

    //! Used to Check Feature Availability
    /*!
      \param productId Product Identifier
      \param featureId Feature Identifier
      \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_INVALID_ID,
                                        LIME_INVALID_LICENSE
    */
    DllExport eLimeReturnCode Lime_CheckLicense
        (char* productId,
         char* featureId);

    //! Used to get Offline verification request
    /*!
      \param verReq Reference to return verification request
      \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_ENCODE_ERROR
    */
    DllExport eLimeReturnCode Lime_VerifyLicenseRequest
        (char* verifyLicenseRequest,
         int   verifyLicenseRequestSize);

    //! Used to set Offline verification response
    /*!
      \param verResp Verification Response
      \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_DECODE_ERROR
    */
    DllExport eLimeReturnCode Lime_VerifyLicenseResponse
        (char* verifyLicenseResponse,
         char* errorMessage,
         int   errorMessageSize);

    //! Used to get Error string for logging
    /*!
    \param errorString reference to return error string
    \param errorStringSize max size of error string
    \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_DECODE_ERROR
    */
    DllExport eLimeReturnCode Lime_GetErrorString
        (char* errorString, 
             int   errorStringSize);

  //! Used to recover all licenses for a user for a machine.
  /*!
  \param userId for requesting user
  \param computerKey for requesting user
  \param activationRequest Reference to return activation Request
  \return eLimeReturnCode LIME_CLIENT_SUCCESS
  */
  DllExport eLimeReturnCode Lime_RecoverLicenseRequest
        (char* recoverLicenseRequest,
         int   recoverLicenseRequestSize);

  //! Used to provide Activate License Response.
  /*!
      \param recover License Response
      \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_DECODE_ERROR
  */
  DllExport eLimeReturnCode Lime_RecoverLicenseResponse
        (char* recoverLicenseResponse,
         char* errorMessage,
         int   errorMessageSize);

  //!  Used to get active Licenses of a feature.
  /*!
    \param productId Product Identifier
    \param featureId Feature Identifier
    \param activationIds Reference to return Activation Identifiers
    \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_LICENSE_UNAVAILABLE
 */
  DllExport eLimeReturnCode Lime_GetActiveLicensesByFeature
        (char* productId,
         char * featureId,
         char * activationIds,
         int activationidListLength
         );

  //!  Used to remove Licenses from the machine on logout
  /*!
    \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_FILE_ACCESS_ERROR
 */
  DllExport eLimeReturnCode Lime_DeleteLicenseFiles();

  //!  Used to set a lock on install state
  /*!
    \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_FILE_ACCESS_ERROR
 */
  DllExport eLimeReturnCode Lime_SetLockState(char* key,
                                              unsigned int state);

  //!  Used to get a lock on install state
  /*!
    \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_FILE_ACCESS_ERROR
 */
  DllExport eLimeReturnCode Lime_GetLockState(char *stateInfo,
                                              int   stateInfoLength);
}
#endif
