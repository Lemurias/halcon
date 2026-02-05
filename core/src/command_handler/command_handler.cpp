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

/*******************************************************************************
* STANDARD HEADERS
********************************************************************************/

#include <memory>

/*******************************************************************************
* LOCAL HEADERS
********************************************************************************/

#include "command_handler.hpp"

/*******************************************************************************
* COMMAND HANDLER CLASS
********************************************************************************/

/**
 * @brief Initialize the command handler.
 * 
 * @param file_name Path to cmd file.
 * @param nested_variable_map Map with the reference of each reflected signal.
 */
void CommandHandler::Init(std::string &file_name, HandlersMap nested_variable_map)
{
    /* Open command file */
    std::ifstream file_handler = OpenFile(file_name);

    /* Load Commands */
    LoadCommands(file_handler);

    /* Close command file */
    file_handler.close();

    /* Link each command with it signals */
    LoadSignals(nested_variable_map);

    /* Define output size */
    logs.reserve(log_list.size());
    flogs.reserve(flog_list.size());
    sets.reserve(set_list.size());
}

/**
 * @brief Open the cmd file
 * 
 * @param file_name 
 * @return std::ifstream 
 */
std::ifstream CommandHandler::OpenFile(std::string &file_name)
{
    std::ifstream file_handler(file_name);
    if (!file_handler.is_open())
    {
        std::string error_text = std::string(__FILE__) + ":"
                               + std::to_string(__LINE__) + ": "
                               + "ERROR [file could not be opened]: "
                               + file_name;
        
        throw std::runtime_error(error_text);
    }
    return file_handler;
}

/**
 * @brief Load the commands from the cmd file
 * 
 * @param file_handler 
 */
void CommandHandler::LoadCommands(std::ifstream &file_handler)
{
    std::string command_str;
    size_t line_counter = 0;
    while (std::getline(file_handler, command_str))
    {
        line_counter++;

        /* Skip commend or an empty line */
        if (command_str.starts_with("#") || command_str.empty() || command_str == "\n")
        {
            continue;
        }

        /* Check if it is a Log command */
        if (command_str.starts_with("LOG "))
        {
            LogCommand log(command_str);
            log_list.push_back(log);
            continue;
        }

        /* Check if it is a Final Log command */
        if (command_str.starts_with("FLOG "))
        {
            FinalLogCommand flog(command_str);
            flog_list.push_back(flog);
            continue;
        }

        /* Check if it is a Set command */
        if (command_str.starts_with("SET "))
        {
            SetCommand set(command_str);
            set_list.push_back(set);
            continue;
        }

        std::string error_text = std::string(__FILE__) + ": "
                               + std::to_string(__LINE__) + ": "
                               + "ERROR [unknown command]: "
                               + " line [" + std::to_string(line_counter)
                               + "]: " + command_str;
        
        throw std::runtime_error(error_text);
    }
}

/**
 * @brief Add to each command the reference to the associated signal.
 * 
 * @param nested_variable_map 
 */
void CommandHandler::LoadSignals(HandlersMap &nested_variable_map)
{
    /* Link log commands with signals */
    for (LogCommand& log : log_list) 
    {
        if (nested_variable_map.count(log.signal) == 0)
        {
            std::string error_text = std::string(__FILE__) + ": "
                                   + std::to_string(__LINE__) + ": "
                                   + "ERROR [unknown reference]: "
                                   + log.signal;
            
            throw std::runtime_error(error_text);
        }
        log.Init(nested_variable_map[log.signal]);
    }

    /* Link final log commands with signals */
    for (FinalLogCommand& flog : flog_list) 
    {
        if (nested_variable_map.count(flog.signal) == 0)
        {
            std::string error_text = std::string(__FILE__) + ": "
                                   + std::to_string(__LINE__) + ": "
                                   + "ERROR [unknown reference]: "
                                   + flog.signal;
            
            throw std::runtime_error(error_text);
        }
        flog.Init(nested_variable_map[flog.signal]);
    }

    /* Link set commands with signals */
    for (SetCommand& set : set_list)
    {
        if (nested_variable_map.count(set.signal) == 0)
        {
            std::string error_text = std::string(__FILE__) + ": "
                                   + std::to_string(__LINE__) + ": "
                                   + "ERROR [unknown reference]: "
                                   + set.signal;

            throw std::runtime_error(error_text);
        }
        set.Init(nested_variable_map[set.signal]);
    }
}

/**
 * @brief Determines the commands to execute related to ptr_clocks
 * 
 * @param ptr_clocks 
 */
void CommandHandler::Run(std::deque<Clock*>& ptr_clocks)
{
    sets.clear();
    logs.clear();

    const auto &ref_clock_tick = i_reference_clock->GetTickCount();

    for (auto &ptr_clock : ptr_clocks)
    {
        const auto &clk_name = ptr_clock->GetFullName();
        const auto &clk_edge = ptr_clock->GetNextEdgeType();

        /* Add log commands to next_logs list */
        for (LogCommand& log : log_list)
        {
            if (log.clock == clk_name
                && (log.edge == 'b' || log.edge == clk_edge)
                && log.end > ref_clock_tick
                && log.begin <= ref_clock_tick
                && (ref_clock_tick % log.step == 0))
            {
                logs.push_back(&log);
            }
        }

        /* Add log commands to next_sets list */
        for (SetCommand& set : set_list)
        {
            if (set.clock == clk_name
                && set.edge == clk_edge
                && set.end > ref_clock_tick
                && set.begin <= ref_clock_tick)
            {
                sets.push_back(&set);
            }
        }
    }
}

/**
 * @brief Ensures that generated commands are completed correctly
 * 
 */
void CommandHandler::Terminate()
{
    sets.clear();
    logs.clear();

    /* Flush incomplete logs */
    for (auto &log : log_list)
    {
        if (log.IsActive())
        {
            logs.push_back(&log);
        }
    }

    /* Flush final logs */
    for (auto &flog : flog_list)
    {
        flogs.push_back(&flog);
    }
}