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

/*******************************************************************************
* HANDLER CLOCK CLASS
********************************************************************************/

template<>
class Handler<Clock> : public AbstractHandler
{
private:

    /* Data */
    Clock* data_ptr;

    /* Buffer */
    std::shared_ptr<long double> buffer { nullptr };
    size_t buffer_index { 0 };
    size_t buffer_size { 1 };
    bool is_buffer_full { false };

public:
    
    Handler() = default;
    Handler(Clock& variable);

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

inline Handler<Clock>::Handler(Clock& variable)
{
    data_ptr = &variable;
}

inline void* Handler<Clock>::GetPointer()
{
    return static_cast<void*>(data_ptr);
}

inline void Handler<Clock>::SetPointer(void* ptr)
{
    data_ptr = static_cast<Clock*>(ptr);
}

inline void Handler<Clock>::SetFromString(std::string str_value)
{
    /* Clocks can't set values */
    (void) str_value;

    std::string error_text = std::string(__FILE__) + ":"
                           + std::to_string(__LINE__) + ": "
                           + "ERROR [invalid set]: "
                           + "Unable to assign values to clocks";
    throw std::runtime_error(error_text);
}

inline std::string Handler<Clock>::GetAsString()
{
    std::stringstream ss;
    ss.precision(std::numeric_limits<long double>::digits10 + 1);
    ss << data_ptr->GetLastEdgeTime();
    return ss.str();
}

inline std::string Handler<Clock>::GetTypeAsString()
{
    std::string type = boost::typeindex::type_id<long double>().pretty_name();
    std::replace(type.begin(), type.end(), ' ', '_');
    return type;
}

inline std::string Handler<Clock>::GetNBytesAsString()
{
    return std::to_string(sizeof(long double));
}

inline std::string Handler<Clock>::GetSizeAsString()
{
    return "1";
}

inline void Handler<Clock>::CreateBuffer(size_t size)
{
    buffer_size = size;
    buffer_index = 0;
    
    buffer = std::shared_ptr<long double>
    (
        new long double[size], 
        std::default_delete<long double[]>()
    );
    
    if (!buffer)
    {
        throw std::bad_alloc();
    }
}

inline void Handler<Clock>::DeleteBuffer()
{
    buffer.reset();
    buffer = nullptr;
    buffer_size = 1;
    buffer_index = 0;
}

inline void Handler<Clock>::SaveSample()
{
    /* At the time of logging the clock, it has already increased. 
    Therefore, the correct thing to do is to log the time before increasing. */
    buffer.get()[buffer_index] = data_ptr->GetLastEdgeTime();

    buffer_index++;
    is_buffer_full = false;

    if (buffer_index > (buffer_size - 1))
    {
        buffer_index = 0;
        is_buffer_full = true;    
    }
}

inline void Handler<Clock>::FlushToBinaryFile(std::ofstream& file)
{
    size_t limit = is_buffer_full ? buffer_size : buffer_index;
    is_buffer_full = false;
    file.write(reinterpret_cast<char*>(buffer.get()), limit * sizeof(long double));
}

inline void Handler<Clock>::FlushToTextFile(std::ofstream& file, char format)
{
    /* Avoid "unused" warning */
    (void) format;

    size_t limit = is_buffer_full ? buffer_size : buffer_index;
    is_buffer_full = false;

    std::stringstream ss;
    ss.precision(std::numeric_limits<long double>::digits10 + 1);

    for (size_t i { 0 }; i < limit; i++)
    {
        ss << buffer.get()[i] << '\n';
    }

    file << ss.str();
}

inline bool Handler<Clock>::IsBufferCreated()
{
    return (buffer != nullptr);
}

inline bool Handler<Clock>::BufferIsFull()
{
    return is_buffer_full;
}