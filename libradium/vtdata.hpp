#ifndef LR_VTDATA_HPP
#define LR_VTDATA_HPP

#include <iostream>
#include <string>
#include <vector>

class vtdata{
public:
    vtdata();
    vtdata(const std::string &str);
    vtdata(const char *chr);
    vtdata(const vtdata &rhs) = default;            // den compiler einen kopierkonstruktor erstellen lassen und beten dass es klappt

    inline void clear(){ _data.clear(); }
    inline std::size_t size() const{ return _data.size();}
    inline void pop_back(){_data.pop_back();}
    
    // kann zu datenverlust führen. Gibt die einzelnen Bytes als String zurück bis zum ersten nullbyte
    std::string str();

    // Zuweisung eines strings erlauben
    vtdata& operator=(const std::string &str); 
    vtdata& operator+=(const char c){ _data.push_back(c); return *this; }
    char operator[](int i) const { return _data[i]; }

    // cout unterstützen
    friend std::ostream& operator<<(std::ostream &out, const vtdata &data);
    
private:
    std::vector<char> _data;
};

#endif