/**************************************************************************/
/*!
    @file     timespan.c
    @author   K. Townsend (microBuilder.eu)

    @note Timespan uses nanoseconds internally, with int64_t as the
          native type, giving +/-292 years range

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2013, K. Townsend (microBuilder.eu)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/
#include <string.h>
#include "timespan.h"

/**************************************************************************/
/*!
    @brief   Create a new timespan_t based on the specified number of
            nanoseconds/ticks

    @param   ticks [in]  Number of nanosecond 'ticks' in the timespan_t
    @param   timespan    Pointer to the timespan_t object to manipulate

    @return  An error_t if an error occurred, or ERROR_NONE if everything
             executed properly and the timespan was created
 */
/**************************************************************************/
error_t timespanCreate(int64_t ticks, timespan_t *timespan)
{
  if (timespan == NULL)
  {
    return ERROR_INVALIDPARAMETER;
  }

  memcpy(timespan, 0, sizeof(timespan_t));

  timespan->days = (ticks / TIMESPAN_NANOSPERDAY) * (ticks < 0 ? -1 : 1);
  timespan->hours = ((ticks / TIMESPAN_NANOSPERHOUR) % 24) * (ticks < 0 ? -1 : 1);
  timespan->minutes = ((ticks / TIMESPAN_NANOSPERMINUTE) % 60) * (ticks < 0 ? -1 : 1);
  timespan->seconds = ((ticks / TIMESPAN_NANOSPERSECOND) % 60) * (ticks < 0 ? -1 : 1);
  timespan->milliseconds = ((ticks / TIMESPAN_NANOSPERMILLISECOND) % 1000) * (ticks < 0 ? -1 : 1);
  timespan->microseconds = ((ticks / TIMESPAN_NANOSPERMICROSECOND) % 1000) * (ticks < 0 ? -1 : 1);
  timespan->nanoseconds = (ticks % 1000) * (ticks < 0 ? -1 : 1);
  timespan->__ticks = ticks;

  return ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief   Create a timespan_t by explicitly supplying the
             individual parameters

    @param   timespan    Pointer to the timespan_t object to manipulate

    @return  An error_t if an error occurred, or ERROR_NONE if everything
             executed properly and the timespan was created
 */
/**************************************************************************/
error_t timespanCreateExplicit(int32_t days, int32_t hours, int32_t minutes,
  int32_t seconds, int32_t milliseconds, int32_t microseconds,
  int32_t nanoseconds, timespan_t *timespan)
{
  int64_t ticks;

  if (timespan == NULL)
  {
    return ERROR_INVALIDPARAMETER;
  }

  /* Check individual parameters */
  if((days > TIMESPAN_MAXDAYS) || (days < TIMESPAN_MINDAYS)) return ERROR_TIMESPAN_OUTOFRANGE;
  if((hours > 23) || (hours < -23)) return ERROR_INVALIDPARAMETER;
  if((minutes > 59) || (minutes < -59)) return ERROR_INVALIDPARAMETER;
  if((seconds > 59) || (seconds < -59)) return ERROR_INVALIDPARAMETER;
  if((milliseconds > 999) || (milliseconds < -999)) return ERROR_INVALIDPARAMETER;
  if((microseconds > 999) || (microseconds < -999)) return ERROR_INVALIDPARAMETER;
  if((nanoseconds > 999) || (nanoseconds < -999)) return ERROR_INVALIDPARAMETER;

  /* Calculate total ticks minus days for int64_t overflow check */
  ticks = hours * TIMESPAN_NANOSPERHOUR +
          minutes * TIMESPAN_NANOSPERMINUTE +
          seconds * TIMESPAN_NANOSPERSECOND +
          milliseconds * TIMESPAN_NANOSPERMILLISECOND +
          microseconds * TIMESPAN_NANOSPERMICROSECOND +
          nanoseconds;

  /* Min/max timespan is +/-106751 days, 23 hours, 47 minutes and 16.854775807 seconds */
  if ((days == TIMESPAN_MAXDAYS ) || (days == TIMESPAN_MINDAYS))
  {
    if (((days == TIMESPAN_MAXDAYS) && (ticks > 85636854775807)) ||
        ((days == TIMESPAN_MINDAYS) && (ticks < -85636854775807)))
    {
      return ERROR_TIMESPAN_OUTOFRANGE;
    }
  }

  /* Add days now that we're sure we're within int64_t limits */
  ticks += days * TIMESPAN_NANOSPERDAY;

  /* Create the timespan based on calculated ticks */
  return timespanCreate(ticks, timespan);
}

/**************************************************************************/
/*!
    Returns the total number of whole hours in the specified timespan
 */
/**************************************************************************/
int32_t timespanToHours(timespan_t *timespan)
{
  if (timespan->days == 0)
  {
    return timespan->hours;
  }

  return timespan->hours + (timespan->days * 24);
}

/**************************************************************************/
/*!
    Returns the total number of whole minutes in the specified timespan
 */
/**************************************************************************/
int32_t timespanToMinutes(timespan_t *timespan)
{
  return (timespanToHours(timespan) * 60) + timespan->minutes;
}

/**************************************************************************/
/*!
    Returns the total number of whole seconds in the specified timespan
 */
/**************************************************************************/
int64_t timespanToSeconds(timespan_t *timespan)
{
  return ((int64_t)timespanToMinutes(timespan) * 60) + timespan->seconds;
}

/**************************************************************************/
/*!
    Returns the total number of whole milliseconds in the specified timespan
 */
/**************************************************************************/
int64_t timespanToMilliseconds(timespan_t *timespan)
{
  return ((int64_t)timespanToSeconds(timespan) * 1000) + timespan->milliseconds;
}

/**************************************************************************/
/*!
    Returns the total number of whole microseconds in the specified timespan
 */
/**************************************************************************/
int64_t timespanToMicroseconds(timespan_t *timespan)
{
  return ((int64_t)timespanToMilliseconds(timespan) * 1000) + timespan->microseconds;
}