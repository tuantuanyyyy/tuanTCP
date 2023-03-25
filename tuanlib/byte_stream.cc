#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

#include <vector>

using namespace std;

ByteStream::ByteStream(const size_t capacity) : _capacity(capacity)  {
	buffer = vector<char>(_capacity);
	read_count = 0;
	write_count = 0;
}

size_t ByteStream::write(const string &data) {
   size_t count = 0;
    for(const auto &item : data){
    	if(write_count >= read_count + _capacity){//注意这个判断条件，
		    break;
	    }
    	buffer[write_count % _capacity] = item;
	    write_count++;
	    count++;
    }
    return count;
}

//! \param[in] len bytes will be copied from the output side of the buffer
//将长度为len的buffer的值获取到
string ByteStream::peek_output(const size_t len) const {
    string s = string();
    for(size_t i = read_count; (i < read_count + len) && (i < write_count); i++){
    	s.push_back(buffer[i % _capacity]);
    }
    return s;
}
 
//! \param[in] len bytes will be removed from the output side of the buffer
//将长度为len的buffer弹出(删除)
//直接移动read_count就行，直接理解为已读，也就是说直接允许覆盖，这样就相当于删除了
void ByteStream::pop_output(const size_t len) {
    if(read_count <= write_count){
    	if(read_count + len <= write_count){
		read_count += len;
    	}else{
		read_count = write_count;
	}
    }else{
	    if(read_count + len  <= _capacity){
	    	read_count += len;
	    }else{
	    	read_count = (len - (_capacity- read_count) % _capacity) - 1;
		if(read_count > write_count){ read_count = write_count; }
	    }
    }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string s = peek_output(len);
    pop_output(s.length());    
	
    return s;
}

void ByteStream::end_input() {	input_end_flag = true;	}

bool ByteStream::input_ended() const { return input_end_flag; }

size_t ByteStream::buffer_size() const { 
   // if(read_count > write_count){
   //	return read_count - write_count - 1;
   // }	
    return write_count - read_count; 
}

bool ByteStream::buffer_empty() const { return buffer_size() == 0; }

bool ByteStream::eof() const { return input_end_flag && buffer_empty(); }

size_t ByteStream::bytes_written() const { return write_count; }

size_t ByteStream::bytes_read() const { return read_count; }

size_t ByteStream::remaining_capacity() const { return _capacity - (write_count -  read_count); }
