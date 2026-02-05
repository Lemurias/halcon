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

#include "ac_fixed.h"

/*******************************************************************************
* FORWARD DECLARATIONS
********************************************************************************/

template <typename T, size_t N>
class Port;

/*******************************************************************************
* HANDLER PORT AC FIXED CLASS
********************************************************************************/

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
class Handler<Port<ac_fixed<W, I, S, Q, O>, N>> : public AbstractHandler
{
private:

    /* Data */
    Port<ac_fixed<W, I, S, Q, O>, N>* data_ptr;

    /* Buffer */
    std::shared_ptr<std::array<double, N>> buffer { nullptr };
    size_t buffer_index { 0 };
    size_t buffer_size { 1 };
    bool is_buffer_full { false };

public:

    Handler() = default;
    Handler(Port<ac_fixed<W, I, S, Q, O>, N>& variable);

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

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
Handler<Port<ac_fixed<W, I, S, Q, O>, N>>::Handler(Port<ac_fixed<W, I, S, Q, O>, N>& variable)
{
    data_ptr = &variable;
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
void* Handler<Port<ac_fixed<W, I, S, Q, O>, N>>::GetPointer()
{
    return static_cast<void*>(data_ptr);
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
void Handler<Port<ac_fixed<W, I, S, Q, O>, N>>::SetPointer(void* ptr)
{
    data_ptr = static_cast<Port<ac_fixed<W, I, S, Q, O>, N>*>(ptr);
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
void Handler<Port<ac_fixed<W, I, S, Q, O>, N>>::SetFromString(std::string str_value)
{
    /* Clean string  */
    auto condition = [](char c)
    {
        return c == '[' || c == ']' || c == ' ' || c == '\t' || c == '\r';
    };

    auto it = std::remove_if(str_value.begin(), str_value.end(), condition);
    str_value.erase(it, str_value.end());

    /* Input Stream */
    std::istringstream iss(str_value);
    iss.precision(std::numeric_limits<double>::digits10 + 1);
    
    /* Auxiliar variables */
    char delimiter;
    double aux_data;
    std::array<ac_fixed<W, I, S, Q, O>, N> aux_array;
    
    /* From iss to aux_array */
    for (size_t i { 0 }; i < N; i++)
    {
        /* Check if it is shorter and complete with zeros */
        if(iss.eof())
        {
            aux_array[i] = 0.0;
            continue;
        }

        /* Load number */
        if (!(iss >> aux_data))
        {
            std::string error_text = std::string(__FILE__) + ":"
                                   + std::to_string(__LINE__) + ": "
                                   + "ERROR [value error]: "
                                   + "the element [" + std::to_string(i) + "] "
                                   + "of the array " + str_value + " "
                                   + "could not be converted correctly. "
                                   + "Verify the format is: "
                                   + "[1, 2, 3, 4, ... ]";
            throw std::runtime_error(error_text);
        }
        aux_array[i] = aux_data;

        /* Discard delimiter */
        iss >> delimiter;
    }
    
    /* Check if it is longer */
    if (!iss.eof())
    {
        std::string error_text = std::string(__FILE__) + ":"
                               + std::to_string(__LINE__) + ": "
                               + "ERROR [value error]: "
                               + "the size of the array " + str_value + " "
                               + "is greater than the maximum supported "
                               + "size (" + std::to_string(N) + ")";
        throw std::runtime_error(error_text);
    }

    /* Load data to port array */
    data_ptr->SetData(aux_array);
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
std::string Handler<Port<ac_fixed<W, I, S, Q, O>, N>>::GetAsString()
{
    std::stringstream ss;
    ss.precision(std::numeric_limits<double>::digits10 + 1);
    std::array<ac_fixed<W, I, S, Q, O>, N> data = data_ptr->GetData();

    for (size_t i{ 0 }; i < N - 1; ++i)
    {
        ss << data[i].to_double() << ',';
    }
    ss << data[N - 1].to_double();

    return ss.str();
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
std::string Handler<Port<ac_fixed<W, I, S, Q, O>, N>>::GetTypeAsString()
{
    std::string type = boost::typeindex::type_id<double>().pretty_name();
    std::replace(type.begin(), type.end(), ' ', '_');
    return type;
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
std::string Handler<Port<ac_fixed<W, I, S, Q, O>, N>>::GetNBytesAsString()
{
    return std::to_string(sizeof(double));
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
std::string Handler<Port<ac_fixed<W, I, S, Q, O>, N>>::GetSizeAsString()
{
    return std::to_string(N);
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
void Handler<Port<ac_fixed<W, I, S, Q, O>, N>>::CreateBuffer(size_t size)
{
    buffer_size = size;
    buffer_index = 0;
    
    buffer = std::shared_ptr<std::array<double, N>>
    (
        new std::array<double, N>[size],
        std::default_delete<std::array<double, N>[]>()
    );
    
    if (!buffer)
    {
        throw std::bad_alloc();
    }
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
void Handler<Port<ac_fixed<W, I, S, Q, O>, N>>::DeleteBuffer()
{
    buffer.reset();
    buffer = nullptr;
    buffer_size = 1;
    buffer_index = 0;
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
void Handler<Port<ac_fixed<W, I, S, Q, O>, N>>::SaveSample()
{
    std::array<ac_fixed<W, I, S, Q, O>, N> data = data_ptr->GetData();

    for (size_t i { 0 }; i < N; ++i)
    {
        buffer.get()[buffer_index][i] = data[i].to_double();
    }

    buffer_index++;
    is_buffer_full = false;

    if (buffer_index > (buffer_size - 1))
    {
        buffer_index = 0;
        is_buffer_full = true;
    }
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
void Handler<Port<ac_fixed<W, I, S, Q, O>, N>>::FlushToBinaryFile(std::ofstream& file)
{
    size_t limit = is_buffer_full ? buffer_size : buffer_index;
    is_buffer_full = false;

    for (size_t i { 0 }; i < limit; i++)
    {
        char* array_ptr = reinterpret_cast<char*>((buffer.get() + i)->data());
        file.write(array_ptr, N * sizeof(double));
    }
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
void Handler<Port<ac_fixed<W, I, S, Q, O>, N>>::FlushToTextFile(std::ofstream& file, char format)
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
                for (size_t j { 0 }; j < N; j++)
                {
                    aux = buffer.get()[i][j];
                    ss << std::hex << aux.to_uint() << '\t';
                }
                ss << '\n';
            }
            break;

        /* UINT */
        case 'i':
            
            ss.precision(std::numeric_limits<unsigned int>::digits10 + 1);

            for (size_t i { 0 }; i < limit; i++)
            {
                for (size_t j { 0 }; j < N; j++)
                {
                    aux = buffer.get()[i][j];
                    ss << aux.to_uint() << '\t';
                }
                ss << '\n';
            }
            break;

        /* DOUBLE */
        default:

            ss.precision(std::numeric_limits<double>::digits10 + 1);

            for (size_t i { 0 }; i < limit; i++)
            {
                for (size_t j { 0 }; j < N; j++)
                {
                    ss << buffer.get()[i][j] << '\t';
                }
                ss << '\n';
            }
            break;
    }

    file << ss.str();
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
bool Handler<Port<ac_fixed<W, I, S, Q, O>, N>>::IsBufferCreated()
{
    return (buffer != nullptr);
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, size_t N>
bool Handler<Port<ac_fixed<W, I, S, Q, O>, N>>::BufferIsFull()
{
    return is_buffer_full;
}