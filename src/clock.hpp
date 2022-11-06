#ifndef CLOCK_HPP
#define CLOCK_HPP

class Clock {
    private:
        float delta;
        float lastFrame;
    public:
        Clock();
        void tick();
        float getDelta();
};

#endif