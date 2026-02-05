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

#include <cstdlib>

/*******************************************************************************
* LOCAL HEADERS
********************************************************************************/

#include "simulator.hpp"

/*******************************************************************************
* SIMULATOR CLASS
********************************************************************************/

void Simulator::ProcessOptions(int argc, char *argv[])
{
    CLI::App app{"HALCON based simulator"};

    /* Options */
    app.add_option("-s,--settings_file", settings_file)->default_str(settings_file);
    app.add_option("-x,--hierarchy_file", hierarchy_file)->default_str(hierarchy_file);
    app.add_option("-c,--commands_file", command_file)->default_str(command_file);
    app.add_option("-l,--logger_dir", logger_dir)->default_str(logger_dir);
    
    /* Flags */
    app.add_flag("-e,--export_files", export_files)->default_val(export_files);

    /* CLI parser */
    try
    {
        app.parse(argc, argv);
    }
    catch (const CLI::CallForHelp& e)
    {
        std::cout << app.help() << std::endl;
        std::exit(0);
    }
    catch (const CLI::ParseError& e)
    {
        std::exit(app.exit(e));
    }

    /* Process options */
    if(export_files)
    {
        CoreReflect();
        ExportSettingsFile(settings_file);
        ExportHierarchyFile(hierarchy_file);
        std::exit(0);
    }
}

void Simulator::Run()
{
    /* Run Tic() */
    tic_toc.Tic("__begin_end__");

    /* Core initialization - step 1 */
    CoreReflect();
    CoreConnect();

    /* Global initialization */
    Initialize(settings_file);

    /* Core initialization - step 2 */
    CoreInit();
    CoreCheck();

    /* Loop Tic() */
    tic_toc.Tic("__loop__");

    /* Main loop */
    do
    {
        scheduler.UpdateNextClocks();
        cmd_handler.Run(scheduler.next_clocks);
        
        if(cmd_handler.sets.size())
        {
            setter.Run(cmd_handler.sets);
        }
        
        scheduler.RunClocks();
        
        if(cmd_handler.logs.size())
        {
            logger.Run(cmd_handler.logs);
        }
        
        Iteration();
        iteration_counter++;
    } while (ContinueRunning());

    /* Loop Time */
    auto loop_time = tic_toc.Toc("__loop__");

    /* End process */
    cmd_handler.Terminate();
    logger.Terminate(cmd_handler.logs, cmd_handler.flogs);
    Terminate();

    /* All Time */
    auto run_time = tic_toc.Toc("__begin_end__");

    /* Time report */
    std::ofstream ofile_handler("time.txt");
    ofile_handler << run_time << ',' << loop_time << std::endl;
    ofile_handler.close();
}

/**
 * @brief Reflection method for simulator core
 * 
 */
void Simulator::CoreReflect()
{
    /* YAML variables */
    REFLECT_YAML(logger_buffer_size);
    
    /* Clocks */
    REFLECT(clk_cmd_handler);
}

/**
 * @brief Connection method for simulator core
 * 
 */
void Simulator::CoreConnect()
{
    cmd_handler.i_reference_clock << clk_cmd_handler;
}

/**
 * @brief Initialization method for simulator core
 * 
 */
void Simulator::CoreInit()
{
    cmd_handler.Init(command_file, GetMapPointers());
    logger.Init(logger_dir, logger_buffer_size);
    scheduler.Init();
}

/**
 * @brief Verification method for simulator core
 * 
 */
void Simulator::CoreCheck()
{
    /* pass */
}

/**
 * @brief User access to iteration counter
 * 
 * @return unsigned long 
 */
unsigned long Simulator::GetIterationCounter()
{
    return iteration_counter;
}

/**
 * @brief User access to iteration rate
 * 
 * @return double
 */
double Simulator::GetIterationRate()
{
    return tic_toc.Rate("__loop__", iteration_counter);
}