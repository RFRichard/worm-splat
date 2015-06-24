package com.game.hanzi.wormsplat;



import com.game.hanzi.wormsplat.R;

import android.media.SoundPool;
import android.media.AudioManager;
import android.util.Log;
import android.content.Context;


public class GameSounds extends Thread{

	private SoundPool mMP;
	private 
	
	int[] Sounds = new int[20];
	
	public int SND_NONE = -1;
	public int SND_BACKGROUND;
	public int SND_HITA;
	public int SND_HITB;
	public int SND_HITC;
	public int SND_HITD;
	public int SND_WIN;
	public int SND_LOSE;
	public int SND_START;
	
	public static final int BACK_SND_1 = 1;
	public static final int BACK_SND_2 = 2;
	
	public boolean mbRun = true;
	public boolean mbBackSndActive;
	public int mBackSound;
	public int mRate = 1;
	
	private boolean mbPlayBack;
	
	
	
	public GameSounds(Context c)
	{
			
		
		mMP = new SoundPool(5,AudioManager.STREAM_MUSIC,0);
		
		// Load Sounds
		//SND_BACKGROUND = mMP.load(c, R.raw.omega_main, 0);
		SND_HITA = mMP.load(c, R.raw.splat1, 0);
		SND_HITB = mMP.load(c, R.raw.splat2, 0);
		SND_HITC = mMP.load(c, R.raw.splat3, 0);
		SND_HITD = mMP.load(c, R.raw.splat4, 0);
		SND_WIN = mMP.load(c, R.raw.levelup, 0);
		SND_LOSE = mMP.load(c, R.raw.gameover, 0);
	}

	@Override
    public void run() {
		
		
		while (mbRun) {
        	
            try {
               
                	/*
                    if (mSP.getSeconds(false)>2&&mbPlayBack==true){
                    	mSP.getSeconds(true);
                    	SoundBackground(true);
                    }
                    */
            		//Log.d("GenericGame-DEBUG", "GGT-Sound");
                    //Log.d("GenericGame-DEBUG",  "SP Secs"+mSP.getSeconds(false));
            	} finally {
                // do this in a finally so that if an exception is thrown
                // during the above, we don't leave the Surface in an
                // inconsistent state
                
            	}
            	
	   }
}
	
	
	public int PlaySound(int snd)
	{
		synchronized(this){
			
			Sounds[Sounds.length] = snd;
		}
		return Sounds.length;
	}
	
	
	public void setRunning(boolean x)
	{
		mbRun = true;
		
	}
	
	public void stopRunning(boolean x)
	{
		mbRun = false;
		
	}
	
	
	
	public int SoundBackground(boolean m)
	{
		if(m)
		{	
			return mMP.play(SND_BACKGROUND, .5f, .5f, 0, 0, (float) mRate);
		}else
		{
			mbPlayBack=false;
			mMP.stop(SND_BACKGROUND);
			
			return 0;
		}
	}

	public int SoundHitA()
	{
		if(SND_HITA>0)
		return mMP.play(SND_HITA, .9f, .9f, 0, 0, 1.0f);
		return 0;
	}
	
	public int SoundHitB()
	{
		return mMP.play(SND_HITB, .9f, .9f, 0, 0, 1.0f);
	}
	
	public int SoundHitC()
	{
		return mMP.play(SND_HITC, .9f, .9f, 0, 0, 1.0f);
	}
	
	public int SoundHitD()
	{
		return mMP.play(SND_HITD, .9f, .9f, 0, 0, 1.0f);
	}
	
	public int SoundSplat(int type)
	{
		switch(type)
		{
		case 1:
			return mMP.play(SND_HITA, .9f, .9f, 0, 0, 1.0f);
		case 2:
			return mMP.play(SND_HITB, .9f, .9f, 0, 0, 1.0f);
		case 3:
			return mMP.play(SND_HITC, .9f, .9f, 0, 0, 1.0f);
		case 4:
			return mMP.play(SND_HITD, .9f, .9f, 0, 0, 1.0f);
		default:
			return 0;
		}
		
	}
	
	public int SoundWin()
	{
		return mMP.play(SND_WIN, .9f, .9f, 0, 0, 1.0f);
	}
	
	public int SoundLose()
	{
		return mMP.play(SND_LOSE, .9f, .9f, 0, 0, 1.0f);
	}
	
	public int SoundStart()
	{
		return mMP.play(SND_WIN, .9f, .9f, 0, 0, 1.0f);
	}
	
	public void PlayBackground(boolean x, int rt)
	{
		
		mbPlayBack = x;
		mRate = rt;
		SoundBackground(true);
		
	}

}