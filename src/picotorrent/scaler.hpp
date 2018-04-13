
#define SX(val) pt::Scaler::X(val)
#define SY(val) pt::Scaler::Y(val)

namespace pt
{
    class Scaler
    {
    public:
        explicit Scaler();

        static long X(long x);
        static long Y(long y);

    private:
        static Scaler& GetInstance();
        int x_;
        int y_;
    };
}
