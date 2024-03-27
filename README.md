# Modified Better String Library

This is an inspired version of Paul Hsieh's [Better String Library][1], [Better String Library][2], [sds][3] 

bstr is a string library for c designated to augmenting the limited libc string handling. Strings are handled similarly to rust where a fat pointer is used to hold the string and slices can be passed around to operate/inspect a string buffer.
The only form of allocation in this library is with bstr so any bstr declared has to be ended with a `bstrFree()`. 

A c style string/bstr can be converted to a slice. A slice is a base pointer + len. This works similar to a `std::span`. 


[1]: http://bstring.sourceforge.net/
[2]: https://github.com/msteinert/bstring/
[3]: https://github.com/antirez/sds