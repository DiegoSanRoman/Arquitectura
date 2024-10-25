
# Add to this list all files related to imgsoa library
add_library(imgsoa
        imgsoa.cpp
        cutfreq.cpp
        cutfreq.hpp
        resize.cpp
)
# Use this line only if you have dependencies from this library to GSL
target_link_libraries (imgsoa PRIVATE common Microsoft.GSL::GSL)