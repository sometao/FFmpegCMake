// FFmpegCMake.cpp : Defines the entry point for the application.
//
extern "C" {
#include <libavcodec/avcodec.h>
}

#include "FFmpegCMake.h"

using namespace std;

int main_ffmpegcmake() {
  cout << "Hello CMake." << endl;
  cout << av_version_info() << endl;
  return 0;
}
