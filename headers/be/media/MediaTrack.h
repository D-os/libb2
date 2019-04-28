#ifndef _MEDIA_TRACK_H
#define _MEDIA_TRACK_H

#include <SupportDefs.h>
#include <MediaDefs.h>
#include <MediaFormats.h>

namespace BPrivate {
	class MediaExtractor;
	class Decoder;
	class MediaWriter;
	class Encoder;
}

class BView;
class BParameterWeb;

enum media_seek_type {
	B_MEDIA_SEEK_CLOSEST_FORWARD = 1,
	B_MEDIA_SEEK_CLOSEST_BACKWARD = 2,
	B_MEDIA_SEEK_DIRECTION_MASK = 3
};

//
// BMediaTrack gives access to a particular media track in a media file
// (as represented by BMediaFile).
//
// You always instantiate a BMediaTrack through BMediaFile::TrackAt()
// or BMediaFile::CreateTrack().  When a BMediaTrack object is
// constructed it finds the necessary decoder or encoder for the type
// of data stored in the track.
//
// Unless you created the BMediaFile() in B_MEDIA_REPLACE_MODE, you
// can only access a track for reading or writing, not both.
//
// If InitCheck() indicates no errors, then the track is ready to be
// used to read and write frames using ReadFrames() and WriteFrames().
// For video data you should always only read one frame.
//
// You can seek a track with SeekToTime() and SeekToFrame().
//
// If no codec could be found for the track, it is still possible to
// access the encoded data using ReadChunk().
//
class BMediaTrack {

protected:

	// Use BMediaFile::ReleaseTrack() instead -- or it will go away
	// on its own when the MediaFile is deleted.
	//
	virtual			~BMediaTrack();
	
public:

	// for read-only access the BMediaTrack should be instantiated
	// through BMediaFile::TrackAt()

	// for write-only access the BMediaTrack should be instantiated
	// through BMediaFile::CreateTrack()

	status_t		InitCheck() const;

	// Get information about the codec being used.
	status_t		GetCodecInfo(media_codec_info *mci) const;
	

	// EncodedFormat returns information about the track's 
	// "native" encoded format.  

	status_t		EncodedFormat(media_format *out_format) const;

	// DecodedFormat is used to negotiate the format that the codec will
	// use when decoding the track's data.  You pass in the format that
	// that you want; the codec will find and return its "best fit"
	// format.  (inout_format is used as both the input and the returned 
	// format.)  The format is typically of the B_MEDIA_RAW_AUDIO or 	
	// B_MEDIA_RAW_VIDEO flavor.
	// The data returned through ReadFrames() will be in the format that's
 	// returned by this function.

	status_t		DecodedFormat(media_format *inout_format);

	// CountFrames and Duration return the total number of frame and the
	// total duration (expressed in microseconds) of a track.

	int64			CountFrames() const;
	bigtime_t		Duration() const;

	// CurrentFrame and CurrentTime return the current position (expressed in
	// microseconds) within the track, expressed in frame index and time.

	int64			CurrentFrame() const;
	bigtime_t		CurrentTime() const;

	// ReadFrames() fills a buffer with the next frames/samples. For a video
	// track,  it decodes the next frame of video in the passed buffer. For
	// an audio track, it fills the buffers with the next N samples, as
	// negotiated by DecodedFormat().  However, if it reaches the end of the
	// file and was not able to fill the whole  buffer, it returns a partial
	// buffer.  Upon return, out_frameCount contains the actual number of
	// frame/samples returned, and the start time for the frame, expressed 
	// in microseconds, is in the media_header structure.

	status_t		ReadFrames(void *out_buffer, int64 *out_frameCount,
							   media_header *mh = NULL);

	status_t		ReadFrames(void *out_buffer, int64 *out_frameCount,
							   media_header *mh, media_decode_info *info);

	status_t		ReplaceFrames(const void *in_buffer, int64 *io_frameCount,
								  const media_header *mh);


	// SeekToTime and SeekToFrame are used for seeking to a particular
	// position in a track, expressed in either frames or microseconds.
	// They return whatever position they were able to seek to. For example,
	// a video codec may not be able to seek to arbitrary frames, but only to
	// key frames. In this case, it would return the closest key frame before
	// the specified seek point.
	//
	// If you want to explicitly seek to the nearest keyframe _before_ this
	// frame or _after_ this frame, pass B_MEDIA_SEEK_CLOSEST_FORWARD or
	// B_MEDIA_SEEK_CLOSEST_BACKWARD as the flags field.
	//

