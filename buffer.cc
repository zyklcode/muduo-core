#include "buffer.h"
#include <sys/uio.h>
#include <unistd.h>

ssize_t Buffer::readFd(int fd, int* savedErrno){
    char extrabuf[65535] = {0};
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + writeIndex_;
    vec[0].iov_len = writable;

    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    const int iovcnt = writable < sizeof(extrabuf) ? 2 : 1;

    const ssize_t n = ::readv(fd, vec, iovcnt);

    if(n < 0){
        *savedErrno = errno;
    }else if (n <= writable) {
        writeIndex_ += n;
    }else{
        writeIndex_ = buffer_.size();
        append(extrabuf, n - writable);
    }
    
    return n;
}

ssize_t Buffer::writeFd(int fd, int* savedErrno) {
    ssize_t n = ::write(fd, peek(), readableBytes());
    if (n < 0) {
        *savedErrno = errno;
    } else {
        retrieve(n);
    }
    return n;
}