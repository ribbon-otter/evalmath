evalmath is a minimalistic simple-to-call math expression interpretor for C++23. It supports the basic arithmatic operators.

# usage
copy `evalmath.h` and `evalmath.cpp` into your project. Compile them both with C++23.

```C++
#include "evalmath.h"

int main() {
    auto result = evalmath::eval("2+5*2");
    if (result) {
        //the forumla parsed successfully
        println("{}", *result);
    } else {
        //there was an error
        // result.error() contains a std::string describing the error
        println(stderr, "the error: {}", result.error());
    }
}
```

# License
see LICENSE.md for details. This software is under MIT-0, so you can do whatever you want with it. No attribution required (though still appreciated if you choose to do so).
