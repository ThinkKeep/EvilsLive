package com.thinkkeep.videolib.model;

import android.util.Log;

import java.nio.ByteBuffer;

/**
 * 无锁循环队列
 * Created by jason on 17/4/21.
 */

class RingBuffer{
    private final String TAG = "RingBuffer";
    private int r_index;
    private int w_index;
    private int size;
    private final byte STATU_INIT = 0;
    private final byte STATU_WAIT_DEQEUE = 1;
    private UserDefinedBuffer[] mUserDefinedBuffer;
    private long last_time;

    public RingBuffer(int max_size, int capacity){
        mUserDefinedBuffer = new UserDefinedBuffer[max_size];
        r_index = w_index = 0;
        size = max_size;
        for(int i=0 ;i<max_size; i++){
            mUserDefinedBuffer[i] = new UserDefinedBuffer();
            mUserDefinedBuffer[i].mVideoFrame = ByteBuffer.allocateDirect(capacity);
        }
    }

    public UserDefinedBuffer getUserDefinedBuffer(int index){
        return mUserDefinedBuffer[index];
    }

    int getRingW(){
        return w_index;
    }

    int getRingR(){
        return r_index;
    }
    int getRingSize(){
        return size;
    }

    void  setUserDefinedBufferStatus(int index, byte status){
        synchronized(mUserDefinedBuffer[index]){
            mUserDefinedBuffer[index].status = status;
        }
    }

    byte getUserDefinedBufferStatus(int index){
        synchronized(mUserDefinedBuffer[index]){
            return mUserDefinedBuffer[index].status;
        }
    }

    void enqueue(byte[] _data){
        int index = w_index & (size -1);
        Log.i(TAG, "#enqueue, index:"+index);
        if (index >= size){
            index = 0;
        }
        if (getUserDefinedBufferStatus(index) != STATU_INIT){
            Log.i(TAG, "i enqueue, index:"+index+", not dequeue" + ", STATUS:"+getUserDefinedBufferStatus(index));
            return;
        }
        setUserDefinedBufferStatus(index, STATU_WAIT_DEQEUE);
        mUserDefinedBuffer[index].mVideoFrame.rewind();
        mUserDefinedBuffer[index].mVideoFrame.put(_data);
        w_index += 1;
    }

    void enqueue(ByteBuffer data){
        int index = w_index & (size -1);
        Log.i(TAG, "enqueue, index:"+index);
        if (index >= size){
            index = 0;
        }
        if (getUserDefinedBufferStatus(index) != STATU_INIT){
            Log.i(TAG, "ii enqueue, index:"+index+", not dequeue" + ", STATUS:"+getUserDefinedBufferStatus(index));
            return;
        }
        setUserDefinedBufferStatus(index, STATU_WAIT_DEQEUE);
        mUserDefinedBuffer[index].mVideoFrame.rewind();
        mUserDefinedBuffer[index].mVideoFrame.put(data);
        w_index += 1;
        //last_time = System.currentTimeMillis();
    }

    long getLastTime(){
        return last_time;
    }

    int dequeue(){
        int index = r_index & (size -1);
        if (index == (w_index & (size -1))){
            Log.i(TAG, "dequeue, w_index:"+w_index + ", r_index:"+r_index);
            return -1;
        }
        Log.i(TAG, "dequeue, index:"+index);

        r_index += 1;
//			ByteBuffer data =  mUserDefinedBuffer[index].mVideoFrame;
//			mUserDefinedBuffer[index].mVideoFrame.rewind();
        return index;
    }



}