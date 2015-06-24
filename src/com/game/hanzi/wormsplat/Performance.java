package com.game.hanzi.wormsplat;

import java.lang.System;
import android.util.Log;

public class Performance {

	public long mNanoTimeHold1;
	public long mNanoTimeHold2;
	public long mMilliTimeHold1;
	public long mMilliTimeHold2;
	private long mTics;
	private long mTicsMax;
	//private long[] mArrayTics;
	public long[] mArrayTics = new long[1000];
	
	public long mMillis;
	public int mSeconds;
	public int mMinutes;
	
	private int mFrameCnt;
	private int mFrameMax;
	private int mFrameMilliMax;
	private long mFrameMilliCnt;
	private long mFramePrevMilliTime;
	
	public long mfMillis;
	public long mfMilliTimeHold1;
	public long mfMilliTimeHold2;
	
	public Performance()
	{
		mNanoTimeHold1 = System.nanoTime();
		mNanoTimeHold2 = System.nanoTime();
		mMilliTimeHold1 = System.currentTimeMillis();
		mMilliTimeHold2 = System.currentTimeMillis();
		mTics = 0;
		mTicsMax = 1000;
		
		//for(long i = 0;i<99;i++)
		//{	mArrayTics[(int)i]=i;
		//Log.d("GenericGame-DEBUG",  "Value " + mArrayTics[(int)i]);
		//}
		Log.d("GenericGame-DEBUG",  "Complete");
		
	}
	
	
	public void Reset()
	{	
		mNanoTimeHold1 = System.nanoTime();
		mNanoTimeHold2 = System.nanoTime();
		mMilliTimeHold1 = System.currentTimeMillis();
		mMilliTimeHold2 = System.currentTimeMillis();
		mTics = 0;
		mTicsMax = 1;
		
		//for(long i = 0;i<99;i++)
		//{	mArrayTics[(int)i]=i;
		//Log.d("GenericGame-DEBUG",  "Value " + mArrayTics[(int)i]);
		//}
		Log.d("GenericGame-DEBUG",  "Game Perfs Reset");
		
	}
	
	public void SetFrameMax(int fps)
	{
		mFrameMax = fps;
		if(fps>0){
			mFrameMilliMax = 1000/fps;
		}else
		{
			mFrameMilliMax = 0;
		}
			
	}
	
	public boolean CountFrames()
	{
		boolean maxed = false;
		if(mFrameMax>0)
		{  // reset count
		  
			mFrameMilliCnt +=FrameMilliCount();
			if(mFrameMilliCnt>mFrameMilliMax)
			{
				mFrameMilliCnt = 0;
				maxed = true;
			}
		}
		return maxed;
	}
	
	public long FrameMilliCount()
	{
		long time = System.currentTimeMillis();
		mFrameMilliCnt += time - mFramePrevMilliTime;
		mFramePrevMilliTime = time;
		
		return 1000/mFrameMilliCnt;
	}
	
	public long NanoRate()
	{
		long time = System.nanoTime();
		long rate = time - mNanoTimeHold1;
		mNanoTimeHold1 = time;
		return rate;
	}
	
	public long MilliRate()
	{
		long time = System.currentTimeMillis();
		long rate = time - mMilliTimeHold1;
		mMilliTimeHold1 = time;
		return rate;
	}
	
	public int getSeconds(boolean bReset)
	{
		long time = System.currentTimeMillis();
		long rate = time - mMilliTimeHold2;
		if(bReset)
		{	mMillis=0;
			return 0;
		}
		mMilliTimeHold2 = time;
		mMillis = (mMillis + rate);
		mSeconds = (int)(mMillis /1000);
		
		return mSeconds;
	}
	
	public long getfSeconds(boolean bReset)
	{
		long time = System.currentTimeMillis();
		long rate = time - mfMilliTimeHold2;
		if(bReset)
		{	mfMillis=0;
			mfMilliTimeHold2=System.currentTimeMillis();
			return 0;
		}
		mfMilliTimeHold2 = time;
		mfMillis = (mfMillis + rate);
		Log.d("com.game.wormsplat","TIME"+mfMillis);
		return  (mfMillis);
	}
		
	public void setTicsMax(long cnt)
	{
		if(cnt>1000)cnt=1000;
		mTicsMax = cnt;
		
	}
	
	public long AvgMilli(boolean bShowraw)
	{	
					
		long time=MilliRate();
		long timeAvg = 0;
		long timeAvgSum = 0;
		boolean bReset = false;
		
		//Log.d("GenericGame-DEBUG",  "Value " + time);
		// 
		if(mTicsMax>1)
		{	
			int idx = (int)mTics;
			
			mArrayTics[idx] = time;
			mTics++;
			if(mTics==mTicsMax);
			{	//average array create new value
				for(int i=0;i<mTicsMax;i++)timeAvgSum=timeAvgSum + mArrayTics[i];
				timeAvg = timeAvgSum/mTicsMax;						
				if(bShowraw)timeAvg=time;
				//Log.d("GenericGame-DEBUG",  "timeAvgSum " + timeAvgSum +" " + timeAvg);
				// reset
				mTics = 0;
				//mArrayTics = null; // FUCK YOU!
			}
			
		}else
		{	timeAvg = time;
			mTics = 0;
		}

		
		return timeAvg;
		
		
	}
	
}