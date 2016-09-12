/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2016 individual OpenKinect contributors. See the CONTRIB file
 * for details.
 *
 * This code is licensed to you under the terms of the Apache License, version
 * 2.0, or, at your option, the terms of the GNU General Public License,
 * version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * If you redistribute this file in source form, modified or unmodified, you
 * may:
 *   1) Leave this header intact and distribute it under the same terms,
 *      accompanying it with the APACHE20 and GPL20 files, or
 *   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
 *   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy
 * of the CONTRIB file.
 *
 * Binary distributions must follow the binary distribution requirements of
 * either License.
 */

/** @file intel_media_sdk_rgb_packet_processor.cpp JPEG decoder with Intel Media SDK. */

#include <libfreenect2/rgb_packet_processor.h>
#include <libfreenect2/logging.h>

#include <mfxdefs.h>
#include <mfxvideo.h>
#include <mfxvideo++.h>
#include <mfxjpeg.h>
#include <mfapi.h>
#include <mfidl.h>
#include <Mfreadwrite.h>
#include <mferror.h>

namespace libfreenect2
{

void printErrorCode(const mfxStatus& status, const std::string& f)
{
  LOG_ERROR << "Error in MFX call: " << f;
  switch (status)
  {
      /* no error */
  case MFX_ERR_NONE:
    return;
  case MFX_ERR_UNKNOWN:
    LOG_ERROR << "Unknown MFX_ERR_UNKNOWN";
    break;
  case MFX_ERR_NULL_PTR:
    LOG_ERROR << "MFX error: MFX_ERR_NULL_PTR";
    break;
  case MFX_ERR_UNSUPPORTED:
    LOG_ERROR << "MFX error: MFX_ERR_UNSUPPORTED";
    break;
  case MFX_ERR_MEMORY_ALLOC:
    LOG_ERROR << "MFX error: MFX_ERR_MEMORY_ALLOC";
    break;
  case MFX_ERR_NOT_ENOUGH_BUFFER:
    LOG_ERROR << "MFX error: MFX_ERR_NOT_ENOUGH_BUFFER";
    break;
  case MFX_ERR_INVALID_HANDLE:
    LOG_ERROR << "MFX error: MFX_ERR_INVALID_HANDLE";
    break;
  case MFX_ERR_LOCK_MEMORY:
    LOG_ERROR << "MFX error: MFX_ERR_LOCK_MEMORY";
    break;
  case MFX_ERR_NOT_INITIALIZED:
    LOG_ERROR << "MFX error: MFX_ERR_NOT_INITIALIZED";
    break;
  case MFX_ERR_NOT_FOUND:
    LOG_ERROR << "MFX error: MFX_ERR_NOT_FOUND";
    break;
  case MFX_ERR_MORE_DATA:
    LOG_ERROR << "MFX error: MFX_ERR_MORE_DATA";
    break;
  case MFX_ERR_MORE_SURFACE:
    LOG_ERROR << "MFX error: MFX_ERR_MORE_SURFACE";
    break;
  case MFX_ERR_ABORTED:
    LOG_ERROR << "MFX error: MFX_ERR_ABORTED";
    break;
  case MFX_ERR_DEVICE_LOST:
    LOG_ERROR << "MFX error: MFX_ERR_DEVICE_LOST";
    break;
  case MFX_ERR_INCOMPATIBLE_VIDEO_PARAM:
    LOG_ERROR << "MFX error: MFX_ERR_INCOMPATIBLE_VIDEO_PARAM";
    break;
  case MFX_ERR_INVALID_VIDEO_PARAM:
    LOG_ERROR << "MFX error: MFX_ERR_INVALID_VIDEO_PARAM";
    break;
  case MFX_ERR_UNDEFINED_BEHAVIOR:
    LOG_ERROR << "MFX error: MFX_ERR_UNDEFINED_BEHAVIOR";
    break;
  case MFX_ERR_DEVICE_FAILED:
    LOG_ERROR << "MFX error: MFX_ERR_DEVICE_FAILED";
    break;
  case MFX_ERR_MORE_BITSTREAM:
    LOG_ERROR << "MFX error: MFX_ERR_MORE_BITSTREAM";
    break;
  case MFX_ERR_INCOMPATIBLE_AUDIO_PARAM:
    LOG_ERROR << "MFX error: MFX_ERR_INCOMPATIBLE_AUDIO_PARAM";
    break;
  case MFX_ERR_INVALID_AUDIO_PARAM:
    LOG_ERROR << "MFX error: MFX_ERR_INVALID_AUDIO_PARAM";
    break;
  case MFX_ERR_GPU_HANG:
    LOG_ERROR << "MFX error: MFX_ERR_GPU_HANG";
    break;
  case MFX_ERR_REALLOC_SURFACE:
    LOG_ERROR << "MFX error: MFX_ERR_REALLOC_SURFACE";
    break;

  /* warnings >0 */
  case MFX_WRN_IN_EXECUTION:
    LOG_ERROR << "MFX warning: MFX_WRN_IN_EXECUTION";
    break;
  case MFX_WRN_DEVICE_BUSY:
    LOG_ERROR << "MFX warning: MFX_WRN_DEVICE_BUSY";
    break;
  case MFX_WRN_VIDEO_PARAM_CHANGED:
    LOG_ERROR << "MFX warning: MFX_WRN_VIDEO_PARAM_CHANGED";
    break;
  case MFX_WRN_PARTIAL_ACCELERATION:
    LOG_ERROR << "MFX warning: MFX_WRN_PARTIAL_ACCELERATION";
    break;
  case MFX_WRN_INCOMPATIBLE_VIDEO_PARAM:
    LOG_ERROR << "MFX warning: MFX_WRN_INCOMPATIBLE_VIDEO_PARAM";
    break;
  case MFX_WRN_VALUE_NOT_CHANGED:
    LOG_ERROR << "MFX warning: MFX_WRN_VALUE_NOT_CHANGED";
    break;
  case MFX_WRN_OUT_OF_RANGE:
    LOG_ERROR << "MFX warning: MFX_WRN_OUT_OF_RANGE";
    break;
  case MFX_WRN_FILTER_SKIPPED:
    LOG_ERROR << "MFX warning: MFX_WRN_FILTER_SKIPPED";
    break;
  case MFX_WRN_INCOMPATIBLE_AUDIO_PARAM:
    LOG_ERROR << "MFX warning: MFX_WRN_INCOMPATIBLE_AUDIO_PARAM";
    break;

  /* threading statuses */
  case MFX_TASK_WORKING: /* there is some more work to do */
  case MFX_TASK_BUSY:    /* task is waiting for resources */
    return;
  /* plug-in statuses */
  case MFX_ERR_MORE_DATA_SUBMIT_TASK: /* return MFX_ERR_MORE_DATA but submit internal asynchronous task */
    return;
  default:
    return;
  }
}

bool checkResult(mfxStatus err_code, const std::string& func, const mfxStatus& desired_code = MFX_ERR_NONE)
{
  // Ignore warnings
  if (err_code > 0)
    err_code = MFX_ERR_NONE;

  if (err_code != MFX_ERR_NONE)
  {
    printErrorCode(err_code, func);
    return false;
  }
  return true;
}


/** Implementation of the IntelMediaSDK decoder processor. */
class IntelMediaSDKRgbPacketProcessorImpl: public WithPerfLogging
{
public:

