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
* Date: 06/19/2024
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
#include <unordered_set>

/*******************************************************************************
* LOCAL HEADERS
********************************************************************************/

#include "final_log_command.hpp"

/*******************************************************************************
* FINAL LOG COMMAND CLASS
********************************************************************************/

/**
 * @brief FinalLogCommand Constructor
 *
 */
FinalLogCommand::FinalLogCommand(std::string command)
{
    /* Required options */
    auto log_options = app->add_subcommand("FLOG", "Log a signal and the end");
    log_options->add_option("-s,--SIGNAL", signal, "Signal name")->required();

    /* Optative options */
    log_options->add_option("-t,--FILETYPE", file_type, "File type");
    log_options->add_option("-n,--FILENAMETYPE", file_name_type, "File name type");
    log_options->add_option("-f,--FORMAT", format, "Output format");

    Parse(command);
}

/**
 * @brief Init the command
 * 
 */
void FinalLogCommand::Init(AbstractHandlerPtr p_signal)
{
    signal_ptr = p_signal;

    /* File name */
    std::string file_type_str = (file_type == FileType::TEXT) ? "txt" : "bin";
    
    if (file_name_type == FileNameType::LONG)
    {
        file_name = "FLOG_SIGNAL_" + signal
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

void FinalLogCommand::Run(std::string output_path)
{
    std::string file_full_name = output_path + file_name;
    
    /* Sample signal */
    signal_ptr->CreateBuffer(1);
    signal_ptr->SaveSample();

    /* Export to file */
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

    signal_ptr->DeleteBuffer();
}