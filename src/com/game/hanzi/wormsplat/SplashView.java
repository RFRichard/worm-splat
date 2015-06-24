package com.game.hanzi.wormsplat;

import java.nio.ByteBuffer;

import com.game.hanzi.wormsplat.R;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.content.Context;
import android.content.res.Resources;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.AvoidXfermode.Mode;

class SplashView extends View { 
	 

	
	private Bitmap mSplash;
	private Bitmap mBitmap;
   
	
    private Context ctx;
    private Paint p = new Paint();
    private Button mb;
    
    private Handler mHandler;
    
    public SplashView(Context context, Handler mHand) {
         super(context);     
        Resources res = context.getResources();
        ctx = context;
        mHandler = mHand;
        
        BitmapFactory.Options opts = new BitmapFactory.Options();
        opts.inScaled = false;
        opts.inPreferredConfig = Bitmap.Config.RGB_565;
        //mBitmap = Bitmap.createBitmap(W, H, Bitmap.Config.RGB_565);
        mSplash = BitmapFactory.decodeResource(res, R.drawable.splash1, opts); 
        
        mb = new Button(context);
		mb.setGravity(Gravity.CENTER);
		mb.setHeight(50);
		mb.setWidth(100);
		mb.setText("Done");
		mb.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                // Perform action on clicks
                //menu_options_close();
            	//Toast.makeText(v.getContext(), "Beep Bop", Toast.LENGTH_SHORT).show();
            	hide();
            	
            }
        });
        
        
    } 
    
         
     
   
      
    @Override protected void onDraw(Canvas canvas) {
    	//92fea1
    	//canvas.drawBitmap(mSplash, 0, 0, null);
        canvas.drawBitmap(mSplash, null, new Rect(0, 0, this.getWidth(), this.getHeight()), p);         
    }
    
    
    @Override
    public boolean onTouchEvent(MotionEvent m)
    {
    	float xpos, ypos;
    	    	
    	if(m.getAction()!=MotionEvent.ACTION_DOWN)return false;
    	xpos = m.getRawX();
    	ypos = m.getRawY()-38;
    	  	
    	hide();
    	if(ypos>240)
    	{	
    	}else
    	{	
    		
    	}
    	 
    	
        
    	if(xpos>160)
    	{
    	   	
    	}
    	  
		return false;
    }
    
    public void hide()
    {
    	Message msg = new Message();
    	msg.arg1 = WormSplat.SHOW_GAME;
    	mHandler.sendMessage(msg);
    }
    
    
}      
