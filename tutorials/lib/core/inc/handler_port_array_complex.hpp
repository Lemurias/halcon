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
* HANDLER PORT ARRAY COMPLEX CLASS
********************************************************************************/

/**
 * @brief Template class representing a vector of ports with a specific type and size.
 *
 * @tparam T Type of the Port elements.
 * @tparam N Size of the Port.
 */
template <typename T, size_t N>
class Port;

/**
 * @brief Handler Class specialization for Port<std::complex<T>, N>
 *
 * This class allows logging or setting a variable of type Port<std::complex<T>, N>.
 *
 * @tparam T Type of the Port elements.
 * @tparam N Size of the Port.
 */
template <typename T, size_t N>
class Handler<Port<std::complex<T>, N>> : public AbstractHandler
{
private:

    /* Data */
    Port<std::complex<T>, N>* data_ptr;

    /* Buffer */
    std::shared_ptr<std::array<std::complex<T>, N>> buffer { nullptr };
    size_t buffer_index { 0 };
    size_t buffer_size { 1 };
    bool is_buffer_full { false };

public:

    Handler() = default;
    Handler(Port<std::complex<T>, N>& variable);

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
 * @brief Constructor for Handler with Port<std::complex<T>, N> type.
 *
 * @param variable Reference to the Port<std::complex<T>, N> variable.
 */
template<typename T, size_t N>
Handler<Port<std::complex<T>, N>>::Handler(Port<std::complex<T>, N>& variable)
{
    data_ptr = &variable;
}

/**
 * @brief Gets the void pointer to the Port<std::complex<T>, N> data.
 *
 * @return Void pointer to the Port<std::complex<T>, N> data.
 */
template<typename T, size_t N>
void* Handler<Port<std::complex<T>, N>>::GetPointer()
{
    return static_cast<void*>(data_ptr);
}

/**
 * @brief Sets the void pointer to the Port<std::complex<T>, N> data.
 *
 * @param ptr Void pointer to be set.
 */
template<typename T, size_t N>
void Handler<Port<std::complex<T>, N>>::SetPointer(void* ptr)
{
    data_ptr = static_cast<Port<std::complex<T>, N>*>(ptr);
}

/**
 * @brief Sets the Port<std::complex<T>, N> data from a string value.
 *
 * @param str_value String representation of the value to be set.
 */
template<typename T, size_t N>
void Handler<Port<std::complex<T>, N>>::SetFromString(std::string str_value)
{
    std::array<std::complex<T>, N> aux_array = { 0 };

    /* Clean string  */
    auto condition = [](char c)
    {
        return c == '[' || c == ']' || c == ' ' || c == '\t' || c == '\r';
    };

    str_value.erase(std::remove_if(str_value.begin(), str_value.end(), condition), str_value.end());

    /* Load array */
    char delimiter;
    std::istringstream iss(str_value);
    iss.precision(std::numeric_limits<T>::digits10 + 1);

    for (size_t i { 0 }; i < N; i++)
    {
        /* Check if it is shorter and complete with zeros */
        if(iss.eof())
        {
            aux_array[i] = 0;
            continue;
        }

        /* Load complex number with format (real, imag) */
        if (!(iss >> aux_array[i]))
        {
            std::string error_text = std::string(__FILE__) + ":"
                                   + std::to_string(__LINE__) + ": "
                                   + "ERROR [value error]: "
                                   + "the element [" + std::to_string(i) + "] "
                                   + "of the array " + str_value + " "
                                   + "could not be converted to complex "
                                   + "format correctly. Verify the format "
                                   + "is: (real, imag), (real, imag), ... "
                                   + "or: real, real, (real, imag), ...";
            throw std::runtime_error(error_text);
        }

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

/**
 * @brief Gets the string representation of the Port<std::complex<T>, N> data.
 *
 * @return String representation of the Port<std::complex<T>, N> data.
 */
template<typename T, size_t N>
std::string Handler<Port<std::complex<T>, N>>::GetAsString()
{
    std::stringstream ss;
    ss.precision(std::numeric_limits<std::complex<T>>::digits10 + 1);
    std::array<std::complex<T>, N> data = data_ptr->GetData();

    for (size_t i { 0 }; i < N - 1; ++i)
    {
        ss << data[i] << ',';
    }
    ss << data[N - 1];
    return ss.str();
}

/**
 * @brief Gets the type of the Port<std::complex<T>, N> as a string.
 *
 * @return Type of the Port<std::complex<T>, N> as a string.
 */
template<typename T, size_t N>
std::string Handler<Port<std::complex<T>, N>>::GetTypeAsString()
{
    std::string type = boost::typeindex::type_id<T>().pretty_name();
    type = "cpx." + type;
    std::replace(type.begin(), type.end(), ' ', '_');
    return type;
}

/**
 * @brief Gets the size in bytes of the Port<std::complex<T>, N> data as a string.
 *
 * @return Size of the Port<std::complex<T>, N> data in bytes as a string.
 */
template<typename T, size_t N>
std::string Handler<Port<std::complex<T>, N>>::GetNBytesAsString()
{
    return std::to_string(sizeof(std::complex<T>));
}

/**
 * @brief Gets the size in bytes of the Port array.
 *
 * @return Size of the Port array.
 */
template<typename T, size_t N>
std::string Handler<Port<std::complex<T>, N>>::GetSizeAsString()
{
    return std::to_string(N);
}

/**
 * @brief Creates a buffer for Port<std::complex<T>, N> data with the specified size.
 *
 * @param size Size of the buffer.
 * @throws std::runtime_error if the buffer size is invalid.
 */
template<typename T, size_t N>
void Handler<Port<std::complex<T>, N>>::CreateBuffer(size_t size)
{
    buffer_size = size;
    buffer_index = 0;
    
    buffer = std::shared_ptr<std::array<std::complex<T>, N>>
    (
        new std::array<std::complex<T>, N>[size], 
        std::default_delete<std::array<std::complex<T>, N>[]>()
    );

    if (!buffer)
    {
        throw std::bad_alloc();
    }
}

/**
 * @brief Deletes the buffer for Port<std::complex<T>, N> data.
 */
template<typename T, size_t N>
void Handler<Port<std::complex<T>, N>>::DeleteBuffer()
{
    buffer.reset();
    buffer = nullptr;
    buffer_size = 1;
    buffer_index = 0;
}

/**
 * @brief Saves a sample of the Port<std::complex<T>, N> data into the buffer.
 */
template<typename T, size_t N>
void Handler<Port<std::complex<T>, N>>::SaveSample()
{
    buffer.get()[buffer_index] = data_ptr->GetData();

    buffer_index++;
    is_buffer_full = false;

    if (buffer_index > (buffer_size - 1))
    {
        buffer_index = 0;
        is_buffer_full = true;
    }
}

/**
 * @brief Flushes the buffer content to a binary file for Port<std::complex<T>, N> data.
 *
 * @param file Output file stream for writing the binary data.
 */
template<typename T, size_t N>
void Handler<Port<std::complex<T>, N>>::FlushToBinaryFile(std::ofstream& file)
{
    size_t limit = is_buffer_full ? buffer_size : buffer_index;
    is_buffer_full = false;

    for (size_t i { 0 }; i < limit; i++)
    {
        char* array_ptr = reinterpret_cast<char*>((buffer.get() + i)->data());
        file.write(array_ptr, N * sizeof(std::complex<T>));
    }
}

/**
 * @brief Flushes the buffer content to a text file for Port<std::complex<T>, N> data.
 *
 * @param file Output file stream for writing the text data.
 */
template<typename T, size_t N>
void Handler<Port<std::complex<T>, N>>::FlushToTextFile(std::ofstream& file, char format)
{
    /* Avoid "unused" warning */
    (void) format;

    size_t limit = is_buffer_full ? buffer_size : buffer_index;
    is_buffer_full = false;

    std::stringstream ss;
    ss.precision(std::numeric_limits<T>::digits10 + 1);
    ss << std::showpos;

    for (size_t i { 0 }; i < limit; i++)
    {
        for (size_t j { 0 }; j < N; j++)
        {
            std::complex<T> data = buffer.get()[i][j];
            ss << '(' << data.real() << data.imag() << "j)\t";            
        }
        ss << '\n';
    }
    file << ss.str();
}

/**
 * @brief Checks if buffer has been created already
 * 
 * @return true 
 * @return false 
 */
template<typename T, size_t N>
bool Handler<Port<std::complex<T>, N>>::IsBufferCreated()
{
    return (buffer != nullptr);
}

/**
 * @brief Checks if buffer is full
 * 
 * @return true 
 * @return false 
 */
template<typename T, size_t N>
bool Handler<Port<std::complex<T>, N>>::BufferIsFull()
{
    return is_buffer_full;
}