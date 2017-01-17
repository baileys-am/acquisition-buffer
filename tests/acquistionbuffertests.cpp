#pragma warning(disable:4996)
#include "acquisitionbuffer.h"
#include "acquisitionlistener.h"
#include <complex>
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

#define CHANNELS 2
#define WRITE_SIZE 2048
#define NUMBER_OF_WRITES 10
#define BUFFER_SIZE WRITE_SIZE * NUMBER_OF_WRITES
#define SAMP_TYPE std::complex<short>

template <class T>
class mock_listener : public acquisition_listener<T>
{
public:
    bool notified;

    mock_listener() : acquisition_listener()
    {
        notified = false;
    }

    void on_write(std::vector<T*> channel_buffer_fronts, size_t write_size)
    {
        this->notified = true;
        std::cout << "mock class notified" << std::endl;
    }
};

template <class T>
class write_listener : public acquisition_listener<T>
{
private:
    size_t _channel;

public:
    FILE* file_pointer;

    write_listener(size_t channel, std::string filename) : acquisition_listener<T>()
    {
        this->_channel = channel;
        file_pointer = fopen(filename.c_str(), "wb");
    }

    ~write_listener()
    {
        fclose(file_pointer);
    }

    void on_write(std::vector<T*> channel_buffer_fronts, size_t write_size)
    {
        fwrite(channel_buffer_fronts[this->_channel], this->_element_size, write_size, file_pointer);
    }
};

void create_test()
{
    // Create buffer
    acquisition_buffer< SAMP_TYPE > buffer(size_t(CHANNELS), size_t(BUFFER_SIZE));
    
    // Assert channel and buffer size are correct from constructor arguments
    assert(buffer.get_channel_count() == CHANNELS && buffer.get_buffer_size() == BUFFER_SIZE);

    std::cout << "create_test passed." << std::endl;
}

void write_test()
{
    // Create buffer
    acquisition_buffer< SAMP_TYPE > buffer(size_t(CHANNELS), size_t(BUFFER_SIZE));
    
    // For each write, write to each channel
    for (size_t i = 0; i < NUMBER_OF_WRITES; i++)
    {
        // Get next write buffers
        std::vector< SAMP_TYPE *> buffer_fronts = buffer.get_next_write_buffer_fronts();

        // Write to each channel
        for (size_t j = 0; j < CHANNELS; j++)
        {
            // Write X samples
            for (size_t k = 0; k < WRITE_SIZE; k++)
            {
                buffer_fronts[j][k] = SAMP_TYPE(k, k * -1);
            }
        }

        buffer.write(size_t(WRITE_SIZE));
    }

    // Create mock buffers for comparison
    std::vector< std::vector< SAMP_TYPE> > compared_channels(CHANNELS, std::vector< SAMP_TYPE >(BUFFER_SIZE));
    for (size_t i = 0; i < NUMBER_OF_WRITES; i++)
    {
        for (size_t j = 0; j < CHANNELS; j++)
        {
            for (size_t k = 0; k < WRITE_SIZE; k++)
            {
                compared_channels[j][k + i * WRITE_SIZE] = SAMP_TYPE(k, k * -1);
            }
        }
    }

    // Assert each written buffer is the same as mock buffer
    for (size_t i = 0; i < CHANNELS; i++)
    {
        assert(std::vector< SAMP_TYPE >(buffer.get_buffer_fronts()[i], buffer.get_buffer_fronts()[i] + BUFFER_SIZE) == compared_channels[i]);
    }

    std::cout << "write_test passed." << std::endl;
}

void subscribe_test()
{
    // Create buffer
    acquisition_buffer< SAMP_TYPE > buffer(size_t(CHANNELS), size_t(BUFFER_SIZE));

    // Crate mock subscriber
    mock_listener< SAMP_TYPE > subscriber;

    // Subscribe to buffer class
    buffer.subscribe(subscriber);

    // Simulate writing
    buffer.get_next_write_buffer_fronts();
    buffer.write(size_t(WRITE_SIZE));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    assert(subscriber.notified == true);

    std::cout << "subscribe_test passed." << std::endl;
}

void acquisition_test()
{
    // Create buffer
    acquisition_buffer< SAMP_TYPE > buffer(size_t(CHANNELS), size_t(BUFFER_SIZE));

    // Subscribe writers
    write_listener< SAMP_TYPE > writer1(0, "writer1_file.bin"), writer2(1, "writer2_file.bin");
    buffer.subscribe(writer1);
    buffer.subscribe(writer2);

    // For each write, write to each channel
    for (size_t i = 0; i < NUMBER_OF_WRITES; i++)
    {
        // Get next write buffers
        std::vector< SAMP_TYPE *> buffer_fronts = buffer.get_next_write_buffer_fronts();

        // Write to each channel
        for (size_t j = 0; j < CHANNELS; j++)
        {
            // Write X samples
            for (size_t k = 0; k < WRITE_SIZE; k++)
            {
                buffer_fronts[j][k] = SAMP_TYPE(k, k * -1);
            }
        }

        buffer.write(size_t(WRITE_SIZE));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "acquisition_test passed." << std::endl;
}

int main()
{
    create_test();
    write_test();
    subscribe_test();
    acquisition_test();

    std::cout << "All tests executed. Press enter to exit.";
    getchar();
}