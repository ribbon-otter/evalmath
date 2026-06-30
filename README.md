evalmath is a minimalistic simple-to-call math expression interpretor for C++23. It supports the basic arithmetic operators (+-\*/). All math is done using doubles.

# Usage
copy `evalmath.h` and `evalmath.cpp` into your project. Compile them both with C++23. You can see an example integration [here](https://github.com/ribbon-otter/LibreSprite/tree/d1951d56bfdd831b301ab3f901c3f6ddc44b405b/src/evalmath) (`evalmath_tests.cpp` was included there only for the eventually of their copy getting customizations and thus needing testing).

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

Evalmath is intended for situations, like math expressions in a textbox, where each math formula will only be evaluated once or a few times, and such doesn't offer a separate compilation step. You likely want a different library, perhaps [muparser](https://beltoforion.de/en/muparser/) if you are evaluating the formula each frame with varying inputs, or need more advanced mathematics.

The error messages are strings in English and are meant to be shown to the user. If you need support for a translation framework or an idea for how to include one without making basic usage more complex, feel free to open a bug report or pull request. 

Currently '(2+3) 4' gives the error 'missing operator between 5 and 4', reporting the evaluation of the expression '2+3' as '5' rather than the original '(2+3)'. I currently judge tracking the original portions of the formula each intermediate value corresponds to as not worth the extra code complexity. However, if you need this feature, consider opening a bug report requesting it.

## Development details
Internally evalmath uses a handwritten shift-reduce parser to parse the math expression. It does not build a parse tree, but instead evaluates the math expressions into new synthesised number tokens. 

Given the challenge of writing a shift-reduce parser by hand correctly, the perl script `fuzz.pl` generates and calls many combinations characters: both valid and invalid formulas. For the valid formulas (according to evalmath's own judgement), the output is compared against perl's math engine. Thus, evalmath is unlikely to have a bug which incorrectly evaluates a formula; though it is somewhat more possible that a bug exists which rejects a valid formula; though the gtest-based unit tests cover the common ones.

`evalm.cpp` is a simple cli calculator used for testing with the `fuzz.pl`.

# License
This software is under MIT-0, so you can do whatever you want with it. No attribution required (though still appreciated if you choose to do so). see [LICENSE.md](./LICENSE.md) for details. 
