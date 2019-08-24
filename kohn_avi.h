//2011_02_14
/*

This code is copyright 2008-2011 by Michael Kohn
This code falls under the LGPL license

http://www.mikekohn.net/
Michael Kohn <mike@mikekohn.net>

*/

#ifndef KOHN_AVI_H
#define KOHN_AVI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

struct kohn_avi_audio_t
{
  int channels;
  int bits;
  int samples_per_second;
};

struct kohn_avi_header_t
{
  int time_delay;
  int data_rate;
  int reserved;
  int flags;
  int number_of_frames;
  int initial_frames;
  int data_streams;
  int buffer_size;
  int width;
  int height;
  int time_scale;
  int playback_data_rate;
  int starting_time;
  int data_length;
};

struct kohn_stream_header_t
{
  char data_type[5];
  char codec[5];
  int flags;
  int priority;
  int initial_frames;
  int time_scale;
  int data_rate;
  int start_time;
  int data_length;
  int buffer_size;
  int quality;
  int sample_size;
};

struct kohn_stream_format_v_t
{
  unsigned int header_size;
  unsigned int width;
  unsigned int height;
  unsigned short int num_planes;
  unsigned short int bits_per_pixel;
  unsigned int compression_type;
  unsigned int image_size;
  unsigned int x_pels_per_meter;
  unsigned int y_pels_per_meter;
  unsigned int colors_used;  
  unsigned int colors_important;  
  unsigned int *palette;
  unsigned int palette_count;
};

struct kohn_stream_format_a_t
{
  unsigned short format_type;
  unsigned short channels;
  unsigned int sample_rate;
  unsigned int bytes_per_second;
  unsigned short block_align;
  unsigned short bits_per_sample;
  unsigned short size;
};

struct kohn_avi_index_t
{
  int id;
  int flags;
  int offset;
  int length;
};

struct kavi_t
{
  FILE *out;
  struct kohn_avi_header_t avi_header;
  struct kohn_stream_header_t stream_header_v;
  struct kohn_stream_format_v_t stream_format_v;
  struct kohn_stream_header_t stream_header_a;
  struct kohn_stream_format_a_t stream_format_a;
  long marker;   /* movi marker */
  int offsets_ptr;
  int offsets_len;
  long offsets_start;
  unsigned int *offsets;
  int offset_count;
};

struct kavi_t *kavi_open(char *filename, int width, int height, char *fourcc, int fps, struct kohn_avi_audio_t *audio);
void kavi_add_frame(struct kavi_t *kavi, unsigned char *buffer, int len);
void kavi_add_audio(struct kavi_t *kavi, unsigned char *buffer, int len);
void kavi_close(struct kavi_t *kavi);

// If needed, these functions can be called before closing the file to
// change the framerate, codec, size.  Note: AVI can only have a single
// frame rate, codec, size for the whole file so this affects anything
// recorded before these functions are called.  These were added so
// libkohn_rtsp.so could open the file first, open the RTSP next, and
// later reset framerate, codec, and size without losing data.
void kavi_set_framerate(struct kavi_t *kavi, float fps);
void kavi_set_codec(struct kavi_t *kavi, const char *fourcc);
void kavi_set_size(struct kavi_t *kavi, int width, int height);

#ifdef __cplusplus
}
#endif

#endif

