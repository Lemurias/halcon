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

#include <iostream>

/*******************************************************************************
* LOCAL HEADERS
********************************************************************************/

#include "CLI11.hpp"
#include "command_handler.hpp"
#include "logger.hpp"
#include "module.hpp"
#include "scheduler.hpp"
#include "setter.hpp"
#include "tictoc.hpp"

/*******************************************************************************
* SIMULATOR CLASS
********************************************************************************/

/**
 * @brief Simulator class: It is the main object in the framework.
 * Instantiates the scheduler, the command_handler, the logger, and the setter.
 * The simulator described by the user must inherit from this class.
 * 
 * The user must implement this methods:
 *   Init(): Initialize user parameters.
 *   Connect(): Describe the connections between blocks.
 *   Iteration(): Used to compute the iteration rate and others rates.
 *   ContinueRunning(): The simulation runs as long as this method returns true.
 *   Terminate(): Last method to execute. Used to print and save results.
 */
class Simulator : public Module
{
private:
    
    /* Module unused methods */
    void RunClockMaster() override {}
    
    /* Core methods */
    void CoreInit();
    void CoreReflect();
    void CoreConnect();
    void CoreCheck();
    
    /* Default vars */
    std::string command_file { "../conf/command.cmd" };
    std::string settings_file { "../conf/settings.yaml" };
    std::string hierarchy_file { "../conf/hierarchy.txt" };
    std::string logger_dir { "./logs/" };
    bool export_files { false };
    unsigned long iteration_counter { 0 };
    unsigned long logger_buffer_size { 1000 };

    /* Private modules */
    Scheduler scheduler;
    CommandHandler cmd_handler;
    Logger logger;
    Setter setter;

protected:

    /* User methods */
    virtual void Init() = 0;
    virtual void Connect() = 0;
    virtual void Iteration() = 0;
    virtual bool ContinueRunning() = 0;
    virtual void Terminate() = 0;

    /* User default modules */
    TicToc tic_toc;
    Clock clk_cmd_handler;

    /* Interface methods */
    double GetIterationRate();
    unsigned long GetIterationCounter();

public:

    /* main.cpp visible methods */
    void ProcessOptions(int argc, char *argv[]);
    void Run();
};