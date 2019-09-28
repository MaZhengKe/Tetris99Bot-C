#include "FrameTrack.h"
#include "Util.h"
#include "Board.h"
#include <windows.h>
#include <condition_variable>


using namespace cv;

cv::Rect FrameTrack::boardRect = cv::Rect(720, 60, 480, 960);
cv::Rect FrameTrack::bigNextPiece = cv::Rect(1222, 123, 104, 52);
cv::Rect FrameTrack::smallNextPieces[5] = {};
mutex m;
condition_variable cva;

long backRows[20];
long backGrayRows[20];
Piece * backNext[6];

void FrameTrack::getFilled(long * rows) {
	if (!filledChanged) {
		unique_lock<mutex> lock(m);
		cva.wait(lock);
	}
	m.lock();
	//std::cout << clock() << " fill get start" << endl;
	memcpy(rows, backRows, 20 * sizeof(long));
	filledChanged = false;
	//std::cout << clock() << " fill get end" << endl;
	m.unlock();
}
void FrameTrack::getGrayFilled(long * rows) {
	if (!grayRowsChanged) {
		unique_lock<mutex> lock(m);
		cva.wait(lock);
	}
	m.lock();
	//std::cout << clock() << " fill get start" << endl;
	memcpy(rows, backGrayRows, 20 * sizeof(long));
	grayRowsChanged = false;
	//std::cout << clock() << " fill get end" << endl;
	m.unlock();
}

void FrameTrack::setAllFilled(long * rows, long * grayRows) {
	Mat img(this->mat, boardRect);
	Mat mat = Util::toHSV(img);

	m.lock();
	Util::getFilled(rows, mat, grayRows);
	filledChanged = true;
	grayRowsChanged = true;
	m.unlock();
}


void FrameTrack::getNextPieces(Piece** nextPieces) {
	if (!nextChanged) {
		unique_lock<mutex> lock(m);
		cva.wait(lock);
	}
	m.lock();
	//std::cout << clock()<<  " next get start" <<endl;
	memcpy(nextPieces, backNext, 6 * sizeof(Piece*));
	nextChanged = false;
	//std::cout << clock() << " next get end" << endl;
	m.unlock();
}

void FrameTrack::setNextPieces(Piece** nextPieces) {
	Mat nextMat(this->mat, bigNextPiece);
	Piece* nextPiece = Util::bigMat2Piece(nextMat);
	m.lock();
	nextPieces[0] = nextPiece;
	for (int i = 0; i < 5; i++) {
		Mat next(mat, smallNextPieces[i]);
		Piece* piece = Util::mat2Piece(next);
		nextPieces[i + 1] = piece;
	}
	nextChanged = true;
	m.unlock();
}

int OpenLocalCamera(AVFormatContext *pFormatCtx)
{
	avdevice_register_all();
	AVInputFormat *ifmt = av_find_input_format("dshow");
	if (ifmt != NULL) {
		AVDictionary* par = nullptr;
		av_dict_set(&par, "video_size", "1920x1080", 0);
		av_dict_set(&par, "framerate", "60.0002", 0);
		av_dict_set(&par, "pixel_format", "yuyv422", 0);
		//av_dict_set(&par, "input_format", "mjpeg", 0);
		if (avformat_open_input(&pFormatCtx, "video=AVerMedia Live Gamer HD 2", ifmt, &par)) {
			//if (avformat_open_input(&pFormatCtx, "video=AVerMedia LGHD Stream Engine - 1", ifmt, &par)) {
			return -1;
		}
		return 0;
	}
	return -1;
}


static void decode(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *pFrame, AVFrame *yuvFrame, struct SwsContext *imgCtx/*, FILE *outfile*/)
{
	int ret;
	/* send the packet with the compressed data to the decoder */
	ret = avcodec_send_packet(dec_ctx, pkt);
	if (ret < 0)
	{
		fprintf(stderr, "Error submitting the packet to the decoder\n");
		exit(1);
	}
	/* read all the output frames (in general there may be any number of them */
	while (ret >= 0)
	{
		ret = avcodec_receive_frame(dec_ctx, pFrame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0)
		{
			fprintf(stderr, "Error during decoding\n");
			exit(1);
		}
		//cout << "decoding the frame " << dec_ctx->frame_number << endl;
		sws_scale(imgCtx, pFrame->data, pFrame->linesize, 0, dec_ctx->height, yuvFrame->data, yuvFrame->linesize);
	}
}

int thread_exit = 0;
int thread_pause = 0;

int sfp_refresh_thread(void *opaque)
{
	thread_exit = 0;
	thread_pause = 0;

	while (!thread_exit)
	{
		if (!thread_pause)
		{
			SDL_Event event;
			event.type = SFM_REFRESH_EVENT;
			SDL_PushEvent(&event);
		}
		SDL_Delay(1);
	}
	thread_exit = 0;
	thread_pause = 0;
	//Break
	SDL_Event event;
	event.type = SFM_BREAK_EVENT;
	SDL_PushEvent(&event);
	return 0;
}

