cmake_minimum_required(VERSION 3.0)

project(qrcodegen CXX)

set(
    SOURCE_FILES
    cpp/BitBuffer.cpp
    cpp/QrCode.cpp
    cpp/QrSegment.cpp
)

set(
    HEADER_FILES
    cpp/BitBuffer.hpp
    cpp/QrCode.hpp
    cpp/QrSegment.hpp
)

add_library(qrcodegen STATIC ${SOURCE_FILES} ${HEADER_FILES})

install(TARGETS qrcodegen ARCHIVE DESTINATION lib)
install(FILES ${HEADER_FILES} DESTINATION include/qrcodegen)