	status_t		SeekToTime(bigtime_t *inout_time, int32 flags=0);
	status_t		SeekToFrame(int64 *inout_frame, int32 flags=0);

	status_t		FindKeyFrameForTime(bigtime_t *inout_time, int32 flags=0) const;
	status_t		FindKeyFrameForFrame(int64 *inout_frame, int32 flags=0) const;

	// ReadChunk returns, in out_buffer, the next out_size bytes of
	// data from the track.  The data is not decoded -- it will be
	// in its native encoded format (as specified by EncodedFormat()).
	// You can not mix calling ReadChunk() and ReadFrames() -- either
	// you access the track raw (i.e. with ReadChunk) or you access
	// it with ReadFrames.

	status_t		ReadChunk(char **out_buffer, int32 *out_size,
							  media_header *mh = NULL);


	//
	// Write-only Functions
	//
	status_t		AddCopyright(const char *data);
	status_t		AddTrackInfo(uint32 code, const void *data, size_t size,
								 uint32 flags = 0);

	//
	// Write num_frames of data to the track. This data is passed
	// through the encoder that was specified when the MediaTrack
	// was constructed.
	// Pass B_MEDIA_KEY_FRAME for flags if it is.
	//
	status_t		WriteFrames(const void *data, int32 num_frames,
								int32 flags = 0);
	status_t		WriteFrames(const void *data, int64 num_frames,
								media_encode_info *info);

	//
	// Write a raw chunk of (presumably already encoded data) to
	// the file.
	// Pass B_MEDIA_KEY_FRAME for flags if it is.
	//
	status_t		WriteChunk(const void *data, size_t size,
							   uint32 flags = 0);
	status_t		WriteChunk(const void *data, size_t size,
							   media_encode_info *info);

	// Flush all buffered encoded datas to disk. You should call it after
	// writing the last frame to be sure all datas are flushed at the right
	// offset into the file.
	status_t		Flush();

	// These are for controlling the underlying encoder and track parameters
	BParameterWeb	*Web();
	status_t 		GetParameterValue(int32 id, void *valu, size_t *size);
	status_t		SetParameterValue(int32 id, const void *valu, size_t size);
	BView			*GetParameterView();

	// This is a simplified control API, only one parameter low=0.0, high=1.0
	// Return B_ERROR if it's not supported by the current encoder.
	status_t		GetQuality(float *quality);
	status_t		SetQuality(float quality);

	status_t 		GetEncodeParameters(encode_parameters *parameters) const;
	status_t 		SetEncodeParameters(encode_parameters *parameters);


virtual	status_t Perform(int32 selector, void * data);

private:
					// for read-only access to a track
					BMediaTrack(BPrivate::MediaExtractor *extractor, int32 stream);

					// for write-only access to a BMediaTrack
					BMediaTrack(BPrivate::MediaWriter *writer,
								int32 stream_num,
								media_format *in_format,
								BPrivate::Encoder *encoder,
								media_codec_info *mci);

	status_t		TrackInfo(media_format *out_format, void **out_info,
							  int32 *out_infoSize);

	status_t				fErr;
	BPrivate::Decoder		*fDecoder;
	int32					fDecoderID;
	BPrivate::MediaExtractor *fExtractor;

	int32					fStream;
	int64					fCurFrame;
	bigtime_t				fCurTime;

	media_codec_info		fMCI;

	BPrivate::Encoder		*fEncoder;
	int32					fEncoderID;
	BPrivate::MediaWriter	*fWriter;
	media_format			fWriterFormat;
	void					*fExtractorCookie;

protected:
	int32			EncoderID() { return fEncoderID; };

private:

friend class BMediaFile;
friend class BPrivate::Decoder;

	BMediaTrack();
	BMediaTrack(const BMediaTrack&);
	BMediaTrack& operator=(const BMediaTrack&);

static	BPrivate::Decoder *	find_decoder(BMediaTrack *track, int32 *id);

	/* fbc data and virtuals */

	uint32 _reserved_BMediaTrack_[31];

virtual	status_t _Reserved_BMediaTrack_0(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_1(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_2(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_3(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_4(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_5(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_6(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_7(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_8(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_9(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_10(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_11(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_12(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_13(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_14(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_15(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_16(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_17(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_18(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_19(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_20(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_21(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_22(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_23(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_24(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_25(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_26(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_27(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_28(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_29(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_30(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_31(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_32(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_33(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_34(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_35(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_36(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_37(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_38(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_39(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_40(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_41(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_42(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_43(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_44(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_45(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_46(int32 arg, ...);
virtual	status_t _Reserved_BMediaTrack_47(int32 arg, ...);
};

#endif
