/*******************************************************************************
 * Copyright (c) 2019 fortiss GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *    Jose Cabral - initial implementation
 *******************************************************************************/

#include "opcua_client_config_parser.h"
#include "arch/devlog.h"

bool CUA_ClientConfigFileParser::loadConfig(std::string &paFileLocation, std::string &paEndpoint, UA_ConfigFromFile &paResult) {
  bool retVal = true;
  UA_StatusCode retValOpcUa = UA_STATUSCODE_GOOD;

  CConfigFileParser configFileParser(paFileLocation);
  bool endpointFound = false;
  std::string endpointKey = "endpoint";

  if(CConfigFileParser::lookForKeyValueInFile(configFileParser, endpointKey, paEndpoint, endpointFound)) {
    if(endpointFound) {

#ifdef UA_ENABLE_ENCRYPTION
# ifdef FORTE_COM_OPC_UA_MASTER_BRANCH
      UA_ByteString certificateFileContent = UA_STRING_NULL;
      UA_ByteString privateKeyFileContent = UA_STRING_NULL;
      UA_MessageSecurityMode securityMode = UA_MESSAGESECURITYMODE_INVALID;
      UA_String securityPolicyUri = UA_STRING_NULL;
# else // FORTE_COM_OPC_UA_MASTER_BRANCH

# endif // FORTE_COM_OPC_UA_MASTER_BRANCH
#endif //UA_ENABLE_ENCRYPTION

      bool moreLinesToRead = true;
      while(moreLinesToRead) {
        std::pair<std::string, std::string> resultPair;

        switch(configFileParser.parseNextLine(resultPair)){
          case CConfigFileParser::eOk:
            if(0 == resultPair.first.compare("endpoint")) {
              moreLinesToRead = false;
            } else if(0 == resultPair.first.compare("username")) {
              paResult.mUsername = resultPair.second;
            } else if(0 == resultPair.first.compare("password")) {
              paResult.mPassword = resultPair.second;
            }
#ifdef UA_ENABLE_ENCRYPTION
# ifdef FORTE_COM_OPC_UA_MASTER_BRANCH
            else if(0 == resultPair.first.compare("certificate")) {
              retVal = loadFileIntoBytestring(resultPair.second, certificateFileContent);
            } else if(0 == resultPair.first.compare("privateKey")) {
              retVal = loadFileIntoBytestring(resultPair.second, privateKeyFileContent);
            } else if(0 == resultPair.first.compare("securityMode")) {
              securityMode = static_cast<UA_MessageSecurityMode>(atoi(resultPair.second.c_str()));
            } else if(0 == resultPair.first.compare("securityPolicy")) {
              securityPolicyUri = UA_STRING_ALLOC(resultPair.second.c_str());
            }
# else // FORTE_COM_OPC_UA_MASTER_BRANCH

# endif // FORTE_COM_OPC_UA_MASTER_BRANCH
#endif //UA_ENABLE_ENCRYPTION
            else {
              DEVLOG_WARNING("[CUA_ClientConfigFileParser]: They %s was not recognized so it will be omitted\n", resultPair.first.c_str());
            }
            break;
          case CConfigFileParser::eEmptyLine:
            //do nothing, keep reading
            break;
          case CConfigFileParser::eEndOfFile:
            moreLinesToRead = false;
            break;
          case CConfigFileParser::eWrongLine:
          case CConfigFileParser::eFileNotOpened:
          case CConfigFileParser::eInternalError:
          default:
            retVal = false;
            moreLinesToRead = false;
            break;
        }
      }

      if(retVal) {
#ifdef UA_ENABLE_ENCRYPTION
# ifdef FORTE_COM_OPC_UA_MASTER_BRANCH
        if(0 != certificateFileContent.length) {
          retValOpcUa = UA_ClientConfig_setDefaultEncryption(&paResult.mClientConfig, certificateFileContent, privateKeyFileContent, NULL, 0, NULL, 0);
          if(UA_STATUSCODE_GOOD == retValOpcUa) {
            paResult.mClientConfig.securityMode = securityMode;
            paResult.mClientConfig.securityPolicyUri = securityPolicyUri;
          }
        } else
# else // FORTE_COM_OPC_UA_MASTER_BRANCH

# endif // FORTE_COM_OPC_UA_MASTER_BRANCH
#endif //UA_ENABLE_ENCRYPTION

#ifdef FORTE_COM_OPC_UA_MASTER_BRANCH
        {
          retValOpcUa = UA_ClientConfig_setDefault(&paResult.mClientConfig);
        }
#else //FORTE_COM_OPC_UA_MASTER_BRANCH

#endif //FORTE_COM_OPC_UA_MASTER_BRANCH
      }

#ifdef UA_ENABLE_ENCRYPTION
# ifdef FORTE_COM_OPC_UA_MASTER_BRANCH
      else {
        UA_String_clear(&securityPolicyUri);
      }

      UA_ByteString_clear(&certificateFileContent);
      UA_ByteString_clear(&privateKeyFileContent);
# else // FORTE_COM_OPC_UA_MASTER_BRANCH

# endif // FORTE_COM_OPC_UA_MASTER_BRANCH
#endif

    } else { //if the endpoint is not found, configuration is initialize as default
      DEVLOG_INFO("[CUA_ClientConfigFileParser]: No entry for endpoint %s was found in file %s\n", paEndpoint.c_str(), paFileLocation.c_str());
#ifdef FORTE_COM_OPC_UA_MASTER_BRANCH
      retValOpcUa = UA_ClientConfig_setDefault(&paResult.mClientConfig);
#else // FORTE_COM_OPC_UA_MASTER_BRANCH

#endif //FORTE_COM_OPC_UA_MASTER_BRANCH
    }

    if(retVal && UA_STATUSCODE_GOOD != retValOpcUa) {
      DEVLOG_ERROR("[CUA_ClientConfigFileParser]: Error setting client configuration. Error: %s\n", UA_StatusCode_name(retValOpcUa));
      retVal = false;
    }
  } else { //if lookForEndpointInFile fails, the errors was already logged
    retVal = false;
  }

  return retVal;
}

#ifdef UA_ENABLE_ENCRYPTION
# ifdef FORTE_COM_OPC_UA_MASTER_BRANCH
bool CUA_ClientConfigFileParser::loadFileIntoBytestring(std::string &paFileLocation, UA_ByteString &paResult) {
  bool retVal = false;

  FILE *file = fopen(paFileLocation.c_str(), "rb"); //read binary
  if(0 != file) {
    //look for size of file
    fseek(file, 0, SEEK_END);
    paResult.length = (size_t) ftell(file);
    paResult.data = (UA_Byte *) UA_malloc(paResult.length * sizeof(UA_Byte));

    fseek(file, 0, SEEK_SET);
    if(paResult.length == fread(paResult.data, sizeof(UA_Byte), paResult.length, file)) {
      retVal = true;
    } else {
      DEVLOG_ERROR("[OPC UA CLIENT]: Error reading file %s\n", paFileLocation.c_str());
      UA_ByteString_clear(&paResult);
    }
    fclose(file);
  } else {
    DEVLOG_ERROR("[OPC UA CLIENT]: Error opening file %s\n", paFileLocation.c_str());
  }

  return retVal;
}
# else // FORTE_COM_OPC_UA_MASTER_BRANCH

# endif // FORTE_COM_OPC_UA_MASTER_BRANCH
#endif //UA_ENABLE_ENCRYPTION
