#pragma once

template <class T>
class acquisition_listener
{
protected:
    size_t _element_size;

public:
    acquisition_listener();
    virtual ~acquisition_listener() = default;
    virtual void on_write(std::vector<T*> channel_buffer_fronts, size_t write_size) = 0;
};

template<class T>
inline acquisition_listener<T>::acquisition_listener()
{
    this->_element_size = sizeof(T);
}
