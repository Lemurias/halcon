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

#include "intermediate_reflection.hpp"
#include "abstract_register.hpp"
#include "abstract_port.hpp"

HandlersMap IntermediateReflection::GetMapPointers(KeyFormat f) const
{
    /* Name prefix */
    std::string prefix = (f == Full)? full_name + "." : "";

    /* Add variables */
    HandlersMap nested_map = BasicReflection::GetMapPointers(f);

    /* Add ports */
    for (auto it = h_port_map.begin(); it != h_port_map.end(); ++it)
    {
        nested_map[prefix + it->first] = it->second;
    }
    
    /* Add registers (not supported yet) */
    // for (auto it = h_reg_map.begin(); it != h_reg_map.end(); ++it)
    // {
    //     nested_map[prefix + it->first] = it->second;
    // }

    /* Add registers variables */
    for (auto it = reg_map.begin(); it != reg_map.end(); ++it)
    {
        for (auto const &pair : it->second->GetMapPointers(f))
        {
            nested_map.insert(pair);
        }
    }

    return nested_map;
}

std::map<std::string, std::string> IntermediateReflection::GetMapTypes(KeyFormat f) const
{
    /* Method pointer */
    const std::string& (BasicReflection::*method_ptr) () const;
    method_ptr = (f == Full)? &BasicReflection::GetFullName : &BasicReflection::GetName;

    /* Add variables */
    std::map<std::string, std::string> nested_map = BasicReflection::GetMapTypes(f);

    /* Add ports */
    for (auto it = port_map.begin(); it != port_map.end(); ++it)
    {
        nested_map[(it->second->*method_ptr)()] = "+port";
    }

    /* Add registers */
    for (auto it = reg_map.begin(); it != reg_map.end(); ++it)
    {
        nested_map[(it->second->*method_ptr)()] = "-register";

        for (auto const &pair: it->second->GetMapTypes(f))
        {
            nested_map.insert(pair);
        }
    }

    return nested_map;
}