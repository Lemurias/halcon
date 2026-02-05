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

#include "advanced_reflection.hpp"
#include "yaml-cpp/yaml.h"

/*******************************************************************************
* MACROS
********************************************************************************/

#define COMBINATIONAL_PORT this

/*******************************************************************************
* FORWARD DECLARATIONS
********************************************************************************/

class Clock;

/*******************************************************************************
* MODULE CLASS
********************************************************************************/

/**
 * @brief Module Class
 * his class serves as the base class for all simulator modules, 
 * giving them a name, the connection and init.
 * The children classes will have to define Init(), Connect(),
 * and the RunClock() (Master and Slave) functions
 */
class Module : public AdvancedReflection
{
private:

    /* Module settings */
    void Configure(const YAML::Node& settings);
    void Optimize();
    void Check();
    
    /* RunClockMaster() status */
    enum MasterStatus {Done = true, NotDone = false};
    bool master_status { MasterStatus::NotDone };

    /* Module type */
    enum LogicType {SequentialOrMixed = true, FullyCombinational = false};
    bool logic_type { LogicType::FullyCombinational };

    /* Export settings methods */
    YAML::Node CreateSettingsYAMLNodeRecursively();
    YAML::Node OverrideSettings(YAML::Node news, YAML::Node olds);

protected:

    /* Module and submodules start-up */
    void NameRecursively();
    void ConnectRecursively();
    void OptimizeRecursively();
    void InitRecursively();
    void ConfigureRecursively(const YAML::Node& settings);
    void CheckRecursively();

public:
    
    /* Module and submodules start-up */
    void Initialize(std::string settings_path);
    void Initialize(YAML::Node& settings);

    /* Module and submodules hierarchy */
    void PrintBigMap() const;

    /* Export file methods */
    void ExportSettingsFile(std::string settings_file);
    void ExportHierarchyFile(std::string hierarchy_file);
    
    /* User methods */
    virtual void Init() = 0;
    virtual void Connect() = 0;
    virtual void RunClockMaster() = 0;

    /* RunClockMaster() status interface */
    void SetMasterAsDone();
    void SetMasterAsNotDone();
    bool IsMasterDone() const;
    bool IsMasterNotDone() const;

    /* Type interface methods */
    void SetAsSequentialOrMixed();
    void SeAsFullyCombinational();
    bool IsSequentialOrMixed() const;
    bool IsFullyCombinational() const;
};