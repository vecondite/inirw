/*
MIT License

Copyright (c) 2026 ᴠ ᴇ ᴄ ᴏ ɴ ᴅ ɪ ᴛ ᴇ

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef INIRW_H
#define INIRW_H

#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
#include <filesystem>
#include <algorithm>
#include <unordered_set>

namespace INIRW {
    template<typename kT, typename vT> class oMap{
        public:
            using Entry = std::pair<kT, vT>;
            using DataList = std::vector<Entry>;
            using LocationMap = std::unordered_map<kT, std::size_t>;
            using LocationIterator = typename LocationMap::iterator;
            using CLocationIterator = typename LocationMap::const_iterator;
        private:
            DataList data;
            LocationMap locs;
        public:
            vT& operator[](const kT& key){
                LocationIterator val = locs.find(key);
                if(val!=locs.end()){
                    return data[val->second].second;
                }else{
                    std::size_t idx = data.size();
                    data.emplace_back(key, vT{});
                    try{
                        locs.emplace(key, idx);
                    }catch(...){
                        data.pop_back();
                        throw;
                    }
                    return data.back().second;
                }
            }
            void erase(const kT& key){
                LocationIterator val = locs.find(key);
                if(val==locs.end()) return;
                std::size_t idx = val->second;
                locs.erase(val);
                data.erase(data.begin()+idx);
                for(std::size_t i = idx; i < data.size(); ++i){
                    locs.find(data[i].first)->second = i;
                }
            }
            vT* find(const kT& key) {
                LocationIterator val = locs.find(key);
                return val!=locs.end() ? &data[val->second].second : nullptr;
            }
            const vT* find(const kT& key) const {
                CLocationIterator val = locs.find(key);
                return val!=locs.end() ? &data[val->second].second : nullptr;
            }
            Entry* begin() { return data.data(); }
            Entry* end() { return data.data() + data.size(); }
            const Entry* begin() const { return data.data(); } 
            const Entry* end() const { return data.data() + data.size(); }
            bool contains(const kT& key) const { return locs.find(key)!=locs.end(); }
            std::size_t size() const { return data.size(); }
            bool empty() const { return data.empty(); }
            void clear() { data.clear(); locs.clear(); }
    };

    struct Output{
        bool stat = false;
        std::string out;

        explicit operator bool() const { return stat; }
    };

    inline std::size_t fU(std::string_view str, char escC, std::string_view filter){
        for(std::size_t i = 0; i<str.length(); ++i){
            if(str[i]==escC){
                ++i;
                continue;
            }else if(filter.find(str[i])!=std::string::npos){
                return i;
            }
        }
        return std::string::npos;
    }

    inline std::string_view trim(std::string_view str, std::string_view tT){
        std::size_t s = str.find_first_not_of(tT);
        if(s==std::string::npos) return "";
        std::size_t e = str.find_last_not_of(tT);
        return str.substr(s, e-s+1);
    }

    inline std::string uesc(std::string_view str, char escC){
        std::string output = "";
        bool skip = false;
        for(char c : str){
            if(skip){
                skip = false;
                output += c;
            }else if(c == escC){
                skip = true;
            }else{
                output += c;
            }
        }
        return output;
    }

    inline std::string esc(std::string_view str, char escC, std::string_view filter){
        std::string output;
        for(char c : str){
            if(c == escC || filter.find(c)!=std::string::npos){
                output+=escC;
            }
            output+=c;
        } 
        return output;
    }

    using Ini = oMap<std::string, oMap<std::string, std::string>>;
    using SectData = oMap<std::string, std::string>;

    inline constexpr std::string_view spaces = " \t\r\n";
    inline constexpr std::string_view kEsc = ";#=[]";
    inline constexpr std::string_view vEsc = ";#";
    inline constexpr std::string_view sEsc = ";#]";

    inline void fSect(std::vector<std::string>& wL, std::size_t& instOffset, const SectData& sect, const std::unordered_set<std::string>& done){
        std::vector<std::string> nL;
        for(const SectData::Entry& kV : sect){
            if(done.count(kV.first)!=0) continue;
            nL.push_back(esc(kV.first, '\\', kEsc) + " = " + esc(kV.second, '\\', vEsc));
        }
        wL.insert(wL.begin() + instOffset, nL.begin(), nL.end());
        instOffset+=nL.size();
    }

    namespace detail{
        inline Output rwFile(std::string_view file, bool write, bool trunc, const Ini& wD, Ini* rD){
            Output out;
            std::size_t instOffset = 0;
            std::vector<std::string> wL;
            std::fstream f;
            std::string section = "";
            std::unordered_set<std::string> dSects;
            std::unordered_set<std::string> dKeys;
            bool crlf = false;
            unsigned int fL = 0;
            if(!trunc){
                f.open(std::string{file}, std::ios::in | std::ios::binary);
                if(f.is_open()){
                    std::string rL;
                    while(std::getline(f, rL)){
                        if(fL == 0){
                            fL = 1;
                            if(rL.length() >= 3 && rL.compare(0, 3, "\xEF\xBB\xBF")==0){
                                fL = 2;
                                rL.erase(0,3);
                            }
                            crlf = !rL.empty() && rL.back()=='\r';
                        }
                        std::string_view l = trim(rL, spaces);
                        std::size_t cmnt = fU(l, '\\', vEsc);
                        std::string_view dL = l.substr(0, cmnt);
                        if(dL.empty()){
                            if(write) wL.push_back(rL);
                            continue;
                        }  
                        if(dL[0] == '['){
                            std::size_t sE = fU(dL, '\\', "]");
                            if(sE!=std::string::npos){
                                if(write){
                                    if(const SectData* rSect = wD.find(section)){
                                        if(dSects.insert(section).second){
                                            fSect(wL, instOffset, *rSect, dKeys);
                                        }
                                    }
                                }
                                dKeys.clear();
                                section = uesc(trim(dL.substr(1, sE-1),spaces), '\\');
                                if(write){
                                    wL.push_back(rL);
                                }else{
                                    (*rD)[section];
                                }
                                instOffset = wL.size();
                                continue;
                            }
                        }
                        std::size_t eq = fU(dL, '\\', "=");
                        std::string key = uesc(trim(dL.substr(0, eq), spaces), '\\');
                        std::string value = eq==std::string::npos ? "" : uesc(trim(dL.substr(eq+1), spaces), '\\');
                        cmnt = fU(rL, '\\', vEsc);
                        dL = std::string_view(rL).substr(0, cmnt);
                        eq = fU(dL, '\\', "=");
                        std::size_t vS = eq == std::string::npos ? std::string::npos : dL.find_first_not_of(spaces, eq+1);
                        std::size_t vE = dL.find_last_not_of(spaces);
                        if(write){
                            const SectData* sD = wD.find(section);
                            if(sD!=nullptr){
                                const std::string* found = sD->find(key);
                                if(found!=nullptr){
                                    if(*found!=value){
                                        std::string eVal = esc(*found, '\\', vEsc);
                                        if(eq==std::string::npos){
                                            rL.insert(vE+1, " = " + eVal);
                                        }else if(vS==std::string::npos){
                                            rL.insert(eq+1, " " + eVal);
                                        }else{
                                            rL.replace(vS, vE-vS+1, eVal);
                                        }
                                    }
                                    dKeys.insert(key);
                                }
                            }
                            wL.push_back(rL);
                            instOffset=wL.size();
                        }else{
                            (*rD)[section][key] = value;
                        }
                    }
                    f.close();
                }else{
                    if(!write) {
                        out.stat = false;
                        out.out = "Failed to open file: " + std::string(file);
                        return out;
                    }
                }
            }
            if(write){
                if(const SectData* lastSect = wD.find(section)){
                    if(dSects.insert(section).second){
                        fSect(wL, instOffset, *lastSect, dKeys);
                    }
                }
                for(const Ini::Entry& rSect : wD){
                    if(dSects.count(rSect.first)!=0) continue;
                    if(!rSect.first.empty()) wL.push_back("[" + esc(rSect.first, '\\', sEsc) + "]");
                    for(const SectData::Entry& kV : rSect.second){
                        wL.push_back(esc(kV.first, '\\', kEsc) + " = " + esc(kV.second, '\\', vEsc));
                    }
                }
                std::string target{file};
                std::string tmp = target + ".tmp";
                std::error_code ec;
                f.open(tmp, std::ios::out | std::ios::trunc | std::ios::binary);
                if(!f.is_open()){
                    out.stat = false;
                    out.out = "Error whilst opening " + tmp + " for writing!";
                    return out;
                }
                if(fL == 2) f << "\xEF\xBB\xBF";
                for(const std::string& wLD : wL){
                    f << wLD << (crlf && (wLD.empty() || wLD.back()!='\r') ? "\r\n" : "\n");
                }
                f.close();
                if(f.fail()){    
                    std::filesystem::remove(tmp, ec);
                    out.stat = false;
                    out.out = "Error whilst writing " + tmp + ", " + target + " left untouched!";
                    return out;
                }
                std::filesystem::rename(tmp, target, ec);
                if(ec){
                    std::filesystem::remove(tmp, ec);
                    out.stat = false;
                    out.out = "Could not replace " + target + ": " + ec.message();
                    return out;
                }
            }
            out.stat = true;
            out.out = "Succeeded without issues.";
            return out;
        }
    }

    inline Output read(std::string_view file, Ini& output){
        output.clear();
        return detail::rwFile(file, false, false, output, &output);
    }

    inline Output write(std::string_view file, Ini& data){
        return detail::rwFile(file, true, false, data, nullptr);
    }

    inline Output generate(std::string_view file, Ini& data){
        return detail::rwFile(file, true, true, data, nullptr);
    }
};

#endif
