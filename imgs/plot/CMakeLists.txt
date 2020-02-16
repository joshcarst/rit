set(SRCS
)

set(HDRS
  plot.h
)

add_subdirectory(plot2d)

rit_add_library(plot SOURCES ${SRCS} HEADERS ${HDRS})

target_link_libraries(plot
  PUBLIC
    opencv_core
    rit::gnuplot-iostream
)
