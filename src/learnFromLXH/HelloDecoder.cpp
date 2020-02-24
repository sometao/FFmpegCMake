/*
 * learning ffmpeg from
 * https://github.com/leixiaohua1020/simplest_ffmpeg_player/blob/master/simplest_ffmpeg_decoder/simplest_ffmpeg_decoder.cpp
 * simplest_ffmpeg_decoder.cpp
 */

#ifdef _WIN32
// Windows
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
};
#else
// Linux...
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
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
namespace decoder {

int launch() {
  cout << "Hi, decoder." << endl;

  char filePath[] = "D:/data/video/VID_20181123_214724.mp4";  // file input here

  FILE* fp_yuv = fopen("D:/data/tmp/ffmepg_test/output003.yuv", "wb+");

  av_register_all();
  avformat_network_init();

  AVFormatContext* pFormatCtx = avformat_alloc_context();

  if (avformat_open_input(&pFormatCtx, filePath, NULL, NULL) != 0) {
    printf("Couldn't open input stream.\n");
    return -1;
  }
  if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
    printf("Couldn't find stream information.\n");
    return -1;
  }
  int videoIndex = -1;
  for (int i = 0; i < pFormatCtx->nb_streams; i++) {
    cout << "stream [" << i << "] type: [" << pFormatCtx->streams[i]->codec->codec_type << "]"
         << endl;
    if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
      videoIndex = i;
      break;
    }
  }

  if (videoIndex == -1) {
    printf("Didn't find a video stream.\n");
    return -1;
  }

  AVCodecContext* pCodecCtx = pFormatCtx->streams[videoIndex]->codec;
  cout << "codec_id        [" << pCodecCtx->codec_id << "]" << endl;
  cout << "pCodecCtx width [" << pCodecCtx->width << "]" << endl;
  cout << "pCodecCtx height [" << pCodecCtx->height << "]" << endl;

  AVCodec* pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
  if (pCodec == NULL) {
    printf("Codec not found.\n");
    return -1;
  }
  cout << "-------" << endl;
  cout << "pCodec codec_name      [" << pCodec->name << "]" << endl;
  cout << "pCodec codec_long_name [" << pCodec->long_name << "]" << endl;

  if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
    printf("Could not open codec.\n");
    return -1;
  }
  cout << "-------" << endl;
  cout << "pCodecCtx->codec codec_name      [" << pCodecCtx->codec->name << "]" << endl;
  cout << "pCodecCtx->codec codec_long_name [" << pCodecCtx->codec->long_name << "]" << endl;



  AVFrame* pFrameYUV = av_frame_alloc();

  unsigned char* out_buffer = (unsigned char*)av_malloc(
      av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));

  av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P,
                       pCodecCtx->width, pCodecCtx->height, 1);

  AVPacket* packet = (AVPacket*)av_malloc(sizeof(AVPacket));
  // Output Info-----------------------------
  printf("--------------- File Information ----------------\n");
  av_dump_format(pFormatCtx, videoIndex, filePath, 0);
  printf("-------------------------------------------------\n");

  struct SwsContext* img_convert_ctx =
      sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width,
                     pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

  int counter = 0;

  int got_picture = 0;
  AVFrame* pFrame = av_frame_alloc();
  while (av_read_frame(pFormatCtx, packet) >= 0) {
    if (packet->stream_index == videoIndex) {
      int ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
      if (ret < 0) {
        printf("Decode Error.\n");
        return -1;
      }
      if (got_picture) {
        sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0,
                  pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);

        int y_size = pCodecCtx->width * pCodecCtx->height;
        fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);      // Y
        fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  // U
        fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  // V
        counter += 1;
        // if ((counter % 20) == 0) {
        printf("Flush Decoder: Succeed to decode %d frame!\n", counter);
        //}
        break;
      }
    }
    av_free_packet(packet);
  }

  cout << "Flush decoder: Flush Frames remained in Codec. ----------------" << endl;
  // flush decoder
  // FIX: Flush Frames remained in Codec
  while (0) {
    int ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
    if (ret < 0) break;
    if (!got_picture) break;
    sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0,
              pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);

    int y_size = pCodecCtx->width * pCodecCtx->height;
    fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);      // Y
    fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  // U
    fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  // V

    counter += 1;
    printf("Flush Decoder: Succeed to decode %d frame!\n", counter);
  }

  sws_freeContext(img_convert_ctx);

  fclose(fp_yuv);

  av_frame_free(&pFrameYUV);
  av_frame_free(&pFrame);
  avcodec_close(pCodecCtx);
  avformat_close_input(&pFormatCtx);

  return 0;
}

}  // namespace decoder

}  // namespace lxh
