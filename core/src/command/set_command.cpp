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

#include <limits>

/*******************************************************************************
* LOCAL HEADERS
********************************************************************************/

#include "set_command.hpp"

/*******************************************************************************
* SET COMMAND CLASS
********************************************************************************/

/**
 * @brief SetCommand Constructor
 *
 */
SetCommand::SetCommand(std::string command)
{
    /* Required options */
    auto set_options = app->add_subcommand("SET", "Set a signal value");
    set_options->add_option("-v,--VALUE", value, "Value to set");
    set_options->add_option("-f,--FILE", file, "File name");
    set_options->add_option("-s,--SIGNAL", signal, "Signal name")->required();
    set_options->add_option("-c,--CLOCK", clock, "Clock name")->required();
    set_options->add_option("-e,--EDGE", edge, "Clock edge")->required();
    set_options->add_option("-b,--BEGIN", begin, "First reference clock tick")->required();
    set_options->add_option("-d,--END", end, "Last reference clock tick")->required();
    
    set_options->callback([&]()
        {
            /* Check if exist FILE or VALUE options */
            size_t count { 0 };
            if (!value.empty()) { count += 1; }
            if (!file.empty() ) { count += 1; }

            if (count != 1)
            {
                std::string error_text = std::string(__FILE__) + ":"
                                       + std::to_string(__LINE__) + ": "
                                       + "ERROR [invalid SET command]: "
                                       + "in command <" + command_string 
                                       + "> only one option"
                                       + " is allowed (VALUE or FILE).";

                throw std::runtime_error(error_text);
            }

            /* Data from file */
            if (!file.empty())
            {
                std::ifstream file_handler(file);

                if (!file_handler.is_open())
                {
                    std::string error_text = std::string(__FILE__) + ":"
                                           + std::to_string(__LINE__) + ": "
                                           + "ERROR [unopened file]: "
                                           + "the file " + file + " in command <" 
                                           + command_string 
                                           + "> could not be opened correctly.";

                    throw std::runtime_error(error_text);
                }
                    
                std::stringstream buffer;
                buffer << file_handler.rdbuf();
                value = buffer.str();
            }
        }
    );

    Parse(command);
}

/**
 * @brief Initialize the command
 * 
 */
void SetCommand::Init(AbstractHandlerPtr p_signal)
{
    signal_ptr = p_signal;

    /* Unsupported edge */
    if (RUN_POSEDGE_LOGIC_ONLY && edge != EdgeType::POSITIVE)
    {
        std::string error_text = std::string(__FILE__) + ":"
                               + std::to_string(__LINE__) + ": "
                               + "ERROR [unsupported edge]: "
                               + "in command <" + command_string 
                               + "> the sampling "
                               + "edge is negedge but "
                               + "RUN_POSEDGE_LOGIC_ONLY is ON.";
        
        throw std::runtime_error(error_text);
    }
    
    /* Full SET */
    if (begin >= end && end == 0)
    {
        end = std::numeric_limits<size_t>::max();
    }

    /* BEGIN before END */
    if (begin > end)
    {
        std::string error_text = std::string(__FILE__) + ":"
                               + std::to_string(__LINE__) + ": "
                               + "ERROR [command error]: "
                               + "in command <" + command_string
                               + "> begin tick <" + std::to_string(begin)
                               + "> is grater than end tick <"
                               + std::to_string(end) + ">.";

        throw std::runtime_error(error_text);
    }

    /* Log window (in ticks) */
    limit = end - begin;
    counter = 0;
}

void SetCommand::Run()
{
    /* Is beginning */
    if(!counter)
    {
        original_value = signal_ptr->GetAsString();
    }

    /* Set new value */
    signal_ptr->SetFromString(value);

    /* Is ending */
    if(counter > limit)
    {
        signal_ptr->SetFromString(original_value);
    }
    else
    {
        counter++;
    }
}