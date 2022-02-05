#pragma once

#include "__functional_base.h"

namespace std23
{

template<class S> class function;

template<class R, class... Args> class function<R(Args...)>
{
  public:
    explicit operator bool() const noexcept { return false; }
};

} // namespace std23