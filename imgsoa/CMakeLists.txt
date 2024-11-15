
# Add to this list all files related to imgsoa library
add_library(imgsoa
        cutfreq.cpp
        cutfreq.hpp
        resize.cpp
        maxlevel.hpp
        maxlevel.cpp
        resize.hpp
        compress.cpp
        compress.hpp
)
# Use this line only if you have dependencies from this library to GSL
target_link_libraries (imgsoa PRIVATE common Microsoft.GSL::GSL)