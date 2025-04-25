#include "tools/index2.hpp"



class WaveFunctionCollapse
{
public:
    using FactorID = int;

    WaveFunctionCollapse(const int h, const int w);
    ~WaveFunctionCollapse();

    void init();

    [[nodiscard]] bool set(const cha::Int2 pos, const FactorID id);

    [[nodiscard]] bool generate();

    void print() const;

    [[nodiscard]] int getHeight() const;
    [[nodiscard]] int getWidth() const;

private:
    struct Impl;
    Impl* p_impl_;
};