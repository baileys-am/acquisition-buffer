#pragma once
#include <vector>

template <class T>
class acquisition_buffer
{
private:
    std::vector< std::vector<T> > _channel_buffers;
    size_t _write_size;
    size_t _write_position;

public:
    // Constructors
    acquisition_buffer(size_t channels, size_t buffer_size, size_t write_size);

    // Public Methods
    size_t get_channel_count();
    size_t get_buffer_size();
    std::vector<T *> get_buffer_fronts();
    std::vector<T *> get_next_write_buffer_fronts();
};

template<class T>
inline acquisition_buffer<T>::acquisition_buffer(size_t channels, size_t buffer_size, size_t write_size)
{
    this->_channel_buffers.resize(channels);
    for (size_t i = 0; i < channels; i++)
    {
        this->_channel_buffers[i].resize(buffer_size);
    }

    this->_write_size = write_size;
    this->_write_position = 0;
}

template<class T>
inline size_t acquisition_buffer<T>::get_channel_count()
{
    return this->_channel_buffers.size();
}

template<class T>
inline size_t acquisition_buffer<T>::get_buffer_size()
{
    return this->_channel_buffers.front().size();
}

template<class T>
inline std::vector<T*> acquisition_buffer<T>::get_buffer_fronts()
{
    std::vector<T *> buffer_fronts(this->get_channel_count());
    for (size_t i = 0; i < this->get_channel_count(); i++)
    {
        buffer_fronts[i] = &this->_channel_buffers[i].front();
    }

    return buffer_fronts;
}

template<class T>
inline std::vector<T*> acquisition_buffer<T>::get_next_write_buffer_fronts()
{
    // Create vector of buffer fronts starting at next write position
    std::vector<T *> buffer_fronts(this->get_channel_count());
    for (size_t i = 0; i < this->get_channel_count(); i++)
    {
        buffer_fronts[i] = &this->_channel_buffers[i].front() + this->_write_position;
    }

    // Update write position
    this->_write_position += this->_write_size;

    // TODO: Handle circular buffer conditions
    // TODO: Signal write processes

    return buffer_fronts;
}
