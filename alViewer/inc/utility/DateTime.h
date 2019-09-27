#ifndef _DATETIME_H_
#define _DATETIME_H_

#include <ctime>
#include <limits>
#include <sstream>
#include <iomanip> // setw

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4996)
#endif

class DateTime
{
    std::clock_t _tick;
    time_t _time;
public:
    DateTime()
    {
        _tick = std::clock();
        time(&_time);
    }

    void Reset()
    {
        _tick = std::clock();
    }

    int Elapsed() const
    {
        return std::clock() - _tick;
    }

    int GetTick()
    {
        return std::clock() - _tick;
    }

    static int GetSystemTick()
    {
        return std::clock();
    }

    std::string GetDate()
    {
        tm *local = localtime(&_time); // C4996
        
        std::stringstream day;
        day<<local->tm_year+1900
           <<std::setfill('0')
           <<std::setw(2)<<local->tm_mon+1
           <<std::setw(2)<<local->tm_mday;
        return day.str();
    }
     
    std::string GetTime(const char* seperater="")
    {
        tm *local = localtime(&_time); // C4996

        std::stringstream time;
        time<<std::setfill('0')
            <<std::setw(2)<<local->tm_hour<<seperater
            <<std::setw(2)<<local->tm_min<<seperater
            <<std::setw(2)<<local->tm_sec;
        return time.str();
    }

	std::string GetHour()
	{
		tm *local = localtime(&_time); // C4996

		std::stringstream time;
		time<<std::setfill('0')
			<<std::setw(2)<<local->tm_hour;
		return time.str();
	}
};

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

#endif  // _DATETIME_H_
