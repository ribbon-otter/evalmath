evalmath is a minimalistic simple-to-call math expression interpretor for C++23. It supports the basic arithmatic operators. All math is done using doubles.

# Usage
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

`evalmath::eval` returns a `std::expected` which on the expected side contains the double which is the answer to the problem.  The unexpected side contains a string error message.

## Development details
Internally evalmath uses a handwritten shift-reduce parser to parse the math expression. It does not build a parse tree, but instead evaluates the math expressions into new synthesised number tokens. 

Given the challenge of writing a shift-reduce parser by hand correctly, the perl script `fuzz.pl` generates and calls many combinations both valid and invalid formulas. For the valid formulas (according to evalmath's own judgement), the output is compared against perl's math engine. Thus, evalmath is unlikely to have a bug which incorrectly evaluates a forumla; though it is somewhat more possible that a bug exists which rejects a valid formula; though the gtest-based unit tests cover the common ones.

# License
see LICENSE.md for details. This software is under MIT-0, so you can do whatever you want with it. No attribution required (though still appreciated if you choose to do so).
