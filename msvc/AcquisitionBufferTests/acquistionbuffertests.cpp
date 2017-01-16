#include "acquisitionbuffer.h"
#include <complex>
#include <iostream>
#include <cassert>

#define CHANNELS 2
#define WRITE_SIZE 2048
#define NUMBER_OF_WRITES 10
#define BUFFER_SIZE WRITE_SIZE * NUMBER_OF_WRITES
#define SAMP_TYPE std::complex<short>

void CreateTest()
{
    // Create buffer
    acquisition_buffer< SAMP_TYPE > acqBuffer(size_t(CHANNELS), size_t(BUFFER_SIZE), size_t(WRITE_SIZE));
    
    // Assert channel and buffer size are correct from constructor arguments
    assert(acqBuffer.get_channel_count() == CHANNELS && acqBuffer.get_buffer_size() == BUFFER_SIZE);
    std::cout << "CreateTest passed." << std::endl;
}

void WriteTest()
{
    // Create buffer
    acquisition_buffer< SAMP_TYPE > acqBuffer(size_t(CHANNELS), size_t(BUFFER_SIZE), size_t(WRITE_SIZE));
    
    // For each write, write to each channel
    for (size_t i = 0; i < NUMBER_OF_WRITES; i++)
    {
        // Get next write buffers
        std::vector< SAMP_TYPE *> bufferFronts = acqBuffer.get_next_write_buffer_fronts();

        // Write to each channel
        for (size_t j = 0; j < CHANNELS; j++)
        {
            // Write X samples
            for (size_t k = 0; k < WRITE_SIZE; k++)
            {
                bufferFronts[j][k] = SAMP_TYPE(k, k * -1);
            }
        }
    }

    // Create mock buffers for comparison
    std::vector< std::vector< SAMP_TYPE> > comparedChannels(CHANNELS, std::vector< SAMP_TYPE >(BUFFER_SIZE));
    for (size_t i = 0; i < NUMBER_OF_WRITES; i++)
    {
        for (size_t j = 0; j < CHANNELS; j++)
        {
            for (size_t k = 0; k < WRITE_SIZE; k++)
            {
                comparedChannels[j][k + i * WRITE_SIZE] = SAMP_TYPE(k, k * -1);
            }
        }
    }

    // Assert each written buffer is the same as mock buffer
    for (size_t i = 0; i < CHANNELS; i++)
    {
        assert(std::vector< SAMP_TYPE >(acqBuffer.get_buffer_fronts()[i], acqBuffer.get_buffer_fronts()[i] + BUFFER_SIZE) == comparedChannels[i]);
    }
    std::cout << "WriteTest passed." << std::endl;
}

int main()
{
    CreateTest();
    WriteTest();

    std::cout << "All tests executed. Press enter to exit.";
    getchar();
}