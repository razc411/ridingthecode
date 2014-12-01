#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <algorithm>
#include <cstring>
#include "definitions.h"

class CircularBuffer
{
    public:
      CircularBuffer(size_t capacity);
      ~CircularBuffer();

      size_t size() const { return size_; }
      size_t capacity() const { return capacity_; }
      size_t write(const char *data, size_t bytes);
      size_t read(char *data, size_t bytes);

    private:
      size_t beg_index_, end_index_, size_, capacity_;
      char *data_;
};

#endif // CIRCULARBUFFER_H
