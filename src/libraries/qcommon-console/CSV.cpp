// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// CSV handling based on RFC 4108

#include "CSV.h"

#include <QString>

void writeCSVLine
(
   QTextStream& textStream,
   const std::vector<std::string>& values
)
{
   bool first(true);

   auto value = values.cbegin();
   while (value != values.cend())
   {
      QString writeValue(QString::fromStdString(*value));

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

   if (values.empty() == false)
      textStream << "\r\n";
}
