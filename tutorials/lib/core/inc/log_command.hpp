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

/*******************************************************************************
* STANDARD HEADERS
********************************************************************************/

#include <string>

/*******************************************************************************
* LOCAL HEADERS
********************************************************************************/

#include "abstract_handler.hpp"
#include "command.hpp"

/*******************************************************************************
* MACROS
********************************************************************************/

#define RUN_POSEDGE_LOGIC_ONLY _RUN_POSEDGE_LOGIC_ONLY

/*******************************************************************************
* LOG COMMAND CLASS
********************************************************************************/

class LogCommand : public Command
{
private:
    
    /* Signal */
    using AbstractHandlerPtr = std::shared_ptr<AbstractHandler>;
    AbstractHandlerPtr signal_ptr;

    /* Window control */
    size_t limit;
    size_t counter;
    size_t n_samples;

    /* Output file */
    std::string file_name;

public:

    /* Options */
    enum FileType {TEXT = 't', BINARY = 'b'};
    enum FileNameType {SHORT = 's', LONG = 'l'};
    enum EdgeType {NEGATIVE = 'n', POSITIVE = 'p', BOTH = 'b'};
    enum Format {DEFAULT = '0'};

    /* Command fields */
    std::string signal;
    std::string clock;
    char edge { EdgeType::POSITIVE };
    size_t begin { 0 };
    size_t step { 1 };
    size_t end { 0 };
    char file_type { FileType::BINARY };
    char file_name_type { FileNameType::LONG };
    char format { Format::DEFAULT };

    /* Command methods */
    LogCommand(std::string command);
    void Init(AbstractHandlerPtr p_signal);
    void Run(std::string output_path, size_t buffer_size);
    bool IsActive();
    void Terminate(std::string output_path);
};