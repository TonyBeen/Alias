/*************************************************************************
    > File Name: Buffer.h
    > Author: hsz
    > Mail:
    > Created Time: Mon Jul  5 13:08:56 2021
 ************************************************************************/

#ifndef __EULAR_BUFFER_H__
#define __EULAR_BUFFER_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>

namespace eular {
class ByteBuffer final
{
public:
    ByteBuffer();
    ByteBuffer(size_t size);
    ByteBuffer(const char *data, size_t dataLength = SIZE_MAX);
    ByteBuffer(const uint8_t *data, size_t dataLength);
    ByteBuffer(const ByteBuffer& other);
    ByteBuffer(ByteBuffer&& other);
    ~ByteBuffer();

    ByteBuffer& operator=(const ByteBuffer& other);
    ByteBuffer& operator=(ByteBuffer&& other);
    uint8_t&    operator[](size_t index);

    // 在offset之后设为data
    size_t      set(const uint8_t *data, size_t dataSize, size_t offset = 0);
    void        append(const char *data, size_t dataSize = SIZE_MAX);
    void        append(const uint8_t *data, size_t dataSize);
    void        append(const ByteBuffer &other);
    // 在offset之后插入数据而不覆盖之后的数据
    size_t      insert(const uint8_t *data, size_t dataSize, size_t offset = 0);

    uint8_t *   data() { return mBuffer ? mBuffer : nullptr; }
    const uint8_t *const_data() const { return mBuffer ? mBuffer : nullptr; }
    const uint8_t *begin() const { return mBuffer ? mBuffer : nullptr; }                       // 返回数据开始地址
    const uint8_t *end() const { return mBuffer ? (mBuffer + mDataSize - 1) : nullptr; }       // 返回数据结束地址
    void        reserve(size_t newSize);
    size_t      capacity() const { return mCapacity; }
    size_t      size() const { return mDataSize; }
    void        clear();
    void        resize(size_t sz) { mDataSize = sz > mCapacity ? mCapacity : sz; }

    std::string dump()  const;
    static size_t hash(const ByteBuffer &buf);

private:
    size_t      calculate(size_t);
    size_t      allocBuffer(size_t size);
    void        freeBuffer();
    void        moveAssign(ByteBuffer &other);
    void        detach();

private:
    uint8_t*    mBuffer;
    size_t      mDataSize;
    size_t      mCapacity;
};

} // namespace eular

namespace std {
    template<>
    struct hash<eular::ByteBuffer> : public __hash_base<size_t, eular::ByteBuffer> {
        size_t operator()(const eular::ByteBuffer &obj) const {
            return eular::ByteBuffer::hash(obj);
        }
    };
}

#endif // __EULAR_BUFFER_H__
