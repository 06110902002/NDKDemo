# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 指定为C++11 版本
set(CMAKE_CXX_STANDARD 11)

add_compile_options(-fno-elide-constructors) #关闭编译器优化


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             native-lib.cpp
        utils/SafeQueue.cpp
        utils/Aircraft.cpp
        utils/Bomber.cpp
        utils/BlockQueue.cpp
        utils/VideoFrame.cpp
        utils/threadsafe_queue.cpp
        utils/StackTest.cpp
        utils/ArrayTest.cpp
        utils/SortMgr.cpp
        model/Cell.cpp
        struct/Person.cpp
        struct/LinkList.cpp
        struct/BinaryTree.cpp
        struct/Memory.cpp
        struct/SmartPoint.cpp
        struct/ObjPool.cpp
        utils/SocketMgr.cpp
        model/BaseMsgEntity.cpp
        struct/LruCache.cpp
        struct/queue/FrameQueue.cpp
        utils/NDKStackUtils.cpp
        utils/JavaObjectMap.hpp
        msgqueue/Handler.cpp
        mediacodec/MediaCodec.cpp
        mediacodec/H264Demux.cpp
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# 引入头文件
#include_directories(${distribution_DIR}/include)
include_directories(head
        mediacodec
        model
        struct
        msgqueue
        utils)   #头文件  放在cpp目录下也可以

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib
                        mediandk
                        android
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )