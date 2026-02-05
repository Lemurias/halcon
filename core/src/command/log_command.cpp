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

#include <cmath>
#include <limits>

/*******************************************************************************
* LOCAL HEADERS
********************************************************************************/

#include "log_command.hpp"

/*******************************************************************************
* LOG COMMAND CLASS
********************************************************************************/

LogCommand::LogCommand(std::string command)
{
    /* Required options */
    auto log_options = app->add_subcommand("LOG", "Log a signal");
    log_options->add_option("-s,--SIGNAL", signal, "Signal name")->required();
    log_options->add_option("-c,--CLOCK", clock, "Clock name")->required();

    /* Optative options */
    log_options->add_option("-e,--EDGE", edge, "Clock edge");
    log_options->add_option("-b,--BEGIN", begin, "First reference clock tick");
    log_options->add_option("-p,--STEP", step, "Clock tick step");
    log_options->add_option("-d,--END", end, "Last reference clock tick");
    log_options->add_option("-t,--FILETYPE", file_type, "File type");
    log_options->add_option("-n,--FILENAMETYPE", file_name_type, "File name type");
    log_options->add_option("-f,--FORMAT", format, "Output format");

    Parse(command);
}

void LogCommand::Init(AbstractHandlerPtr p_signal)
{
    signal_ptr = p_signal;

    /* Unsupported edge */
    if (RUN_POSEDGE_LOGIC_ONLY && edge != EdgeType::POSITIVE)
    {
        std::string error_text = std::string(__FILE__) + ":"
                               + std::to_string(__LINE__) + ": "
                               + "ERROR [unsupported edge]: "
                               + "in command <" + command_string 
                               + "> the sampling edge is negedge but "
                               + "RUN_POSEDGE_LOGIC_ONLY is ON.";
        
        throw std::runtime_error(error_text);
    }

    /* Full LOG */
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
    n_samples = limit / step + (limit % step != 0 ? 1UL : 0UL);

    /* File name */
    size_t max_value = std::numeric_limits<size_t>::max();
    std::string file_type_str = (file_type == FileType::TEXT) ? "txt" : "bin";
    std::string end_str = (end == max_value) ? "last" : std::to_string(end);
    
    if (file_name_type == FileNameType::LONG)
    {    
        file_name = "LOG_SIGNAL_" + signal
                  + "_CLOCK_" + clock
                  + "_BEGIN_" + std::to_string(begin)
                  + "_END_" + end_str
                  + "_STEP_" + std::to_string(step)
                  + "_TYPE_" + signal_ptr->GetTypeAsString()
                  + "_BYTES_" + signal_ptr->GetNBytesAsString()
                  + "_SIZE_" + signal_ptr->GetSizeAsString()
                  + "." + file_type_str;
    }
    else
    {
        file_name = signal + "." + file_type_str;
    }
}

void LogCommand::Run(std::string output_path, size_t buffer_size)
{
    /* Create buffer if it doesn't exist */
    if (!signal_ptr->IsBufferCreated())
    {
        if (n_samples < buffer_size)
        {
            signal_ptr->CreateBuffer(n_samples);
        }
        else
        {
            signal_ptr->CreateBuffer(buffer_size);
        }
    }

    /* Create and clear the file */
    if (counter == 0)
    {
        std::string file_full_name = output_path + file_name;

        if (file_type == FileType::TEXT)
        {
            std::ofstream file(file_full_name, std::ofstream::out);
            file.close();
        }
        else
        {
            std::ofstream file(file_full_name, std::ofstream::out
                                             | std::ofstream::binary);
            file.close();
        }
    }

    /* Save signal sample in the buffer */
    signal_ptr->SaveSample();
    counter++;

    /* Flush to file */
    if (counter >= limit || signal_ptr->BufferIsFull())
    {
        std::string file_full_name = output_path + file_name;

        if (file_type == FileType::TEXT)
        {
            std::ofstream file(file_full_name, std::ofstream::out
                                             | std::ofstream::app);
            
            if (file.is_open())
            {
                signal_ptr->FlushToTextFile(file, format);
                file.close();
            }
        }
        else
        {
            std::ofstream file(file_full_name, std::ofstream::out
                                             | std::ofstream::app 
                                             | std::ofstream::binary);
            if (file.is_open())
            {
                signal_ptr->FlushToBinaryFile(file);
                file.close();
            }
        }
    }

    /* Delete buffer */
    if (counter >= limit)
    {
        signal_ptr->DeleteBuffer();
    }
}

bool LogCommand::IsActive()
{
    return signal_ptr->IsBufferCreated();
}

void LogCommand::Terminate(std::string output_path)
{
    std::string file_full_name = output_path + file_name;

    /* Flush to file */
    if (file_type == FileType::TEXT)
    {
        std::ofstream file(file_full_name, std::ofstream::out
                                         | std::ofstream::app);
        if (file.is_open())
        {
            signal_ptr->FlushToTextFile(file, format);
            file.close();
        }
    }
    else
    {
        std::ofstream file(file_full_name, std::ofstream::out
                                         | std::ofstream::app 
                                         | std::ofstream::binary);
        if (file.is_open())
        {
            signal_ptr->FlushToBinaryFile(file);
            file.close();
        }
    }

    /* Delete buffer */
    signal_ptr->DeleteBuffer();
}