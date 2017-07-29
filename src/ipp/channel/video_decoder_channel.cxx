#include <fcntl.h>
#include "config.h"
#include "ipp/logger/logger.hxx"
#include "ipp/ipp_system_exception.hxx"
#include "ipp/channel/video_decoder_channel.hxx"

namespace ipp {
	namespace channel {
		namespace {
			struct video_decoder_exception : ipp_exception {
				video_decoder_exception(const std::string& s) : ipp_exception(s) {}
			};
		}

		video_decoder_channel::video_decoder_channel(ipphone& ipp, uint32_t ch_id,
		                                             channel_wrapper::channel_type ch_type)
				: channel_wrapper(ch_id, ch_type), _ipp(ipp) {
			avcodec_register_all();
			av_init_packet(&_avpkt);

			/* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
			memset(_inbuf + inbuf_size, 0, AV_INPUT_BUFFER_PADDING_SIZE);

			/* find the MPEG-1 video decoder */
			_codec = avcodec_find_decoder(AV_CODEC_ID_MPEG2VIDEO);
			if (!_codec) {
				IPP_THROW_EXCEPTION(ipp_exception("codec not found"));
			}

			_context = avcodec_alloc_context3(_codec);
			if (!_context) {
				IPP_THROW_EXCEPTION(ipp_exception("could not allocate video codec context"));
			}
			if (_codec->capabilities & AV_CODEC_CAP_TRUNCATED) {
				_context->flags |= AV_CODEC_FLAG_TRUNCATED;
			} // we do not send complete frames
			if (avcodec_open2(_context, _codec, NULL) < 0) {
				IPP_THROW_EXCEPTION(ipp_exception("could not open codec"));
			}
			_frame = av_frame_alloc();
			if (!_frame) {
				IPP_THROW_EXCEPTION(ipp_exception("could not allocate video frame"));
			}
		}

		void video_decoder_channel::receive(const std::uint8_t* buf, const std::uint16_t len) {
			std::memcpy(_inbuf, buf, len);
			_avpkt.size = len;
			_avpkt.data = _inbuf;
#ifdef IPPBIN_HAVE_AVCODEC_SEND_PACKET
			avcodec_send_packet(_context, &_avpkt);
			while (avcodec_receive_frame(_context, _frame) == 0) {
				callback(_frame->data[0], _frame->linesize[0], _frame->width, _frame->height);
				_frame_count++;
			}
#else
			int got_frame;
			int frame_len = avcodec_decode_video2(_context, _frame, &got_frame, &_avpkt);
			if (frame_len < 0) {
				fprintf(stderr, "Error while decoding frame %d\n", _frame_count);
			}
			if (got_frame) {
				LOG(DEBUG) << "Saving frame " << _frame_count;
				callback(_frame->data[0], _frame->linesize[0], _frame->width, _frame->height);
				_frame_count++;
			}
			if (_avpkt.data) {
				_avpkt.size -= len;
				_avpkt.data += len;
			}
#endif
		}

		void video_decoder_channel::flush_packets() {
		}

		void video_decoder_channel::close() {
		}

		void video_decoder_channel::callback(callback_type c) {
			_callback = std::move(c);
		}

		void video_decoder_channel::callback(uint8_t* mono_data, int line_height, int width, int height) {
			if (_callback) {
				_callback(mono_data, line_height, width, height);
			}
		}
	}
}
