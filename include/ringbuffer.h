#ifndef LISP_CIRCULARBUFFER_H
#define LISP_CIRCULARBUFFER_H

#include <cstddef>
template <class T, size_t Size> class RingBuffer {
  public:
    using value_type = T;
    using size_type = size_t;
    //using difference_type = ;
    using iterator = T *;
    using const_iterator = const T *;

  private:
    T m_buf[Size];
    iterator m_tail = m_buf;
	iterator m_head = m_buf;
    bool m_full = false;

  public:
    RingBuffer()= default;

	T& tail(){
        return *m_tail;
	}
	T& head(){
        return *m_head;
	}

    void push(T val){
        push_back(val);
    }

    void reset(){
		m_head = m_tail;
		m_full = false;
	}
	bool empty() const {
        return m_full && (m_head == m_tail);
	}
	bool full() const {
        return m_full;
	}
	size_t capacity(){ return Size; }

	size_t size(){

		if(!m_full){
			if(m_head >= m_tail){
				return m_head - m_tail;
			} else {
                return Size + (m_head - m_tail);
			}
		}
		return Size;
	}

    void push_back(T val){
        *m_head = val;
		if(m_full){
			m_tail = m_buf + (((m_tail - m_buf) + 1) % Size);
		}
		m_head = m_buf + (((m_head - m_buf) + 1) % Size);
		m_full = m_head == m_tail;
    }
    T pop(){
        if(empty()){
			return T();
		}
		auto val = *m_tail;
		m_full = false;
		m_tail = m_buf + (((m_tail - m_buf) + 1) % Size);
		return val;
    }

    T &operator[](size_t index) {
        return *(m_buf + (((m_head - m_buf) + index) % Size));
	}
};

#endif //LISP_CIRCULARBUFFER_H