  /** Holds the main SDK context */
  MFXVideoSession session;

  /** Error codes */
  mfxStatus status;

  /** Constructed once on init. Instructs SDK to take certain type of input */
  mfxVideoParam jpegParams;

  /** Constructed once on init. Instructs SDK to take decoded output and convert it to RGB4 */
  mfxVideoParam VPPParams;

  /** Data buffer that will feed data in the SDK */
  mfxBitstream mfx_bitstream;
  
  /** Memory allocation request that reserves some memory for the required surfaces */
  mfxFrameAllocRequest DecRequest;
  mfxFrameAllocRequest VPPRequest[2];

  /** Number of surfaces required for decompressing. Usually 4 */
  mfxU16 nSurfNumDecVPP;

  /** Number of surfaces required for converting NV12 -> RGB4. Usually 4 */
  mfxU16 nSurfNumVPPOut;

  /** Current indices in the map of surfaces */
  int nIndex;
  int nIndex2;

  /** Collection of surfaces that pass and receive data */
  mfxFrameSurface1* pmfxOutSurface;
  mfxFrameSurface1** pmfxSurfaces;
  mfxFrameSurface1** pmfxSurfaces2;

  /** Image dimensions */
  mfxU16 width;
  mfxU16 height;
  mfxU8 bitsPerPixel; // NV12 format is a 12 bits per pixel format
  mfxU32 surfaceSize;
  mfxU8* surfaceBuffers;

