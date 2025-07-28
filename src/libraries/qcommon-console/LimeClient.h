/*===========================================================================
FILE:
   LimeClient.h

DESCRIPTION:
   Defines the API for the License Management Client

Copyright (C) 2020 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#include <ctime>
#include <string>
#include <list>
#include <map>
namespace Lime
{
#ifndef LIME_DEFS
#define LIME_DEFS

  /*!======================================================================*/
  //! enum eLimeReturnCode
  //!    enumeration of LimeReturnCodes
  /*!======================================================================*/
  enum eLimeReturnCode
  {
    LIME_CLIENT_SUCCESS      = 0,
    LIME_FILE_ACCESS_ERROR   = 1,
    LIME_INVALID_MACHINE     = 2,
    LIME_INVALID_ID          = 3,
    LIME_LICENSE_UNAVAILABLE = 4,
    LIME_DECODE_ERROR        = 5,
    LIME_ENCODE_ERROR        = 6,
    LIME_SERVER_ERROR        = 7,
    LIME_PARSING_ERROR       = 8,
    LIME_STALE_UPDATE        = 9,
    LIME_STALE_LICENSE_KEY   = 10,
    LIME_AUTHENTICATION_ERROR= 11,
    LIME_CONNECTION_ERROR    = 12,
    LIME_INVALID_VERSION     = 13,
    LIME_NO_RELEASE_NOTES    = 14,
    LIME_NO_DISK_SPACE       = 15,
    LIME_INVALID_KEY         = 16,
    LIME_OTHER_ERROR         = 99
  };

  enum eLimeLockState
  {
      LIME_UNLOCKED = 0,
      LIME_LOCKED   = 1
  };

  /*!======================================================================*/
  //! Class cLimeFeatures
  //!    Provides details for each feature
  /*!======================================================================*/
  class cLimeFeature
  {
    public:
      cLimeFeature( std::string featureId,
                    std::string featureType,
                    std::string featureName,
                    std::string productId,
                    std::string startDate,
                    std::string endDate,
                    unsigned int duration );

      //! Used to get the ID for this feature
      std::string getFeatureId();

      //! Used to get the type of this feature
      std::string getFeatureType();

      //! Used to get the name of this feature
      std::string getFeatureName();

      //! Used to get the product ID for this feature
      std::string getProductId();

      //! Used to get the start date for this feature
      tm getStartDate();

      //! Used to get the end date for this feature
      tm getEndDate();

      //! Used to get the total duration (in days) this feature is available
      unsigned int getDuration();

      //Destructor
      ~cLimeFeature();

    protected:
      std::string mFeatureId;
      std::string mFeatureType;
      std::string mFeatureName;
      std::string mProductId;
      tm mStartDate;
      tm mEndDate;
      unsigned int mDuration;
  };

  /*!======================================================================*/
  //! Class cLimeLicense
  //!    Provides details for each license
  /*!======================================================================*/
  class cLimeLicense
  {
    public:
      //! Basic constructor
      cLimeLicense();

      //! Constructor
      cLimeLicense( std::string activationId,
                    std::string licenseGroupId,
                    std::string computerKey,
                    std::string licenseStateId,
                    std::string username,
                    std::string partnerId,
                    std::string activatedDate,
                    unsigned int daysUsed,
                    std::list<cLimeFeature> features );
         
      // Destructor
      ~cLimeLicense();

      //! Used to get the activation ID
      std::string getActivationId();

      //! Used to get the license group ID
      std::string getLicenseGroupId();

      //! Used to get the computer key
      std::string getComputerKey();

      //! Used to get the license state ID
      std::string getLicenseStateId();

      //! Used to get the username
      std::string getUsername();

      //! Used to get the partner ID
      std::string getPartnerId();

      //! Used to get the activation date
      tm getActivationDate();

      //! Used to get the number of days license has been used
      unsigned int getDaysUsed();

      //! Used to get the list of features
      std::list<cLimeFeature> getFeatures();
     
    protected:
         
      std::string mActivationId;
      std::string mLicenseGroupId;
      std::string mComputerKey;
      std::string mLicenseStateId;
      std::string mUsername;
      std::string mPartnerId;
      tm mActivatedDate;
      unsigned int mDaysUsed;
      std::list <cLimeFeature> mFeatures;
  };

  /*======================================================================*/
  // Class cLimeLicense
  //    Provides callbacks when required
  /*======================================================================*/
  class cLimeCallback
  {
    public:
      virtual void licenseDeactivated( std::string licenseId,
                                       std::string activationId );

      virtual void error( std::string errorString );

      virtual void warning( std::string warningString );

      virtual void info( std::string infoString );
  };

  /*======================================================================*/
  // Class cLimeLicense
  //    Provides all licensing functionality
  /*======================================================================*/
  class cLimeClient
  {
    public:
      //! cLimeClient is a singleton.
      //! Use this to get the instance of cLimeClient
      static cLimeClient * getInstance();

      //! Used to initialize the lime client with the session
      /*!
          \param pSession Pointer to cSession
          \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_OTHER_ERROR
          !!!!!!!!THIS FUNCTION IS NOT THREADSAFE!!!!!!!!!
      */
      eLimeReturnCode initialize( std::string userId, 
                                  std::string storageLocation );


      //! Used to getVersion of the LimeCLient.
      /*
          \return string Version of the LimeClient
      */
      std::string getVersion();

      //! Used to set class for asynchronous callbacks.
      /*!
         \param callback    Class defining callback methods
         \return
      */
      void setCallback( cLimeCallback * pCallback );

      //! Used to set function pointer for error callbacks.
      /*!
        \param callback    function pointer for error callbacks
        \return
      */
      void setErrorCallback(cLimeCallback * pCallback);

      //! Used to set function pointer for debug callbacks.
      /*!
         \param callback    function pointer for debug callbacks
         \return
      */
      void setInfoCallback(cLimeCallback * pCallback);

      //! Used to generate Host Id. 
      /*
            \param hostId Reference to return HostID
            \return eLimeReturnCode LIME_CLIENT_SUCCESS
      */
      eLimeReturnCode generateHostId(std::string& hostId);

      //! Used to generate Activate License Request.
      /*!
            \param productId GUID for specific tool eg. QXDM, APEX etc.
            \param licenseId License Identifier.
            \param activationRequest Reference to return activation Request
            \return eLimeReturnCode LIME_CLIENT_SUCCESS
      */
      eLimeReturnCode activateLicenseRequest(std::string  productId,
                                             std::string  licenseId,
                                             std::string& activateLicenseRequest,
                                             std::string hostId="" );

      //! Used to provide Activate License Response.
      /*!
      \param productId GUID for specific tool eg. QXDM, APEX etc.
      \param licenseId License Identifier.
      \param activationRequest Reference to return activation Request
      \return eLimeReturnCode LIME_CLIENT_SUCCESS
      */
      eLimeReturnCode activateLicenseResponse(std::string  activateLicenseResponse,
                                              std::string& activationId,
                                              std::string& errorMessageString);
      
      //! Used to generate deActivate License Request.
          /*!
             \param activationId Activation Identifier
             \return eLimeReturnCode LIME_CLIENT_SUCCESS
          */
      eLimeReturnCode deactivateLicenseRequest(std::string  activationId,
                                               std::string& deActivationRequest,
                                               std::string hostId="" ); 
 
      //! Used to generate deActivate License Request.
      /*!
      \param activationId Activation Identifier
      \return eLimeReturnCode LIME_CLIENT_SUCCESS
      */
      eLimeReturnCode deactivateLicenseResponse(std::string  deActivationResponse,
                                                std::string& errorMessageString); 
 
      //! Used to get active Licenses.
      /*!
          \param activationIds Reference to return Activation Identifiers
          \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_CANNOT_REACH_SERVER
      */
      eLimeReturnCode getActiveLicenseList( std::list<std::string>& activationIds );
 
      //! Used to get active Licenses.
      /*!
      \param activationIds Reference to return Activation Identifiers
      \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_CANNOT_REACH_SERVER
       */
      eLimeReturnCode getActiveLicenses(std::string& activationIds);

      //! Used to get all Licenses.
      /*!
          \param activationIds Reference to return Activation Identifiers
          \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_CANNOT_REACH_SERVER
      */
      eLimeReturnCode getLicenseList(std::list<std::string>& activationIds);

      //! Used to get all Licenses.
      /*!
          \param activationIds Reference to return Activation Identifiers
          \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_CANNOT_REACH_SERVER
      */
      eLimeReturnCode getLicenses(std::string& activationIds);
          
      //! Used to get licenses for a particular product
      /*!
        \param activationIds Reference to return Activation Identifiers
        \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_CANNOT_REACH_SERVER
      */
      eLimeReturnCode getLicensesByProduct(std::string  productId,
                                           std::string& activationIds);

      //! Used to get active Licenses of a feature.
      /*!
         \param productId Product Identifier
         \param featureId Feature Identifier
         \param activationIds Reference to return Activation Identifiers
         \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_LICENSE_UNAVAILABLE
      */
      eLimeReturnCode getActiveLicensesByFeature(
                                 std::string  productId,
                                 std::string  featureId,
                                 std::string& activationIds);

      //! Used to get License Information.
      /*!
          \param activationId Activation Identifier
          \param license Reference to return license information
          \return eLimeReturnCode LIME_SUCCESS, LIME_CANNOT_REACH_SERVER
                                     LIME_INVALID_ID
      */
      eLimeReturnCode getLicenseInfo( std::string activationId,
                                      std::string& license );

      //! Used to Check Feature Availability
      /*!
         \param productId Product Identifier
         \param featureId Feature Identifier
         \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_INVALID_ID,
                                 LIME_INVALID_LICENSE
      */
      eLimeReturnCode checkLicense( std::string productId,
                                    std::string featureId );

      //! Used to get Offline verification request
      /*!
         \param verReq Reference to return verification request
         \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_ENCODE_ERROR
      */
      eLimeReturnCode verifyLicenseRequest( std::string& verifyLicenseRequest,
                                            std::string hostId=""  );

      //! Used to get Offline verification request from Activation IDs
      /*!
         \param verReq Reference to return verification request
         \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_ENCODE_ERROR
      */
      eLimeReturnCode verifyLicenseRequestFromActivationIds( 
                                            std::string& verifyLicenseRequest,
                                            std::list<std::string>& activationIds,
                                            std::string hostId="" );

      //! Used to set Offline verification response
      /*!
          \param verResp Verification Response
          \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_DECODE_ERROR
      */
      eLimeReturnCode verifyLicenseResponse(std::string  verifyLicenseResponse,
                                            std::string& errorMessageString);

      //! Used to get Offline recover request
      /*!
        \param recoverReq Reference to return verification request
        \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_ENCODE_ERROR
      */
      eLimeReturnCode recoverLicenseRequest(std::string& recoverLicenseRequest,
                                            std::string hostId="" );

      //! Used to set Offline recover response 
      /*!
        \param recoverResp Recover Response
        \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_DECODE_ERROR
       */
      eLimeReturnCode recoverLicenseResponse(std::string  recoverLicenseResponse,
                                             std::string& errorMessageString);

      
      //!Used to get a licenseKey to be verified by verifyLicenseKey
      eLimeReturnCode getLicenseKey(std::string  productId,
                                    std::string  featureId,
                                    std::string& licenseKey);

      //!Used to verify the license Key generated by getLicenseKey
      eLimeReturnCode verifyLicenseKey(std::string  licenseKey,
                                       std::string& productId,
                                       std::string& featureId,
                                       std::string& timestamp);

      //!Used to get license expiry date
      /*!
         \param productId Product Identifier
         \param featureId Feature Identifier
         \param Reference to return License Expiry date
         \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_LICENSE_UNAVAILABLE
      */
      eLimeReturnCode getActiveLicenseExpiryDate(std::string  productId,
                                                 std::string  featureId,
                                                 std::string& expiryDate);

      //!Used to get num of days left before the license expires
      /*!
         \param productId Product Identifier
         \param featureId Feature Identifier
         \param Reference to return Num of days remaining for license expiry
         \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_LICENSE_UNAVAILABLE
      */
      eLimeReturnCode getDaysRemaining(std::string   productId,
                                       std::string   featureId,
                                       unsigned int& days);

      //!Used to set the install lock state
      /*!
         \param key key to set the lock state
         \param state INIT, INSTALLING_TO_INIT, INSTALLING_TO_LOCKED, LOCKED
         \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_ENCODE_ERROR
      */
      eLimeReturnCode setLockState(std::string key, eLimeLockState state);

      //!Used to get the install lock state
      /*!
         \param key key to retrieve the lock state
         \param Reference to lock state 
         \return eLimeReturnCode LIME_CLIENT_SUCCESS, LIME_DECODE_ERROR
      */
      eLimeReturnCode getLockState(std::string& stateInfo);

      //! Used to delete all license files 
      eLimeReturnCode deleteLicenseFiles();
 
      //! Used to get active Licenses for a product
      eLimeReturnCode getActiveLicenseListByProduct(std::string productId, 
                                                    std::map<std::string,std::string>& activationIds);
      //! Used to get error string
      static std::string getErrorString();

    private:
      //! Constructor
      cLimeClient();

      //! Deconstructor
      ~cLimeClient();

      //! Static pointer to single instance of this class
      static cLimeClient * mpLimeClient;
  };
#endif // LIME_DEFS
}
