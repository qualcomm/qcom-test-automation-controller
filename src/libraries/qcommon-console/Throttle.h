#ifndef THROTTLE_H
#define THROTTLE_H
// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2013-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include <QtGlobal>

class Throttle
{
public:
   Throttle(quint32 min, quint32 max, quint32 step = 1)
   {
      Q_ASSERT(min <= max);

      m_current = m_min = min;
      m_max = max;
      m_step = step;
   }

   void floorIt()
   {
      m_current = m_min;
   }

   void coast()
   {
      if (m_current != m_max)
      {
         m_current += m_step;
         if (m_current > m_max)
            m_current = m_max;
      }
   }

   quint32 sleepTime()
   {
      return m_current;
   }

private:
   quint32                     m_current{0};
   quint32                     m_min{0};
   quint32                     m_max{0};
   quint32                     m_step{0};
};

#endif // THROTTLE_H

