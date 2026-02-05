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

#include <iomanip>
#include <fstream>

#include "module.hpp"
#include "clock.hpp"
#include "abstract_port.hpp"
#include "abstract_register.hpp"

/**
 * @brief Initialize modules recursively using external configuration file
 *
 * @param settings_file
 */
void Module::Initialize(std::string settings_file)
{
    std::ifstream file_handler(settings_file);
    if (!file_handler.is_open())
    {
        std::string error_text = std::string(__FILE__) + ":"
                               + std::to_string(__LINE__) + ": "
                               + "ERROR [file could not be opened]: "
                               + settings_file;
        throw std::runtime_error(error_text);
    }
    file_handler.close();

    YAML::Node settings = YAML::LoadFile(settings_file);
    this->Initialize(settings);
}

/**
 * @brief Initialize modules recursively using external YAML::Node
 *
 * @param settings
 */
void Module::Initialize(YAML::Node& settings)
{
    NameRecursively();
    ConnectRecursively();
    ConfigureRecursively(settings);
    CheckRecursively();
    OptimizeRecursively();
    InitRecursively();
}

/**
 * @brief Set name of "reflected" submodules recursively
 *
 */
void Module::NameRecursively()
{
    /* Name clocks */
    for (const auto& [clk_name, ptr] : clock_map)
    {
        ptr->SetFullName(full_name, clk_name);
    }

    /* Name registers */
    for (const auto& [reg_name, ptr] : reg_map)
    {
        ptr->SetFullName(full_name, reg_name);
    }

    /* Name ports */
    for (const auto& [port_name, ptr] : port_map)
    {
        ptr->SetFullName(full_name, port_name);
    }

    /* Name sub-modules */
    for (const auto& [sub_name, ptr] : module_map)
    {
        ptr->SetFullName(full_name, sub_name);
        ptr->NameRecursively();
    }
}

/**
 * @brief Connects modules recursively
 *
 */
void Module::ConnectRecursively()
{
    Connect();
    for (const auto& [key, ptr] : module_map)
    {
        ptr->ConnectRecursively();
    }
}

/**
 * @brief Configures submodules recursively
 *
 * @param settings
 */
void Module::ConfigureRecursively(const YAML::Node& settings)
{
    /* Configure module if it is in settings yaml */
    if (settings[name])
    {
        Configure(settings[this->name]);

        for (const auto& [sub_name, ptr] : module_map)
        {
            ptr->ConfigureRecursively(settings[name]);
        }
    }
}

/**
 * @brief Configure this module
 *
 * @param settings
 */
void Module::Configure(const YAML::Node& settings)
{
    YAML::Node variable_settings;

    for (auto it = settings.begin(); it != settings.end(); it++)
    {
        std::string key = it->first.as<std::string>();

        /* Skip sub-module settings */
        if (settings[key].IsMap())
        {
            if (this->module_map.find(key) == this->module_map.end() &&
                this->clock_map.find(key) == this->clock_map.end())
            {
                std::string error_text = std::string(__FILE__) + ":"
                                       + std::to_string(__LINE__) + ": "
                                       + "ERROR [non-existent submodule]: "
                                       + "submodule <" + key + "> " 
                                       + "was not found in module <" + this->full_name + ">";
                throw std::runtime_error(error_text);
            }
            continue;
        }

        /* Check if variable exists */
        if (this->h_variable_map.find(key) == this->h_variable_map.end() &&
            this->port_map.find(key) == this->port_map.end())
        {
            std::string error_text = std::string(__FILE__) + ":"
                                   + std::to_string(__LINE__) + ": "
                                   + "ERROR [non-existent variable]: "
                                   + "variable <" + key + "> " 
                                   + "was not found in module <" + this->full_name + ">";
            throw std::runtime_error(error_text);
        }

        /* Settings as arrays */
        if (settings[key].IsSequence())
        {
            std::string array_str;
            array_str += std::to_string(settings[key][0].as<double>());
            
            for (size_t i {1}; i < settings[key].size(); i++)
            {
                array_str += "," + std::to_string(settings[key][i].as<double>());
            }
            
            if (array_str.empty())
            {
                array_str = " ";
            }

            variable_settings[key] = array_str;
            continue;
        }
        
        /* Settings as variables */
        variable_settings[key] = settings[key];
    }

    /* Set variables from YAML */
    for (auto& [key, ptr] : this->h_variable_map)
    {

        std::stringstream ss;
        ss << variable_settings[key];

        /* Checking if string is empty */
        if (!ss.str().empty())
        {   
            ptr->SetFromString(variable_settings[key].Scalar());
        }
    }

    /* Set ports from YAML */
    for (auto& [key, ptr] : this->port_map)
    {
        if (ptr->IsNull())
        {
            std::stringstream ss;
            ss << variable_settings[key];

            /* Checking if string is empty */
            if (!ss.str().empty())
            {
                std::cout << "Port variable '" << key
                          << "' is not connected in module '" << this->name
                          << "'; it will be overwritten by settings.yaml"
                          << std::endl;

                this->h_port_map[key]->SetFromString(variable_settings[key].Scalar());
            }
            else
            {
                std::cerr << "Port variable '" << key
                          << "' is not connected in module '" << this->name
                          << "' and is missing in the settings.yaml"
                          << std::endl;
                assert(0);
            }
        }
    }
}

/**
 * @brief Optimize modules recursively. Optimizes connection of the ports. 
 * If a node is connected to another node, it assigns data and module 
 * pointers of the latter to the first. It only works for reflected ports.
 * 
 */
