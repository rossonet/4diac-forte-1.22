/*******************************************************************************
 * Copyright (c) 2016 - 2018 Johannes Messmer (admin@jomess.com), fortiss GmbH
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *   Johannes Messmer - initial API and implementation and/or initial documentation
 *   Jose Cabral - Cleaning of namespaces
 *******************************************************************************/

#ifndef SRC_CORE_IO_MAPPER_OBSERVER_H_
#define SRC_CORE_IO_MAPPER_OBSERVER_H_

#include "io_handle.h"
#include "io_mapper.h"

namespace forte {
  namespace core {
    namespace IO {

class IOObserver {
  friend class IOMapper;

public:
  IOObserver();
  virtual ~IOObserver();

  virtual bool onChange() = 0;

protected:
  virtual void onHandle(IOHandle *handle);
  virtual void dropHandle();

  IOHandle* handle;
  CIEC_ANY::EDataTypeID type;
  IOMapper::Direction direction;
};

    } //namespace IO
  } //namepsace core
} //namespace forte

#endif /* SRC_CORE_IO_MAPPER_OBSERVER_H_ */