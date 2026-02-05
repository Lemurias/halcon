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

#include "logger.hpp"

/**
 * @brief Initialize the logger.
 * 
 * @param output_path The path to the folder where the files are stored.
 * @param max_buffer_size The maximum size of the buffer. The optimal value depends on the host machine.
 */
void Logger::Init(std::string out_path, size_t max_size)
{
    /* Buffer size must be begger than one */
    if (max_size < 1)
    {
        std::string error_text = std::string(__FILE__) + ":"
                               + std::to_string(__LINE__) + ": "
                               + "ERROR [invalid buffer size]: "
                               + "size = " + std::to_string(max_size);
        throw std::runtime_error(error_text);
    }

    max_buffer_size = max_size;
    output_path = out_path;

    /* Create logs folder */
    if (!((std::filesystem::exists(output_path) &&
           std::filesystem::is_directory(output_path))))
    {
        std::filesystem::create_directory(output_path);
    }
}

/**
 * @brief Process each log command in log_list.
 * 
 * @param log_list 
 */
void Logger::Run(std::vector<LogCommand*> log_list)
{
    for (auto &&log : log_list)
    {
        log->Run(output_path, max_buffer_size);
    }
}

/**
 * @brief Ensures that logger finishes correctly, saving buffers in files and freeing memory
 * 
 * @param log_list 
 */
void Logger::Terminate(std::vector<LogCommand*> log_list, std::vector<FinalLogCommand*> flog_list)
{
    /* Flush incomplete buffer */
    for (auto &&log : log_list)
    {
        log->Terminate(output_path);
    }

    /* Final Log */
    for (auto &&flog : flog_list)
    {
        flog->Run(output_path);
    }
}