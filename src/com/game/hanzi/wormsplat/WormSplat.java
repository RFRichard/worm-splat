
package com.game.hanzi.wormsplat;

import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.ShortBuffer;

import com.game.hanzi.wormsplat.R;

import android.app.Activity;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
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
 
public class WormSplat extends Activity
{
	public static long mTime;
	public static long mMilliTimeHold1;
	public int FPS;
	
	public GraphicsView Game;
	public SplashView Splash;
	private AdView adView;
	
	// Menu
	private static final int MENU_ABOUT = 0;
    private static final int MENU_EXIT = 1;
	
    // views
    LinearLayout MainView;
    
    // Handler Actions
    Handler mUIHandler;
    public static final int UPDATE_SCORE = 0;
    public static final int UPDATE_WORMS = 1;
    public static final int UPDATE_TIMER = 2;
    public static final int UPDATE_LEVEL = 3;
    public static final int SHOW_SPLASH = 4;
    public static final int SHOW_GAME = 5;
    //
    TextView txtScore;
    TextView txtTime;
    TextView txtWorms;
    
		
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
    	Log.d("PPPPPPPPPPPPPPPPPPPPPPPPPPP","Creating=======");
    	
    	super.onCreate(savedInstanceState);
  	    
    	mUIHandler = new Handler() {
            
            public void handleMessage(Message m) {
            
            	int mMsgDataA = m.arg1;
                int mMsgDataB = m.arg2;
            	Bundle data = m.getData();
            	
            	
            	switch(mMsgDataA){
            		case WormSplat.UPDATE_SCORE:
            			txtScore.setText("共有: "+mMsgDataB);
            			break;
            		case WormSplat.UPDATE_TIMER:
            			String tm = String.valueOf(mMsgDataB);
            			int slen = tm.length();
            			String tot1 = tm.substring(0, slen-1);
            			String tot2 = tm.substring(slen-1);
            			txtTime.setText("期限: "+tot1+"."+tot2);
            			break;
            		case WormSplat.UPDATE_WORMS:
            			txtWorms.setText("蟲子: "+mMsgDataB);
            			break;	
            		case WormSplat.SHOW_GAME:
            			setContentView(MainView);
            			break;
            		case WormSplat.SHOW_SPLASH:
            			setContentView(Splash);
            			break;
            	}
            	                            
            }
        };

    	
    	Splash = new SplashView(this, mUIHandler);
        
    	MainView = (LinearLayout) View.inflate(this, R.layout.main, null);
  	    
    	if(MainView!=null){
    		Game = new GraphicsView(this, mUIHandler);
    		//MainView.addView(adView);
    		MainView.addView(Game);
    		
    		// Initiate a generic request to load it with an ad
            //adView.loadAd(adRequest);
        }
    	if(MainView!=null){
    		txtWorms = (TextView) MainView.findViewById(R.id.txtWormCount);
    		txtScore = (TextView) MainView.findViewById(R.id.txtScore);
    		txtTime = (TextView) MainView.findViewById(R.id.txtTime);
    		
    	}
    	
    	setContentView(Splash);
    	
    	
    	
    	
    } 
    
    public static long MilliRate()
	{
		long time = System.currentTimeMillis();
		long rate = time - mMilliTimeHold1;
		mMilliTimeHold1 = time;
		return rate;
	}
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        super.onCreateOptionsMenu(menu);

        menu.add(0, MENU_ABOUT, 0, "About");
        menu.add(0, MENU_EXIT, 0, "Exit");

        return true;
    }
    
    @Override
    public boolean onPrepareOptionsMenu(Menu menu)
    {
    	//super.onPrepareOptionsMenu(menu);
    	menu.getItem(MENU_ABOUT).setEnabled(true);
    	menu.getItem(MENU_EXIT).setEnabled(true);
    	    	
    	return true;
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case MENU_ABOUT:
            	 String url = "http://www.bumperspace.com/wormsplat";
            	 launchWebIntent(url);
            	return true;
             case MENU_EXIT:
            	 finish();
                return true;
        }

        return super.onOptionsItemSelected(item);
    }
    
    public void launchWebIntent(String url){
    	
    	if(url!=null){
    		Intent i = new Intent(Intent.ACTION_VIEW);
    		i.setData(Uri.parse(url));
    		startActivity(i);
    	}
    	
    }
    
    @Override
    public void onPause()
    {	
    	super.onPause();
    	//Log.d("PPPPPPPPPPPPPPPPPPPPPPPPPPP","PAUSE=======");
    	
    }   
    
    @Override
	public boolean onKeyDown(int keyCode, KeyEvent msg) 
    {
        boolean bResult = false;
    	        
        if(keyCode==KeyEvent.KEYCODE_BACK){	
        	closeApp();
        }
		return bResult;
    }
    
    
    public void closeApp(){
    	//Log.d("PPPPPPPPPPPPPPPPPPPPPPPPPPP","CLOSING APP");
    	
    	Game.finishGraphicsView();
    	Game = null;
    	MainView = null;
    	Splash = null;
    	finish();
    	
    }
    
    /* load our native library */
    static {
        System.loadLibrary("wormsplat");
    }
}
                               
