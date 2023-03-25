#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity): _output(capacity), _capacity(capacity){}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
//data是子字符串，index是这个子字符串的第一个字节的index，eof是此子字符串是否是最后的
void StreamReassembler::push_substring(const string &data, const uint64_t index, const bool eof) {
    if(eof){
        _eof = true;
        _eof_index = index + data.length();
    }

    if(first_unassembled() > index){//有已处理过的数据到来
        if(first_unassembled() - index >= data.length()){
            //全部都是老数据的情况,全部丢弃
            return;
        }
        //如果一部分是已装配数据，一部分是未装配数据，则将未装配的数据递归处理
        push_substring(data.substr(first_unassembled() - index), first_unassembled(), eof);
        return;
    }

    size_t old_first_unassembled = first_unassembled();
    size_t old_first_unacceptable = first_unacceptable();

    if(first_unassembled() == index){
        //此情况是可以直接放入_output中的.
        // 因为index==最后一个已装配的字节index+1，
        // 此时就可以直接将收到的字节流写到_capacity中
        _output.write(data);
        //滑动窗口右移，且判断右移后，是否也可以写到_output中
        reassemble(old_first_unassembled, old_first_unacceptable);
    }else{//乱序到达的，写入_unassemble_str缓存中
        put_in_buf(data, index);
    }

    if(_eof && first_unassembled() == _eof_index){
        _buf.clear();//清空
        _output.end_input();//表明此后不会再有字符串过来
    }
}

void StreamReassembler::reassemble(size_t old_first_unassembled, size_t old_first_unacceptable) {
    string s = string();
    for(size_t i = old_first_unassembled; i < old_first_unacceptable; i++){
        if(_buf.find(i) != _buf.end()){
            if(i < first_unassembled()){//如果已经写入，则清除
                _buf.erase(i);
                continue;
            }
            if(i == first_unassembled() + s.length()){
                s.push_back(_buf.at(i));//拼接字节流
                _buf.erase(i);//清除
            }else{
                break;
            }
        }
    }
    _output.write(s);//写入_output，且会改变first_unassembled & first_unacceptable
}

void StreamReassembler::put_in_buf(const string &data, const size_t index){
    for(size_t i = 0; i < data.length(); i++){
        if(index + i < first_unacceptable()){
            _buf.insert(std::pair<size_t, char>(index+ i, data[i]));
        }else{
            break;
        }
    }
}

//返回第一个未装配的index 也就是已装配的最后一个字节的index+1
size_t StreamReassembler::first_unassembled() const { return _output.bytes_written(); }

size_t StreamReassembler::first_unacceptable() const { return _output.bytes_read() + _capacity; }

size_t StreamReassembler::unassembled_bytes() const { return _buf.size(); }

bool StreamReassembler::empty() const { return unassembled_bytes() == 0; }