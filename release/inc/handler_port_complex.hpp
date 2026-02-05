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

#include <complex>

/*******************************************************************************
* HANDLER PORT CLOCK CLASS
********************************************************************************/

/**
 * @brief Handler Class specialization for Port<std::complex<T>>
 *
 * This class allows to log or set a variable of type Port<std::complex<T>>
 *
 * @tparam T Type of elements in the complex class.
 */
template <typename T>
class Handler<Port<std::complex<T>>> : public AbstractHandler
{
private:

    /* Data */
    Port<std::complex<T>>* data_ptr;

    /* Buffer */
    std::shared_ptr<std::complex<T>> buffer { nullptr };
    size_t buffer_index { 0 };
    size_t buffer_size { 1 };
    bool is_buffer_full { false };

public:

    Handler() = default;
    Handler(Port<std::complex<T>>& variable);

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

/**
 * @brief Constructor for Handler with std::complex type.
 *
 * @tparam T Type of elements in the complex.
 * @param variable Reference to the std::complex variable.
 */
template <typename T>
Handler<Port<std::complex<T>>>::Handler(Port<std::complex<T>>& variable)
{
    data_ptr = &variable;
}

/**
 * @brief Gets the void pointer to the std::complex data.
 *
 * @return Void pointer to the std::complex data.
 */
template <typename T>
void* Handler<Port<std::complex<T>>>::GetPointer()
{
    return static_cast<void*>(data_ptr);
}

/**
 * @brief Sets the void pointer to the std::complex data.
 *
 * @param ptr Void pointer to be set.
 */
template <typename T>
void Handler<Port<std::complex<T>>>::SetPointer(void* ptr)
{
    data_ptr = static_cast<Port<std::complex<T>>*>(ptr);
}

/**
 * @brief Sets the data from a string value.
 *
 * @param str_value String representation of the value to be set.
 */
template<typename T>
void Handler<Port<std::complex<T>>>::SetFromString(std::string str_value)
{
    std::stringstream ss(str_value);
    ss.precision(std::numeric_limits<T>::digits10 + 1);
    std::complex<T> aux_complex;
    ss >> aux_complex;
    data_ptr->SetData(aux_complex);
}

/**
 * @brief Gets the string representation of the std::complex data.
 *
 * @return String representation of the std::complex data.
 */
template <typename T>
std::string Handler<Port<std::complex<T>>>::GetAsString()
{
    std::stringstream ss;
    ss.precision(std::numeric_limits<T>::digits10 + 1);
    ss << data_ptr->GetData();
    return ss.str();
}

/**
 * @brief Gets the type of the std::complex as a string.
 *
 * @return Type of the std::complex as a string.
 */
template <typename T>
std::string Handler<Port<std::complex<T>>>::GetTypeAsString()
{
    std::string type = boost::typeindex::type_id<T>().pretty_name();
    type = "cpx." + type;
    std::replace(type.begin(), type.end(), ' ', '_');
    return type;
}

/**
 * @brief Gets the size of the std::complex data in bytes as a string.
 *
 * @return Size of the std::complex data in bytes as a string.
 */
template <typename T>
std::string Handler<Port<std::complex<T>>>::GetNBytesAsString()
{
    return std::to_string(sizeof(std::complex<T>));
}

/**
 * @brief Gets the size of the std::complex array.
 *
 * @return Size of the std::complex array.
 */
template <typename T>
std::string Handler<Port<std::complex<T>>>::GetSizeAsString()
{
    return "1";
}

/**
 * @brief Creates a buffer for std::complex data.
 *
 * @param size Size of the buffer to be created.
 */
template <typename T>
void Handler<Port<std::complex<T>>>::CreateBuffer(size_t size)
{
    buffer_size = size;
    buffer_index = 0;
    buffer = std::shared_ptr<std::complex<T>>(new std::complex<T>[size], std::default_delete<std::complex<T>[]>());
        
    if (!buffer)
    {
        throw std::bad_alloc();
    }
}

/**
 * @brief Deletes the buffer for std::complex data.
 */
template <typename T>
void Handler<Port<std::complex<T>>>::DeleteBuffer()
{
    buffer.reset();
    buffer = nullptr;
    buffer_size = 1;
    buffer_index = 0;
}

/**
 * @brief Saves a sample of std::complex data to the buffer.
 */
template <typename T>
void Handler<Port<std::complex<T>>>::SaveSample()
{
    *(buffer.get() + buffer_index) = data_ptr->GetData();
    
    buffer_index++;
    is_buffer_full = false;

    if (buffer_index > (buffer_size - 1))
    {
        buffer_index = 0;
        is_buffer_full = true;    
    }
}

/**
 * @brief Flushes the buffer to a binary file for std::complex data.
 *
 * @param file Output file stream to write the binary data.
 */
template <typename T>
void Handler<Port<std::complex<T>>>::FlushToBinaryFile(std::ofstream& file)
{
    size_t limit = is_buffer_full ? buffer_size : buffer_index;
    is_buffer_full = false;
    file.write(reinterpret_cast<char*>(buffer.get()), limit * sizeof(std::complex<T>));
}

/**
 * @brief Flushes the buffer to a text file for std::complex data.
 *
 * @param file Output file stream to write the text data.
 */
template <typename T>
void Handler<Port<std::complex<T>>>::FlushToTextFile(std::ofstream& file, char format)
{
    /* Avoid "unused" warning */
    (void) format;
    
    size_t limit = is_buffer_full ? buffer_size : buffer_index;
    is_buffer_full = false;

    std::stringstream ss;
    ss.precision(std::numeric_limits<T>::digits10 + 1);
    ss << std::showpos;

    for (size_t i = 0; i < limit; i++)
    {
        T real = buffer.get()[i].real();
        T imag = buffer.get()[i].imag();
        ss << '(' << real << imag << "j)\n";
    }
    file << ss.str();
}

/**
 * @brief Checks if buffer has been created already
 * 
 * @return true 
 * @return false 
 */
template<typename T>
bool Handler<Port<std::complex<T>>>::IsBufferCreated()
{
    return (buffer != nullptr);
}

/**
 * @brief Checks if buffer is full
 * 
 * @return true 
 * @return false 
 */
template<typename T>
bool Handler<Port<std::complex<T>>>::BufferIsFull()
{
    return is_buffer_full;
}