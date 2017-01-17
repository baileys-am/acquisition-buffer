#include <vector>
#include <boost/signals2/signal.hpp>
#include "acquisitionlistener.h"

template <class T>
class acquisition_buffer
{
private:
    std::vector< std::vector<T> > _channel_buffers;
    size_t _element_size;
    size_t _write_position;
    boost::signals2::signal<void(std::vector<T*>, size_t)> _listeners;

public:
    // Constructors
    acquisition_buffer(size_t channels, size_t buffer_size);

    // Public Methods
    size_t get_channel_count();
    size_t get_buffer_size();
    std::vector<T*> get_buffer_fronts();
    std::vector<T*> get_next_write_buffer_fronts();
    void subscribe(acquisition_listener<T>& listener);
    void write(size_t write_size);
};

template<class T>
inline acquisition_buffer<T>::acquisition_buffer(size_t channels, size_t buffer_size)
{
    this->_channel_buffers.resize(channels);
    for (size_t i = 0; i < channels; i++)
    {
        this->_channel_buffers[i].resize(buffer_size);
    }

    this->_element_size = sizeof(T);
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
    std::vector<T*> buffer_fronts(this->get_channel_count());
    for (size_t i = 0; i < this->get_channel_count(); i++)
    {
        buffer_fronts[i] = &this->_channel_buffers[i].front();
    }

    return buffer_fronts;
}

template<class T>
inline std::vector<T*> acquisition_buffer<T>::get_next_write_buffer_fronts()
{
    // Create vector of channel buffer fronts
    std::vector<T*> buffer_fronts(this->get_channel_count());
    for (size_t i = 0; i < this->get_channel_count(); i++)
    {
        buffer_fronts[i] = &this->_channel_buffers[i].front() + this->_write_position;
    }

    return buffer_fronts;
}

template<class T>
inline void acquisition_buffer<T>::subscribe(acquisition_listener<T>& listener)
{
    this->_listeners.connect(boost::bind(&acquisition_listener<T>::on_write, &listener, _1, _2));
}

template<class T>
inline void acquisition_buffer<T>::write(size_t write_size)
{
    // Create vector of channel buffer fronts for listeners
    std::vector<T*> listener_buffer_fronts(this->get_channel_count());
    for (size_t i = 0; i < this->get_channel_count(); i++)
    {
        listener_buffer_fronts[i] = &this->_channel_buffers[i].front() + this->_write_position;
    }

    // Signal listeners to write
    this->_listeners(listener_buffer_fronts, write_size);

    // Update write position
    this->_write_position += write_size;

    // TODO: Handle circular buffer conditions
}
