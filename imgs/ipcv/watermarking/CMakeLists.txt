rit_add_library(ipcv_watermarking
  SOURCES
    Watermarking.cpp
  HEADERS
    Watermarking.h
)

target_link_libraries(ipcv_watermarking
  PUBLIC 
    rit::ipcv_utils 
    opencv_core
    opencv_highgui
)
