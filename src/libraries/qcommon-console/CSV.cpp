// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// CSV handling based on RFC 4108

#include "CSV.h"

#include <QStringList>

void writeCSVLine
(
   QTextStream& textStream,
   const QStringList& values
)
{
   bool first(true);

   QStringList::const_iterator value = values.constBegin();
   while (value != values.constEnd())
   {
      QString writeValue(*value);
      
      writeValue.replace(",", "%x2C");
      writeValue.replace("\r", "%x0D");
      writeValue.replace("\n", "%x0A");
      writeValue.replace("\"", "%x22");

      if (first)
         first = false;
      else
         textStream << " , ";

      textStream << "\"" << writeValue << "\"";
      
      value++;
   }

   if (values.isEmpty() == false)
      textStream << "\r\n";
}