class GraphicsView extends View { 
	 
	public static final int NDK_2_1 = 0;
	
	private Bitmap mInterBitmap;
	private Bitmap mBitmap;
    private Bitmap mBackgroundBitmap;
    private Bitmap mScreenBitmap;
    private Bitmap mSplat1;
    private Bitmap mSplat2;
    private Bitmap mSplat3;
    private Bitmap mSplatmap;
    private Bitmap mReadyBitmap;
    private Bitmap mSetBitmap;
    private Bitmap mGoBitmap;
    private Bitmap mGameOverBitmap;
    private Bitmap mLevelBitmap;
    
    
    private float splatx, splaty;
     
    private Canvas sc;
    
    private Matrix matrix = new Matrix(); 
    private ByteBuffer buffer;
    private ByteBuffer combinedBuffer;
    private long mStartTime;
    Paint p = new Paint(Paint.ANTI_ALIAS_FLAG);
    
    private boolean bLocal = true;
    private boolean bSinglePass = false;
    private boolean bPassComplete = false;
	private int GraphicsRoutine = 0;
    
    public static final int FIRE = 1;
    public static final int PLASMA1 = 2;
    public static final int ALPHA_TEST = 3;
    public static final int B_LINE_TEST = 4;
    public static final int TUNNEL = 5;
    public static final int PLASMA2 = 6;
    public static final int JULIA_SET = 7;
    public static final int NOISE_CLOUD = 8;
    public static final int PHYSICS_SIM =9;
    public static final int PHYSICS_PARTICLE= 10;
    // bitmap 
    public static  int W = 320;         
    public static int H = 480;
    public Rect srcRect;
    public Rect dstRect;
    // device
    public static int viewW;         
    public static int viewH;
     
    
    public GameSounds snds;
    public Physics physics;
    // frame timing
    public static final int FPS = 120;
    public Performance pf = new Performance();
    public boolean breset = false;
    
    
    private Context ctx;
    private Button mb;
    
    private static native void psystemsettings(int  total_particles, double  speed_limit, double bounce_damping, double factor, double min_proximity);
    private static native void render(Object  bitmap, Object  bitmap2, long time_ms, int type, int w, int h, int action);
    private static native int uihandler(int x, int y, int w, int h, int action);
    
    private Handler mHandler;
    
    private int activeParticleCount = 0;
    private int gameScore = 0;
    private float elapsedTime = 0;
    
