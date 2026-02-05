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

#include "advanced_reflection.hpp"
#include "module.hpp"
#include "clock.hpp"

HandlersMap AdvancedReflection::GetMapPointers(KeyFormat f) const
{
    /* Name prefix */
    std::string prefix = (f == Full)? full_name + "." : "";

    /* Add variables, ports and registers */
    HandlersMap nested_map = IntermediateReflection::GetMapPointers(f);

    /* Add clocks */
    for (auto it = h_clock_map.begin(); it != h_clock_map.end(); ++it)
    {
        nested_map[prefix + it->first] = it->second;
    }

    /* Add clocks variables */
    for (auto it = clock_map.begin(); it != clock_map.end(); ++it)
    {
        for (auto const &pair : it->second->GetMapPointers(f))
        {
            nested_map.insert(pair);
        }
    }

    /* Add sub-modules */
    for (auto it = module_map.begin(); it != module_map.end(); ++it)
    {
        for (auto const &pair : it->second->GetMapPointers(f))
        {
            nested_map.insert(pair);
        }
    }

    return nested_map;
}

std::map<std::string, std::string> AdvancedReflection::GetMapTypes(KeyFormat f) const
{
    /* Method pointer */
    const std::string& (BasicReflection::*method_ptr) () const;
    method_ptr = (f == Full)? &BasicReflection::GetFullName : &BasicReflection::GetName;
    
    /* Add variables, ports and registers */
    std::map<std::string, std::string> nested_map = IntermediateReflection::GetMapTypes(f);

    /* Add clocks */
    for (auto it = clock_map.begin(); it != clock_map.end(); ++it)
    {
        nested_map[(it->second->*method_ptr)()] = "+clock";

        for (auto const &pair : it->second->GetMapTypes(f))
        {
            nested_map.insert(pair);
        }
    }

    /* Add sub-modules */
    for (auto it = module_map.begin(); it != module_map.end(); ++it)
    {
        nested_map[(it->second->*method_ptr)()] = "-module";
        
        for (auto const &pair : it->second->GetMapTypes(f))
        {
            nested_map.insert(pair);
        }
    }

    /* Add this module */
    std::string key = (f == Full)? full_name : name;
    nested_map[key] = "-module";

    return nested_map;
}