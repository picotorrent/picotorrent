
#define SX(val) pt::Scaler::X(val)
#define SY(val) pt::Scaler::Y(val)

#define iSX(val) pt::Scaler::inverseX(val)
#define iSY(val) pt::Scaler::inverseY(val)

namespace pt
{
    class Scaler
    {
    public:
        explicit Scaler();

        static long X(long x);
        static long Y(long y);

        static long inverseX(long x);
        static long inverseY(long y);

    private:
        static Scaler& GetInstance();
        int x_;
        int y_;
    };
}
