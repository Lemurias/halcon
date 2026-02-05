/*******************************************************************************
* ██████████████████████████████████████████████████████████████████████████████
* █▀▀▀▀███▀▀▀▀█████▀▀▀▀▀██████▀▀▀█████████▀▀▀▀▀▀▀███████▀▀▀▀▀▀▀█████▀▀▀▀████▀▀▀█
* █    ███    ████▌      ████▌   ████████    ▄▄    ███▀   ▄▄▄   ▀███     ▀██   █
* █    ▀▀▀    ████   ▄   ▐███▌   ███████    ████▄▄▄██▌   ▐███▌   ▐██       ▀   █
* █           ███   ▐█▌   ███▌   ███████    █████████▌   ▐███▌    ██   ▄       █
* █    ███    ██▌          ██▌   ███████▄   ▀██▀   ▐██    ███    ███   ██▄     █
* █    ███    ██   █████   ██▌        ████▄      ▄█████▄       ▄████   ████▄   █
* ██████████████████████████████████████████████████████████████████████████████
* █████████████████████████ DSP SIMULATION ENGINE ██████████████████████████████
* ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
********************************************************************************
* Author: Patricio Reus Merlo
* Date: 05/16/2024
********************************************************************************
* MIT License
* 
* Copyright (c) 2024 Fundacion Fulgor
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
********************************************************************************/

#pragma once

#include <concepts>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "nameable.hpp"
#include "handler.hpp"
#include "handler_array.hpp"
#include "handler_ac_fixed.hpp"
#include "handler_array_ac_fixed.hpp"

#include "handler_complex.hpp"
#include "handler_array_complex.hpp"

/**
 * @brief Macro function to call Reflect method passing instance name as string
 * 
 */
#define REFLECT(var) this->Reflect(#var, var)
#define REFLECT_YAML(var) this->ReflectYAML(#var, var)

using AbstractHandlerPtr = std::shared_ptr<AbstractHandler>;
using HandlersMap = std::unordered_map<std::string, AbstractHandlerPtr>;

/**
 * @brief Base class of the framework. This class allows Module and Clock
 * instances to be able to "know" what variables it has instantiated inside.
 * 
 */
class BasicReflection : public Nameable
{

protected:

    BasicReflection() = default;
    virtual ~BasicReflection() = default;

    /* Map */
    HandlersMap h_variable_map;
    HandlersMap h_var_yaml_map;

    /* Reflect */
    template <typename T>
    void Reflect(std::string key, T& var);

    template <typename T>
    void Reflect(std::string key, std::complex<T>& var);

    template <typename T, size_t N>
    void Reflect(std::string key, std::array<T, N>& var);

    template <typename T, size_t N>
    void Reflect(std::string key, std::array<std::complex<T>, N>& var);

    template <typename T>
    void Reflect([[maybe_unused]] std::string key, [[maybe_unused]] std::vector<T>& var);

    /* Reflect YAML */
    template <typename T>
    void ReflectYAML(std::string key, T& var);

    template <typename T>
    void ReflectYAML(std::string key, std::complex<T>& var);

    template <typename T, size_t N>
    void ReflectYAML(std::string key, std::array<T, N>& var);

    template <typename T, size_t N>
    void ReflectYAML(std::string key, std::array<std::complex<T>, N>& var);

public:

    /* Flags */
    enum KeyFormat
    {
        Full = true,
        Local = false
    };

    virtual HandlersMap GetMapPointers(KeyFormat f=Full) const;
    virtual std::map<std::string, std::string> GetMapTypes(KeyFormat f=Full) const;
};

/**
 * @brief Reflects the name and the pointer of a variable in the variables map
 * 
 * @tparam T Variable type
 * @param key Variable name
 * @param var Variable reference
 */
template <typename T>
void BasicReflection::Reflect(std::string key, T& var)
{
    h_variable_map[key] = std::make_shared<Handler<T>>(var);
}

/**
 * @brief Reflects the name and the pointer of a complex variable in the variables map
 * 
 * @tparam T Complex variable type
 * @param key Complex variable name
 * @param var Complex variable reference
 */
template <typename T>
void BasicReflection::Reflect(std::string key, std::complex<T>& var)
{
    h_variable_map[key] = std::make_shared<Handler<std::complex<T>>>(var);
}

/**
 * @brief Reflects the name and the pointer of an array in the variables map
 * 
 * @tparam T Variable type
 * @tparam N Array size
 * @param key Variable name 
 * @param var Variable reference
 */
template <typename T, size_t N>
void BasicReflection::Reflect(std::string key, std::array<T, N>& var)
{
    h_variable_map[key] = std::make_shared<Handler<std::array<T, N>>>(var);
}

/**
 * @brief Reflects the name and the pointer of an array of complex variables in the variables map
 * 
 * @tparam T Complex variable type
 * @tparam N Array size
 * @param key Variable name 
 * @param var Variable reference
 */
template <typename T, size_t N>
void BasicReflection::Reflect(std::string key, std::array<std::complex<T>, N>& var)
{
    h_variable_map[key] = std::make_shared<Handler<std::array<std::complex<T>, N>>>(var);
}

/**
 * @brief Throw an error if a std::vector is reflected
 * 
 * @tparam T Variable type
 * @param key Variable name 
 * @param var Variable reference
 */
template <typename T>
void BasicReflection::Reflect(std::string key, std::vector<T>& var)
{
    std::string error_text = std::string(__FILE__) + ":"
                           + std::to_string(__LINE__) + ": "
                           + "ERROR [unsupported type]: "
                           + "a variable of type std::vector as <"
                           + full_name + "." + key + "> can not be reflected.";
    throw std::runtime_error(error_text);
}

template <typename T>
void BasicReflection::ReflectYAML(std::string key, T& var)
{
    Reflect(key, var);
    h_var_yaml_map[key] = h_variable_map[key];
}

template <typename T>
void BasicReflection::ReflectYAML(std::string key, std::complex<T>& var)
{
    Reflect(key, var);
    h_var_yaml_map[key] = h_variable_map[key];
}

template <typename T, size_t N>
void BasicReflection::ReflectYAML(std::string key, std::array<T, N>& var)
{
    Reflect(key, var);
    h_var_yaml_map[key] = h_variable_map[key];
}

template <typename T, size_t N>
void BasicReflection::ReflectYAML(std::string key, std::array<std::complex<T>, N>& var)
{
    Reflect(key, var);
    h_var_yaml_map[key] = h_variable_map[key];
}