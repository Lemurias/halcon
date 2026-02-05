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
* LOCAL HEADERS
********************************************************************************/

#include "handler.hpp"
#include "ac_fixed.h"

/*******************************************************************************
* HANDLER AC FIXED CLASS
********************************************************************************/

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
class Handler<ac_fixed<W, I, S, Q, O>> : public AbstractHandler
{
private:

    /* Data */
    ac_fixed<W, I, S, Q, O>* data_ptr;

    /* Buffer */
    std::shared_ptr<double> buffer { nullptr };
    size_t buffer_index { 0 };
    size_t buffer_size { 1 };
    bool is_buffer_full { false };

public:

    Handler(ac_fixed<W, I, S, Q, O>& variable);

    /* Data pointer */
    void* GetPointer() override;
    void SetPointer(void* ptr) override;

    /* Set and Get with strings */
    void SetFromString(std::string str_value) override;
    std::string GetAsString() override;
    std::string GetTypeAsString() override;
    std::string GetNBytesAsString() override;
    std::string GetSizeAsString() override;

    /* Buffer commands */
    void CreateBuffer(size_t size) override;
    void DeleteBuffer() override;
    void SaveSample() override;
    void FlushToBinaryFile(std::ofstream& file) override;
    void FlushToTextFile(std::ofstream& file, char format) override;

    /* Buffer information */
    bool BufferIsFull() override;
    bool IsBufferCreated() override;
};

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
Handler<ac_fixed<W, I, S, Q, O>>::Handler(ac_fixed<W, I, S, Q, O>& variable)
{
    data_ptr = &variable;
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
void* Handler<ac_fixed<W, I, S, Q, O>>::GetPointer()
{
    return static_cast<void*>(data_ptr);
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
void Handler<ac_fixed<W, I, S, Q, O>>::SetPointer(void* ptr)
{
    data_ptr = static_cast<ac_fixed<W, I, S, Q, O>*>(ptr);
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
void Handler<ac_fixed<W, I, S, Q, O>>::SetFromString(std::string str_value)
{
    std::stringstream ss(str_value);
    ss.precision(std::numeric_limits<double>::digits10 + 1);
    double aux_data;
    ss >> aux_data;
    *data_ptr = aux_data;
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
std::string Handler<ac_fixed<W, I, S, Q, O>>::GetAsString()
{
    std::stringstream ss;
    ss.precision(std::numeric_limits<double>::digits10 + 1);
    ss << data_ptr->to_double();
    return ss.str();
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
std::string Handler<ac_fixed<W, I, S, Q, O>>::GetTypeAsString()
{
    std::string type = boost::typeindex::type_id<double>().pretty_name();
    std::replace(type.begin(), type.end(), ' ', '_');
    return type;
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
std::string Handler<ac_fixed<W, I, S, Q, O>>::GetNBytesAsString()
{
    return std::to_string(sizeof(double));
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
std::string Handler<ac_fixed<W, I, S, Q, O>>::GetSizeAsString()
{
    return "1";
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
void Handler<ac_fixed<W, I, S, Q, O>>::CreateBuffer(size_t size)
{
    buffer_size = size;
    buffer_index = 0;
    
    buffer = std::shared_ptr<double>
    (
        new double[size], 
        std::default_delete<double[]>()
    );
    
    if (!buffer)
    {
        throw std::bad_alloc();
    }
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
void Handler<ac_fixed<W, I, S, Q, O>>::DeleteBuffer()
{
    buffer.reset();
    buffer = nullptr;
    buffer_size = 1;
    buffer_index = 0;
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
void Handler<ac_fixed<W, I, S, Q, O>>::SaveSample()
{
    buffer.get()[buffer_index] = data_ptr->to_double();
    
    buffer_index++;
    is_buffer_full = false;

    if (buffer_index > (buffer_size - 1))
    {
        buffer_index = 0;
        is_buffer_full = true;
    }
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
void Handler<ac_fixed<W, I, S, Q, O>>::FlushToBinaryFile(std::ofstream& file)
{
    size_t limit = is_buffer_full ? buffer_size : buffer_index;
    is_buffer_full = false;
    file.write(reinterpret_cast<char*>(buffer.get()), limit * sizeof(double));
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
void Handler<ac_fixed<W, I, S, Q, O>>::FlushToTextFile(std::ofstream& file, char format)
{
    /* Avoid "unused" warning */
    (void) format;

    size_t limit = is_buffer_full ? buffer_size : buffer_index;
    is_buffer_full = false;

    std::stringstream ss;
    ac_fixed<W, I, S, Q, O> aux;

    switch (format)
    {
        /* HEX */
        case 'h':

            ss.precision(std::numeric_limits<unsigned int>::digits10 + 1);

            for (size_t i { 0 }; i < limit; i++)
            {
                aux = buffer.get()[i];
                ss << std::hex << aux.to_uint() << '\t';
            }
            break;

        /* UINT */
        case 'i':
            
            ss.precision(std::numeric_limits<unsigned int>::digits10 + 1);

            for (size_t i { 0 }; i < limit; i++)
            {
                aux = buffer.get()[i];
                ss << aux.to_uint() << '\t';
            }
            break;

        /* DOUBLE */
        default:

            ss.precision(std::numeric_limits<double>::digits10 + 1);

            for (size_t i { 0 }; i < limit; i++)
            {
                ss << buffer.get()[i] << '\n';
            }
            break;
    }

    file << ss.str();
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
bool Handler<ac_fixed<W, I, S, Q, O>>::IsBufferCreated()
{
    return (buffer != nullptr);
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
bool Handler<ac_fixed<W, I, S, Q, O>>::BufferIsFull()
{
    return is_buffer_full;
}