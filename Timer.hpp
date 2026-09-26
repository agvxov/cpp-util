/* The following header is part of CPP-Util version c91b.
 * The upstream can be found at: https://github.com/agvxov/cpp-util
 * It is in the Public Domain.
 */
class Timer {
  public:
    int start;
    int end;
    int interval;
    int counter;

    bool is_going_off;

    Timer(int start_, int interval_) {
        start    = start_;
        end      = start_ + interval_;
        interval = interval_;
        counter  = start_;
    }

    void rearm(int i) {
        is_going_off = false;
        start   = i;
        end     = i + interval;
        counter = i;
    }

    float progress(void) {
        return (float)(counter - start) / (float)(end - start);
    }

    bool is_expired(void) {
        return counter >= end;
    }

    void update(int i) {
        if (is_expired()) {
            is_going_off = true;
        }

        counter = i;

        if (is_expired()) {
            is_going_off = true;
        }
    }
};
