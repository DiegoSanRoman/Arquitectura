# Add to this list all files related to imgaos library
add_library(imgaos
        cutfreq.cpp
        cutfreq.hpp
        maxlevel.hpp
        maxlevel.cpp
        resize.hpp
        resize.cpp
        compress.cpp
        compress.hpp
)
# Use this line only if you have dependencies from this library to GSL
target_link_libraries (imgaos PRIVATE common Microsoft.GSL::GSL)