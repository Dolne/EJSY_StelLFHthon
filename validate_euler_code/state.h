#ifndef state_h
#define state_h

#include <Arduino.h>

template <typename T>
class State {
public:

    bool is(T state) const
    {
        return state_ == state;
    }

    bool changed() const
    {
        return previous_ != state_;
    }

    bool changedFrom(T state) const
    {
        return previous_ == state && changed();
    }

    bool changedTo(T state) const
    {
        return state_ == state && changed();
    }

    void set(T state)
    {
        next_ = state;
    }

    void update()
    {
        previous_ = state_;
        state_ = next_;
        if (changed()) {
            since_ = millis();
        }
    }
    
    long since() const
    {
        return since_;
    }

private:
    T state_{};
    T previous_{};
    T next_{};
    long since_{};
};

#endif