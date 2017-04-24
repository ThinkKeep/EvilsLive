//package com.thinkkeep.videolib.model;
//
//import android.util.Log;
//
//import java.nio.ByteBuffer;
//
///**
// * Created by jsson on 17/4/21.
// */
//
//public class PushVideoThread extends Thread{
//    private static final String TAG = "PushVideoThread";
//    private boolean mExitFlag = false;
//    private final byte STATU_INIT = 0;
//    private final byte STATU_WAIT_DEQEUE = 1;
//
//    public void setExitFlg(boolean bFlag){
//        mExitFlag = bFlag;
//    }
//
//    @Override
//    public void run() {
//        android.os.Process
//                .setThreadPriority(android.os.Process.THREAD_PRIORITY_URGENT_AUDIO);
//
//        Log.i(TAG, "PushVideoThread() run start.");
//        final int delay = (100 / mFps);//
//        while(!mExitFlag){
//            long start=System.currentTimeMillis();
//            if (mRingBuffer == null){
//                try {
//                    Thread.sleep(delay);
//                } catch (Exception e) {
//                    // TODO Auto-generated catch block
//                    e.printStackTrace();
//                }
//                continue;
//            }
//            int index = mRingBuffer.dequeue();
//            if (index == -1){
//                try {
//                    Thread.sleep(delay);
//                } catch (Exception e) {
//                    // TODO Auto-generated catch block
//                    e.printStackTrace();
//                }
//                continue;
//            }
//            if (STATU_WAIT_DEQEUE != mRingBuffer.getUserDefinedBufferStatus(index)){
//
//                Log.i(TAG, "Ana  dequeue mRingBuffer.getUserDefinedBufferStatus(index):"+ mRingBuffer.getUserDefinedBufferStatus(index));
//
//                try {
//                    Thread.sleep(delay);
//                } catch (Exception e) {
//                    // TODO Auto-generated catch block
//                    e.printStackTrace();
//                }
//                continue;
//            }
//            UserDefinedBuffer userDefindedBuffer = mRingBuffer.getUserDefinedBuffer(index);
//
//            ByteBuffer byteBuffer = userDefindedBuffer.mVideoFrame;
//            if (byteBuffer != null){
//                framesRendered ++;
//                if ((framesRendered % 100) == 0) {
//                    logStatistics();
//                    framesRendered = 0;
//                    startTimeNs= System.nanoTime();
//                }
////				    Log.i(TAG, "Ana  dequeue getRingW:"+ write +",getRingR:"+ read);
//                mProducer.push(byteBuffer, mVideoFrame.capacity());
//                mRingBuffer.setUserDefinedBufferStatus(index, STATU_INIT);
//            }
//
//            long end=System.currentTimeMillis();
//            if ((end - start) < delay && (end - start) > 0){
//                try {
//                    long value = delay - (end -start);
//                    if (value > 0){
//                        Thread.sleep(value);
//                    }
//                } catch (Exception e) {
//                    // TODO Auto-generated catch block
//                    e.printStackTrace();
//                }
//            }
//        }
//        Log.i(TAG, "PushVideoThread() run End.");
//    }
//}
