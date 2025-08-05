#ifndef THROTTLE_H
#define THROTTLE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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

