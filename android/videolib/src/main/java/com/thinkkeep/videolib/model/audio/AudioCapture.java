package com.thinkkeep.videolib.model.audio;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.SystemClock;
import android.util.Log;

/**
 * 音频采集
 * Created by jason on 17/4/22.
 */

public class AudioCapture {
    private static final String TAG = "AudioCapturer";

    private static final int DEFAULT_SOURCE = MediaRecorder.AudioSource.MIC;

    private static final int DEFAULT_SAMPLE_RATE = 44100;

    private static final int DEFAULT_CHANNEL_CONFIG = AudioFormat.CHANNEL_IN_MONO;

    private static final int DEFAULT_AUDIO_FORMAT = AudioFormat.ENCODING_PCM_16BIT;
    private static final int TIMER_INTERVAL = 120;

    private AudioRecord mAudioRecord;

    private int mMinBufferSize = 0;

    private Thread mCaptureThread;

    private boolean mIsCaptureStarted = false;

    private volatile boolean mIsLoopExit = false;

    private OnAudioFrameCapturedListener mAudioFrameCapturedListener;


    public interface OnAudioFrameCapturedListener {
        void onAudioFrameCaptured(byte[] audioData);
    }


    public boolean isCaptureStarted() {
        return mIsCaptureStarted;

    }


    public void setOnAudioFrameCapturedListener(OnAudioFrameCapturedListener listener) {
        mAudioFrameCapturedListener = listener;

    }


    public boolean startCapture() {
        return startCapture(DEFAULT_SOURCE, DEFAULT_SAMPLE_RATE, DEFAULT_CHANNEL_CONFIG,
                DEFAULT_AUDIO_FORMAT);
    }

    private int getMinBufferSize(int sampleRate, int channelConfig, int audioFormat) {
        int numOfChannels, bitsPersample;
        if (channelConfig == AudioFormat.CHANNEL_IN_MONO) {
            numOfChannels = 1;
        } else {
            numOfChannels = 2;
        }
        if (AudioFormat.ENCODING_PCM_16BIT == audioFormat) {
            bitsPersample = 16;
        } else {
            bitsPersample = 8;
        }
        int periodInFrames = sampleRate * TIMER_INTERVAL / 1000;		//num of frames in a second is same as sample rate
        //refer to android/4.1.1/frameworks/av/media/libmedia/AudioRecord.cpp, AudioRecord::getMinFrameCount method
        //we times 2 for ping pong use of record buffer
        mMinBufferSize = periodInFrames * 2  * numOfChannels * bitsPersample / 8;
        if (mMinBufferSize < AudioRecord.getMinBufferSize(sampleRate, channelConfig, audioFormat)) {
            // Check to make sure buffer size is not smaller than the smallest allowed one
            mMinBufferSize = AudioRecord.getMinBufferSize(sampleRate, channelConfig, audioFormat);
            // Set frame period and timer interval accordingly
//            periodInFrames = mMinBufferSize / ( 2 * bitsPersample * numOfChannels / 8 );
        }

        return mMinBufferSize;
    }

    public boolean startCapture(int audioSource, int sampleRateInHz, int channelConfig, int audioFormat) {

        if (mIsCaptureStarted) {
            Log.e(TAG, "hujd Capture already started !");
            return false;
        }

        mMinBufferSize = AudioRecord.getMinBufferSize(sampleRateInHz, channelConfig, audioFormat);

        if (mMinBufferSize == AudioRecord.ERROR_BAD_VALUE) {
            Log.e(TAG, "hujd Invalid parameter !");
            return false;
        }

        Log.e(TAG, "hujd getMinBufferSize = " + mMinBufferSize + " bytes !");

        mAudioRecord = new AudioRecord(audioSource, sampleRateInHz, channelConfig, audioFormat, mMinBufferSize);

        if (mAudioRecord.getState() == AudioRecord.STATE_UNINITIALIZED) {
            Log.e(TAG, "hujd AudioRecord initialize fail !");
            return false;
        }

        mAudioRecord.startRecording();


        mIsLoopExit = false;
        mCaptureThread = new Thread(new AudioCaptureRunnable());

        mCaptureThread.start();

        mIsCaptureStarted = true;

        Log.e(TAG, "hujd Start audio capture success !");
        return true;

    }


    public void stopCapture() {
        if (!mIsCaptureStarted) {
            return;
        }

        mIsLoopExit = true;

        try {
            mCaptureThread.interrupt();
            mCaptureThread.join(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        if (mAudioRecord.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING) {
            mAudioRecord.stop();
        }

        mAudioRecord.release();
        mIsCaptureStarted = false;
        mAudioFrameCapturedListener = null;

        Log.d(TAG, "Stop audio capture success !");

    }


    private class AudioCaptureRunnable implements Runnable {
        @Override
        public void run() {
            while (!mIsLoopExit) {
                byte[] buffer = new byte[mMinBufferSize];

                int ret = mAudioRecord.read(buffer, 0, mMinBufferSize);

                if (ret == AudioRecord.ERROR_INVALID_OPERATION) {
                    Log.e(TAG, "Error ERROR_INVALID_OPERATION");
                } else if (ret == AudioRecord.ERROR_BAD_VALUE) {
                    Log.e(TAG, "Error ERROR_BAD_VALUE");
                } else {
                    if (mAudioFrameCapturedListener != null) {
                        mAudioFrameCapturedListener.onAudioFrameCaptured(buffer);
                    }
                    Log.d(TAG, "OK, Captured " + ret + " bytes !");
                }
                SystemClock.sleep(10);
            }

        }

    }
}