  // Decoder for decompressing JPEG into YUV NV12
  MFXVideoDECODE* decoder;

  // Video processor for converting NV12 into RGB4
  MFXVideoVPP* vpp_pipeline;

  /** True if session initialization was successful */
  bool initialized;

  /** Set to true once the jpeg ecoding pipeline has been fed the first JPEG image */
  bool headers_read;

  Frame *frame;

  IntelMediaSDKRgbPacketProcessorImpl()
  {
    initialized = false;
    headers_read = false;
    decoder = NULL;
    vpp_pipeline = NULL;
    pmfxOutSurface = NULL;
    pmfxSurfaces = NULL;
    pmfxSurfaces2 = NULL;
    nIndex = nIndex2 = -1;
    width = height = bitsPerPixel = 0;
    surfaceSize = 0;
    surfaceBuffers = NULL;
    nSurfNumDecVPP = nSurfNumVPPOut = 0;

    mfxVersion swversion = { 0, 1 };
    status = session.Init(MFX_IMPL_HARDWARE, &swversion);
    if (status != MFX_ERR_NONE)
    {
      printErrorCode(status, "MFXInit (hardware)");
      LOG_ERROR << "Failed starting IntelMediaSDK in hardware mode";
      status = session.Init(MFX_IMPL_SOFTWARE, &swversion);
      if (status != MFX_ERR_NONE)
      {
        LOG_ERROR << "Failed starting IntelMediaSDK in software mode";
        printErrorCode(status, "MFXInit (software)");
        if (status < 0)
          return;
      }
      LOG_INFO << "Using IntelMediaSDK in software mode";
    }
    else
    {
      LOG_INFO << "Using IntelMediaSDK in hardware mode";
    }

    initialized = true;

    newFrame();
  }

  ~IntelMediaSDKRgbPacketProcessorImpl()
  {
    delete frame;

    if (decoder)
      decoder->Close();
    if (vpp_pipeline)
      vpp_pipeline->Close();

    for (int i = 0; i < nSurfNumDecVPP; i++)
    {
      if (pmfxSurfaces[i])
        delete pmfxSurfaces[i];
    }
    for (int i = 0; i < nSurfNumVPPOut; i++)
    {
      if (pmfxSurfaces2[i])
        delete pmfxSurfaces2[i];
    }

    if (pmfxSurfaces)
      delete[] pmfxSurfaces;
    if (pmfxSurfaces2)
      delete[] pmfxSurfaces2;
    if (surfaceBuffers)
      delete[] surfaceBuffers;
    if (mfx_bitstream.Data)
      delete[] mfx_bitstream.Data;

    if (initialized == true)
    {
      status = session.Close();
      if (status != MFX_ERR_NONE)
      {
        LOG_ERROR << "Failed closing IntelMediaSDK session";
        printErrorCode(status, "session.Close()");
      }
    }
  }

