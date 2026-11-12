#ifndef THROTTLE_H
#define THROTTLE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <QtGlobal>

#include <cstdint>

class Throttle
{
public:
   Throttle(uint32_t min, uint32_t max, uint32_t step = 1)
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

   uint32_t sleepTime()
   {
      return m_current;
   }

private:
   uint32_t                     m_current{0};
   uint32_t                     m_min{0};
   uint32_t                     m_max{0};
   uint32_t                     m_step{0};
};

#endif // THROTTLE_H

