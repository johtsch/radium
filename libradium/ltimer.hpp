#include <iostream>
#include <chrono>

/* thx to gongzhitaao you are a real mvp
    so-link: https://stackoverflow.com/questions/728068/how-to-calculate-a-time-difference-in-c
*/

class LTimer{
public:
    LTimer() : beg_(clock_::now()) {}
    void reset() { beg_ = clock_::now(); }
    double elapsed() const { 
        return std::chrono::duration_cast<second_>
            (clock_::now() - beg_).count(); }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1> > second_;
    std::chrono::time_point<clock_> beg_;
};