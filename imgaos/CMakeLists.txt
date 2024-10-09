# Add to this list all files related to imgaos library
add_library(imgaos
        imgaos.cpp
)
# Use this line only if you have dependencies from this library to GSL
target_link_libraries (imgaos PRIVATE common Microsoft.GSL::GSL)