void Module::OptimizeRecursively()
{
    Optimize();
    for (const auto& [key, ptr] : module_map)
    {
        ptr->OptimizeRecursively();
    }
}

/**
 * @brief Optimize this module.
 */
void Module::Optimize()
{
    for (const auto& [key, ptr] : port_map)
    {
        ptr->Optimize();
    }
}

/**
 * @brief Initializes modules recursively
 * 
 */
void Module::InitRecursively()
{
    Init();

    for (const auto& [key, ptr] : clock_map)
    {
        ptr->Init();
    }

    for (const auto& [key, ptr] : module_map)
    {
        ptr->InitRecursively();
    }
}

/**
 * @brief Check ports recursively
 *
 */
void Module::CheckRecursively()
{
    Check();
    for (const auto& [key, ptr] : module_map)
    {
        ptr->CheckRecursively();
    }
}

/**
 * @brief Check that all the ports are connected. 
 * It only works for reflected ports.
 * 
 */
void Module::Check()
{
    for (const auto& [key, ptr] : port_map)
    {
        if (ptr->IsNull())
        {
            std::string error_text = std::string(__FILE__) + ":"
                                   + std::to_string(__LINE__) + ": "
                                   + "ERROR [disconnected port]: "
                                   + "port <" + key + "> " 
                                   + "of module <" + this->full_name + "> is not connected";
            throw std::runtime_error(error_text);
        }
    }
}

/**
 * @brief Generates or update settings.yaml file
 * 
 */
void Module::ExportSettingsFile(std::string settings_file)
{
    /* Check for new settings */
    NameRecursively();
    YAML::Node new_settings;
    new_settings[name] = CreateSettingsYAMLNodeRecursively();

     /* Load old settings */
    YAML::Node old_settings;
    std::ifstream ifile_handler(settings_file);
    if (ifile_handler.is_open())
    {
        old_settings = YAML::LoadFile(settings_file);
        ifile_handler.close();
    }

    /* Override unchanged settings */
    new_settings = OverrideSettings(new_settings, old_settings);

    /* Export template */
    std::ofstream ofile_handler(settings_file);
    ofile_handler << new_settings;
    ofile_handler.close();
}

/**
 * @brief 
 * 
 * @param hierarchy_file 
 */
void Module::ExportHierarchyFile(std::string hierarchy_file)
{
    std::ofstream ofile_handler(hierarchy_file);
    for (auto const& [key, type] : this->GetMapTypes())
    {
        ofile_handler << std::left << std::setw(30) << type << key << std::endl;
    }
    ofile_handler.close();
}

/**
 * @brief Generates YAML ports recursively.
 * 
 */
YAML::Node Module::CreateSettingsYAMLNodeRecursively()
{
    YAML::Node settings_yaml;

    /* Sub-modules */
    for (const auto& [key, ptr] : module_map)
    {
        YAML::Node sub_module_yaml = ptr->CreateSettingsYAMLNodeRecursively();
        
        if(sub_module_yaml.IsMap())
        {
            settings_yaml[key] = sub_module_yaml;
        }
    }

    /* Variables */
    for (const auto& [key, ptr] : h_var_yaml_map)
    {
        settings_yaml[key] = ptr->GetAsString();
    }
    h_var_yaml_map.clear();

    return settings_yaml;
}

/**
 * @brief Override new settings with old settings that comes from settings.yaml
 * 
 */
YAML::Node Module::OverrideSettings(YAML::Node news, YAML::Node olds)
{
    for (auto it = news.begin(); it != news.end(); it++)
    {
        std::string key = it->first.as<std::string>();

        if (!olds[key].IsDefined())
        {
            continue;
        }
            
        if (news[key].IsMap() && olds[key].IsMap())
        {
            news[key] = OverrideSettings(news[key], olds[key]);
            continue;
        }

        news[key] = olds[key];
    }
    return news;
}

/**
 * @brief Print the Big Map.
 * 
 */
void Module::PrintBigMap() const
{
    std::cout << std::endl;
    for (auto const& [key, type] : this->GetMapTypes())
    {
        std::cout << std::left << std::setw(15) << type << key << std::endl;
    }
    std::cout << "\n (*) LOG or SET commands are supported"
              << "\n (+) Only LOG commands are not supported"
              << "\n (-) LOG or SET commands are not supported"
              << std::endl;
}

/**
 * @brief Set the Master execution done
 *  
 */
void Module::SetMasterAsDone()
{
    master_status = MasterStatus::Done;
}

/**
 * @brief Set the Master execution not done
 *  
 */
void Module::SetMasterAsNotDone()
{
    master_status = MasterStatus::NotDone;
}

/**
 * @brief Return the status of master execution
 * 
 * @return bool
 */
bool Module::IsMasterDone() const
{
    return (master_status == MasterStatus::Done);
}

bool Module::IsMasterNotDone() const
{
    return (master_status == MasterStatus::NotDone);
}

void Module::SetAsSequentialOrMixed()
{
    logic_type = LogicType::SequentialOrMixed;
}

void Module::SeAsFullyCombinational()
{
    logic_type = LogicType::FullyCombinational;
}

bool Module::IsSequentialOrMixed() const
{
    return (logic_type == LogicType::SequentialOrMixed);
}

bool Module::IsFullyCombinational() const
{
    return (logic_type == LogicType::FullyCombinational);
}