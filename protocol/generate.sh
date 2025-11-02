#!/bin/bash

# Генерация для C++
protoc --cpp_out=../server/src/protocol messages.proto

# Генерация для Java
protoc --java_out=../android/app/src/main/java messages.proto
