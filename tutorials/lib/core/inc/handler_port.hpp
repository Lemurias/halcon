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
* HANDLER PORT CLASS
********************************************************************************/

/**
 * @brief Handler Class specialization for Port<T> 
 *
 * This class allows to log or set a variable of type Port<T> 
 *
 * @tparam T Type of the Port data.
 */
template <typename T>
class Handler<Port<T>> : public AbstractHandler
{
private:

    /* Data */
    Port<T>* data_ptr;

    /* Buffer */
    std::shared_ptr<T> buffer { nullptr };
    size_t buffer_index { 0 };
    size_t buffer_size { 1 };
    bool is_buffer_full { false };

public:
    Handler() = default;
    Handler(Port<T>& variable);

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
 * @brief Constructor for Handler with Port<T> type.
 *
 * @param variable Reference to the Port<T> variable.
 */
template<typename T>
Handler<Port<T>>::Handler(Port<T>& variable)
{
    data_ptr = &variable;
}

/**
 * @brief Gets the void pointer to the Port<T> data.
 *
 * @return Void pointer to the Port<T> data.
 */
template<typename T>
void* Handler<Port<T>>::GetPointer()
{
    return static_cast<void*>(data_ptr);
}

/**
 * @brief Sets the void pointer to the Port<T> data.
 *
 * @param ptr Void pointer to be set.
 */
template<typename T>
void Handler<Port<T>>::SetPointer(void* ptr)
{
    data_ptr = static_cast<Port<T>*>(ptr);
}

/**
 * @brief Sets the Port<T> data from a string value.
 *
 * @param str_value String representation of the value to be set.
 */
template<typename T>
void Handler<Port<T>>::SetFromString(std::string str_value)
{
    std::stringstream ss(str_value);
    ss.precision(std::numeric_limits<T>::digits10 + 1);
    T aux_data;
    ss >> aux_data;
    data_ptr->SetData(aux_data);
}

/**
 * @brief Gets the string representation of the Port<T> data.
 *
 * @return String representation of the Port<T> data.
 */
template<typename T>
std::string Handler<Port<T>>::GetAsString()
{
    std::stringstream ss;
    ss.precision(std::numeric_limits<T>::digits10 + 1);
    ss << data_ptr->GetData();
    return ss.str();
}

/**
 * @brief Gets the type of the Port<T> as a string.
 *
 * @return Type of the Port<T> as a string.
 */
template<typename T>
std::string Handler<Port<T>>::GetTypeAsString()
{
    std::string type = boost::typeindex::type_id<T>().pretty_name();
    std::replace(type.begin(), type.end(), ' ', '_');
    return type;
}

/**
 * @brief Gets the size in bytes of the Port<T> data as a string.
 *
 * @return Size of the Port<T> data in bytes as a string.
 */
template<typename T>
std::string Handler<Port<T>>::GetNBytesAsString()
{
    return std::to_string(sizeof(T));
}

/**
 * @brief Gets the size in bytes of the Port array.
 *
 * @return Size of the Port array.
 */
template<typename T>
std::string Handler<Port<T>>::GetSizeAsString()
{
    return "1";
}

/**
 * @brief Creates a buffer for Port<T> data with the specified size.
 *
 * @param size Size of the buffer.
 */
template<typename T>
void Handler<Port<T>>::CreateBuffer(size_t size)
{
    buffer_size = size;
    buffer_index = 0;
    buffer = std::shared_ptr<T>(new T[size], std::default_delete<T[]>());
       
    if (!buffer)
    {
        throw std::bad_alloc();
    }
}

/**
 * @brief Deletes the buffer for Port<T> data.
 */
template<typename T>
void Handler<Port<T>>::DeleteBuffer()
{
    buffer.reset();
    buffer = nullptr;
    buffer_size = 1;
    buffer_index = 0;
}

/**
 * @brief Saves a sample of the Port<T> data to the buffer.
 */
template<typename T>
void Handler<Port<T>>::SaveSample()
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
 * @brief Flushes the buffer content to a binary file for Port<T> data.
 *
 * @param file Output file stream for writing the binary data.
 */
template<typename T>
void Handler<Port<T>>::FlushToBinaryFile(std::ofstream& file)
{
    size_t limit = is_buffer_full ? buffer_size : buffer_index;
    is_buffer_full = false;
    file.write(reinterpret_cast<char*>(buffer.get()), limit * sizeof(T));
}

/**
 * @brief Flushes the buffer content to a text file for Port<T> data.
 *
 * @param file Output file stream for writing the text data.
 */
template<typename T>
void Handler<Port<T>>::FlushToTextFile(std::ofstream& file, char format)
{
    /* Avoid "unused" warning */
    (void) format;
    
    size_t limit = is_buffer_full ? buffer_size : buffer_index;
    is_buffer_full = false;

    std::stringstream ss;
    ss.precision(std::numeric_limits<T>::digits10 + 1);

    for (size_t i { 0 }; i < limit; i++)
    {
        ss << buffer.get()[i] << '\n';
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
bool Handler<Port<T>>::IsBufferCreated()
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
bool Handler<Port<T>>::BufferIsFull()
{
    return is_buffer_full;
}