  void newFrame()
  {
    frame = new Frame(1920, 1080, 4);
    frame->format = Frame::BGRX;
  }

  bool prepare(const RgbPacket &packet)
  {
    memset(&jpegParams, 0, sizeof(jpegParams));
    memset(&mfx_bitstream, 0, sizeof(mfxBitstream));

    // Size of the work buffer. Can be smaller than total work size
    mfx_bitstream.MaxLength = 1920ULL * 1080ULL * 4ULL;
    if (mfx_bitstream.Data)
      delete[] mfx_bitstream.Data;
    mfx_bitstream.Data = new mfxU8[mfx_bitstream.MaxLength];

    if (mfx_bitstream.Data == NULL)
    {
      LOG_ERROR << "Error allocating imagestream buffer";
      return false;
    }

    mfx_bitstream.ExtParam = 0;
    mfx_bitstream.NumExtParam = 0;
    mfx_bitstream.DecodeTimeStamp = MFX_TIMESTAMP_UNKNOWN;
    mfx_bitstream.TimeStamp = MFX_TIMESTAMP_UNKNOWN;
    mfx_bitstream.DataOffset = 0;

    // How much data there is to process in total
    mfx_bitstream.DataLength = packet.jpeg_buffer_length;
    mfx_bitstream.DataFlag = MFX_BITSTREAM_COMPLETE_FRAME;

    // Fill initial chunk of the data. In our case, all of the data
    memcpy(mfx_bitstream.Data, packet.jpeg_buffer, packet.jpeg_buffer_length);

    // Decoder for decompressing JPEG into YUV NV12
    decoder = new MFXVideoDECODE(session);

    // Video processor for converting NV12 into RGB4
    vpp_pipeline = new MFXVideoVPP(session);

    jpegParams.mfx.CodecId = MFX_CODEC_JPEG;
    jpegParams.mfx.FrameInfo.AspectRatioW = 1;
    jpegParams.mfx.FrameInfo.AspectRatioH = 1;
    jpegParams.IOPattern = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    jpegParams.mfx.FrameInfo.FourCC = MFX_FOURCC_NV12;
    jpegParams.mfx.FrameInfo.Width = 1920;
    jpegParams.mfx.FrameInfo.Height = 1088; // next multiple of 16
    jpegParams.mfx.FrameInfo.CropX = 0;
    jpegParams.mfx.FrameInfo.CropY = 0;
    jpegParams.mfx.FrameInfo.CropH = 1920;
    jpegParams.mfx.FrameInfo.CropW = 1088;

    status = MFXVideoDECODE_DecodeHeader(session, &mfx_bitstream, &jpegParams);
    if (!checkResult(status, "MFXVideoDECODE_DecodeHeader"))
      return false;

    jpegParams.mfx.FrameInfo.AspectRatioW = 1;
    jpegParams.mfx.FrameInfo.AspectRatioH = 1;
    jpegParams.AsyncDepth = 4;

    status = decoder->Query(&jpegParams, &jpegParams);
    if (!checkResult(status, "decoder query"))
      return false;

    memset(&VPPParams, 0, sizeof(VPPParams));

    VPPParams.vpp.In.FourCC = MFX_FOURCC_NV12;
    VPPParams.vpp.In.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
    VPPParams.vpp.In.CropX = 0;
    VPPParams.vpp.In.CropY = 0;
    VPPParams.vpp.In.CropW = 1920;
    VPPParams.vpp.In.CropH = 1088;
    VPPParams.vpp.In.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
    VPPParams.vpp.In.FrameRateExtN = 30;
    VPPParams.vpp.In.FrameRateExtD = 1;
    VPPParams.vpp.In.Width = 1920;
    VPPParams.vpp.In.Height = 1088;

    memcpy(&VPPParams.vpp.Out, &VPPParams.vpp.In, sizeof(mfxFrameInfo));
    VPPParams.vpp.Out.FourCC = MFX_FOURCC_RGB4;
    VPPParams.vpp.Out.CropW = 1920;
    VPPParams.vpp.Out.CropH = 1080;

    VPPParams.IOPattern = MFX_IOPATTERN_OUT_SYSTEM_MEMORY | MFX_IOPATTERN_IN_SYSTEM_MEMORY;
    VPPParams.AsyncDepth = jpegParams.AsyncDepth;

    memset(&DecRequest, 0, sizeof(DecRequest));
    status = decoder->QueryIOSurf(&jpegParams, &DecRequest);
    if (!checkResult(status, "decoder QueryIOSurf"))
      return false;

    memset(&VPPRequest, 0, 2 * sizeof(mfxFrameAllocRequest));
    status = vpp_pipeline->QueryIOSurf(&VPPParams, VPPRequest);
    if (!checkResult(status, "VPP QueryIOSurf"))
      return false;

    // Determine the required number of surfaces for decoder output (VPP input)
    // and for VPP output
    nSurfNumDecVPP = DecRequest.NumFrameSuggested + VPPRequest[0].NumFrameSuggested;
    nSurfNumVPPOut = VPPRequest[1].NumFrameSuggested;

    // This probably means VPP reads the data directly from the decode output?
    // This causes UNSUPPORTED error on system memory allocation though
    //VPPRequest[0].Type = MFX_MEMTYPE_FROM_DECODE;

    // Allocate surfaces for decoder and VPP In
    // - Width and height of buffer must be aligned, a multiple of 32
    // - Frame surface array keeps pointers all surface planes and general frame info
    width = (mfxU16)DecRequest.Info.Width;
    height = (mfxU16)DecRequest.Info.Height;
    bitsPerPixel = 12; // NV12 format is a 12 bits per pixel format
    surfaceSize = width * height * bitsPerPixel / 8;
    if (surfaceBuffers)
    {
      delete[] surfaceBuffers;
    }
    surfaceBuffers = (mfxU8*) new mfxU8[surfaceSize * nSurfNumDecVPP];

    if (pmfxSurfaces)
    {
      for (int i = 0; i < nSurfNumDecVPP; i++)
      {
        if (pmfxSurfaces[i])
          delete pmfxSurfaces[i];
      }
      delete[] pmfxSurfaces;
    }

    pmfxSurfaces = new mfxFrameSurface1 *[nSurfNumDecVPP];
    if (!pmfxSurfaces)
    {
      LOG_ERROR << "Failed allocating surfaces for decompressor";
      return false;
    }

    for (int i = 0; i < nSurfNumDecVPP; i++)
    {
      pmfxSurfaces[i] = new mfxFrameSurface1;
      if (!pmfxSurfaces[i])
      {
        LOG_ERROR << "Failed allocating surfaces for decompressor";
        return false;
      }
      memset(pmfxSurfaces[i], 0, sizeof(mfxFrameSurface1));
      memcpy(&(pmfxSurfaces[i]->Info), &(jpegParams.mfx.FrameInfo), sizeof(mfxFrameInfo));
      pmfxSurfaces[i]->Data.Y = &surfaceBuffers[surfaceSize * i];
      pmfxSurfaces[i]->Data.U = pmfxSurfaces[i]->Data.Y + width * height;
      pmfxSurfaces[i]->Data.V = pmfxSurfaces[i]->Data.U + 1;
      pmfxSurfaces[i]->Data.Pitch = width;
    }

    // Allocate surfaces for VPP Out
    // - Width and height of buffer must be aligned, a multiple of 32
    // - Frame surface array keeps pointers all surface planes and general frame info
    width = (mfxU16)VPPRequest[1].Info.Width;
    height = (mfxU16)VPPRequest[1].Info.Height;
    bitsPerPixel = 12;      // NV12 format is a 12 bits per pixel format
    surfaceSize = width * height * bitsPerPixel / 8;

    if (pmfxSurfaces2)
    {
      for (int i = 0; i < nSurfNumVPPOut; i++)
      {
        if (pmfxSurfaces2[i])
          delete pmfxSurfaces2[i];
      }
      delete[] pmfxSurfaces2;
    }

    pmfxSurfaces2 = new mfxFrameSurface1 *[nSurfNumVPPOut];
    if (!pmfxSurfaces2)
    {
      LOG_ERROR << "Failed allocating surfaces for VPP";
      return false;
    }

    for (int i = 0; i < nSurfNumVPPOut; i++)
    {
      pmfxSurfaces2[i] = new mfxFrameSurface1;
      if (!pmfxSurfaces2[i])
      {
        LOG_ERROR << "Failed allocating surfaces for VPP";
        return false;
      }
      memset(pmfxSurfaces2[i], 0, sizeof(mfxFrameSurface1));
      memcpy(&(pmfxSurfaces2[i]->Info), &(VPPParams.vpp.Out), sizeof(mfxFrameInfo));
      pmfxSurfaces2[i]->Data.B = &surfaceBuffers[surfaceSize * i];
      pmfxSurfaces2[i]->Data.G = pmfxSurfaces2[i]->Data.B + 1;
      pmfxSurfaces2[i]->Data.R = pmfxSurfaces2[i]->Data.B + 2;
      pmfxSurfaces2[i]->Data.A = pmfxSurfaces2[i]->Data.B + 3;
      pmfxSurfaces2[i]->Data.Pitch = width * 4;
    }

    // Initialize the Media SDK decoder
    status = decoder->Init(&jpegParams);
    if (status == MFX_WRN_PARTIAL_ACCELERATION) status = MFX_ERR_NONE;
    if (!checkResult(status, "Decoder init"))
      return false;

    // Initialize Media SDK VPP
    status = vpp_pipeline->Init(&VPPParams);
    if (!checkResult(status, "VPP init"))
      return false;

    // ===============================================================
    // Start decoding the frames from the stream
    //
    pmfxOutSurface = NULL;
    nIndex = 0;
    nIndex2 = 0;

    headers_read = true;
    LOG_INFO << "IntelMediaSDK prepared successfully";
    return true;
  }

