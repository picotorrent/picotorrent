#pragma once

#include <windows.h>

#define SX(val) Scaler::X(val)
#define SY(val) Scaler::Y(val)

class Scaler
{
public:
    explicit Scaler();
    ~Scaler();

    static long X(long x);
    static long Y(long y);

private:
    static Scaler& GetInstance();
    const HDC hdc_;
    int x_;
    int y_;
};