    private static final int MODE_FINISHED=0;
    private static final int MODE_PLAYING=1;
    private static final int MODE_STARTING=2;
    private static final int MODE_WAITING_ACTION=3;
    private int gameMode = 2;
    private int startModeDisplay=0;
    private GameLevels gameLevels;
    private boolean leveled = false;
    public GraphicsView(Context context, Handler pHandler) {
        super(context);     

        gameLevels = new GameLevels();

        mHandler = pHandler;

        DisplayMetrics metrics;
        WindowManager mWM;
        metrics = new DisplayMetrics();
        mWM = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        mWM.getDefaultDisplay().getMetrics(metrics);
        int dp = metrics.densityDpi;

        viewW = metrics.widthPixels;
        viewH =  metrics.heightPixels;

        Resources res = context.getResources();
        snds = new GameSounds(context);
        //snds.start();
        ctx = context;
         
        BitmapFactory.Options opts = new BitmapFactory.Options();
        opts.inScaled = false;
        opts.inPreferredConfig = Bitmap.Config.RGB_565;
        mBitmap = Bitmap.createBitmap(W, H, Bitmap.Config.RGB_565);
        mBackgroundBitmap = BitmapFactory.decodeResource(res, R.drawable.dirtback2, opts); 
        mScreenBitmap = Bitmap.createBitmap(W, H, Bitmap.Config.RGB_565);
        mInterBitmap = Bitmap.createScaledBitmap(mBackgroundBitmap, W, H, false);
        mSplatmap = Bitmap.createBitmap(W, H, Bitmap.Config.ARGB_8888);
       
                
        sc = new Canvas(mScreenBitmap);
        matrix.setScale(1.0f, 1.0f); // scaleX = scaleY = 1.0 for no scaling 
                
        buffer = ByteBuffer.allocateDirect(W * H * 2);
        combinedBuffer = ByteBuffer.allocateDirect(W * H * 2);
        //W=viewW;
        //H=viewH;
        // set bitmap rects        
        //srcRect = new Rect(0,0,W,H);
        //dstRect = new Rect(0,0,viewW,viewH);
        //dstRect = new Rect(0,0,480,800);
        
        //if(mInterBitmap.getConfig()==Bitmap.Config.RGB_565&&mInterBitmap.getWidth()==W&&mInterBitmap.getHeight()==H)
        mInterBitmap.copyPixelsToBuffer(combinedBuffer);
        
       	// words
        mReadyBitmap = BitmapFactory.decodeResource(res, R.drawable.ready, opts);
        mSetBitmap= BitmapFactory.decodeResource(res, R.drawable.set, opts);
        mGoBitmap= BitmapFactory.decodeResource(res, R.drawable.go, opts);
        mGameOverBitmap= BitmapFactory.decodeResource(res, R.drawable.gameover, opts);
        mLevelBitmap= BitmapFactory.decodeResource(res, R.drawable.level, opts);
       
        // splat bitmaps
       	opts.inPreferredConfig = Bitmap.Config.ARGB_8888;
        mSplat1 = BitmapFactory.decodeResource(res, R.drawable.splat1, opts);
        mSplat2 = BitmapFactory.decodeResource(res, R.drawable.splat2, opts);
        mSplat3 = BitmapFactory.decodeResource(res, R.drawable.splat3, opts);
        	
       
        mStartTime = System.currentTimeMillis();
              
        bSinglePass = false;
        GraphicsRoutine = PHYSICS_PARTICLE;
        bLocal = false;
        pf.SetFrameMax(FPS+3); 
         
        reset();
    
        mb = new Button(context);
		mb.setGravity(Gravity.CENTER);
		mb.setHeight(50);
		mb.setWidth(100);
		mb.setText("Reset");
		mb.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                // Perform action on clicks
                //menu_options_close();
            	//Toast.makeText(v.getContext(), "Beep Bop", Toast.LENGTH_SHORT).show();
            	breset = true;
            	reset();
            	
            }
        });
    
		
		
    } 
    
    private void sendGameUpdate(int msgA, int msgB){
	    Message themsg;
		themsg = Message.obtain(mHandler);
		themsg.arg1 = msgA;
		themsg.arg2 = msgB;
		
		mHandler.sendMessage(themsg);
    }
	
    public void finishGraphicsView(){
    	
    	//this.srcRect=null;
    	//this.dstRect=null;
    	this.snds = null;
    	this.mb = null;
    	this.sc=null;
    	this.matrix=null;
    	this.ctx = null;
    
    }
     
    private void renderBitmap(Canvas canvas)
    {
    	int thereset = 0;
    	
    	
    	if(breset==true)
    	{
    		thereset = 1;
    		render(mBitmap, mBackgroundBitmap, System.currentTimeMillis() - mStartTime, GraphicsRoutine, W, H , thereset);
     	   	breset = false;
    		
    	}
    	 
    	 
		if(pf.CountFrames())   
        {
		   render(mBitmap, mBackgroundBitmap, System.currentTimeMillis() - mStartTime, GraphicsRoutine, W, H , thereset);
    	   
        }
		//canvas.drawBitmap(mBitmap, ((this.getWidth()-W)/2), 0, null);
		canvas.drawBitmap(mBitmap, null, dstRect, null);
	      
    }
       
    @Override protected void onDraw(Canvas canvas) {
        //canvas.drawColor(0xFFCCCCCC);
    	    
    	if(srcRect==null)
    		srcRect = new Rect(0,0,W,H);
    	if(dstRect==null){
        	dstRect = new Rect(0,0,this.getWidth(),this.getHeight());
        	
    	}
    	
        if(true)   
        {
        	
           if(bLocal)
	       {   
	    	   physics.draw(canvas);
	    	   
	       }else
	       {
	    	   if(bSinglePass&&!bPassComplete)
		       {	
	    		   renderBitmap(canvas);
		    	   bPassComplete = true;
		       }
		         
		       if(!bSinglePass)
		       {
		    	   renderBitmap(canvas);
		       }
	                                
	       }
           
           // render splat map
           if(splatx>0&&splaty>0)
           {	// draw splat
        	   int n = 0;
        	   n = (int)(3.0 * Math.random()) + 1;
        	   switch(n)
        	   {
        	   case 1:
        		   canvas.drawBitmap(mSplat1, splatx-52, splaty-48, null);
        		   break;
        	   case 2:
        		   canvas.drawBitmap(mSplat3, splatx-24, splaty-23, null);
        		   break;
        	   case 3:
        		   canvas.drawBitmap(mSplat3, splatx-24, splaty-23, null);
        		   break;
        	        	   
        	   
        	   }
        	   splatx=0;
        	   splaty=0;
        	   
           }
        
           
           if(this.gameMode==GraphicsView.MODE_STARTING){
        	     	   
        	   
        	   switch(startModeDisplay){
        	   		case 0:
        	   			pf.getSeconds(true);
        	   			canvas.drawBitmap(mReadyBitmap, (this.getWidth()/2)-90, (this.getHeight()/4), null);
        	   			startModeDisplay = 1;
        	   			break;
        	   		case 1:
        	   			if(pf.getSeconds(false)<2){
        	   				canvas.drawBitmap(mReadyBitmap, (this.getWidth()/2)-90, (this.getHeight()/4), null);
            	   			
        	   			}else
        	   			{
        	   				startModeDisplay = 2;
        	   			}
        	   			break;
        	   		case 2:
        	   			if(pf.getSeconds(false)<3){
        	   				canvas.drawBitmap(mSetBitmap, (this.getWidth()/2)-56, (this.getHeight()/4), null);
        	   				
        	   			}else
        	   			{
        	   				startModeDisplay = 3;
        	   			}
        	   			break;
        	   		case 3:
        	   			if(pf.getSeconds(false)<4){
        	   				canvas.drawBitmap(mGoBitmap, (this.getWidth()/2)-65, (this.getHeight()/4), null);
        	   				
        	   			}else
        	   			{
        	   				
        	   				startModeDisplay = 0;
        	   				
        	   			}
        	   			if(pf.getSeconds(false)<6){
        	   				this.elapsedTime = pf.getfSeconds(true);
        	   				this.gameMode = GraphicsView.MODE_PLAYING;
        	   			}
        	   			break;
        	   			
        	   }
        	   
           }
           
           if(this.gameMode == GraphicsView.MODE_FINISHED||this.gameMode == GraphicsView.MODE_WAITING_ACTION){
        	   int tmr = pf.getSeconds(false); 
        	   if(this.gameMode == GraphicsView.MODE_FINISHED&&tmr>12){
        		   this.gameMode = GraphicsView.MODE_WAITING_ACTION;
        		   
        	   }
      	   
        	   if(this.activeParticleCount==0){
        		   // new level
        		   if(!leveled){
        			   gameLevels.increaseLevel();
        			   leveled = true;
        			   snds.SoundWin();
        		   }
        		   canvas.drawBitmap(mLevelBitmap, (this.getWidth()/2)-70, (this.getHeight()/4), null);
        		   p.setColor(Color.rgb(0x66,00,00));
                   p.setTextSize(80f);
                   p.setShadowLayer(5, 5, 5, 0xff);
                   canvas.drawText(""+gameLevels.getCurrentLevel() , (this.getWidth()/2) ,((this.getHeight()/3)*2), p);
       	        
                   
        	   }
        	   if(this.activeParticleCount>0){
        	       // game over
        		   canvas.drawBitmap(mGameOverBitmap, (this.getWidth()/2)-180, (this.getHeight()/4), null);
	   			   
        	   }
        	   
        	   
        	   
           }
           
           if(this.gameMode == GraphicsView.MODE_PLAYING){
        	   this.elapsedTime = pf.getfSeconds(false); 
        	   if(this.activeParticleCount==0){
        		   this.gameMode = GraphicsView.MODE_FINISHED;
        		   pf.getSeconds(true);
        	   }else if(this.elapsedTime>gameLevels.getCurrentMinTime()){
        		   this.gameMode = GraphicsView.MODE_FINISHED;
        		   snds.SoundLose();
        		   pf.getSeconds(true);
        	   }
           
           }
	        // Display Processing Stats
            p.setColor(Color.WHITE);
            p.setTextSize(12f);
            
            //WormSplat.mTime = WormSplat.MilliRate();
	        //canvas.drawText("Count = "+activeParticleCount+" H "+mBitmap.getHeight()+"  Processing  : "+(int)WormSplat.mTime+"ms  FPS Rate "+1000/(WormSplat.mTime)+"       Displaying: FPS "+FPS , 10,30, p);
	        //canvas.drawText("MODE :"+this.gameMode , 10,30, p);
	        
	        sendGameUpdate(WormSplat.UPDATE_WORMS, this.activeParticleCount);
	        sendGameUpdate(WormSplat.UPDATE_SCORE, this.gameScore);
	        sendGameUpdate(WormSplat.UPDATE_TIMER, ((gameLevels.getCurrentMinTime()/100)-((int)this.elapsedTime/100))); // (int) (this.elapsedTime*1000)
	        
	        // force a redraw
	        invalidate();        
        }                                                  
                                                                                                                                                                                            
                 
    }
      
    @Override
    public boolean onTouchEvent(MotionEvent m)
    {
    	float xpos, ypos;
    	float xposhist, yposhist;
    	int histsize;
    	int hit = 0;
    	int n;
    	boolean handled = false;
  //  	return handled;
    	
    	xpos = m.getRawX();
    	ypos = m.getRawY();
    	
    	xpos = m.getX();
    	ypos = m.getY();
    	
    	// swipe 
    	if(m.getAction()==MotionEvent.ACTION_MOVE)
    	{
    		histsize = m.getHistorySize();
    		histsize = 0; // set to zero no need in this game
    		if(histsize>0)
    		{
    			// changed from 1 to 0 due to error 
    			xposhist = m.getHistoricalX(0);
    			if(xposhist>xpos)
    			{
    				if((xposhist-xpos)>150)
    				{
    					//Toast.makeText(ctx, "Swipe "+this.getWidth()+"  "+this.getHeight(), Toast.LENGTH_SHORT).show();	
    				}
    			}
    		}	
    			
    		
    		handled = true;
    	}
    	
    	if(m.getAction()==MotionEvent.ACTION_DOWN)
    	{
		    if(this.gameMode==GraphicsView.MODE_PLAYING){
    			// compensate -- why?
		    	ypos +=68;
		    	  	
		    	// Reset Button?
		    	/*
		    	if(ypos>(713-30)&&xpos>381)
		    	{	breset = true;
		    		reset();
		    		return true;
		    	}
		    	*/
		    	// 
		    	float cx, cy;
		    	 
		    	cx = (xpos/viewW)*W;
		    	cy = (ypos/viewH)*H;
		    	Log.d("=======TTTTT", "xpos="+xpos+" ypos="+ypos+" viewW="+viewW+" viewH="+viewH);
		    	hit = uihandler((int) cx, (int) cy, (int)xpos, (int) ypos,  0);
		    	activeParticleCount -=hit;
		    	if(hit>0)
		    	{	
		    		this.gameScore +=hit*10;
		    		// handle a hit 
		    		// playsound
		    		n = (int)(3.0 * Math.random()) + 1;
		    		snds.SoundSplat((int)(3.0 * Math.random()) + 1);
		    		//Toast.makeText(ctx, "Splat!", Toast.LENGTH_SHORT).show();
		    		splatx=xpos-12;
		    		splaty=ypos-85;
		    		
		    	}else
		    	{
		    		splatx = 0;
		    		splaty = 0;
		    	}
		    }
		    if(this.gameMode==GraphicsView.MODE_WAITING_ACTION){
		    	reset();
		    	this.gameMode = GraphicsView.MODE_STARTING;
		    }
		    handled = true;
    	}
    	
    	 
		return handled;
 
    }
    
    public void reset()
    {
    	int total_particles = 50;
        double  speed_limit = 20.0;
        double bounce_damping = 0.75;
        double factor = 9.0;
        double min_proximity = 20.0;
        
        gameMode = GraphicsView.MODE_STARTING;
        this.elapsedTime = 0;
        if(!leveled){
        	gameLevels.setCurrentLevel(1);
        	this.gameScore = 0;
        }
        total_particles = gameLevels.getCurrentEnemyCount();
        activeParticleCount = total_particles;        
        mStartTime = System.currentTimeMillis();
        breset=true;
        leveled = false;
        psystemsettings( total_particles, speed_limit, bounce_damping, factor, min_proximity);
    }
      
    
}      
