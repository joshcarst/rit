rit_add_library(ipcv_seam_carving
  SOURCES
    SeamCarving.cpp
  HEADERS
    SeamCarving.h
)

target_link_libraries(ipcv_seam_carving
  PUBLIC 
    opencv_core
)
