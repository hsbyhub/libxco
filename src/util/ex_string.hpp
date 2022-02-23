/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：string_ex.h
      	创 建 者：hsby
      	创建日期：2022/2/19
 *================================================================*/
#pragma once

#include <sstream>

#define EX_STRING_ERROR (std::string("errno=") + std::to_string(errno) + ", strerr=" + strerror(errno))
#define EX_STRING_VARS(...) (ParseArgList(#__VA_ARGS__, 0, "", __VA_ARGS__))
#define EX_STRING_FUNC_WITH_ARG(...) (ParseFuncExp(__PRETTY_FUNCTION__, EX_STRING_VARS(__VA_ARGS__)))
#define EX_STRING_FUNC_ERROR (std::string(__PRETTY_FUNCTION__ )+ " error, " + EX_STRING_ERROR)
#define EX_STRING_FUNC_ERROR_WITH_ARG(...) (EX_STRING_FUNC_WITH_ARG(__VA_ARGS__) + " error, " + EX_STRING_ERROR)

inline std::string ParseFuncExp(const std::string& func_name, const std::string& func_args_express) {
    std::stringstream ss;
    size_t i = 0;
    while(i < func_name.size()) {
        ss << func_name[i];
        if (func_name[i] == '(') break;
        i++;
    }
    ss << func_args_express;
    while(i < func_name.size()) {
        if (func_name[i] == ')') break;
        i++;
    }
    if (i < func_name.size()) {
        ss << func_name[i];
    }
    return ss.str();
}

inline bool IsArgListSplit(char ch) {
    return ch == ',';
}

// 终止遍历可变参数
inline void ParseArgList(std::stringstream& ss,
                         const std::string& format, int format_idx,
                         const std::string& prefix) {
}
// 遍历可变参数
template<typename VarType, typename ...VarTypes>
void ParseArgList(std::stringstream& ss,
                  const std::string& format, int format_idx,
                  const std::string& prefix,
                  VarType var, VarTypes...vars) {
    ss << prefix;
    bool found_name = false;
    for (; format_idx < format.size(); format_idx++) {
        char ch = format[format_idx];
        if (!found_name && !IsArgListSplit(ch)) {
            found_name = true;
        }
        if (found_name) {
            if (IsArgListSplit(ch))  {
                break;
            }
            ss << ch;
        }
    }
    ss << "=" << var;
    ParseArgList(ss, format, format_idx, ",", vars...);
}

template<typename VarType, typename ...VarTypes>
std::string ParseArgList(const std::string& format, int format_begin,
                         const std::string& prefix,
                         VarType var, VarTypes...vars) {
    std::stringstream ss;
    ParseArgList(ss, format, format_begin, prefix, var, vars...);
    return ss.str();
}
