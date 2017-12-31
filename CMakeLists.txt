cmake_minimum_required(VERSION 2.8.3)
project(chessboard)
find_package( OpenCV 3.1 REQUIRED )
link_directories(/usr/local/lib)

add_executable(chessboard src/main.cpp)
target_link_libraries( chessboard
   ${OpenCV_LIBS} hpdf
)
