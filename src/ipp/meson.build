libipphone_files = [
	'channel/file_channel.cxx',
	'channel/sound_channel.cxx',
	'channel/video_decoder_channel.cxx',
	'channel/video_encoder_channel.cxx',
	'device/handler.cxx',
	'device/sox_read_handler.cxx',
	'device/sox_write_handler.cxx',
	'error_info.cxx',
	'logger/log_console_sink.cxx',
	'network/socket.cxx',
	'network/socket_address.cxx',
	'network/socket_connection.cxx',
	'protocol/connection.cxx',
	'protocol/connection_manager.cxx',
	'protocol/ippp.cxx',
	'protocol/packet/packet_builder.cxx',
	'ipphone.cxx',
]

vcs_rev = vcs_tag(input: 'version.hxx.in', output: 'version.hxx',
	fallback: meson.project_version())

libipp = library('ipphone', [libipphone_files, vcs_rev],
	dependencies: [boost_dep, g3logger_dep, libsoxcpp_dep, ffmpeg_dep],
	include_directories: include_directories(['..']),
	cpp_args: project_warnings)

libipp_dep = declare_dependency(
	sources: vcs_rev,
	link_with: libipp,
	dependencies: [boost_dep, g3logger_dep, libsoxcpp_dep, ffmpeg_dep],
	include_directories: include_directories(['..']))
