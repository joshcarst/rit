rit_add_executable(character_recognition
  SOURCES
    character_recognition.cpp
)

target_link_libraries(character_recognition
  Boost::filesystem 
  Boost::program_options 
  opencv_core
  opencv_highgui
  opencv_imgcodecs
  rit::plot
)
