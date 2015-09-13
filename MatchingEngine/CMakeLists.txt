cmake_minimum_required(VERSION 2.8.10)
PROJECT(NimbusExchange)

file(GLOB MATCHINGENGINE_SRCS "*.cpp" "*.h")
ADD_EXECUTABLE (NimbusExchange ${MATCHINGENGINE_SRCS})
