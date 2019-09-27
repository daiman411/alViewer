#ifndef _INIFILE_H_
#define _INIFILE_H_

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include<functional>

#include "TextCastTraits.h"
#include "SimpleTypeTraits.h"


class IniFile
{
    typedef std::greater<std::string> predicate;
    typedef std::map<std::string, std::string, predicate> associate_map;
    typedef std::map<std::string, associate_map, predicate> section_map;
    typedef std::stringstream stringstream;

public:
    IniFile(){ }

    bool Load(const std::string& path, bool add=false);
    bool Save(const std::string& path);

    bool Load(const char* path, bool add=false){ return Load(std::string(path), add); }
    bool Save(const char* path){ return Save(std::string(path)); }

    void Clear(){ _section_map.clear(); }
    bool IsEmpty(){ return _section_map.empty(); }

    void SetFloatPrecision(int precision);
    void ResetFloatPrecision();

private:
    template<class map_type>
    static typename map_type::value_type& get_map_value(map_type& map, const std::string& key)
    {
        map_type::iterator it = map.find(key);
        return it==map.end() ? *(map.insert(make_pair(key, map_type::mapped_type())).first) : *it;
    }

public:
    class Section;
    class Associate;
    
    friend class Section;
    friend class Associate;

    class Section
    {
        friend class IniFile;
        friend class Associate;
    public:
        bool IsValid(){ return !_value.second.empty(); }

        Associate operator[](const std::string& key_name)
        { return Associate(*this, key_name); }

        template<class T> Section operator+(T sub_name)
        {
            stringstream& stream = _ini.get_stream();
            stream<<_value.first<<sub_name;
            return Section(_ini, stream.str());
        }

    private:
        Section(IniFile& ini, const std::string& name)
        :_ini(ini), _value(get_map_value(ini._section_map, name)){ }

        IniFile& _ini;
        section_map::value_type& _value;
    };

    class Associate
    {
        friend class Section;
    public:
        bool IsValid(){ return !_value.second.empty(); }

        Associate operator()(const std::string& def_value)
        { return Associate(*this, def_value); }

        Associate operator()(const char* def_value)
        { return Associate(*this, def_value); }
	
        const char* GetText(){ return get_value().c_str(); }

        void SetText(const char * Text){ _value.second = Text; }

        template<class T> void Set(typename simple_typetraits::param_type<T>::type value)
        {
            stringstream& stream = _section._ini.get_stream();
            textcast_traits::text_cast<T>(stream).set(value);
            _value.second = stream.str();            
        }

        template<class T> void Get(T& value)
        {
            stringstream& stream = _section._ini.get_stream(get_value());
            textcast_traits::text_cast<T>(stream).get(value);
        }

        template<typename T> void operator=(T rhs)
        {
            Set<T>(rhs);
        }

        template<typename T> operator T()
        {
            stringstream& stream = _section._ini.get_stream(get_value());
            return textcast_traits::text_cast<T>(stream).get();
        }

        void operator=(Section& section)
        {
            _value.second = section._value.first;
        }

        operator Section()
        {
            _value.second = get_value();
            return Section(_section._ini, _value.second);
        }

        template<class T> Associate operator+(T sub_key)
        {
            stringstream& stream = _section._ini.get_stream();
            stream<<_value.first<<sub_key;
            return Associate(_section, stream.str());
        }

    private:
        Associate(Section& section, const std::string& key)
        :_section(section), _value(get_map_value(_section._value.second, key)){ set_def("0"); }

        Associate(Section& section, const std::string& key, const char* def)
        :_section(section), _value(get_map_value(_section._value.second, key)){ set_def(def); }

        Associate(Associate& rhs, const std::string& def)
        :_section(rhs._section), _value(rhs._value){ set_def(def); }

        Associate(Associate& rhs, const char* def)
        :_section(rhs._section), _value(rhs._value){ set_def(def); }

        const std::string& get_value()
        { return _value.second.empty() ? _section._ini._def_value : _value.second; }

        void set_def(const std::string& def)
        { _section._ini._def_value = def; }

        void set_def(const char* def)
        { _section._ini._def_value = def; }
       
        Section& _section;
        associate_map::value_type& _value;
    };

	Section operator[](const std::string& section_name)
	{ return Section(*this, section_name); }

private:
    stringstream& get_stream(const char* str)
    { _stream.clear(); _stream.str(str); return _stream; }

    stringstream& get_stream(const std::string& str)
    { return get_stream(str.c_str()); }

    stringstream& get_stream()
    { return get_stream(""); }

    bool Load(std::ifstream& fin, bool add);
    bool Save(std::ofstream& fout);
    bool GetNewLine(std::istream& is, std::string& newLine);

    // non copyable
    IniFile(const IniFile&);
    const IniFile& operator=(const IniFile&);

private:
    section_map _section_map;
    stringstream _stream;
    std::string _def_value;
};

inline bool IniFile::Load(const std::string& path, bool add)
{
    std::ifstream file(path.c_str(), std::ios::in);
    return file.is_open() ? Load(file, add) : false;
}

inline bool IniFile::Load(std::ifstream& file, bool add)
{
    if(!add) _section_map.clear();

    std::string temp, section;
    
    while(GetNewLine(file, temp))
    {
_INIFILE_CHECK_SECTION:
        size_t head, tail;
        if( (head=temp.find_first_of("["))==0 && (tail=temp.find_last_of("]"))!=std::string::npos )
            section = temp.substr(head+1, tail-head-1);
        else
            continue;

        associate_map& map = _section_map[section];
        while(GetNewLine(file, temp))
        {
            size_t equal;
            if( (equal=temp.find_first_of("=")) != std::string::npos )
                map[temp.substr(0, equal)] = temp.substr(equal+1);
            else
                goto _INIFILE_CHECK_SECTION;
        }
    }
    return true;
}

inline bool IniFile::Save(const std::string& path)
{
    std::ofstream file(path.c_str(), std::ios::out);
    return file.is_open() ? Save(file) : false;
}

inline bool IniFile::Save(std::ofstream& fout)
{
    for(section_map::reverse_iterator sit=_section_map.rbegin(); sit!=_section_map.rend(); sit++)
    {
        associate_map& map = sit->second;
        if(map.size() == 0) continue;

        fout<<"["<<sit->first<<"]"<<std::endl;
        for(associate_map::reverse_iterator ait=map.rbegin(); ait!=map.rend(); ait++)
        {
            if(!ait->second.empty()) fout<<ait->first<<"="<<ait->second<<std::endl;
        }
        fout<<std::endl;
    }
    return true;
}

inline bool IniFile::GetNewLine(std::istream& is, std::string& newline)
{
    bool eof = is.eof();
    getline(is, newline);
    return !eof;
}

inline void IniFile::SetFloatPrecision(int precision)
{
    _stream.setf(std::ios_base::fixed, std::ios_base::floatfield);
    _stream.precision(precision);
}

inline void IniFile::ResetFloatPrecision()
{
    _stream.setf(0, std::ios_base::floatfield);
}

#endif // _INIFILE_H_