#ifndef _TEXTCAST_TRAITS_H_
#define _TEXTCAST_TRAITS_H_

#include <string>
#include <sstream>
#include "SimpleTypeTraits.h"

namespace textcast_traits
{
    template<typename T> struct text_cast
    {
        std::stringstream& _stream;
        text_cast(std::stringstream& stream):_stream(stream){ }

        typedef typename simple_typetraits::param_type<T>::type param;
        void set(param value){ (_stream)<<value; }

        T get(){ T temp; (_stream)>>temp; return temp; }

        void get(T& temp){ (_stream)>>temp; }
    };

    template<typename type>
    std::stringstream& operator<<(std::stringstream& ss, const std::vector<type>& vec)
    {
        if(vec.empty()) return ss;

        std::vector<type>::const_iterator it = vec.begin();
        textcast_traits::text_cast<type> textcast(ss); 

        textcast.set(*it);
        while(++it!=vec.end())
        {
            ss<<",";
            textcast.set(*it);
        }
        return ss;
    }

    template<typename type>
    std::stringstream& operator>>(std::stringstream& ss, std::vector<type>& vec)
    {
        type value = type(); char dummy = ' ';
        textcast_traits::text_cast<type> textcast(ss);

        while(!ss.eof())
        {
            textcast.get(value);
            ss>>dummy;
            vec.push_back(value);
        }
        return ss;
    }

    template<> struct text_cast<std::string>
    {
        std::stringstream& _stream;
        text_cast(std::stringstream& stream):_stream(stream){ }

        void set(const std::string& value)
        { _stream<<value; }

        std::string get()
        { std::string temp; get(temp); return temp; }

        void get(std::string& temp)
        { getline(_stream, temp, ','); _stream.seekg(-1, std::ios_base::cur); }
    };
}

#endif // _TEXTCAST_TRAITS_H_