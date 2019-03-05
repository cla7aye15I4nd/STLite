#ifndef _algorithm_hpp
#define _algorithm_hpp

namespace sakura{
  template <typename T>
  void swap(T &a, T& b) {
    T &c = a;
    a = b; b = c;
  }
  
  template <typename T>
  T max(const T &a, const T &b) {
    return b < a ? a : b;
  }
    
  template <typename T>
  T min(const T &a, const T &b) {
    return a < b ? a : b;
  }
    
} // namespace rem

#endif
