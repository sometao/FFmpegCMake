/*
 * learning ffmpeg from
 * https://github.com/leixiaohua1020/simplest_ffmpeg_player/blob/master/simplest_ffmpeg_helloworld/simplest_ffmpeg_helloworld.cpp
 * Simplest FFmpeg HelloWorld
 */

#ifdef _WIN32
// Windows
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
};
#else
// Linux...
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#ifdef __cplusplus
};
#endif
#endif

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

using std::cout;
using std::endl;
using std::string;
using std::stringstream;

namespace lxh {
namespace hello {

/**
 * Configuration Information
 */
const string configurationinfo() {
  av_register_all();
  const string info = avcodec_configuration();
  return info;
}

// FIX
struct URLProtocol;
/**
 * Protocol Support Information
 */
const string urlprotocolinfo() {
  av_register_all();
  stringstream infoStream{};

  struct URLProtocol* pup = NULL;
  struct URLProtocol** p_temp = &pup;

  // Input
  avio_enum_protocols((void**)p_temp, 0);
  while ((*p_temp) != NULL) {
    auto protocol = avio_enum_protocols((void**)p_temp, 0);
    if (protocol != nullptr) {
      infoStream << "[In ]: " << protocol << endl;
    }
  }

  pup = NULL;
  // Output
  avio_enum_protocols((void**)p_temp, 1);
  while ((*p_temp) != NULL) {
    auto protocol = avio_enum_protocols((void**)p_temp, 1);
    if (protocol != nullptr) {
      infoStream << "[Out]: " << protocol << endl;
    }
  }

  return infoStream.str();
}

/**
 * AVFormat Support Information
 */
const string avformatinfo() {
  stringstream infoStream{};

  av_register_all();

  // Input
  AVInputFormat* if_temp = av_iformat_next(NULL);
  while (if_temp != NULL) {
    infoStream << "[In ]: " << if_temp->name << endl;
    if_temp = if_temp->next;
  }

  // Output
  AVOutputFormat* of_temp = av_oformat_next(NULL);
  while (of_temp != NULL) {
    infoStream << "[Out]: " << of_temp->name << endl;
    of_temp = of_temp->next;
  }
  return infoStream.str();
}

/**
 * AVCodec Support Information
 */
auto avcodecinfo() {
  stringstream infoStream{};

  av_register_all();

  AVCodec* c_temp = av_codec_next(NULL);

  string codeDirection;
  string codeType;
  string codeName;

  while (c_temp != NULL) {
    if (c_temp->decode != NULL) {
      codeDirection = "[Dec]";
    } else {
      codeDirection = "[Enc]";
    }
    switch (c_temp->type) {
      case AVMEDIA_TYPE_VIDEO:
        codeType = "[Video]";
        break;
      case AVMEDIA_TYPE_AUDIO:
        codeType = "[Audio]";
        break;
      default:
        codeType = "[Other]";
        break;
    }

    infoStream << codeDirection << codeType << ":" << c_temp->name << endl;
    ;

    c_temp = c_temp->next;
  }
  return infoStream.str();
}

/**
 * AVFilter Support Information
 */
auto avfilterinfo() {
  stringstream infoStream{};
  avfilter_register_all();

  AVFilter* f_temp = (AVFilter*)avfilter_next(NULL);

  while (f_temp != NULL) {
    infoStream << "[Filter]: " << f_temp->name << endl;
    ;
    f_temp = f_temp->next;
  }
  return infoStream.str();
}

int launch() {
  char* infostr = NULL;

  cout << "Hello FFmpeg" << endl;
  auto confInfo = configurationinfo();
  cout << "<<Configuration>>" << endl;
  cout << confInfo << endl;

  auto urlProtocalInfo = urlprotocolinfo();
  cout << "<<URLProtocol>>" << endl;
  cout << urlProtocalInfo << endl;
  urlProtocalInfo;

  auto avFormatInfo = avformatinfo();
  cout << "<<AVFormat>>" << endl;
  cout << avFormatInfo << endl;

  auto avCodes = avcodecinfo();
  cout << "<<AVCodec>>" << endl;
  cout << avCodes << endl;

  auto filterInfo = avfilterinfo();
  cout << "<<AVFilter>>" << endl;
  cout << filterInfo << endl;

  return 0;
}

}  // namespace hello

}  // namespace lxh
