#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    // 未开始
    //如果_isn还未有值，且到来的seg的syn为false,则说明我们还不知道第一个seg是谁，且不是现在到来的这个seg
    //所以可以直接return这次的调用
    if (!_isn.has_value() && !seg.header().syn) {
        return;
    }
    //走到这里，说明我们已经知道的_isn是什么了

    //这里的if逻辑是专门针对_isn的seg
    //如果已经知道_isn 但不是_isn是不会走这里面的逻辑的
    //当然。这里的逻辑会被走多次，因为，比如会传多次_isn的seg，但是push_substring中应该会判断，如果是已经传过的seg则会丢弃
    //设置初始序列号
    if (seg.header().syn) {
        _isn = seg.header().seqno;
        _reassembler.push_substring(seg.payload().copy(), 0, seg.header().fin);
        update_ackno();
        return;
    }

    //走到这里，就说明是已经知道_isn了，且syn为false的seg，这里是更经常走的逻辑，即
    // 先将seqno转为abs_seqno，然后交给push_substring处理
    // seqno -> abs seqno
    uint16_t abs_seqno = unwrap(seg.header().seqno, _isn.value(), _reassembler.first_unassembled());
    //这里为什么要选first_unassembled作为checkpoint？ 因为简单而言，我们已知的一个肯定是下一次要接受的字节的编号一定在first_unassembled附近

    //之所以要abs_seqno-1是因为abs_seqno从syn开始，字节流的标记没有包括syn，导致相差1
    _reassembler.push_substring(seg.payload().copy(), abs_seqno - 1, seg.header().fin);

    update_ackno();
}

void TCPReceiver::update_ackno(){
    if(_reassembler.stream_out().input_ended()){
        //_reassembler.first_unassembled()是字节流标记，要转为abs_seqno标记就要考虑是否有syn fin,
        // 有两个就要+2才能转为abs_seqno, (这就解释了，下面有+1或者+2的原因)
        // 然后我们再调用wrap函数，将n(是一个abs seqno)转为seqno
        _ackno = wrap(_reassembler.first_unassembled() + 2, _isn.value());
    }else{
        _ackno = wrap(_reassembler.first_unassembled() + 1, _isn.value());
    }
}

optional<WrappingInt32> TCPReceiver::ackno() const { return _ackno; }

size_t TCPReceiver::window_size() const { return _reassembler.first_unacceptable() - _reassembler.first_unassembled(); }
