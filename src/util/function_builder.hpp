/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：function_builder.h
      	创 建 者：hsby
      	创建日期：2022/2/19
 *================================================================*/
#pragma once

/**
 * @brief 生成变量的setter
 */
#define FUNCTION_BUILDER_VAR_SETTER(name, var)  \
void Set##name(const decltype(var)& value) {    \
    (var) = value;                              \
}

/**
 * @brief 生成变量的getter
 */
#define FUNCTION_BUILDER_VAR_GETTER(name, var)  \
decltype(var) Get##name() const {               \
    return var;                                 \
}

/**
 * @brief 生成变量的setter和getter
 */
#define FUNCTION_BUILDER_VAR(name, var)         \
        FUNCTION_BUILDER_VAR_SETTER(name, var)  \
        FUNCTION_BUILDER_VAR_GETTER(name, var)

/**
 * @brief 生成map的setter
 */
#define FUNCTION_BUILDER_MAP_SETTER(name, mp)                                   \
void Set##name(const typename decltype(mp)::key_type& key,                      \
               const typename decltype(mp)::value_type::second_type& value) {   \
    (mp)[key] = value;                                                          \
}

/**
 * @brief 生成map的getter
 */
#define FUNCTION_BUILDER_MAP_GETTER(name, mp)                                   \
typename decltype(mp)::value_type::second_type                                  \
Get##name(const typename decltype(mp)::key_type& key) const {                   \
    auto it = (mp).find(key);                                                   \
    return it == (mp).end() ? decltype(mp)::key_type() : it->second;            \
}

/**
 * @brief 生成map的getter with default
 */
#define FUNCTION_BUILDER_MAP_GETTER_DEFAULT(name, mp)                           \
typename decltype(mp)::value_type::second_type                                  \
Get##name(const typename decltype(mp)::key_type& key,                           \
          const typename decltype(mp)::value_type::second_type& def) const {    \
    auto it = (mp).find(key);                                                   \
    return it == (mp).end() ? def : it->second;                                 \
}

/**
 * @brief 生成map的del
 */
#define FUNCTION_BUILDER_MAP_DEL(name, mp)                                      \
void Del##name(const decltype(mp)::key_type& key) {                             \
    (mp).erase(key);                                                            \
}

/**
 * @brief 生成map的has
 */
#define FUNCTION_BUILDER_MAP_HAS(name, mp)                                      \
bool Has##name(const decltype(mp)::key_type& key) const {                       \
    return (mp).find(key) != (mp).end();                                        \
}

/**
 * @brief 生成map的所有操作函数
 */
#define FUNCTION_BUILDER_MAP(name, mp)                  \
        FUNCTION_BUILDER_MAP_SETTER(name, mp)           \
        FUNCTION_BUILDER_MAP_GETTER(name, mp)           \
        FUNCTION_BUILDER_MAP_GETTER_DEFAULT(name, mp)   \
        FUNCTION_BUILDER_MAP_DEL(name, mp)              \
        FUNCTION_BUILDER_MAP_HAS(name, mp)              \

/**
 * @brief 生成可以强制转换为某种类型的Getter
 * @param[in] key:键值
 * @param[in] def:默认值
 * @param[out] val:返回值
 * @return T类型的返回值
 */
#define FUNCTION_BUILDER_MAP_AS(name, mp)                                       \
template<class T>                                                               \
T Get##name##As(const typename decltype(mp)::key_type& key,                     \
                const T& def = T()) const {                                     \
    return GetAsFromMap(mp, key, def);                                          \
}                                                                               \
                                                                                \
template<class T>                                                               \
bool Get##name##AsWithCheck(const typename decltype(mp)::key_type& key, T& val, \
                            const T& def = T()) const {                         \
    return GetAsFromMapWithCheck(mp, key, val, def);                            \
}

/**
 * @brief 生成可以强制转换为某种类型的Getter
 * @param[in] key:键值
 * @param[in] def:默认值
 * @param[out] val:返回值
 * @return T类型的返回值
 */
#define FUNCTION_BUILDER_MAP_AS_WITH_PREDEAL(name, mp, predel)                  \
template<class T>                                                               \
T Get##name##As(const typename decltype(mp)::key_type& key,                     \
                const T& def = T()) {                                           \
    predel;                                                                     \
    return GetAsFromMap(mp, key, def);                                          \
}                                                                               \
                                                                                \
template<class T>                                                               \
bool Get##name##AsWithCheck(const typename decltype(mp)::key_type& key, T& val, \
                            const T& def = T()) {                               \
    predel;                                                                     \
    return GetAsFromMapWithCheck(mp, key, val, def);                            \
}
