//////////////////////////////////////////////////////////////////////////////////////////
// A cross platform socket APIs, support ios & android & wp8 & window store
// universal app
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2019 halx99

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "obstream.h"
#include <iostream>
#include <fstream>

namespace yasio
{

obstream::obstream(size_t buffersize) { buffer_.reserve(buffersize); }

void obstream::push8()
{
  offset_stack_.push(buffer_.size());
  write_i(static_cast<uint8_t>(0));
}
void obstream::pop8()
{
  auto offset = offset_stack_.top();
  set_i(offset, static_cast<uint8_t>(buffer_.size()));
  offset_stack_.pop();
}
void obstream::pop8(uint8_t value)
{
  auto offset = offset_stack_.top();
  set_i(offset, value);
  offset_stack_.pop();
}

void obstream::push16()
{
  offset_stack_.push(buffer_.size());
  write_i(static_cast<uint16_t>(0));
}
void obstream::pop16()
{
  auto offset = offset_stack_.top();
  set_i(offset, static_cast<uint16_t>(buffer_.size()));
  offset_stack_.pop();
}
void obstream::pop16(uint16_t value)
{
  auto offset = offset_stack_.top();
  set_i(offset, value);
  offset_stack_.pop();
}

void obstream::push24()
{
  offset_stack_.push(buffer_.size());

  unsigned char u32buf[4] = {0, 0, 0, 0};
  write_bytes(u32buf, 3);
}
void obstream::pop24()
{
  auto offset = offset_stack_.top();
  auto value  = htonl(static_cast<uint32_t>(buffer_.size())) >> 8;
  memcpy(wdata(offset), &value, 3);
  offset_stack_.pop();
}
void obstream::pop24(uint32_t value)
{
  auto offset = offset_stack_.top();
  value       = htonl(value) >> 8;
  memcpy(wdata(offset), &value, 3);
  offset_stack_.pop();
}

void obstream::push32()
{
  offset_stack_.push(buffer_.size());
  write_i(static_cast<uint32_t>(0));
}

void obstream::pop32()
{
  auto offset = offset_stack_.top();
  set_i(offset, static_cast<uint32_t>(buffer_.size()));
  offset_stack_.pop();
}

void obstream::pop32(uint32_t value)
{
  auto offset = offset_stack_.top();
  set_i(offset, value);
  offset_stack_.pop();
}

obstream::obstream(const obstream &right) : buffer_(right.buffer_) {}

obstream::obstream(obstream &&right) : buffer_(std::move(right.buffer_)) {}

obstream::~obstream() {}

std::vector<char> obstream::take_buffer() { return std::move(buffer_); };

obstream &obstream::operator=(const obstream &right)
{
  buffer_ = right.buffer_;
  return *this;
}

obstream &obstream::operator=(obstream &&right)
{
  buffer_ = std::move(right.buffer_);
  return *this;
}

size_t obstream::write_i24(uint32_t value)
{
  value = htonl(value) >> 8;
  write_bytes(&value, 3);
  return buffer_.size();
}

size_t obstream::write_v(stdport::string_view value)
{
  return write_v(value.data(), static_cast<int>(value.size()));
}
size_t obstream::write_v16(stdport::string_view value)
{
  return write_v16(value.data(), static_cast<int>(value.size()));
}
size_t obstream::write_v8(stdport::string_view value)
{
  return write_v8(value.data(), static_cast<int>(value.size()));
}

size_t obstream::write_v(const void *v, int size) { return write_vx<uint32_t>(v, size); }
size_t obstream::write_v16(const void *v, int size) { return write_vx<uint16_t>(v, size); }
size_t obstream::write_v8(const void *v, int size) { return write_vx<uint8_t>(v, size); }

size_t obstream::write_bytes(stdport::string_view v)
{
  return write_bytes(v.data(), static_cast<int>(v.size()));
}

size_t obstream::write_bytes(const void *v, int vl)
{
  if (vl > 0)
  {
    auto offset = buffer_.size();
    buffer_.resize(buffer_.size() + vl);
    ::memcpy(buffer_.data() + offset, v, vl);
  }
  return buffer_.size();
}

void obstream::save(const char *filename)
{
  std::ofstream fout;
  fout.open(filename, std::ios::binary);
  if (!this->buffer_.empty())
    fout.write(&this->buffer_.front(), this->length());
  fout.close();
}

obstream obstream::sub(size_t offset, size_t count)
{
  obstream obs;
  auto n = length();
  if (offset < n)
  {
    if (count > (n - offset))
      count = (n - offset);

    obs.buffer_.assign(this->data() + offset, this->data() + offset + count);
  }
  return obs;
}

} // namespace purelib
