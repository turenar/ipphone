#include <fcntl.h>
#include "ipp/logger/logger.hxx"
#include "ipp/ipp_system_exception.hxx"
#include "ipp/channel/video_encoder_channel.hxx"

namespace ipp {
	namespace channel {
		namespace {
			int fork_gif_encoder() {
				int inout[2]; // read=[0], write=[1]
				if (pipe(inout) == -1) {
					throw ipp::ipp_system_exception("pipe failed", errno, std::system_category());
				}

				const char* display = getenv("DISPLAY");
				if (display == nullptr) {
					display = ":1";
				}

				int nullfd = open("/tmp/ffmpeg.out", O_RDWR | O_TRUNC | O_CREAT, 0600);
				if (nullfd < 0) {
					throw ipp::ipp_system_exception("/dev/null", errno, std::system_category());
				}

				pid_t cpid = fork();
				if (cpid == -1) {
					throw ipp::ipp_system_exception("fork failed", errno, std::system_category());
				} else if (cpid == 0) {
					// child process: ffmpeg -video_size 600x600 -f x11grab -i :1 -r 5 -f mpeg2video -
					close(STDIN_FILENO);
					close(STDOUT_FILENO);
					close(STDERR_FILENO);
					close(inout[0]);
					dup2(nullfd, STDIN_FILENO);
					dup2(inout[1], STDOUT_FILENO);
					dup2(nullfd, STDERR_FILENO);

					const char* args[] = {
							"ffmpeg",
							"-video_size", "600x600",
							"-f", "x11grab",
							"-i", display,
							"-r", "5",
							"-f", "mpeg2video",
							"-",
							NULL,
					};
					execvp("ffmpeg", const_cast<char**>(args));
					std::abort();
					// noreturn
				} else {
					close(nullfd);
					close(inout[1]);
					return inout[0];
				}
			}
		}

		video_encoder_channel::video_encoder_channel(ipphone& ipp, uint32_t ch_id,
		                                             channel_wrapper::channel_type ch_type)
				: channel_wrapper(ch_id, ch_type), _ipp(ipp) {
			_read_fd = unique_fd(fork_gif_encoder());
		}

		void video_encoder_channel::receive(const std::uint8_t*, const std::uint16_t) {
			LOG(ERROR) << "#receive is called";
		}

		void video_encoder_channel::flush_packets() {
			std::uint8_t buf[0x8000];
			ssize_t len;
			while (_read_fd.readable(std::chrono::milliseconds(0))) {
				len = read(_read_fd.get(), buf, sizeof(buf));
				if (len < 0) {
					throw ipp_system_exception("read pipe", errno, std::system_category());
				} else if (len == 0) {
					break;
				}
				for (auto& pair : _ipp.connection_manager().get_connections()) {
					pair.second->protocol().channel_data(_channel_id, buf, static_cast<std::uint16_t>(len));
				}
			}
		}

		void video_encoder_channel::close() {
		}
	}
}
