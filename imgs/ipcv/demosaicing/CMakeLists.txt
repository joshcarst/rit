rit_add_library(ipcv_demosaicing
  SOURCES
    Bilinear.cpp
    GBTF.cpp
    LarochePrescott.cpp
  HEADERS
    Bilinear.h
    GBTF.h
    LarochePrescott.h
    Demosaic.h
)

target_link_libraries(ipcv_demosaicing
  PUBLIC 
    opencv_core
)
