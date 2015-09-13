cmake_minimum_required(VERSION 2.8.10)
PROJECT(MatchingEngine)

file(GLOB MATCHINGENGINE_SRCS "*.cpp")
ADD_EXECUTABLE (MatchingEngine ${MATCHINGENGINE_SRCS})
