/*----------------------------------------------------------------------------------------------------------------------
--	Source File:		CircularBuffer.cpp
--
--	Functions: size_t size() const { return size_; }
--             size_t capacity() const { return capacity_; }
--             size_t write(const char *data, size_t bytes);
--             size_t read(char *data, size_t bytes);
--
--	Date:			November 24 2014
--
--	Revisions:
--
--
--	DESIGNERS:		Ramzi Chennafi
--
--
--	PROGRAMMER:		Ramzi Chennafi
--
--	NOTES:
--	A circular buffer for the storing of connection data.
--
-------------------------------------------------------------------------------------------------------------------------*/
#include "../include/CircularBuffer.h"
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: CircularBuffer
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: CircularBuffer(size_t capacity)
--                  capacity - maximum size of the circularbuffer.
--
--      RETURNS: A new CircularBuffer object.
--
--      NOTES:
--      Constructor for the circularbuffer. Intiates the buffer to the specified size and sets all other values to 0.
----------------------------------------------------------------------------------------------------------------------*/
CircularBuffer::CircularBuffer(size_t capacity) : beg_index_(0), end_index_(0), size_(0), capacity_(capacity)
{
    data_ = new char[capacity];
    memset(data_, 0, capacity);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: ~CircularBuffer
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: ~CircularBuffer()
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Deconstructor for the CircularBuffer object.
----------------------------------------------------------------------------------------------------------------------*/
CircularBuffer::~CircularBuffer()
{
    delete [] data_;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: write
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: write(const char * data, size_t bytes)
--                  data - buffer to be written from
--                  bytes - amount of data to be written to the circular buffer
--
--      RETURNS: A size_t of the amount of data written.
--
--      NOTES:
--      Writes data of a specified size to the circular buffer. If buffer is full, places as much data
--      as it can. If buffer is empty, returns 0.
----------------------------------------------------------------------------------------------------------------------*/
size_t CircularBuffer::write(const char *data, size_t bytes)
{
    if (bytes == 0) return 0;

    size_t capacity = capacity_;
    size_t bytes_to_write = std::min(bytes, capacity - size_);

    if (bytes_to_write <= capacity - end_index_)
    {
        memcpy(data_ + end_index_, data, bytes_to_write);
        end_index_ += bytes_to_write;
        if (end_index_ == capacity) end_index_ = 0;
    }
    else
    {
        size_t size_1 = capacity - end_index_;
        memcpy(data_ + end_index_, data, size_1);
        size_t size_2 = bytes_to_write - size_1;
        memcpy(data_, data + size_1, size_2);
        end_index_ = size_2;
    }

    size_ += bytes_to_write;
    return bytes_to_write;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: read
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: read(const char * data, size_t bytes)
--                  data - buffer to be read into
--                  bytes - amount of data to be read into data
--
--      RETURNS: A size_t of the amount of data read.
--
--      NOTES:
--      Reads an amount of data specified by bytes. If empty, returns 0. If requested bytes is larger than the buffer,
--      returns an entire buffer.
----------------------------------------------------------------------------------------------------------------------*/
size_t CircularBuffer::read(char *data, size_t bytes)
{
    if (bytes == 0) return 0;

    size_t capacity = capacity_;
    size_t bytes_to_read = std::min(bytes, size_);

    if (bytes_to_read <= capacity - beg_index_)
    {
        memcpy(data, data_ + beg_index_, bytes_to_read);
        beg_index_ += bytes_to_read;
        if (beg_index_ == capacity) beg_index_ = 0;
    }
    else
    {
        size_t size_1 = capacity - beg_index_;
        memcpy(data, data_ + beg_index_, size_1);
        size_t size_2 = bytes_to_read - size_1;
        memcpy(data + size_1, data_, size_2);
        beg_index_ = size_2;
    }

    size_ -= bytes_to_read;
    return bytes_to_read;
}
