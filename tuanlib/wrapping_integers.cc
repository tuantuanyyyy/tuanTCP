#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
//  abs seqno -> seqno
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    //这里如果isn + n 超出了范围，是要循环的 不要取余吗？ :(isn + n) % 范围
    //不需要取余，因为 n是64位的，这里+运算符重载了。unsigned 64位的n变成了unsigned 32位。超出范围时，刚好被溢出(舍弃掉了前32位bit),就相当于取余了。
    uint32_t nn = static_cast<uint32_t>(n);
    printf("%d",nn);

    return isn + nn;
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.


// seqno -> abs seqno
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    // 思路一：checkpoint 为 c，seqno 为 s，M = 2^{32}。
    // 那么问题就转化为了：求一个 s_a = (asb seqno)，
    // 使得 s_a = (s - isn) mod M，同时，最小化 |s_a - c| 。
    /* test:
    uint32_t c = static_cast<uint32_t>((1UL << 32) - 1UL);//4,294,967,295
    printf("%d", c);
    INT32_MAX;
    UINT32_MAX; //4,294,967,295
    uint32_t a = 1;
    uint32_t b = 0;
    uint32_t x = a - b; //1
    int32_t  y = a - b; //1
    printf("%d %d", x,y);
    // x=1 y=1
    uint32_t xx = b - a; //4,294,967,295
    int32_t  yy = b - a; //-1
    printf("%d %d", xx,yy);

    uint32_t aa = 1;
    uint32_t bb = static_cast<uint32_t>((1UL << 32) - 1UL);
    uint32_t xxx = aa - bb; //2
    int32_t  yyy = aa - bb; //2
    // x=2 y=2
    uint32_t xxxx = bb - aa;
    int32_t  yyyy = bb - aa; //-2
    printf("%d %d %d %d", xxx, yyy,xxxx,yyyy);
    */

    int32_t distance = n - wrap(checkpoint, isn);
    int64_t ret = distance + checkpoint;

    if(ret >= 0){
        return static_cast<uint64_t>(ret);
    }else{
        return ret + (1ul << 32);
    }
}