using namespace cv;

int FrameTrack::startR()
{
	for (int i = 0; i < 5; i++) {
		FrameTrack::smallNextPieces[i] = Rect(1224, 216 + i * (44 + 38), 88, 44);
	}

	pFormatCtx = avformat_alloc_context();
	//pFormatCtx->probesize = 10000 * 1024;
	//pFormatCtx->duration = 100 * AV_TIME_BASE;

	// 打开本地摄像头
	OpenLocalCamera(pFormatCtx);

	//printf("---------------- File Information ---------------\n");
	av_dump_format(pFormatCtx, 0, NULL, 0);
	//printf("-------------------------------------------------\n");

	// 寻找视频流信息
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}

	// 打开视频获取视频流，设置视频默认索引值
	int videoindex = -1;
	for (int i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
			//break;
		}
	}
	// 如果没有找到视频的索引说明没有视频流
	if (videoindex == -1)
	{
		printf("Didn't find a video stream.\n");
		return -1;
	}
	// 分配解码器上下文
	AVCodecContext  *pCodecCtx = avcodec_alloc_context3(NULL);
	// 获取解码器上下文信息
	if (avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoindex]->codecpar) < 0)
	{
		cout << "Copy stream failed!" << endl;
		return -1;
	}
	// 查找解码器
	AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}
	// 打开解码器
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("Could not open codec.\n");
		return -1;
	}

	//AVPacket* packet = av_packet_alloc();
	//AVFrame* frame = av_frame_alloc();
	//AVFrame* frame_yuv = av_frame_alloc();
	//int bufSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);

	//uint8_t* buffer = (uint8_t*)av_malloc(bufSize);
	//av_image_fill_arrays(frame_yuv->data, frame_yuv->linesize, (const uint8_t*)buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
	//frame_yuv->format = AV_PIX_FMT_YUV420P;


	// 对图形进行裁剪以便于显示得更好
	SwsContext *img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
	if (NULL == img_convert_ctx)
	{
		cout << "Get swscale context failed!" << endl;
		return -1;
	}

	//SDL Init--------------------------------------------------------------
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}


	//SDL 2.0 Support for multiple windows
	int screen_w = pCodecCtx->width;
	int screen_h = pCodecCtx->height;
	SDL_Window* screen = SDL_CreateWindow("FFmpegPlayer", 100, 100, screen_w, screen_h, SDL_WINDOW_OPENGL);
	if (!screen)
	{
		printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
		return -1;
	}

	SDL_Renderer* sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	//SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);
	SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);
	//SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_YVYU, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);

	SDL_Rect sdlRect;
	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = screen_w;
	sdlRect.h = screen_h;

	AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	SDL_Thread *video_tid = SDL_CreateThread(sfp_refresh_thread, "Camera Thread", NULL);

	AVFrame *pFrame = av_frame_alloc();
	AVFrame *frameBGR = av_frame_alloc();

	uint8_t *out_buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height, 1));
	av_image_fill_arrays(frameBGR->data, frameBGR->linesize, out_buffer, AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height, 1);
	av_image_fill_arrays(pFrame->data, frameBGR->linesize, out_buffer, AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height, 1);
	this->mat = Mat(pCodecCtx->height, pCodecCtx->width, CV_8UC3, frameBGR->data[0], frameBGR->linesize[0]);

	//Event Loop
	SDL_Event event;
	for (;;)
	{
		//不响应任何事件，包括移动窗口等，以加速帧的获取
		av_read_frame(pFormatCtx, packet);
		//std::cout << clock() << " set start" << endl;
		avcodec_send_packet(pCodecCtx, packet);
		avcodec_receive_frame(pCodecCtx, pFrame);
		//std::cout << clock() << " got" << endl;

		//TODO 直接从yuv图像中获取信息以节省(5ms)时间？
		sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, frameBGR->data, frameBGR->linesize);
		//std::cout << clock() << " set end" << endl;

		setNextPieces(backNext);
		//std::cout << clock() << " set next end" << endl;
		setAllFilled(backRows, backGrayRows);
		//std::cout << clock() << " set fill end" << endl;
		cva.notify_all();

		//SDL---------------------------
		SDL_UpdateTexture(sdlTexture, NULL, frameBGR->data[0], frameBGR->linesize[0]);
		SDL_RenderClear(sdlRenderer);
		SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
		SDL_RenderPresent(sdlRenderer);
		//SDL End-----------------------
		//std::cout << clock() << " sdl end" << endl;

		av_packet_unref(packet);

	}
	SDL_Quit();
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
	return getchar();
}