  int GetFreeSurfaceIndex(mfxFrameSurface1** pSurfacesPool, mfxU16 nPoolSize)
  {
    if (pSurfacesPool)
    {
      for (mfxU16 i = 0; i < nPoolSize; i++)
      {
        if (0 == pSurfacesPool[i]->Data.Locked)
          return i;
      }
    }

    return MFX_ERR_NOT_FOUND;
  }

  bool processNewFrame(const RgbPacket &packet)
  {
    LOG_INFO << "processing new frame";

    // How much data there is to process in total
    mfx_bitstream.DataLength = packet.jpeg_buffer_length;
    mfx_bitstream.DataFlag = MFX_BITSTREAM_COMPLETE_FRAME;

    // Fill initial chunk of the data. In our case, all of the data
    memcpy(mfx_bitstream.Data, packet.jpeg_buffer, packet.jpeg_buffer_length);

    status = MFX_ERR_NONE;
    mfxSyncPoint syncpD;
    mfxSyncPoint syncpV;
    pmfxOutSurface = NULL;
    nIndex = 0;
    nIndex2 = 0;

    //
    // Main decoding loop
    //
    while (MFX_ERR_NONE <= status || MFX_ERR_MORE_DATA == status || MFX_ERR_MORE_SURFACE == status)
    {
      if (MFX_ERR_MORE_DATA == status)
        break;

      if (MFX_WRN_DEVICE_BUSY == status)
      {
        // Wait if device is busy, then repeat the same call to DecodeFrameAsync
#ifdef WIN32
        Sleep(1);
#else
        usleep(1000);
#endif
      }

      if (MFX_ERR_MORE_DATA == status)
      {
        // Read more data into input bit stream
        // TODO: Possibly not needed?
      }

      if (MFX_ERR_MORE_SURFACE == status || MFX_ERR_NONE == status)
      {
        // Find free frame surface
        nIndex = GetFreeSurfaceIndex(pmfxSurfaces, nSurfNumDecVPP);
        if (nIndex < 0)
        {
          LOG_ERROR << "No free surfaces for decoding";
          return false;
        }
      }

      // Decode a frame asychronously (returns immediately)
      status = decoder->DecodeFrameAsync(&mfx_bitstream, pmfxSurfaces[nIndex], &pmfxOutSurface, &syncpD);

      // Ignore warnings if output is available,
      // if no output and no action required just repeat the DecodeFrameAsync call
      if (MFX_ERR_NONE < status && syncpD)
        status = MFX_ERR_NONE;

      if (MFX_ERR_NONE == status)
      {
        // Find free frame surface
        nIndex2 = GetFreeSurfaceIndex(pmfxSurfaces2, nSurfNumVPPOut);
        if (nIndex2 < 0)
        {
          LOG_ERROR << "No free surfaces for decoding";
          return false;
        }

        for (;;)
        {
          // Process a frame asychronously (returns immediately)
          status = vpp_pipeline->RunFrameVPPAsync(pmfxOutSurface, pmfxSurfaces2[nIndex2], NULL, &syncpV);

          if (MFX_ERR_NONE < status && !syncpV)
          {    // repeat the call if warning and no output
            if (MFX_WRN_DEVICE_BUSY == status)
            {
              // wait if device is busy
#ifdef WIN32
              Sleep(1);
#else
              usleep(1000);
#endif
            }
          }
          else if (MFX_ERR_NONE < status && syncpV)
          {
            status = MFX_ERR_NONE;     // ignore warnings if output is available
            break;
          }
          else
            break;  // not a warning
        }

        // VPP needs more data, let decoder decode another frame as input
        if (MFX_ERR_MORE_DATA == status)
        {
          continue;
        }
        else if (MFX_ERR_MORE_SURFACE == status)
        {
          // Not relevant for the illustrated workload! Therefore not handled.
          // Relevant for cases when VPP produces more frames at output than consumes at input. E.g. framerate conversion 30 fps -> 60 fps
          break;
        }
        else
        {
          if (!checkResult(status, "processing error"))
            return false;
        }
      }

      if (MFX_ERR_NONE == status)
      {
        // Synchronize. Wait until decoded frame is ready
        status = session.SyncOperation(syncpV, 60000);
      }

      if (MFX_ERR_NONE == status)
      {
        // Data is ready. Output it
        mfxFrameSurface1* finished_frame = pmfxSurfaces2[nIndex2];

        mfxFrameInfo* pInfo = &finished_frame->Info;
        mfxFrameData* pData = &finished_frame->Data;

        memcpy(frame->data, finished_frame->Data.B, pInfo->CropW * pInfo->CropH * 4);
        break;
      }
    }

    return true;
  }
};

IntelMediaSDKRgbPacketProcessor::IntelMediaSDKRgbPacketProcessor() :
  impl_(new IntelMediaSDKRgbPacketProcessorImpl())
{
}

IntelMediaSDKRgbPacketProcessor::~IntelMediaSDKRgbPacketProcessor()
{
  delete impl_;
}

void IntelMediaSDKRgbPacketProcessor::process(const RgbPacket &packet)
{
  if (!impl_->initialized || listener_ == 0)
    return;

  if (!impl_->headers_read)
  {
    if (!impl_->prepare(packet))
    {
      impl_->initialized = false;
      return;
    }
  }

  if (!impl_->processNewFrame(packet))
  {
    impl_->initialized = false;
    return;
  }

  if (listener_->onNewFrame(Frame::Color, impl_->frame))
  {
    impl_->newFrame();
  }
}

} /* namespace libfreenect2 */
