/*******************************************************************************
 * Copyright (c) 2015 fortiss GmbH
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *   Martin Jobst
 *   - initial API and implementation and/or initial documentation
 *******************************************************************************/

#ifndef SRC_CORE_LUATYPE_H_
#define SRC_CORE_LUATYPE_H_

#include "typelib.h"
#include "resource.h"
#include "basicfb.h"

#include <string>

class CLuaFBTypeEntry: public CTypeLib::CFBTypeEntry {
private:

  const std::string definitionsPath;
  SFBInterfaceSpec interfaceSpec;
  SInternalVarsInformation internalVarsInformation;
  const CIEC_STRING paLuaScriptAsString;

  CLuaFBTypeEntry(CStringDictionary::TStringId typeNameId, CIEC_STRING paLuaScriptAsString,  SFBInterfaceSpec& interfaceSpec,
      SInternalVarsInformation& internalVarsInformation);


  CLuaFBTypeEntry(CStringDictionary::TStringId typeNameId, std::string& definitionsPath, SFBInterfaceSpec& interfaceSpec,
      SInternalVarsInformation& internalVarsInformation);
  virtual ~CLuaFBTypeEntry();

  static bool initInterfaceSpec(SFBInterfaceSpec& interfaceSpec, CLuaEngine* luaEngine, int index);
  static void deleteInterfaceSpec(SFBInterfaceSpec& interfaceSpec);
  static bool initInternalVarsInformation(SInternalVarsInformation& internalVarsInformation, CLuaEngine* luaEngine, int index);
  static void deleteInternalVarsInformation(SInternalVarsInformation& internalVarsInformation);
public:
  static CLuaFBTypeEntry* createLuaFBTypeEntry(CStringDictionary::TStringId typeNameId, std::string& definitionsPath);
  static bool createLuaFBTypeEntryFromLuaString(CStringDictionary::TStringId typeNameId, CIEC_STRING& paLuaScriptAsString);

  virtual CFunctionBlock* createFBInstance(CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes);

  const SFBInterfaceSpec* getInterfaceSpec() const {
    return &interfaceSpec;
  }
  const SInternalVarsInformation* getInternalVarsInformation() const {
    return &internalVarsInformation;
  }
};

#endif /* SRC_CORE_LUATYPE_H_ */
