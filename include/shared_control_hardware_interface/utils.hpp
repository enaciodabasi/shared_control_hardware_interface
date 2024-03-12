/**
 * @file utils.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <chrono>
#include <thread>

namespace schi
{
  
  namespace utils
  {
    
    template<class Clock = std::chrono::steady_clock>
    class PeriodController
    {
      public: 

      PeriodController()
        : m_Period(0) 
      {
      }

      explicit PeriodController(const int frequency)
        : PeriodController<Clock>(static_cast<double>(1 / frequency))
      {

      }

      explicit PeriodController(const double period)
        : m_Period(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(period))),
          m_LastInterval(Clock::now())
      {
        //clock_gettime(CLOCK_MONOTONIC, &m_SleepTime);
      }

      bool setPeriod(const double period)
      {
        m_Period = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(period));
        m_LastInterval = Clock::now();
        //clock_gettime(CLOCK_MONOTONIC, &m_SleepTime);
      } 

      bool sleep()
      {
        auto currentTime = Clock::now();

        auto nextInterval = m_LastInterval + m_Period;
        if(currentTime < lastInterval)
        {
          nextInterval = currentTime + m_Period; // Set next interval to the next period start from now.
        }

        auto timeToSleepFor = nextInterval - now;

        m_LastInterval += m_Period;

        if(timeToSleepFor <= std::chrono::seconds(0))
        {
          if(currentTime > nextInterval + m_Period)
          {
            m_LastInterval = currentTime + m_Period;
          }

          return false;
        }
        
        //clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, )

        std::this_thread::sleep_for(timeToSleepFor);
        return true;
      }

      private:

      std::chrono::nanoseconds m_Period;

      std::chrono::time_point<Clock, std::chrono::duration<typename Clock::rep, std::nano>> m_LastInterval;

      //timespec m_SleepTime;

    };

  } // End of namespace utils
  

} // End of namespace schi


#endif 